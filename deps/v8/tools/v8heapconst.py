# Copyright 2019 the V8 project authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can
# be found in the LICENSE file.

# This file is automatically generated by mkgrokdump and should not
# be modified manually.

# List of known V8 instance types.
INSTANCE_TYPES = {
  0: "INTERNALIZED_STRING_TYPE",
  2: "EXTERNAL_INTERNALIZED_STRING_TYPE",
  8: "ONE_BYTE_INTERNALIZED_STRING_TYPE",
  10: "EXTERNAL_ONE_BYTE_INTERNALIZED_STRING_TYPE",
  18: "UNCACHED_EXTERNAL_INTERNALIZED_STRING_TYPE",
  26: "UNCACHED_EXTERNAL_ONE_BYTE_INTERNALIZED_STRING_TYPE",
  32: "STRING_TYPE",
  33: "CONS_STRING_TYPE",
  34: "EXTERNAL_STRING_TYPE",
  35: "SLICED_STRING_TYPE",
  37: "THIN_STRING_TYPE",
  40: "ONE_BYTE_STRING_TYPE",
  41: "CONS_ONE_BYTE_STRING_TYPE",
  42: "EXTERNAL_ONE_BYTE_STRING_TYPE",
  43: "SLICED_ONE_BYTE_STRING_TYPE",
  45: "THIN_ONE_BYTE_STRING_TYPE",
  50: "UNCACHED_EXTERNAL_STRING_TYPE",
  58: "UNCACHED_EXTERNAL_ONE_BYTE_STRING_TYPE",
  96: "SHARED_STRING_TYPE",
  104: "SHARED_ONE_BYTE_STRING_TYPE",
  128: "SYMBOL_TYPE",
  129: "BIG_INT_BASE_TYPE",
  130: "HEAP_NUMBER_TYPE",
  131: "ODDBALL_TYPE",
  132: "PROMISE_FULFILL_REACTION_JOB_TASK_TYPE",
  133: "PROMISE_REJECT_REACTION_JOB_TASK_TYPE",
  134: "CALLABLE_TASK_TYPE",
  135: "CALLBACK_TASK_TYPE",
  136: "PROMISE_RESOLVE_THENABLE_JOB_TASK_TYPE",
  137: "LOAD_HANDLER_TYPE",
  138: "STORE_HANDLER_TYPE",
  139: "FUNCTION_TEMPLATE_INFO_TYPE",
  140: "OBJECT_TEMPLATE_INFO_TYPE",
  141: "ACCESS_CHECK_INFO_TYPE",
  142: "ACCESSOR_INFO_TYPE",
  143: "ACCESSOR_PAIR_TYPE",
  144: "ALIASED_ARGUMENTS_ENTRY_TYPE",
  145: "ALLOCATION_MEMENTO_TYPE",
  146: "ALLOCATION_SITE_TYPE",
  147: "ARRAY_BOILERPLATE_DESCRIPTION_TYPE",
  148: "ASM_WASM_DATA_TYPE",
  149: "ASYNC_GENERATOR_REQUEST_TYPE",
  150: "BREAK_POINT_TYPE",
  151: "BREAK_POINT_INFO_TYPE",
  152: "CACHED_TEMPLATE_OBJECT_TYPE",
  153: "CALL_HANDLER_INFO_TYPE",
  154: "CLASS_POSITIONS_TYPE",
  155: "DEBUG_INFO_TYPE",
  156: "ENUM_CACHE_TYPE",
  157: "FEEDBACK_CELL_TYPE",
  158: "FUNCTION_TEMPLATE_RARE_DATA_TYPE",
  159: "INTERCEPTOR_INFO_TYPE",
  160: "INTERPRETER_DATA_TYPE",
  161: "MODULE_REQUEST_TYPE",
  162: "PROMISE_CAPABILITY_TYPE",
  163: "PROMISE_REACTION_TYPE",
  164: "PROPERTY_DESCRIPTOR_OBJECT_TYPE",
  165: "PROTOTYPE_INFO_TYPE",
  166: "REG_EXP_BOILERPLATE_DESCRIPTION_TYPE",
  167: "SCRIPT_TYPE",
  168: "SCRIPT_OR_MODULE_TYPE",
  169: "SOURCE_TEXT_MODULE_INFO_ENTRY_TYPE",
  170: "STACK_FRAME_INFO_TYPE",
  171: "TEMPLATE_OBJECT_DESCRIPTION_TYPE",
  172: "TUPLE2_TYPE",
  173: "WASM_CONTINUATION_OBJECT_TYPE",
  174: "WASM_EXCEPTION_TAG_TYPE",
  175: "WASM_INDIRECT_FUNCTION_TABLE_TYPE",
  176: "FIXED_ARRAY_TYPE",
  177: "HASH_TABLE_TYPE",
  178: "EPHEMERON_HASH_TABLE_TYPE",
  179: "GLOBAL_DICTIONARY_TYPE",
  180: "NAME_DICTIONARY_TYPE",
  181: "NUMBER_DICTIONARY_TYPE",
  182: "ORDERED_HASH_MAP_TYPE",
  183: "ORDERED_HASH_SET_TYPE",
  184: "ORDERED_NAME_DICTIONARY_TYPE",
  185: "SIMPLE_NUMBER_DICTIONARY_TYPE",
  186: "CLOSURE_FEEDBACK_CELL_ARRAY_TYPE",
  187: "OBJECT_BOILERPLATE_DESCRIPTION_TYPE",
  188: "SCRIPT_CONTEXT_TABLE_TYPE",
  189: "BYTE_ARRAY_TYPE",
  190: "BYTECODE_ARRAY_TYPE",
  191: "FIXED_DOUBLE_ARRAY_TYPE",
  192: "INTERNAL_CLASS_WITH_SMI_ELEMENTS_TYPE",
  193: "SLOPPY_ARGUMENTS_ELEMENTS_TYPE",
  194: "AWAIT_CONTEXT_TYPE",
  195: "BLOCK_CONTEXT_TYPE",
  196: "CATCH_CONTEXT_TYPE",
  197: "DEBUG_EVALUATE_CONTEXT_TYPE",
  198: "EVAL_CONTEXT_TYPE",
  199: "FUNCTION_CONTEXT_TYPE",
  200: "MODULE_CONTEXT_TYPE",
  201: "NATIVE_CONTEXT_TYPE",
  202: "SCRIPT_CONTEXT_TYPE",
  203: "WITH_CONTEXT_TYPE",
  204: "FOREIGN_TYPE",
  205: "WASM_FUNCTION_DATA_TYPE",
  206: "WASM_CAPI_FUNCTION_DATA_TYPE",
  207: "WASM_EXPORTED_FUNCTION_DATA_TYPE",
  208: "WASM_JS_FUNCTION_DATA_TYPE",
  209: "WASM_TYPE_INFO_TYPE",
  210: "TURBOFAN_BITSET_TYPE_TYPE",
  211: "TURBOFAN_HEAP_CONSTANT_TYPE_TYPE",
  212: "TURBOFAN_OTHER_NUMBER_CONSTANT_TYPE_TYPE",
  213: "TURBOFAN_RANGE_TYPE_TYPE",
  214: "TURBOFAN_UNION_TYPE_TYPE",
  215: "EXPORTED_SUB_CLASS_BASE_TYPE",
  216: "EXPORTED_SUB_CLASS_TYPE",
  217: "EXPORTED_SUB_CLASS2_TYPE",
  218: "SMALL_ORDERED_HASH_MAP_TYPE",
  219: "SMALL_ORDERED_HASH_SET_TYPE",
  220: "SMALL_ORDERED_NAME_DICTIONARY_TYPE",
  221: "ABSTRACT_INTERNAL_CLASS_SUBCLASS1_TYPE",
  222: "ABSTRACT_INTERNAL_CLASS_SUBCLASS2_TYPE",
  223: "DESCRIPTOR_ARRAY_TYPE",
  224: "STRONG_DESCRIPTOR_ARRAY_TYPE",
  225: "SOURCE_TEXT_MODULE_TYPE",
  226: "SYNTHETIC_MODULE_TYPE",
  227: "UNCOMPILED_DATA_WITH_PREPARSE_DATA_TYPE",
  228: "UNCOMPILED_DATA_WITHOUT_PREPARSE_DATA_TYPE",
  229: "WEAK_FIXED_ARRAY_TYPE",
  230: "TRANSITION_ARRAY_TYPE",
  231: "CALL_REF_DATA_TYPE",
  232: "CELL_TYPE",
  233: "CODE_TYPE",
  234: "CODE_DATA_CONTAINER_TYPE",
  235: "COVERAGE_INFO_TYPE",
  236: "EMBEDDER_DATA_ARRAY_TYPE",
  237: "FEEDBACK_METADATA_TYPE",
  238: "FEEDBACK_VECTOR_TYPE",
  239: "FILLER_TYPE",
  240: "FREE_SPACE_TYPE",
  241: "INTERNAL_CLASS_TYPE",
  242: "INTERNAL_CLASS_WITH_STRUCT_ELEMENTS_TYPE",
  243: "MAP_TYPE",
  244: "MEGA_DOM_HANDLER_TYPE",
  245: "ON_HEAP_BASIC_BLOCK_PROFILER_DATA_TYPE",
  246: "PREPARSE_DATA_TYPE",
  247: "PROPERTY_ARRAY_TYPE",
  248: "PROPERTY_CELL_TYPE",
  249: "SCOPE_INFO_TYPE",
  250: "SHARED_FUNCTION_INFO_TYPE",
  251: "SMI_BOX_TYPE",
  252: "SMI_PAIR_TYPE",
  253: "SORT_STATE_TYPE",
  254: "SWISS_NAME_DICTIONARY_TYPE",
  255: "WASM_API_FUNCTION_REF_TYPE",
  256: "WEAK_ARRAY_LIST_TYPE",
  257: "WEAK_CELL_TYPE",
  258: "WASM_ARRAY_TYPE",
  259: "WASM_STRUCT_TYPE",
  260: "JS_PROXY_TYPE",
  1057: "JS_OBJECT_TYPE",
  261: "JS_GLOBAL_OBJECT_TYPE",
  262: "JS_GLOBAL_PROXY_TYPE",
  263: "JS_MODULE_NAMESPACE_TYPE",
  1040: "JS_SPECIAL_API_OBJECT_TYPE",
  1041: "JS_PRIMITIVE_WRAPPER_TYPE",
  1058: "JS_API_OBJECT_TYPE",
  2058: "JS_LAST_DUMMY_API_OBJECT_TYPE",
  2059: "JS_BOUND_FUNCTION_TYPE",
  2060: "JS_FUNCTION_TYPE",
  2061: "BIGINT64_TYPED_ARRAY_CONSTRUCTOR_TYPE",
  2062: "BIGUINT64_TYPED_ARRAY_CONSTRUCTOR_TYPE",
  2063: "FLOAT32_TYPED_ARRAY_CONSTRUCTOR_TYPE",
  2064: "FLOAT64_TYPED_ARRAY_CONSTRUCTOR_TYPE",
  2065: "INT16_TYPED_ARRAY_CONSTRUCTOR_TYPE",
  2066: "INT32_TYPED_ARRAY_CONSTRUCTOR_TYPE",
  2067: "INT8_TYPED_ARRAY_CONSTRUCTOR_TYPE",
  2068: "UINT16_TYPED_ARRAY_CONSTRUCTOR_TYPE",
  2069: "UINT32_TYPED_ARRAY_CONSTRUCTOR_TYPE",
  2070: "UINT8_CLAMPED_TYPED_ARRAY_CONSTRUCTOR_TYPE",
  2071: "UINT8_TYPED_ARRAY_CONSTRUCTOR_TYPE",
  2072: "JS_ARRAY_CONSTRUCTOR_TYPE",
  2073: "JS_PROMISE_CONSTRUCTOR_TYPE",
  2074: "JS_REG_EXP_CONSTRUCTOR_TYPE",
  2075: "JS_CLASS_CONSTRUCTOR_TYPE",
  2076: "JS_ARRAY_ITERATOR_PROTOTYPE_TYPE",
  2077: "JS_ITERATOR_PROTOTYPE_TYPE",
  2078: "JS_MAP_ITERATOR_PROTOTYPE_TYPE",
  2079: "JS_OBJECT_PROTOTYPE_TYPE",
  2080: "JS_PROMISE_PROTOTYPE_TYPE",
  2081: "JS_REG_EXP_PROTOTYPE_TYPE",
  2082: "JS_SET_ITERATOR_PROTOTYPE_TYPE",
  2083: "JS_SET_PROTOTYPE_TYPE",
  2084: "JS_STRING_ITERATOR_PROTOTYPE_TYPE",
  2085: "JS_TYPED_ARRAY_PROTOTYPE_TYPE",
  2086: "JS_MAP_KEY_ITERATOR_TYPE",
  2087: "JS_MAP_KEY_VALUE_ITERATOR_TYPE",
  2088: "JS_MAP_VALUE_ITERATOR_TYPE",
  2089: "JS_SET_KEY_VALUE_ITERATOR_TYPE",
  2090: "JS_SET_VALUE_ITERATOR_TYPE",
  2091: "JS_GENERATOR_OBJECT_TYPE",
  2092: "JS_ASYNC_FUNCTION_OBJECT_TYPE",
  2093: "JS_ASYNC_GENERATOR_OBJECT_TYPE",
  2094: "JS_DATA_VIEW_TYPE",
  2095: "JS_TYPED_ARRAY_TYPE",
  2096: "JS_MAP_TYPE",
  2097: "JS_SET_TYPE",
  2098: "JS_WEAK_MAP_TYPE",
  2099: "JS_WEAK_SET_TYPE",
  2100: "JS_ARGUMENTS_OBJECT_TYPE",
  2101: "JS_ARRAY_TYPE",
  2102: "JS_ARRAY_BUFFER_TYPE",
  2103: "JS_ARRAY_ITERATOR_TYPE",
  2104: "JS_ASYNC_FROM_SYNC_ITERATOR_TYPE",
  2105: "JS_COLLATOR_TYPE",
  2106: "JS_CONTEXT_EXTENSION_OBJECT_TYPE",
  2107: "JS_DATE_TYPE",
  2108: "JS_DATE_TIME_FORMAT_TYPE",
  2109: "JS_DISPLAY_NAMES_TYPE",
  2110: "JS_ERROR_TYPE",
  2111: "JS_FINALIZATION_REGISTRY_TYPE",
  2112: "JS_LIST_FORMAT_TYPE",
  2113: "JS_LOCALE_TYPE",
  2114: "JS_MESSAGE_OBJECT_TYPE",
  2115: "JS_NUMBER_FORMAT_TYPE",
  2116: "JS_PLURAL_RULES_TYPE",
  2117: "JS_PROMISE_TYPE",
  2118: "JS_REG_EXP_TYPE",
  2119: "JS_REG_EXP_STRING_ITERATOR_TYPE",
  2120: "JS_RELATIVE_TIME_FORMAT_TYPE",
  2121: "JS_SEGMENT_ITERATOR_TYPE",
  2122: "JS_SEGMENTER_TYPE",
  2123: "JS_SEGMENTS_TYPE",
  2124: "JS_STRING_ITERATOR_TYPE",
  2125: "JS_TEMPORAL_CALENDAR_TYPE",
  2126: "JS_TEMPORAL_DURATION_TYPE",
  2127: "JS_TEMPORAL_INSTANT_TYPE",
  2128: "JS_TEMPORAL_PLAIN_DATE_TYPE",
  2129: "JS_TEMPORAL_PLAIN_DATE_TIME_TYPE",
  2130: "JS_TEMPORAL_PLAIN_MONTH_DAY_TYPE",
  2131: "JS_TEMPORAL_PLAIN_TIME_TYPE",
  2132: "JS_TEMPORAL_PLAIN_YEAR_MONTH_TYPE",
  2133: "JS_TEMPORAL_TIME_ZONE_TYPE",
  2134: "JS_TEMPORAL_ZONED_DATE_TIME_TYPE",
  2135: "JS_V8_BREAK_ITERATOR_TYPE",
  2136: "JS_WEAK_REF_TYPE",
  2137: "WASM_GLOBAL_OBJECT_TYPE",
  2138: "WASM_INSTANCE_OBJECT_TYPE",
  2139: "WASM_MEMORY_OBJECT_TYPE",
  2140: "WASM_MODULE_OBJECT_TYPE",
  2141: "WASM_SUSPENDER_OBJECT_TYPE",
  2142: "WASM_TABLE_OBJECT_TYPE",
  2143: "WASM_TAG_OBJECT_TYPE",
  2144: "WASM_VALUE_OBJECT_TYPE",
}

