// Copyright 2016 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/snapshot/code-serializer.h"

#include <memory>

#include "src/base/logging.h"
#include "src/base/platform/elapsed-timer.h"
#include "src/base/platform/platform.h"
#include "src/codegen/macro-assembler.h"
#include "src/common/globals.h"
#include "src/debug/debug.h"
#include "src/handles/maybe-handles.h"
#include "src/handles/persistent-handles.h"
#include "src/heap/heap-inl.h"
#include "src/heap/local-factory-inl.h"
#include "src/heap/parked-scope.h"
#include "src/logging/counters-scopes.h"
#include "src/logging/log.h"
#include "src/logging/runtime-call-stats-scope.h"
#include "src/objects/objects-inl.h"
#include "src/objects/shared-function-info.h"
#include "src/objects/slots.h"
#include "src/objects/visitors.h"
#include "src/snapshot/object-deserializer.h"
#include "src/snapshot/snapshot-utils.h"
#include "src/snapshot/snapshot.h"
#include "src/utils/version.h"

namespace v8 {
namespace internal {

AlignedCachedData::AlignedCachedData(const byte* data, int length)
    : owns_data_(false), rejected_(false), data_(data), length_(length) {
  if (!IsAligned(reinterpret_cast<intptr_t>(data), kPointerAlignment)) {
    byte* copy = NewArray<byte>(length);
    DCHECK(IsAligned(reinterpret_cast<intptr_t>(copy), kPointerAlignment));
    CopyBytes(copy, data, length);
    data_ = copy;
    AcquireDataOwnership();
  }
}

CodeSerializer::CodeSerializer(Isolate* isolate, uint32_t source_hash)
    : Serializer(isolate, Snapshot::kDefaultSerializerFlags),
      source_hash_(source_hash) {}

// static
ScriptCompiler::CachedData* CodeSerializer::Serialize(
    Handle<SharedFunctionInfo> info) {
  Isolate* isolate = info->GetIsolate();
  TRACE_EVENT_CALL_STATS_SCOPED(isolate, "v8", "V8.Execute");
  NestedTimedHistogramScope histogram_timer(
      isolate->counters()->compile_serialize());
  RCS_SCOPE(isolate, RuntimeCallCounterId::kCompileSerialize);
  TRACE_EVENT0(TRACE_DISABLED_BY_DEFAULT("v8.compile"), "V8.CompileSerialize");

  base::ElapsedTimer timer;
  if (FLAG_profile_deserialization) timer.Start();
  Handle<Script> script(Script::cast(info->script()), isolate);
  if (FLAG_trace_serializer) {
    PrintF("[Serializing from");
    script->name().ShortPrint();
    PrintF("]\n");
  }
#if V8_ENABLE_WEBASSEMBLY
  // TODO(7110): Enable serialization of Asm modules once the AsmWasmData is
  // context independent.
  if (script->ContainsAsmModule()) return nullptr;
#endif  // V8_ENABLE_WEBASSEMBLY

  // Serialize code object.
  Handle<String> source(String::cast(script->source()), isolate);
  HandleScope scope(isolate);
  CodeSerializer cs(isolate, SerializedCodeData::SourceHash(
                                 source, script->origin_options()));
  DisallowGarbageCollection no_gc;
  cs.reference_map()->AddAttachedReference(*source);
  AlignedCachedData* cached_data = cs.SerializeSharedFunctionInfo(info);

  if (FLAG_profile_deserialization) {
    double ms = timer.Elapsed().InMillisecondsF();
    int length = cached_data->length();
    PrintF("[Serializing to %d bytes took %0.3f ms]\n", length, ms);
  }

  ScriptCompiler::CachedData* result =
      new ScriptCompiler::CachedData(cached_data->data(), cached_data->length(),
                                     ScriptCompiler::CachedData::BufferOwned);
  cached_data->ReleaseDataOwnership();
  delete cached_data;

  return result;
}

AlignedCachedData* CodeSerializer::SerializeSharedFunctionInfo(
    Handle<SharedFunctionInfo> info) {
  DisallowGarbageCollection no_gc;

  VisitRootPointer(Root::kHandleScope, nullptr,
                   FullObjectSlot(info.location()));
  SerializeDeferredObjects();
  Pad();

  SerializedCodeData data(sink_.data(), this);

  return data.GetScriptData();
}

bool CodeSerializer::SerializeReadOnlyObject(Handle<HeapObject> obj) {
  if (!ReadOnlyHeap::Contains(*obj)) return false;

  // For objects on the read-only heap, never serialize the object, but instead
  // create a back reference that encodes the page number as the chunk_index and
  // the offset within the page as the chunk_offset.
  Address address = obj->address();
  BasicMemoryChunk* chunk = BasicMemoryChunk::FromAddress(address);
  uint32_t chunk_index = 0;
  ReadOnlySpace* const read_only_space = isolate()->heap()->read_only_space();
  for (ReadOnlyPage* page : read_only_space->pages()) {
    if (chunk == page) break;
    ++chunk_index;
  }
  uint32_t chunk_offset = static_cast<uint32_t>(chunk->Offset(address));
  sink_.Put(kReadOnlyHeapRef, "ReadOnlyHeapRef");
  sink_.PutInt(chunk_index, "ReadOnlyHeapRefChunkIndex");
  sink_.PutInt(chunk_offset, "ReadOnlyHeapRefChunkOffset");
  return true;
}

void CodeSerializer::SerializeObjectImpl(Handle<HeapObject> obj) {
  if (SerializeHotObject(obj)) return;

  if (SerializeRoot(obj)) return;

  if (SerializeBackReference(obj)) return;

  if (SerializeReadOnlyObject(obj)) return;

  CHECK(!obj->IsCode());

  ReadOnlyRoots roots(isolate());
  if (ElideObject(*obj)) {
    return SerializeObject(roots.undefined_value_handle());
  }

  if (obj->IsScript()) {
    Handle<Script> script_obj = Handle<Script>::cast(obj);
    DCHECK_NE(script_obj->compilation_type(), Script::COMPILATION_TYPE_EVAL);
    // We want to differentiate between undefined and uninitialized_symbol for
    // context_data for now. It is hack to allow debugging for scripts that are
    // included as a part of custom snapshot. (see debug::Script::IsEmbedded())
    Object context_data = script_obj->context_data();
    if (context_data != roots.undefined_value() &&
        context_data != roots.uninitialized_symbol()) {
      script_obj->set_context_data(roots.undefined_value());
    }
    // We don't want to serialize host options to avoid serializing unnecessary
    // object graph.
    FixedArray host_options = script_obj->host_defined_options();
    script_obj->set_host_defined_options(roots.empty_fixed_array());
    SerializeGeneric(obj);
    script_obj->set_host_defined_options(host_options);
    script_obj->set_context_data(context_data);
    return;
  }

  if (obj->IsSharedFunctionInfo()) {
    Handle<SharedFunctionInfo> sfi = Handle<SharedFunctionInfo>::cast(obj);
    DCHECK(!sfi->IsApiFunction());
#if V8_ENABLE_WEBASSEMBLY
    // TODO(7110): Enable serializing of Asm modules once the AsmWasmData
    // is context independent.
    DCHECK(!sfi->HasAsmWasmData());
#endif  // V8_ENABLE_WEBASSEMBLY

    DebugInfo debug_info;
    BytecodeArray debug_bytecode_array;
    if (sfi->HasDebugInfo()) {
      // Clear debug info.
      debug_info = sfi->GetDebugInfo();
      if (debug_info.HasInstrumentedBytecodeArray()) {
        debug_bytecode_array = debug_info.DebugBytecodeArray();
        sfi->SetActiveBytecodeArray(debug_info.OriginalBytecodeArray());
      }
      sfi->set_script_or_debug_info(debug_info.script(), kReleaseStore);
    }
    DCHECK(!sfi->HasDebugInfo());

    SerializeGeneric(obj);

    // Restore debug info
    if (!debug_info.is_null()) {
      sfi->set_script_or_debug_info(debug_info, kReleaseStore);
      if (!debug_bytecode_array.is_null()) {
        sfi->SetActiveBytecodeArray(debug_bytecode_array);
      }
    }
    return;
  }

  // NOTE(mmarchini): If we try to serialize an InterpreterData our process
  // will crash since it stores a code object. Instead, we serialize the
  // bytecode array stored within the InterpreterData, which is the important
  // information. On deserialization we'll create our code objects again, if
  // --interpreted-frames-native-stack is on. See v8:9122 for more context
#ifndef V8_TARGET_ARCH_ARM
  if (V8_UNLIKELY(FLAG_interpreted_frames_native_stack) &&
      obj->IsInterpreterData()) {
    obj = handle(InterpreterData::cast(*obj).bytecode_array(), isolate());
  }
#endif  // V8_TARGET_ARCH_ARM

  // Past this point we should not see any (context-specific) maps anymore.
  CHECK(!obj->IsMap());
  // There should be no references to the global object embedded.
  CHECK(!obj->IsJSGlobalProxy() && !obj->IsJSGlobalObject());
  // Embedded FixedArrays that need rehashing must support rehashing.
  CHECK_IMPLIES(obj->NeedsRehashing(), obj->CanBeRehashed());
  // We expect no instantiated function objects or contexts.
  CHECK(!obj->IsJSFunction() && !obj->IsContext());

  SerializeGeneric(obj);
}

void CodeSerializer::SerializeGeneric(Handle<HeapObject> heap_object) {
  // Object has not yet been serialized.  Serialize it here.
  ObjectSerializer serializer(this, heap_object, &sink_);
  serializer.Serialize();
}

namespace {

#ifndef V8_TARGET_ARCH_ARM
// NOTE(mmarchini): when FLAG_interpreted_frames_native_stack is on, we want to
// create duplicates of InterpreterEntryTrampoline for the deserialized
// functions, otherwise we'll call the builtin IET for those functions (which
// is not what a user of this flag wants).
void CreateInterpreterDataForDeserializedCode(Isolate* isolate,
                                              Handle<SharedFunctionInfo> sfi,
                                              bool log_code_creation) {
  Handle<Script> script(Script::cast(sfi->script()), isolate);
  String name = ReadOnlyRoots(isolate).empty_string();
  if (script->name().IsString()) name = String::cast(script->name());
  Handle<String> name_handle(name, isolate);

  SharedFunctionInfo::ScriptIterator iter(isolate, *script);
  for (SharedFunctionInfo shared_info = iter.Next(); !shared_info.is_null();
       shared_info = iter.Next()) {
    if (!shared_info.HasBytecodeArray()) continue;
    Handle<SharedFunctionInfo> info = handle(shared_info, isolate);
    Handle<Code> code = isolate->factory()->CopyCode(Handle<Code>::cast(
        isolate->factory()->interpreter_entry_trampoline_for_profiling()));

    Handle<InterpreterData> interpreter_data =
        Handle<InterpreterData>::cast(isolate->factory()->NewStruct(
            INTERPRETER_DATA_TYPE, AllocationType::kOld));

    interpreter_data->set_bytecode_array(info->GetBytecodeArray(isolate));
    interpreter_data->set_interpreter_trampoline(ToCodeT(*code));

    info->set_interpreter_data(*interpreter_data);

    if (!log_code_creation) continue;
    Handle<AbstractCode> abstract_code = Handle<AbstractCode>::cast(code);
    int line_num = script->GetLineNumber(info->StartPosition()) + 1;
    int column_num = script->GetColumnNumber(info->StartPosition()) + 1;
    PROFILE(isolate,
            CodeCreateEvent(CodeEventListener::FUNCTION_TAG, abstract_code,
                            info, name_handle, line_num, column_num));
  }
}
#endif  // V8_TARGET_ARCH_ARM

class StressOffThreadDeserializeThread final : public base::Thread {
 public:
  explicit StressOffThreadDeserializeThread(Isolate* isolate,
                                            AlignedCachedData* cached_data)
      : Thread(
            base::Thread::Options("StressOffThreadDeserializeThread", 2 * MB)),
        isolate_(isolate),
        cached_data_(cached_data) {}

