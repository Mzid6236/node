#include "crypto/crypto_sig.h"
#include "crypto/crypto_ec.h"
#include "crypto/crypto_keys.h"
#include "crypto/crypto_util.h"
#include "allocated_buffer-inl.h"
#include "async_wrap-inl.h"
#include "base_object-inl.h"
#include "env-inl.h"
#include "memory_tracker-inl.h"
#include "threadpoolwork-inl.h"
#include "v8.h"

namespace node {

using v8::FunctionCallbackInfo;
using v8::FunctionTemplate;
using v8::HandleScope;
using v8::Int32;
using v8::Just;
using v8::Local;
using v8::Maybe;
using v8::Nothing;
using v8::Object;
using v8::Uint32;
using v8::Value;

namespace crypto {
namespace {
bool ValidateDSAParameters(EVP_PKEY* key) {
  /* Validate DSA2 parameters from FIPS 186-4 */
#if OPENSSL_VERSION_MAJOR >= 3
  if (EVP_default_properties_is_fips_enabled(nullptr) &&
      EVP_PKEY_DSA == EVP_PKEY_base_id(key)) {
#else
  if (FIPS_mode() && EVP_PKEY_DSA == EVP_PKEY_base_id(key)) {
#endif
    const DSA* dsa = EVP_PKEY_get0_DSA(key);
    const BIGNUM* p;
    DSA_get0_pqg(dsa, &p, nullptr, nullptr);
    size_t L = BN_num_bits(p);
    const BIGNUM* q;
    DSA_get0_pqg(dsa, nullptr, &q, nullptr);
    size_t N = BN_num_bits(q);

    return (L == 1024 && N == 160) ||
           (L == 2048 && N == 224) ||
           (L == 2048 && N == 256) ||
           (L == 3072 && N == 256);
  }

  return true;
}

bool ApplyRSAOptions(const ManagedEVPPKey& pkey,
                     EVP_PKEY_CTX* pkctx,
                     int padding,
                     const Maybe<int>& salt_len) {
  if (EVP_PKEY_id(pkey.get()) == EVP_PKEY_RSA ||
      EVP_PKEY_id(pkey.get()) == EVP_PKEY_RSA2 ||
      EVP_PKEY_id(pkey.get()) == EVP_PKEY_RSA_PSS) {
    if (EVP_PKEY_CTX_set_rsa_padding(pkctx, padding) <= 0)
      return false;
    if (padding == RSA_PKCS1_PSS_PADDING && salt_len.IsJust()) {
      if (EVP_PKEY_CTX_set_rsa_pss_saltlen(pkctx, salt_len.FromJust()) <= 0)
        return false;
    }
  }

  return true;
}

AllocatedBuffer Node_SignFinal(Environment* env,
                               EVPMDPointer&& mdctx,
                               const ManagedEVPPKey& pkey,
                               int padding,
                               Maybe<int> pss_salt_len) {
  unsigned char m[EVP_MAX_MD_SIZE];
  unsigned int m_len;

  if (!EVP_DigestFinal_ex(mdctx.get(), m, &m_len))
    return AllocatedBuffer();

  int signed_sig_len = EVP_PKEY_size(pkey.get());
  CHECK_GE(signed_sig_len, 0);
  size_t sig_len = static_cast<size_t>(signed_sig_len);
  AllocatedBuffer sig = AllocatedBuffer::AllocateManaged(env, sig_len);
  unsigned char* ptr = reinterpret_cast<unsigned char*>(sig.data());

  EVPKeyCtxPointer pkctx(EVP_PKEY_CTX_new(pkey.get(), nullptr));
  if (pkctx &&
      EVP_PKEY_sign_init(pkctx.get()) &&
      ApplyRSAOptions(pkey, pkctx.get(), padding, pss_salt_len) &&
      EVP_PKEY_CTX_set_signature_md(pkctx.get(), EVP_MD_CTX_md(mdctx.get())) &&
      EVP_PKEY_sign(pkctx.get(), ptr, &sig_len, m, m_len)) {
    sig.Resize(sig_len);
    return sig;
  }

  return AllocatedBuffer();
}

int GetDefaultSignPadding(const ManagedEVPPKey& m_pkey) {
  return EVP_PKEY_id(m_pkey.get()) == EVP_PKEY_RSA_PSS ? RSA_PKCS1_PSS_PADDING :
                                                         RSA_PKCS1_PADDING;
}

unsigned int GetBytesOfRS(const ManagedEVPPKey& pkey) {
  int bits, base_id = EVP_PKEY_base_id(pkey.get());

  if (base_id == EVP_PKEY_DSA) {
    const DSA* dsa_key = EVP_PKEY_get0_DSA(pkey.get());
    // Both r and s are computed mod q, so their width is limited by that of q.
    bits = BN_num_bits(DSA_get0_q(dsa_key));
  } else if (base_id == EVP_PKEY_EC) {
    const EC_KEY* ec_key = EVP_PKEY_get0_EC_KEY(pkey.get());
    const EC_GROUP* ec_group = EC_KEY_get0_group(ec_key);
    bits = EC_GROUP_order_bits(ec_group);
  } else {
    return kNoDsaSignature;
  }

  return (bits + 7) / 8;
}

bool ExtractP1363(
    const unsigned char* sig_data,
    unsigned char* out,
    size_t len,
    size_t n) {
  ECDSASigPointer asn1_sig(d2i_ECDSA_SIG(nullptr, &sig_data, len));
  if (!asn1_sig)
    return false;

  const BIGNUM* pr = ECDSA_SIG_get0_r(asn1_sig.get());
  const BIGNUM* ps = ECDSA_SIG_get0_s(asn1_sig.get());

  return BN_bn2binpad(pr, out, n) > 0 && BN_bn2binpad(ps, out + n, n) > 0;
}

// Returns the maximum size of each of the integers (r, s) of the DSA signature.
AllocatedBuffer ConvertSignatureToP1363(Environment* env,
                                        const ManagedEVPPKey& pkey,
                                        AllocatedBuffer&& signature) {
  unsigned int n = GetBytesOfRS(pkey);
  if (n == kNoDsaSignature)
    return std::move(signature);

  const unsigned char* sig_data =
      reinterpret_cast<unsigned char*>(signature.data());

  AllocatedBuffer buf = AllocatedBuffer::AllocateManaged(env, 2 * n);
  unsigned char* data = reinterpret_cast<unsigned char*>(buf.data());

  if (!ExtractP1363(sig_data, data, signature.size(), n))
    return std::move(signature);

  return buf;
}

// Returns the maximum size of each of the integers (r, s) of the DSA signature.
ByteSource ConvertSignatureToP1363(
    Environment* env,
    const ManagedEVPPKey& pkey,
    const ByteSource& signature) {
  unsigned int n = GetBytesOfRS(pkey);
  if (n == kNoDsaSignature)
    return ByteSource();

  const unsigned char* sig_data =
      reinterpret_cast<const unsigned char*>(signature.get());

  char* outdata = MallocOpenSSL<char>(n * 2);
  memset(outdata, 0, n * 2);
  ByteSource out = ByteSource::Allocated(outdata, n * 2);
  unsigned char* ptr = reinterpret_cast<unsigned char*>(outdata);

  if (!ExtractP1363(sig_data, ptr, signature.size(), n))
    return ByteSource();

  return out;
}

ByteSource ConvertSignatureToDER(
      const ManagedEVPPKey& pkey,
      ByteSource&& out) {
  unsigned int n = GetBytesOfRS(pkey);
  if (n == kNoDsaSignature)
    return std::move(out);

  const unsigned char* sig_data =
      reinterpret_cast<const unsigned char*>(out.get());

  if (out.size() != 2 * n)
    return ByteSource();

  ECDSASigPointer asn1_sig(ECDSA_SIG_new());
  CHECK(asn1_sig);
  BIGNUM* r = BN_new();
  CHECK_NOT_NULL(r);
  BIGNUM* s = BN_new();
  CHECK_NOT_NULL(s);
  CHECK_EQ(r, BN_bin2bn(sig_data, n, r));
  CHECK_EQ(s, BN_bin2bn(sig_data + n, n, s));
  CHECK_EQ(1, ECDSA_SIG_set0(asn1_sig.get(), r, s));

  unsigned char* data = nullptr;
  int len = i2d_ECDSA_SIG(asn1_sig.get(), &data);

  if (len <= 0)
    return ByteSource();

  CHECK_NOT_NULL(data);

  return ByteSource::Allocated(reinterpret_cast<char*>(data), len);
}

void CheckThrow(Environment* env, SignBase::Error error) {
  HandleScope scope(env->isolate());

  switch (error) {
    case SignBase::Error::kSignUnknownDigest:
      return THROW_ERR_CRYPTO_INVALID_DIGEST(env);

    case SignBase::Error::kSignNotInitialised:
      return THROW_ERR_CRYPTO_INVALID_STATE(env, "Not initialised");

    case SignBase::Error::kSignMalformedSignature:
      return THROW_ERR_CRYPTO_OPERATION_FAILED(env, "Malformed signature");

    case SignBase::Error::kSignInit:
    case SignBase::Error::kSignUpdate:
    case SignBase::Error::kSignPrivateKey:
    case SignBase::Error::kSignPublicKey:
      {
        unsigned long err = ERR_get_error();  // NOLINT(runtime/int)
        if (err)
          return ThrowCryptoError(env, err);
        switch (error) {
          case SignBase::Error::kSignInit:
            return THROW_ERR_CRYPTO_OPERATION_FAILED(env,
                "EVP_SignInit_ex failed");
          case SignBase::Error::kSignUpdate:
            return THROW_ERR_CRYPTO_OPERATION_FAILED(env,
                "EVP_SignUpdate failed");
          case SignBase::Error::kSignPrivateKey:
            return THROW_ERR_CRYPTO_OPERATION_FAILED(env,
                "PEM_read_bio_PrivateKey failed");
          case SignBase::Error::kSignPublicKey:
            return THROW_ERR_CRYPTO_OPERATION_FAILED(env,
                "PEM_read_bio_PUBKEY failed");
          default:
            ABORT();
        }
      }

    case SignBase::Error::kSignOk:
      return;
  }
}

bool IsOneShot(const ManagedEVPPKey& key) {
  switch (EVP_PKEY_id(key.get())) {
    case EVP_PKEY_ED25519:
    case EVP_PKEY_ED448:
      return true;
    default:
      return false;
  }
}

bool UseP1363Encoding(const ManagedEVPPKey& key,
                      const DSASigEnc& dsa_encoding) {
  switch (EVP_PKEY_id(key.get())) {
    case EVP_PKEY_EC:
    case EVP_PKEY_DSA:
      return dsa_encoding == kSigEncP1363;
    default:
      return false;
  }
}
}  // namespace

SignBase::Error SignBase::Init(const char* sign_type) {
  CHECK_NULL(mdctx_);
  // Historically, "dss1" and "DSS1" were DSA aliases for SHA-1
  // exposed through the public API.
  if (strcmp(sign_type, "dss1") == 0 ||
      strcmp(sign_type, "DSS1") == 0) {
    sign_type = "SHA1";
  }
  const EVP_MD* md = EVP_get_digestbyname(sign_type);
  if (md == nullptr)
    return kSignUnknownDigest;

  mdctx_.reset(EVP_MD_CTX_new());
  if (!mdctx_ || !EVP_DigestInit_ex(mdctx_.get(), md, nullptr)) {
    mdctx_.reset();
    return kSignInit;
  }

  return kSignOk;
}

SignBase::Error SignBase::Update(const char* data, size_t len) {
  if (mdctx_ == nullptr)
    return kSignNotInitialised;
  if (!EVP_DigestUpdate(mdctx_.get(), data, len))
    return kSignUpdate;
  return kSignOk;
}

SignBase::SignBase(Environment* env, Local<Object> wrap)
    : BaseObject(env, wrap) {}

void SignBase::MemoryInfo(MemoryTracker* tracker) const {
  tracker->TrackFieldWithSize("mdctx", mdctx_ ? kSizeOf_EVP_MD_CTX : 0);
}

Sign::Sign(Environment* env, Local<Object> wrap) : SignBase(env, wrap) {
  MakeWeak();
}

void Sign::Initialize(Environment* env, Local<Object> target) {
  Local<FunctionTemplate> t = env->NewFunctionTemplate(New);

  t->InstanceTemplate()->SetInternalFieldCount(
      SignBase::kInternalFieldCount);
  t->Inherit(BaseObject::GetConstructorTemplate(env));

  env->SetProtoMethod(t, "init", SignInit);
  env->SetProtoMethod(t, "update", SignUpdate);
  env->SetProtoMethod(t, "sign", SignFinal);

  env->SetConstructorFunction(target, "Sign", t);

  SignJob::Initialize(env, target);

  constexpr int kSignJobModeSign = SignConfiguration::kSign;
  constexpr int kSignJobModeVerify = SignConfiguration::kVerify;

  NODE_DEFINE_CONSTANT(target, kSignJobModeSign);
  NODE_DEFINE_CONSTANT(target, kSignJobModeVerify);
  NODE_DEFINE_CONSTANT(target, kSigEncDER);
  NODE_DEFINE_CONSTANT(target, kSigEncP1363);
  NODE_DEFINE_CONSTANT(target, RSA_PKCS1_PSS_PADDING);
}

void Sign::RegisterExternalReferences(ExternalReferenceRegistry* registry) {
  registry->Register(New);
  registry->Register(SignInit);
  registry->Register(SignUpdate);
  registry->Register(SignFinal);
  SignJob::RegisterExternalReferences(registry);
}

void Sign::New(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  new Sign(env, args.This());
}

void Sign::SignInit(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  Sign* sign;
  ASSIGN_OR_RETURN_UNWRAP(&sign, args.Holder());

  const node::Utf8Value sign_type(args.GetIsolate(), args[0]);
  crypto::CheckThrow(env, sign->Init(*sign_type));
}

void Sign::SignUpdate(const FunctionCallbackInfo<Value>& args) {
  Decode<Sign>(args, [](Sign* sign, const FunctionCallbackInfo<Value>& args,
                        const char* data, size_t size) {
    Environment* env = Environment::GetCurrent(args);
    if (UNLIKELY(size > INT_MAX))
      return THROW_ERR_OUT_OF_RANGE(env, "data is too long");
    Error err = sign->Update(data, size);
    crypto::CheckThrow(sign->env(), err);
  });
}

Sign::SignResult Sign::SignFinal(
    const ManagedEVPPKey& pkey,
    int padding,
    const Maybe<int>& salt_len,
    DSASigEnc dsa_sig_enc) {
  if (!mdctx_)
    return SignResult(kSignNotInitialised);

  EVPMDPointer mdctx = std::move(mdctx_);

  if (!ValidateDSAParameters(pkey.get()))
    return SignResult(kSignPrivateKey);

  AllocatedBuffer buffer =
      Node_SignFinal(env(), std::move(mdctx), pkey, padding, salt_len);
  Error error = buffer.data() == nullptr ? kSignPrivateKey : kSignOk;
  if (error == kSignOk && dsa_sig_enc == kSigEncP1363) {
    buffer = ConvertSignatureToP1363(env(), pkey, std::move(buffer));
    CHECK_NOT_NULL(buffer.data());
  }
  return SignResult(error, std::move(buffer));
}

void Sign::SignFinal(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  Sign* sign;
  ASSIGN_OR_RETURN_UNWRAP(&sign, args.Holder());

  ClearErrorOnReturn clear_error_on_return;

  unsigned int offset = 0;
  ManagedEVPPKey key = ManagedEVPPKey::GetPrivateKeyFromJs(args, &offset, true);
  if (!key)
    return;

  int padding = GetDefaultSignPadding(key);
  if (!args[offset]->IsUndefined()) {
    CHECK(args[offset]->IsInt32());
    padding = args[offset].As<Int32>()->Value();
  }

  Maybe<int> salt_len = Nothing<int>();
  if (!args[offset + 1]->IsUndefined()) {
    CHECK(args[offset + 1]->IsInt32());
    salt_len = Just<int>(args[offset + 1].As<Int32>()->Value());
  }

  CHECK(args[offset + 2]->IsInt32());
  DSASigEnc dsa_sig_enc =
      static_cast<DSASigEnc>(args[offset + 2].As<Int32>()->Value());

  SignResult ret = sign->SignFinal(
      key,
      padding,
      salt_len,
      dsa_sig_enc);

  if (ret.error != kSignOk)
    return crypto::CheckThrow(env, ret.error);

  args.GetReturnValue().Set(ret.signature.ToBuffer().FromMaybe(Local<Value>()));
}

Verify::Verify(Environment* env, Local<Object> wrap)
  : SignBase(env, wrap) {
  MakeWeak();
}

void Verify::Initialize(Environment* env, Local<Object> target) {
  Local<FunctionTemplate> t = env->NewFunctionTemplate(New);

  t->InstanceTemplate()->SetInternalFieldCount(
      SignBase::kInternalFieldCount);
  t->Inherit(BaseObject::GetConstructorTemplate(env));

  env->SetProtoMethod(t, "init", VerifyInit);
  env->SetProtoMethod(t, "update", VerifyUpdate);
  env->SetProtoMethod(t, "verify", VerifyFinal);

  env->SetConstructorFunction(target, "Verify", t);
}

void Verify::RegisterExternalReferences(ExternalReferenceRegistry* registry) {
  registry->Register(New);
  registry->Register(VerifyInit);
  registry->Register(VerifyUpdate);
  registry->Register(VerifyFinal);
}

void Verify::New(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  new Verify(env, args.This());
}

void Verify::VerifyInit(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  Verify* verify;
  ASSIGN_OR_RETURN_UNWRAP(&verify, args.Holder());

  const node::Utf8Value verify_type(args.GetIsolate(), args[0]);
  crypto::CheckThrow(env, verify->Init(*verify_type));
}

void Verify::VerifyUpdate(const FunctionCallbackInfo<Value>& args) {
  Decode<Verify>(args, [](Verify* verify,
                          const FunctionCallbackInfo<Value>& args,
                          const char* data, size_t size) {
    Environment* env = Environment::GetCurrent(args);
    if (UNLIKELY(size > INT_MAX))
      return THROW_ERR_OUT_OF_RANGE(env, "data is too long");
    Error err = verify->Update(data, size);
    crypto::CheckThrow(verify->env(), err);
  });
}

SignBase::Error Verify::VerifyFinal(const ManagedEVPPKey& pkey,
                                    const ByteSource& sig,
                                    int padding,
                                    const Maybe<int>& saltlen,
                                    bool* verify_result) {
  if (!mdctx_)
    return kSignNotInitialised;

  unsigned char m[EVP_MAX_MD_SIZE];
  unsigned int m_len;
  *verify_result = false;
  EVPMDPointer mdctx = std::move(mdctx_);

  if (!EVP_DigestFinal_ex(mdctx.get(), m, &m_len))
    return kSignPublicKey;

  EVPKeyCtxPointer pkctx(EVP_PKEY_CTX_new(pkey.get(), nullptr));
  if (pkctx &&
      EVP_PKEY_verify_init(pkctx.get()) > 0 &&
      ApplyRSAOptions(pkey, pkctx.get(), padding, saltlen) &&
      EVP_PKEY_CTX_set_signature_md(pkctx.get(),
                                    EVP_MD_CTX_md(mdctx.get())) > 0) {
    const unsigned char* s = reinterpret_cast<const unsigned char*>(sig.get());
    const int r = EVP_PKEY_verify(pkctx.get(), s, sig.size(), m, m_len);
    *verify_result = r == 1;
  }

  return kSignOk;
}

void Verify::VerifyFinal(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  ClearErrorOnReturn clear_error_on_return;

  Verify* verify;
  ASSIGN_OR_RETURN_UNWRAP(&verify, args.Holder());

  unsigned int offset = 0;
  ManagedEVPPKey pkey =
      ManagedEVPPKey::GetPublicOrPrivateKeyFromJs(args, &offset);
  if (!pkey)
    return;

  ArrayBufferOrViewContents<char> hbuf(args[offset]);
  if (UNLIKELY(!hbuf.CheckSizeInt32()))
    return THROW_ERR_OUT_OF_RANGE(env, "buffer is too big");

  int padding = GetDefaultSignPadding(pkey);
  if (!args[offset + 1]->IsUndefined()) {
    CHECK(args[offset + 1]->IsInt32());
    padding = args[offset + 1].As<Int32>()->Value();
  }

  Maybe<int> salt_len = Nothing<int>();
  if (!args[offset + 2]->IsUndefined()) {
    CHECK(args[offset + 2]->IsInt32());
    salt_len = Just<int>(args[offset + 2].As<Int32>()->Value());
  }

  CHECK(args[offset + 3]->IsInt32());
  DSASigEnc dsa_sig_enc =
      static_cast<DSASigEnc>(args[offset + 3].As<Int32>()->Value());

  ByteSource signature = hbuf.ToByteSource();
  if (dsa_sig_enc == kSigEncP1363) {
    signature = ConvertSignatureToDER(pkey, hbuf.ToByteSource());
    if (signature.get() == nullptr)
      return crypto::CheckThrow(env, Error::kSignMalformedSignature);
  }

  bool verify_result;
  Error err = verify->VerifyFinal(pkey, signature, padding,
                                  salt_len, &verify_result);
  if (err != kSignOk)
    return crypto::CheckThrow(env, err);
  args.GetReturnValue().Set(verify_result);
}

SignConfiguration::SignConfiguration(SignConfiguration&& other) noexcept
    : job_mode(other.job_mode),
      mode(other.mode),
      key(std::move(other.key)),
      data(std::move(other.data)),
      signature(std::move(other.signature)),
      digest(other.digest),
      flags(other.flags),
      padding(other.padding),
      salt_length(other.salt_length),
      dsa_encoding(other.dsa_encoding) {}

SignConfiguration& SignConfiguration::operator=(
    SignConfiguration&& other) noexcept {
  if (&other == this) return *this;
  this->~SignConfiguration();
  return *new (this) SignConfiguration(std::move(other));
}

void SignConfiguration::MemoryInfo(MemoryTracker* tracker) const {
  tracker->TrackField("key", key);
  if (job_mode == kCryptoJobAsync) {
    tracker->TrackFieldWithSize("data", data.size());
    tracker->TrackFieldWithSize("signature", signature.size());
  }
}

Maybe<bool> SignTraits::AdditionalConfig(
    CryptoJobMode mode,
    const FunctionCallbackInfo<Value>& args,
    unsigned int offset,
    SignConfiguration* params) {
  ClearErrorOnReturn clear_error_on_return;
  Environment* env = Environment::GetCurrent(args);

  params->job_mode = mode;

  CHECK(args[offset]->IsUint32());  // Sign Mode

  params->mode =
      static_cast<SignConfiguration::Mode>(args[offset].As<Uint32>()->Value());

  ManagedEVPPKey key;
  unsigned int keyParamOffset = offset + 1;
  if (params->mode == SignConfiguration::kVerify) {
    key = ManagedEVPPKey::GetPublicOrPrivateKeyFromJs(args, &keyParamOffset);
  } else {
    key = ManagedEVPPKey::GetPrivateKeyFromJs(args, &keyParamOffset, true);
  }
  if (!key)
    return Nothing<bool>();
  params->key = key;

  ArrayBufferOrViewContents<char> data(args[offset + 5]);
  if (UNLIKELY(!data.CheckSizeInt32())) {
    THROW_ERR_OUT_OF_RANGE(env, "data is too big");
    return Nothing<bool>();
  }
  params->data = mode == kCryptoJobAsync
      ? data.ToCopy()
      : data.ToByteSource();

  if (args[offset + 6]->IsString()) {
    Utf8Value digest(env->isolate(), args[offset + 6]);
    params->digest = EVP_get_digestbyname(*digest);
    if (params->digest == nullptr) {
      THROW_ERR_CRYPTO_INVALID_DIGEST(env);
      return Nothing<bool>();
    }
  }

  if (args[offset + 7]->IsInt32()) {  // Salt length
    params->flags |= SignConfiguration::kHasSaltLength;
    params->salt_length = args[offset + 7].As<Int32>()->Value();
  }
  if (args[offset + 8]->IsUint32()) {  // Padding
    params->flags |= SignConfiguration::kHasPadding;
    params->padding = args[offset + 8].As<Uint32>()->Value();
  }

  if (args[offset + 9]->IsUint32()) {  // DSA Encoding
    params->dsa_encoding =
        static_cast<DSASigEnc>(args[offset + 9].As<Uint32>()->Value());
    if (params->dsa_encoding != kSigEncDER &&
        params->dsa_encoding != kSigEncP1363) {
      THROW_ERR_OUT_OF_RANGE(env, "invalid signature encoding");
      return Nothing<bool>();
    }
  }

  if (params->mode == SignConfiguration::kVerify) {
    ArrayBufferOrViewContents<char> signature(args[offset + 10]);
    if (UNLIKELY(!signature.CheckSizeInt32())) {
      THROW_ERR_OUT_OF_RANGE(env, "signature is too big");
      return Nothing<bool>();
    }
    // If this is an EC key (assuming ECDSA) we need to convert the
    // the signature from WebCrypto format into DER format...
    ManagedEVPPKey m_pkey = params->key;
    Mutex::ScopedLock lock(*m_pkey.mutex());
    if (UseP1363Encoding(m_pkey, params->dsa_encoding)) {
      params->signature =
          ConvertSignatureToDER(m_pkey, signature.ToByteSource());
    } else {
      params->signature = mode == kCryptoJobAsync
          ? signature.ToCopy()
          : signature.ToByteSource();
    }
  }

  return Just(true);
}

bool SignTraits::DeriveBits(
    Environment* env,
    const SignConfiguration& params,
    ByteSource* out) {
  ClearErrorOnReturn clear_error_on_return;
  EVPMDPointer context(EVP_MD_CTX_new());
  EVP_PKEY_CTX* ctx = nullptr;

  switch (params.mode) {
    case SignConfiguration::kSign:
      if (!EVP_DigestSignInit(
              context.get(),
              &ctx,
              params.digest,
              nullptr,
              params.key.get())) {
        crypto::CheckThrow(env, SignBase::Error::kSignInit);
        return false;
      }
      break;
    case SignConfiguration::kVerify:
      if (!EVP_DigestVerifyInit(
              context.get(),
              &ctx,
              params.digest,
              nullptr,
              params.key.get())) {
        crypto::CheckThrow(env, SignBase::Error::kSignInit);
        return false;
      }
      break;
  }

  int padding = params.flags & SignConfiguration::kHasPadding
      ? params.padding
      : GetDefaultSignPadding(params.key);

  Maybe<int> salt_length = params.flags & SignConfiguration::kHasSaltLength
      ? Just<int>(params.salt_length) : Nothing<int>();

  if (!ApplyRSAOptions(
          params.key,
          ctx,
          padding,
          salt_length)) {
    crypto::CheckThrow(env, SignBase::Error::kSignPrivateKey);
    return false;
  }

  switch (params.mode) {
    case SignConfiguration::kSign: {
      size_t len;
      unsigned char* data = nullptr;
      if (IsOneShot(params.key)) {
        if (!EVP_DigestSign(
            context.get(),
            nullptr,
            &len,
            params.data.data<unsigned char>(),
            params.data.size())) {
          crypto::CheckThrow(env, SignBase::Error::kSignPrivateKey);
          return false;
        }
        data = MallocOpenSSL<unsigned char>(len);
        if (!EVP_DigestSign(
            context.get(),
            data,
            &len,
            params.data.data<unsigned char>(),
            params.data.size())) {
              crypto::CheckThrow(env, SignBase::Error::kSignPrivateKey);
              return false;
            }
        ByteSource buf =
            ByteSource::Allocated(reinterpret_cast<char*>(data), len);
        *out = std::move(buf);
      } else {
        if (!EVP_DigestSignUpdate(
                context.get(),
                params.data.data<unsigned char>(),
                params.data.size()) ||
            !EVP_DigestSignFinal(context.get(), nullptr, &len)) {
          crypto::CheckThrow(env, SignBase::Error::kSignPrivateKey);
          return false;
        }
        data = MallocOpenSSL<unsigned char>(len);
        ByteSource buf =
            ByteSource::Allocated(reinterpret_cast<char*>(data), len);
        if (!EVP_DigestSignFinal(context.get(), data, &len)) {
          crypto::CheckThrow(env, SignBase::Error::kSignPrivateKey);
          return false;
        }

        if (UseP1363Encoding(params.key, params.dsa_encoding)) {
          *out = ConvertSignatureToP1363(env, params.key, buf);
        } else {
          buf.Resize(len);
          *out = std::move(buf);
        }
      }
      break;
    }
    case SignConfiguration::kVerify: {
      char* data = MallocOpenSSL<char>(1);
      data[0] = 0;
      *out = ByteSource::Allocated(data, 1);
      if (EVP_DigestVerify(
              context.get(),
              params.signature.data<unsigned char>(),
              params.signature.size(),
              params.data.data<unsigned char>(),
              params.data.size()) == 1) {
        data[0] = 1;
      }
    }
  }

  return true;
}

Maybe<bool> SignTraits::EncodeOutput(
    Environment* env,
    const SignConfiguration& params,
    ByteSource* out,
    Local<Value>* result) {
  switch (params.mode) {
    case SignConfiguration::kSign:
      *result = out->ToArrayBuffer(env);
      break;
    case SignConfiguration::kVerify:
      *result = out->get()[0] == 1
          ? v8::True(env->isolate())
          : v8::False(env->isolate());
      break;
    default:
      UNREACHABLE();
  }
  return Just(!result->IsEmpty());
}

}  // namespace crypto
}  // namespace node