# List of known V8 maps.
KNOWN_MAPS = {
  ("read_only_space", 0x02119): (243, "MetaMap"),
  ("read_only_space", 0x02141): (131, "NullMap"),
  ("read_only_space", 0x02169): (224, "StrongDescriptorArrayMap"),
  ("read_only_space", 0x02191): (229, "WeakFixedArrayMap"),
  ("read_only_space", 0x021d1): (156, "EnumCacheMap"),
  ("read_only_space", 0x02205): (176, "FixedArrayMap"),
  ("read_only_space", 0x02251): (8, "OneByteInternalizedStringMap"),
  ("read_only_space", 0x0229d): (240, "FreeSpaceMap"),
  ("read_only_space", 0x022c5): (239, "OnePointerFillerMap"),
  ("read_only_space", 0x022ed): (239, "TwoPointerFillerMap"),
  ("read_only_space", 0x02315): (131, "UninitializedMap"),
  ("read_only_space", 0x0238d): (131, "UndefinedMap"),
  ("read_only_space", 0x023d1): (130, "HeapNumberMap"),
  ("read_only_space", 0x02405): (131, "TheHoleMap"),
  ("read_only_space", 0x02465): (131, "BooleanMap"),
  ("read_only_space", 0x02509): (189, "ByteArrayMap"),
  ("read_only_space", 0x02531): (176, "FixedCOWArrayMap"),
  ("read_only_space", 0x02559): (177, "HashTableMap"),
  ("read_only_space", 0x02581): (128, "SymbolMap"),
  ("read_only_space", 0x025a9): (40, "OneByteStringMap"),
  ("read_only_space", 0x025d1): (249, "ScopeInfoMap"),
  ("read_only_space", 0x025f9): (250, "SharedFunctionInfoMap"),
  ("read_only_space", 0x02621): (233, "CodeMap"),
  ("read_only_space", 0x02649): (232, "CellMap"),
  ("read_only_space", 0x02671): (248, "GlobalPropertyCellMap"),
  ("read_only_space", 0x02699): (204, "ForeignMap"),
  ("read_only_space", 0x026c1): (230, "TransitionArrayMap"),
  ("read_only_space", 0x026e9): (45, "ThinOneByteStringMap"),
  ("read_only_space", 0x02711): (238, "FeedbackVectorMap"),
  ("read_only_space", 0x02749): (131, "ArgumentsMarkerMap"),
  ("read_only_space", 0x027a9): (131, "ExceptionMap"),
  ("read_only_space", 0x02805): (131, "TerminationExceptionMap"),
  ("read_only_space", 0x0286d): (131, "OptimizedOutMap"),
  ("read_only_space", 0x028cd): (131, "StaleRegisterMap"),
  ("read_only_space", 0x0292d): (188, "ScriptContextTableMap"),
  ("read_only_space", 0x02955): (186, "ClosureFeedbackCellArrayMap"),
  ("read_only_space", 0x0297d): (237, "FeedbackMetadataArrayMap"),
  ("read_only_space", 0x029a5): (176, "ArrayListMap"),
  ("read_only_space", 0x029cd): (129, "BigIntMap"),
  ("read_only_space", 0x029f5): (187, "ObjectBoilerplateDescriptionMap"),
  ("read_only_space", 0x02a1d): (190, "BytecodeArrayMap"),
  ("read_only_space", 0x02a45): (234, "CodeDataContainerMap"),
  ("read_only_space", 0x02a6d): (235, "CoverageInfoMap"),
  ("read_only_space", 0x02a95): (191, "FixedDoubleArrayMap"),
  ("read_only_space", 0x02abd): (179, "GlobalDictionaryMap"),
  ("read_only_space", 0x02ae5): (157, "ManyClosuresCellMap"),
  ("read_only_space", 0x02b0d): (244, "MegaDomHandlerMap"),
  ("read_only_space", 0x02b35): (176, "ModuleInfoMap"),
  ("read_only_space", 0x02b5d): (180, "NameDictionaryMap"),
  ("read_only_space", 0x02b85): (157, "NoClosuresCellMap"),
  ("read_only_space", 0x02bad): (181, "NumberDictionaryMap"),
  ("read_only_space", 0x02bd5): (157, "OneClosureCellMap"),
  ("read_only_space", 0x02bfd): (182, "OrderedHashMapMap"),
  ("read_only_space", 0x02c25): (183, "OrderedHashSetMap"),
  ("read_only_space", 0x02c4d): (184, "OrderedNameDictionaryMap"),
  ("read_only_space", 0x02c75): (246, "PreparseDataMap"),
  ("read_only_space", 0x02c9d): (247, "PropertyArrayMap"),
  ("read_only_space", 0x02cc5): (153, "SideEffectCallHandlerInfoMap"),
  ("read_only_space", 0x02ced): (153, "SideEffectFreeCallHandlerInfoMap"),
  ("read_only_space", 0x02d15): (153, "NextCallSideEffectFreeCallHandlerInfoMap"),
  ("read_only_space", 0x02d3d): (185, "SimpleNumberDictionaryMap"),
  ("read_only_space", 0x02d65): (218, "SmallOrderedHashMapMap"),
  ("read_only_space", 0x02d8d): (219, "SmallOrderedHashSetMap"),
  ("read_only_space", 0x02db5): (220, "SmallOrderedNameDictionaryMap"),
  ("read_only_space", 0x02ddd): (225, "SourceTextModuleMap"),
  ("read_only_space", 0x02e05): (254, "SwissNameDictionaryMap"),
  ("read_only_space", 0x02e2d): (226, "SyntheticModuleMap"),
  ("read_only_space", 0x02e55): (206, "WasmCapiFunctionDataMap"),
  ("read_only_space", 0x02e7d): (207, "WasmExportedFunctionDataMap"),
  ("read_only_space", 0x02ea5): (208, "WasmJSFunctionDataMap"),
  ("read_only_space", 0x02ecd): (255, "WasmApiFunctionRefMap"),
  ("read_only_space", 0x02ef5): (209, "WasmTypeInfoMap"),
  ("read_only_space", 0x02f1d): (256, "WeakArrayListMap"),
  ("read_only_space", 0x02f45): (178, "EphemeronHashTableMap"),
  ("read_only_space", 0x02f6d): (236, "EmbedderDataArrayMap"),
  ("read_only_space", 0x02f95): (257, "WeakCellMap"),
  ("read_only_space", 0x02fbd): (32, "StringMap"),
  ("read_only_space", 0x02fe5): (41, "ConsOneByteStringMap"),
  ("read_only_space", 0x0300d): (33, "ConsStringMap"),
  ("read_only_space", 0x03035): (37, "ThinStringMap"),
  ("read_only_space", 0x0305d): (35, "SlicedStringMap"),
  ("read_only_space", 0x03085): (43, "SlicedOneByteStringMap"),
  ("read_only_space", 0x030ad): (34, "ExternalStringMap"),
  ("read_only_space", 0x030d5): (42, "ExternalOneByteStringMap"),
  ("read_only_space", 0x030fd): (50, "UncachedExternalStringMap"),
  ("read_only_space", 0x03125): (0, "InternalizedStringMap"),
  ("read_only_space", 0x0314d): (2, "ExternalInternalizedStringMap"),
  ("read_only_space", 0x03175): (10, "ExternalOneByteInternalizedStringMap"),
  ("read_only_space", 0x0319d): (18, "UncachedExternalInternalizedStringMap"),
  ("read_only_space", 0x031c5): (26, "UncachedExternalOneByteInternalizedStringMap"),
  ("read_only_space", 0x031ed): (58, "UncachedExternalOneByteStringMap"),
  ("read_only_space", 0x03215): (104, "SharedOneByteStringMap"),
  ("read_only_space", 0x0323d): (96, "SharedStringMap"),
  ("read_only_space", 0x03265): (131, "SelfReferenceMarkerMap"),
  ("read_only_space", 0x0328d): (131, "BasicBlockCountersMarkerMap"),
  ("read_only_space", 0x032d1): (147, "ArrayBoilerplateDescriptionMap"),
  ("read_only_space", 0x033d1): (159, "InterceptorInfoMap"),
  ("read_only_space", 0x05c65): (132, "PromiseFulfillReactionJobTaskMap"),
  ("read_only_space", 0x05c8d): (133, "PromiseRejectReactionJobTaskMap"),
  ("read_only_space", 0x05cb5): (134, "CallableTaskMap"),
  ("read_only_space", 0x05cdd): (135, "CallbackTaskMap"),
  ("read_only_space", 0x05d05): (136, "PromiseResolveThenableJobTaskMap"),
  ("read_only_space", 0x05d2d): (139, "FunctionTemplateInfoMap"),
  ("read_only_space", 0x05d55): (140, "ObjectTemplateInfoMap"),
  ("read_only_space", 0x05d7d): (141, "AccessCheckInfoMap"),
  ("read_only_space", 0x05da5): (142, "AccessorInfoMap"),
  ("read_only_space", 0x05dcd): (143, "AccessorPairMap"),
  ("read_only_space", 0x05df5): (144, "AliasedArgumentsEntryMap"),
  ("read_only_space", 0x05e1d): (145, "AllocationMementoMap"),
  ("read_only_space", 0x05e45): (148, "AsmWasmDataMap"),
  ("read_only_space", 0x05e6d): (149, "AsyncGeneratorRequestMap"),
  ("read_only_space", 0x05e95): (150, "BreakPointMap"),
  ("read_only_space", 0x05ebd): (151, "BreakPointInfoMap"),
  ("read_only_space", 0x05ee5): (152, "CachedTemplateObjectMap"),
  ("read_only_space", 0x05f0d): (154, "ClassPositionsMap"),
  ("read_only_space", 0x05f35): (155, "DebugInfoMap"),
  ("read_only_space", 0x05f5d): (158, "FunctionTemplateRareDataMap"),
  ("read_only_space", 0x05f85): (160, "InterpreterDataMap"),
  ("read_only_space", 0x05fad): (161, "ModuleRequestMap"),
  ("read_only_space", 0x05fd5): (162, "PromiseCapabilityMap"),
  ("read_only_space", 0x05ffd): (163, "PromiseReactionMap"),
  ("read_only_space", 0x06025): (164, "PropertyDescriptorObjectMap"),
  ("read_only_space", 0x0604d): (165, "PrototypeInfoMap"),
  ("read_only_space", 0x06075): (166, "RegExpBoilerplateDescriptionMap"),
  ("read_only_space", 0x0609d): (167, "ScriptMap"),
  ("read_only_space", 0x060c5): (168, "ScriptOrModuleMap"),
  ("read_only_space", 0x060ed): (169, "SourceTextModuleInfoEntryMap"),
  ("read_only_space", 0x06115): (170, "StackFrameInfoMap"),
  ("read_only_space", 0x0613d): (171, "TemplateObjectDescriptionMap"),
  ("read_only_space", 0x06165): (172, "Tuple2Map"),
  ("read_only_space", 0x0618d): (173, "WasmContinuationObjectMap"),
  ("read_only_space", 0x061b5): (174, "WasmExceptionTagMap"),
  ("read_only_space", 0x061dd): (175, "WasmIndirectFunctionTableMap"),
  ("read_only_space", 0x06205): (193, "SloppyArgumentsElementsMap"),
  ("read_only_space", 0x0622d): (223, "DescriptorArrayMap"),
  ("read_only_space", 0x06255): (228, "UncompiledDataWithoutPreparseDataMap"),
  ("read_only_space", 0x0627d): (227, "UncompiledDataWithPreparseDataMap"),
  ("read_only_space", 0x062a5): (245, "OnHeapBasicBlockProfilerDataMap"),
  ("read_only_space", 0x062cd): (210, "TurbofanBitsetTypeMap"),
  ("read_only_space", 0x062f5): (214, "TurbofanUnionTypeMap"),
  ("read_only_space", 0x0631d): (213, "TurbofanRangeTypeMap"),
  ("read_only_space", 0x06345): (211, "TurbofanHeapConstantTypeMap"),
  ("read_only_space", 0x0636d): (212, "TurbofanOtherNumberConstantTypeMap"),
  ("read_only_space", 0x06395): (241, "InternalClassMap"),
  ("read_only_space", 0x063bd): (252, "SmiPairMap"),
  ("read_only_space", 0x063e5): (251, "SmiBoxMap"),
  ("read_only_space", 0x0640d): (215, "ExportedSubClassBaseMap"),
  ("read_only_space", 0x06435): (216, "ExportedSubClassMap"),
  ("read_only_space", 0x0645d): (221, "AbstractInternalClassSubclass1Map"),
  ("read_only_space", 0x06485): (222, "AbstractInternalClassSubclass2Map"),
  ("read_only_space", 0x064ad): (192, "InternalClassWithSmiElementsMap"),
  ("read_only_space", 0x064d5): (242, "InternalClassWithStructElementsMap"),
  ("read_only_space", 0x064fd): (217, "ExportedSubClass2Map"),
  ("read_only_space", 0x06525): (253, "SortStateMap"),
  ("read_only_space", 0x0654d): (231, "CallRefDataMap"),
  ("read_only_space", 0x06575): (146, "AllocationSiteWithWeakNextMap"),
  ("read_only_space", 0x0659d): (146, "AllocationSiteWithoutWeakNextMap"),
  ("read_only_space", 0x065c5): (137, "LoadHandler1Map"),
  ("read_only_space", 0x065ed): (137, "LoadHandler2Map"),
  ("read_only_space", 0x06615): (137, "LoadHandler3Map"),
  ("read_only_space", 0x0663d): (138, "StoreHandler0Map"),
  ("read_only_space", 0x06665): (138, "StoreHandler1Map"),
  ("read_only_space", 0x0668d): (138, "StoreHandler2Map"),
  ("read_only_space", 0x066b5): (138, "StoreHandler3Map"),
  ("map_space", 0x02119): (1057, "ExternalMap"),
  ("map_space", 0x02141): (2114, "JSMessageObjectMap"),
}