  void Run() final {
    LocalIsolate local_isolate(isolate_, ThreadKind::kBackground);
    UnparkedScope unparked_scope(&local_isolate);
    LocalHandleScope handle_scope(&local_isolate);
    off_thread_data_ =
        CodeSerializer::StartDeserializeOffThread(&local_isolate, cached_data_);
  }

  MaybeHandle<SharedFunctionInfo> Finalize(Isolate* isolate,
                                           Handle<String> source,
                                           ScriptOriginOptions origin_options) {
    return CodeSerializer::FinishOffThreadDeserialize(
        isolate, std::move(off_thread_data_), cached_data_, source,
        origin_options);
  }

 private:
  Isolate* isolate_;
  AlignedCachedData* cached_data_;
  CodeSerializer::OffThreadDeserializeData off_thread_data_;
};

void FinalizeDeserialization(Isolate* isolate,
                             Handle<SharedFunctionInfo> result,
                             const base::ElapsedTimer& timer) {
  const bool log_code_creation =
      isolate->logger()->is_listening_to_code_events() ||
      isolate->is_profiling() ||
      isolate->code_event_dispatcher()->IsListeningToCodeEvents();

#ifndef V8_TARGET_ARCH_ARM
  if (V8_UNLIKELY(FLAG_interpreted_frames_native_stack))
    CreateInterpreterDataForDeserializedCode(isolate, result,
                                             log_code_creation);
#endif  // V8_TARGET_ARCH_ARM

  bool needs_source_positions = isolate->NeedsSourcePositionsForProfiling();

  if (log_code_creation || FLAG_log_function_events) {
    Handle<Script> script(Script::cast(result->script()), isolate);
    Handle<String> name(script->name().IsString()
                            ? String::cast(script->name())
                            : ReadOnlyRoots(isolate).empty_string(),
                        isolate);

    if (FLAG_log_function_events) {
      LOG(isolate,
          FunctionEvent("deserialize", script->id(),
                        timer.Elapsed().InMillisecondsF(),
                        result->StartPosition(), result->EndPosition(), *name));
    }
    if (log_code_creation) {
      Script::InitLineEnds(isolate, script);

      SharedFunctionInfo::ScriptIterator iter(isolate, *script);
      for (SharedFunctionInfo info = iter.Next(); !info.is_null();
           info = iter.Next()) {
        if (info.is_compiled()) {
          Handle<SharedFunctionInfo> shared_info(info, isolate);
          if (needs_source_positions) {
            SharedFunctionInfo::EnsureSourcePositionsAvailable(isolate,
                                                               shared_info);
          }
          DisallowGarbageCollection no_gc;
          int line_num =
              script->GetLineNumber(shared_info->StartPosition()) + 1;
          int column_num =
              script->GetColumnNumber(shared_info->StartPosition()) + 1;
          PROFILE(
              isolate,
              CodeCreateEvent(
                  shared_info->is_toplevel() ? CodeEventListener::SCRIPT_TAG
                                             : CodeEventListener::FUNCTION_TAG,
                  handle(shared_info->abstract_code(isolate), isolate),
                  shared_info, name, line_num, column_num));
        }
      }
    }
  }

  if (needs_source_positions) {
    Handle<Script> script(Script::cast(result->script()), isolate);
    Script::InitLineEnds(isolate, script);
  }
}

}  // namespace

MaybeHandle<SharedFunctionInfo> CodeSerializer::Deserialize(
    Isolate* isolate, AlignedCachedData* cached_data, Handle<String> source,
    ScriptOriginOptions origin_options) {
  if (FLAG_stress_background_compile) {
    StressOffThreadDeserializeThread thread(isolate, cached_data);
    CHECK(thread.Start());
    thread.Join();
    return thread.Finalize(isolate, source, origin_options);
    // TODO(leszeks): Compare off-thread deserialized data to on-thread.
  }

  base::ElapsedTimer timer;
  if (FLAG_profile_deserialization || FLAG_log_function_events) timer.Start();

  HandleScope scope(isolate);

  SerializedCodeSanityCheckResult sanity_check_result =
      SerializedCodeSanityCheckResult::kSuccess;
  const SerializedCodeData scd = SerializedCodeData::FromCachedData(
      cached_data, SerializedCodeData::SourceHash(source, origin_options),
      &sanity_check_result);
  if (sanity_check_result != SerializedCodeSanityCheckResult::kSuccess) {
    if (FLAG_profile_deserialization) PrintF("[Cached code failed check]\n");
    DCHECK(cached_data->rejected());
    isolate->counters()->code_cache_reject_reason()->AddSample(
        static_cast<int>(sanity_check_result));
    return MaybeHandle<SharedFunctionInfo>();
  }

  // Deserialize.
  MaybeHandle<SharedFunctionInfo> maybe_result =
      ObjectDeserializer::DeserializeSharedFunctionInfo(isolate, &scd, source);

  Handle<SharedFunctionInfo> result;
  if (!maybe_result.ToHandle(&result)) {
    // Deserializing may fail if the reservations cannot be fulfilled.
    if (FLAG_profile_deserialization) PrintF("[Deserializing failed]\n");
    return MaybeHandle<SharedFunctionInfo>();
  }

  if (FLAG_profile_deserialization) {
    double ms = timer.Elapsed().InMillisecondsF();
    int length = cached_data->length();
    PrintF("[Deserializing from %d bytes took %0.3f ms]\n", length, ms);
  }

  FinalizeDeserialization(isolate, result, timer);

  return scope.CloseAndEscape(result);
}

CodeSerializer::OffThreadDeserializeData
CodeSerializer::StartDeserializeOffThread(LocalIsolate* local_isolate,
                                          AlignedCachedData* cached_data) {
  OffThreadDeserializeData result;

  DCHECK(!local_isolate->heap()->HasPersistentHandles());

  const SerializedCodeData scd =
      SerializedCodeData::FromCachedDataWithoutSource(
          cached_data, &result.sanity_check_result);
  if (result.sanity_check_result != SerializedCodeSanityCheckResult::kSuccess) {
    // Exit early but don't report yet, we'll re-check this when finishing on
    // the main thread
    DCHECK(cached_data->rejected());
    return result;
  }

  MaybeHandle<SharedFunctionInfo> local_maybe_result =
      OffThreadObjectDeserializer::DeserializeSharedFunctionInfo(
          local_isolate, &scd, &result.scripts);

  result.maybe_result =
      local_isolate->heap()->NewPersistentMaybeHandle(local_maybe_result);
  result.persistent_handles = local_isolate->heap()->DetachPersistentHandles();

  return result;
}

MaybeHandle<SharedFunctionInfo> CodeSerializer::FinishOffThreadDeserialize(
    Isolate* isolate, OffThreadDeserializeData&& data,
    AlignedCachedData* cached_data, Handle<String> source,
    ScriptOriginOptions origin_options) {
  base::ElapsedTimer timer;
  if (FLAG_profile_deserialization || FLAG_log_function_events) timer.Start();

  HandleScope scope(isolate);

  // Do a source sanity check now that we have the source. It's important for
  // FromPartiallySanityCheckedCachedData call that the sanity_check_result
  // holds the result of the off-thread sanity check.
  SerializedCodeSanityCheckResult sanity_check_result =
      data.sanity_check_result;
  const SerializedCodeData scd =
      SerializedCodeData::FromPartiallySanityCheckedCachedData(
          cached_data, SerializedCodeData::SourceHash(source, origin_options),
          &sanity_check_result);
  if (sanity_check_result != SerializedCodeSanityCheckResult::kSuccess) {
    // The only case where the deserialization result could exist despite a
    // check failure is on a source mismatch, since we can't test for this
    // off-thread.
    DCHECK_IMPLIES(!data.maybe_result.is_null(),
                   sanity_check_result ==
                       SerializedCodeSanityCheckResult::kSourceMismatch);
    // The only kind of sanity check we can't test for off-thread is a source
    // mismatch.
    DCHECK_IMPLIES(sanity_check_result != data.sanity_check_result,
                   sanity_check_result ==
                       SerializedCodeSanityCheckResult::kSourceMismatch);
    if (FLAG_profile_deserialization) PrintF("[Cached code failed check]\n");
    DCHECK(cached_data->rejected());
    isolate->counters()->code_cache_reject_reason()->AddSample(
        static_cast<int>(sanity_check_result));
    return MaybeHandle<SharedFunctionInfo>();
  }

  Handle<SharedFunctionInfo> result;
  if (!data.maybe_result.ToHandle(&result)) {
    // Deserializing may fail if the reservations cannot be fulfilled.
    if (FLAG_profile_deserialization) {
      PrintF("[Off-thread deserializing failed]\n");
    }
    return MaybeHandle<SharedFunctionInfo>();
  }

  // Change the result persistent handle into a regular handle.
  DCHECK(data.persistent_handles->Contains(result.location()));
  result = handle(*result, isolate);

  // Fix up the source on the script. This should be the only deserialized
  // script, and the off-thread deserializer should have set its source to
  // the empty string.
  DCHECK_EQ(data.scripts.size(), 1);
  DCHECK_EQ(result->script(), *data.scripts[0]);
  DCHECK_EQ(Script::cast(result->script()).source(),
            ReadOnlyRoots(isolate).empty_string());
  Script::cast(result->script()).set_source(*source);

  // Fix up the script list to include the newly deserialized script.
  Handle<WeakArrayList> list = isolate->factory()->script_list();
  for (Handle<Script> script : data.scripts) {
    DCHECK(data.persistent_handles->Contains(script.location()));
    list =
        WeakArrayList::AddToEnd(isolate, list, MaybeObjectHandle::Weak(script));
  }
  isolate->heap()->SetRootScriptList(*list);

  if (FLAG_profile_deserialization) {
    double ms = timer.Elapsed().InMillisecondsF();
    int length = cached_data->length();
    PrintF("[Finishing off-thread deserialize from %d bytes took %0.3f ms]\n",
           length, ms);
  }

  FinalizeDeserialization(isolate, result, timer);

  return scope.CloseAndEscape(result);
}

SerializedCodeData::SerializedCodeData(const std::vector<byte>* payload,
                                       const CodeSerializer* cs) {
  DisallowGarbageCollection no_gc;

  // Calculate sizes.
  uint32_t size = kHeaderSize + static_cast<uint32_t>(payload->size());
  DCHECK(IsAligned(size, kPointerAlignment));

  // Allocate backing store and create result data.
  AllocateData(size);

  // Zero out pre-payload data. Part of that is only used for padding.
  memset(data_, 0, kHeaderSize);

  // Set header values.
  SetMagicNumber();
  SetHeaderValue(kVersionHashOffset, Version::Hash());
  SetHeaderValue(kSourceHashOffset, cs->source_hash());
  SetHeaderValue(kFlagHashOffset, FlagList::Hash());
  SetHeaderValue(kPayloadLengthOffset, static_cast<uint32_t>(payload->size()));

  // Zero out any padding in the header.
  memset(data_ + kUnalignedHeaderSize, 0, kHeaderSize - kUnalignedHeaderSize);

  // Copy serialized data.
  CopyBytes(data_ + kHeaderSize, payload->data(),
            static_cast<size_t>(payload->size()));
  uint32_t checksum =
      FLAG_verify_snapshot_checksum ? Checksum(ChecksummedContent()) : 0;
  SetHeaderValue(kChecksumOffset, checksum);
}

SerializedCodeSanityCheckResult SerializedCodeData::SanityCheck(
    uint32_t expected_source_hash) const {
  SerializedCodeSanityCheckResult result = SanityCheckWithoutSource();
  if (result != SerializedCodeSanityCheckResult::kSuccess) return result;
  return SanityCheckJustSource(expected_source_hash);
}

SerializedCodeSanityCheckResult SerializedCodeData::SanityCheckJustSource(
    uint32_t expected_source_hash) const {
  uint32_t source_hash = GetHeaderValue(kSourceHashOffset);
  if (source_hash != expected_source_hash) {
    return SerializedCodeSanityCheckResult::kSourceMismatch;
  }
  return SerializedCodeSanityCheckResult::kSuccess;
}

SerializedCodeSanityCheckResult SerializedCodeData::SanityCheckWithoutSource()
    const {
  if (this->size_ < kHeaderSize) {
    return SerializedCodeSanityCheckResult::kInvalidHeader;
  }
  uint32_t magic_number = GetMagicNumber();
  if (magic_number != kMagicNumber) {
    return SerializedCodeSanityCheckResult::kMagicNumberMismatch;
  }
  uint32_t version_hash = GetHeaderValue(kVersionHashOffset);
  if (version_hash != Version::Hash()) {
    return SerializedCodeSanityCheckResult::kVersionMismatch;
  }
  uint32_t flags_hash = GetHeaderValue(kFlagHashOffset);
  if (flags_hash != FlagList::Hash()) {
    return SerializedCodeSanityCheckResult::kFlagsMismatch;
  }
  uint32_t payload_length = GetHeaderValue(kPayloadLengthOffset);
  uint32_t max_payload_length = this->size_ - kHeaderSize;
  if (payload_length > max_payload_length) {
    return SerializedCodeSanityCheckResult::kLengthMismatch;
  }
  if (FLAG_verify_snapshot_checksum) {
    uint32_t checksum = GetHeaderValue(kChecksumOffset);
    if (Checksum(ChecksummedContent()) != checksum) {
      return SerializedCodeSanityCheckResult::kChecksumMismatch;
    }
  }
  return SerializedCodeSanityCheckResult::kSuccess;
}

uint32_t SerializedCodeData::SourceHash(Handle<String> source,
                                        ScriptOriginOptions origin_options) {
  const uint32_t source_length = source->length();

  static constexpr uint32_t kModuleFlagMask = (1 << 31);
  const uint32_t is_module = origin_options.IsModule() ? kModuleFlagMask : 0;
  DCHECK_EQ(0, source_length & kModuleFlagMask);

  return source_length | is_module;
}

// Return ScriptData object and relinquish ownership over it to the caller.
AlignedCachedData* SerializedCodeData::GetScriptData() {
  DCHECK(owns_data_);
  AlignedCachedData* result = new AlignedCachedData(data_, size_);
  result->AcquireDataOwnership();
  owns_data_ = false;
  data_ = nullptr;
  return result;
}

base::Vector<const byte> SerializedCodeData::Payload() const {
  const byte* payload = data_ + kHeaderSize;
  DCHECK(IsAligned(reinterpret_cast<intptr_t>(payload), kPointerAlignment));
  int length = GetHeaderValue(kPayloadLengthOffset);
  DCHECK_EQ(data_ + size_, payload + length);
  return base::Vector<const byte>(payload, length);
}

SerializedCodeData::SerializedCodeData(AlignedCachedData* data)
    : SerializedData(const_cast<byte*>(data->data()), data->length()) {}

SerializedCodeData SerializedCodeData::FromCachedData(
    AlignedCachedData* cached_data, uint32_t expected_source_hash,
    SerializedCodeSanityCheckResult* rejection_result) {
  DisallowGarbageCollection no_gc;
  SerializedCodeData scd(cached_data);
  *rejection_result = scd.SanityCheck(expected_source_hash);
  if (*rejection_result != SerializedCodeSanityCheckResult::kSuccess) {
    cached_data->Reject();
    return SerializedCodeData(nullptr, 0);
  }
  return scd;
}

SerializedCodeData SerializedCodeData::FromCachedDataWithoutSource(
    AlignedCachedData* cached_data,
    SerializedCodeSanityCheckResult* rejection_result) {
  DisallowGarbageCollection no_gc;
  SerializedCodeData scd(cached_data);
  *rejection_result = scd.SanityCheckWithoutSource();
  if (*rejection_result != SerializedCodeSanityCheckResult::kSuccess) {
    cached_data->Reject();
    return SerializedCodeData(nullptr, 0);
  }
  return scd;
}

SerializedCodeData SerializedCodeData::FromPartiallySanityCheckedCachedData(
    AlignedCachedData* cached_data, uint32_t expected_source_hash,
    SerializedCodeSanityCheckResult* rejection_result) {
  DisallowGarbageCollection no_gc;
  // The previous call to FromCachedDataWithoutSource may have already rejected
  // the cached data, so re-use the previous rejection result if it's not a
  // success.
  if (*rejection_result != SerializedCodeSanityCheckResult::kSuccess) {
    // FromCachedDataWithoutSource doesn't check the source, so there can't be
    // a source mismatch.
    DCHECK_NE(*rejection_result,
              SerializedCodeSanityCheckResult::kSourceMismatch);
    cached_data->Reject();
    return SerializedCodeData(nullptr, 0);
  }
  SerializedCodeData scd(cached_data);
  *rejection_result = scd.SanityCheckJustSource(expected_source_hash);
  if (*rejection_result != SerializedCodeSanityCheckResult::kSuccess) {
    // This check only checks the source, so the only possible failure is a
    // source mismatch.
    DCHECK_EQ(*rejection_result,
              SerializedCodeSanityCheckResult::kSourceMismatch);
    cached_data->Reject();
    return SerializedCodeData(nullptr, 0);
  }
  return scd;
}

}  // namespace internal
}  // namespace v8