# List of known V8 objects.
KNOWN_OBJECTS = {
  ("read_only_space", 0x021b9): "EmptyWeakFixedArray",
  ("read_only_space", 0x021c1): "EmptyDescriptorArray",
  ("read_only_space", 0x021f9): "EmptyEnumCache",
  ("read_only_space", 0x0222d): "EmptyFixedArray",
  ("read_only_space", 0x02235): "NullValue",
  ("read_only_space", 0x0233d): "UninitializedValue",
  ("read_only_space", 0x023b5): "UndefinedValue",
  ("read_only_space", 0x023f9): "NanValue",
  ("read_only_space", 0x0242d): "TheHoleValue",
  ("read_only_space", 0x02459): "HoleNanValue",
  ("read_only_space", 0x0248d): "TrueValue",
  ("read_only_space", 0x024cd): "FalseValue",
  ("read_only_space", 0x024fd): "empty_string",
  ("read_only_space", 0x02739): "EmptyScopeInfo",
  ("read_only_space", 0x02771): "ArgumentsMarker",
  ("read_only_space", 0x027d1): "Exception",
  ("read_only_space", 0x0282d): "TerminationException",
  ("read_only_space", 0x02895): "OptimizedOut",
  ("read_only_space", 0x028f5): "StaleRegister",
  ("read_only_space", 0x032b5): "EmptyPropertyArray",
  ("read_only_space", 0x032bd): "EmptyByteArray",
  ("read_only_space", 0x032c5): "EmptyObjectBoilerplateDescription",
  ("read_only_space", 0x032f9): "EmptyArrayBoilerplateDescription",
  ("read_only_space", 0x03305): "EmptyClosureFeedbackCellArray",
  ("read_only_space", 0x0330d): "EmptySlowElementDictionary",
  ("read_only_space", 0x03331): "EmptyOrderedHashMap",
  ("read_only_space", 0x03345): "EmptyOrderedHashSet",
  ("read_only_space", 0x03359): "EmptyFeedbackMetadata",
  ("read_only_space", 0x03365): "EmptyPropertyDictionary",
  ("read_only_space", 0x0338d): "EmptyOrderedPropertyDictionary",
  ("read_only_space", 0x033a5): "EmptySwissPropertyDictionary",
  ("read_only_space", 0x033f9): "NoOpInterceptorInfo",
  ("read_only_space", 0x03421): "EmptyWeakArrayList",
  ("read_only_space", 0x0342d): "InfinityValue",
  ("read_only_space", 0x03439): "MinusZeroValue",
  ("read_only_space", 0x03445): "MinusInfinityValue",
  ("read_only_space", 0x03451): "SelfReferenceMarker",
  ("read_only_space", 0x03491): "BasicBlockCountersMarker",
  ("read_only_space", 0x034d5): "OffHeapTrampolineRelocationInfo",
  ("read_only_space", 0x034e1): "TrampolineTrivialCodeDataContainer",
  ("read_only_space", 0x034ed): "TrampolinePromiseRejectionCodeDataContainer",
  ("read_only_space", 0x034f9): "GlobalThisBindingScopeInfo",
  ("read_only_space", 0x03529): "EmptyFunctionScopeInfo",
  ("read_only_space", 0x0354d): "NativeScopeInfo",
  ("read_only_space", 0x03565): "HashSeed",
  ("old_space", 0x04211): "ArgumentsIteratorAccessor",
  ("old_space", 0x04255): "ArrayLengthAccessor",
  ("old_space", 0x04299): "BoundFunctionLengthAccessor",
  ("old_space", 0x042dd): "BoundFunctionNameAccessor",
  ("old_space", 0x04321): "ErrorStackAccessor",
  ("old_space", 0x04365): "FunctionArgumentsAccessor",
  ("old_space", 0x043a9): "FunctionCallerAccessor",
  ("old_space", 0x043ed): "FunctionNameAccessor",
  ("old_space", 0x04431): "FunctionLengthAccessor",
  ("old_space", 0x04475): "FunctionPrototypeAccessor",
  ("old_space", 0x044b9): "StringLengthAccessor",
  ("old_space", 0x044fd): "InvalidPrototypeValidityCell",
  ("old_space", 0x04505): "EmptyScript",
  ("old_space", 0x04545): "ManyClosuresCell",
  ("old_space", 0x04551): "ArrayConstructorProtector",
  ("old_space", 0x04565): "NoElementsProtector",
  ("old_space", 0x04579): "MegaDOMProtector",
  ("old_space", 0x0458d): "IsConcatSpreadableProtector",
  ("old_space", 0x045a1): "ArraySpeciesProtector",
  ("old_space", 0x045b5): "TypedArraySpeciesProtector",
  ("old_space", 0x045c9): "PromiseSpeciesProtector",
  ("old_space", 0x045dd): "RegExpSpeciesProtector",
  ("old_space", 0x045f1): "StringLengthProtector",
  ("old_space", 0x04605): "ArrayIteratorProtector",
  ("old_space", 0x04619): "ArrayBufferDetachingProtector",
  ("old_space", 0x0462d): "PromiseHookProtector",
  ("old_space", 0x04641): "PromiseResolveProtector",
  ("old_space", 0x04655): "MapIteratorProtector",
  ("old_space", 0x04669): "PromiseThenProtector",
  ("old_space", 0x0467d): "SetIteratorProtector",
  ("old_space", 0x04691): "StringIteratorProtector",
  ("old_space", 0x046a5): "SingleCharacterStringCache",
  ("old_space", 0x04aad): "StringSplitCache",
  ("old_space", 0x04eb5): "RegExpMultipleCache",
  ("old_space", 0x052bd): "BuiltinsConstantsTable",
  ("old_space", 0x056e5): "AsyncFunctionAwaitRejectSharedFun",
  ("old_space", 0x05709): "AsyncFunctionAwaitResolveSharedFun",
  ("old_space", 0x0572d): "AsyncGeneratorAwaitRejectSharedFun",
  ("old_space", 0x05751): "AsyncGeneratorAwaitResolveSharedFun",
  ("old_space", 0x05775): "AsyncGeneratorYieldResolveSharedFun",
  ("old_space", 0x05799): "AsyncGeneratorReturnResolveSharedFun",
  ("old_space", 0x057bd): "AsyncGeneratorReturnClosedRejectSharedFun",
  ("old_space", 0x057e1): "AsyncGeneratorReturnClosedResolveSharedFun",
  ("old_space", 0x05805): "AsyncIteratorValueUnwrapSharedFun",
  ("old_space", 0x05829): "PromiseAllResolveElementSharedFun",
  ("old_space", 0x0584d): "PromiseAllSettledResolveElementSharedFun",
  ("old_space", 0x05871): "PromiseAllSettledRejectElementSharedFun",
  ("old_space", 0x05895): "PromiseAnyRejectElementSharedFun",
  ("old_space", 0x058b9): "PromiseCapabilityDefaultRejectSharedFun",
  ("old_space", 0x058dd): "PromiseCapabilityDefaultResolveSharedFun",
  ("old_space", 0x05901): "PromiseCatchFinallySharedFun",
  ("old_space", 0x05925): "PromiseGetCapabilitiesExecutorSharedFun",
  ("old_space", 0x05949): "PromiseThenFinallySharedFun",
  ("old_space", 0x0596d): "PromiseThrowerFinallySharedFun",
  ("old_space", 0x05991): "PromiseValueThunkFinallySharedFun",
  ("old_space", 0x059b5): "ProxyRevokeSharedFun",
}

# Lower 32 bits of first page addresses for various heap spaces.
HEAP_FIRST_PAGES = {
  0x080c0000: "old_space",
  0x08100000: "map_space",
  0x08000000: "read_only_space",
}

# List of known V8 Frame Markers.
FRAME_MARKERS = (
  "ENTRY",
  "CONSTRUCT_ENTRY",
  "EXIT",
  "WASM",
  "WASM_TO_JS",
  "JS_TO_WASM",
  "RETURN_PROMISE_ON_SUSPEND",
  "WASM_DEBUG_BREAK",
  "C_WASM_ENTRY",
  "WASM_EXIT",
  "WASM_COMPILE_LAZY",
  "INTERPRETED",
  "BASELINE",
  "OPTIMIZED",
  "STUB",
  "BUILTIN_CONTINUATION",
  "JAVA_SCRIPT_BUILTIN_CONTINUATION",
  "JAVA_SCRIPT_BUILTIN_CONTINUATION_WITH_CATCH",
  "INTERNAL",
  "CONSTRUCT",
  "BUILTIN",
  "BUILTIN_EXIT",
  "NATIVE",
)

# This set of constants is generated from a shipping build.
