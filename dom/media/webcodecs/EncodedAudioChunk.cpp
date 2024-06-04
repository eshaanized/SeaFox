/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim:set ts=2 sw=2 sts=2 et cindent: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "mozilla/dom/EncodedAudioChunk.h"
#include "mozilla/dom/EncodedAudioChunkBinding.h"

#include <utility>

#include "MediaData.h"
#include "TimeUnits.h"
#include "mozilla/CheckedInt.h"
#include "mozilla/Logging.h"
#include "mozilla/PodOperations.h"
#include "mozilla/dom/StructuredCloneHolder.h"
#include "mozilla/dom/StructuredCloneTags.h"
#include "mozilla/dom/WebCodecsUtils.h"

extern mozilla::LazyLogModule gWebCodecsLog;
using mozilla::media::TimeUnit;

namespace mozilla::dom {

#ifdef LOG_INTERNAL
#  undef LOG_INTERNAL
#endif  // LOG_INTERNAL
#define LOG_INTERNAL(level, msg, ...) \
  MOZ_LOG(gWebCodecsLog, LogLevel::level, (msg, ##__VA_ARGS__))

#ifdef LOGW
#  undef LOGW
#endif  // LOGW
#define LOGW(msg, ...) LOG_INTERNAL(Warning, msg, ##__VA_ARGS__)

#ifdef LOGE
#  undef LOGE
#endif  // LOGE
#define LOGE(msg, ...) LOG_INTERNAL(Error, msg, ##__VA_ARGS__)

// Only needed for refcounted objects.
NS_IMPL_CYCLE_COLLECTION_WRAPPERCACHE(EncodedAudioChunk, mParent)
NS_IMPL_CYCLE_COLLECTING_ADDREF(EncodedAudioChunk)
NS_IMPL_CYCLE_COLLECTING_RELEASE(EncodedAudioChunk)
NS_INTERFACE_MAP_BEGIN_CYCLE_COLLECTION(EncodedAudioChunk)
  NS_WRAPPERCACHE_INTERFACE_MAP_ENTRY
  NS_INTERFACE_MAP_ENTRY(nsISupports)
NS_INTERFACE_MAP_END

EncodedAudioChunkData::EncodedAudioChunkData(
    already_AddRefed<MediaAlignedByteBuffer> aBuffer,
    const EncodedAudioChunkType& aType, int64_t aTimestamp,
    Maybe<uint64_t>&& aDuration)
    : mBuffer(aBuffer),
      mType(aType),
      mTimestamp(aTimestamp),
      mDuration(aDuration) {
  MOZ_ASSERT(mBuffer);
  MOZ_ASSERT(mBuffer->Length() == mBuffer->Size());
  MOZ_ASSERT(mBuffer->Length() <=
             static_cast<size_t>(std::numeric_limits<uint32_t>::max()));
}

UniquePtr<EncodedAudioChunkData> EncodedAudioChunkData::Clone() const {
  if (!mBuffer) {
    LOGE("No buffer in EncodedAudioChunkData %p to clone!", this);
    return nullptr;
  }

  // Since EncodedAudioChunkData can be zero-sized, cloning a zero-sized chunk
  // is allowed.
  if (mBuffer->Size() == 0) {
    LOGW("Cloning an empty EncodedAudioChunkData %p", this);
  }

  auto buffer =
      MakeRefPtr<MediaAlignedByteBuffer>(mBuffer->Data(), mBuffer->Length());
  if (!buffer || buffer->Size() != mBuffer->Size()) {
    LOGE("OOM to copy EncodedAudioChunkData %p", this);
    return nullptr;
  }

  return MakeUnique<EncodedAudioChunkData>(buffer.forget(), mType, mTimestamp,
                                           Maybe<uint64_t>(mDuration));
}

already_AddRefed<MediaRawData> EncodedAudioChunkData::TakeData() {
  if (!mBuffer || !(*mBuffer)) {
    LOGE("EncodedAudioChunkData %p has no data!", this);
    return nullptr;
  }

  RefPtr<MediaRawData> sample(new MediaRawData(std::move(*mBuffer)));
  sample->mKeyframe = mType == EncodedAudioChunkType::Key;
  sample->mTime = TimeUnit::FromMicroseconds(mTimestamp);
  sample->mTimecode = TimeUnit::FromMicroseconds(mTimestamp);

  if (mDuration) {
    CheckedInt64 duration(*mDuration);
    if (!duration.isValid()) {
      LOGE("EncodedAudioChunkData %p 's duration exceeds TimeUnit's limit",
           this);
      return nullptr;
    }
    sample->mDuration = TimeUnit::FromMicroseconds(duration.value());
  }

  return sample.forget();
}

EncodedAudioChunk::EncodedAudioChunk(
    nsIGlobalObject* aParent, already_AddRefed<MediaAlignedByteBuffer> aBuffer,
    const EncodedAudioChunkType& aType, int64_t aTimestamp,
    Maybe<uint64_t>&& aDuration)
    : EncodedAudioChunkData(std::move(aBuffer), aType, aTimestamp,
                            std::move(aDuration)),
      mParent(aParent) {}

EncodedAudioChunk::EncodedAudioChunk(nsIGlobalObject* aParent,
                                     const EncodedAudioChunkData& aData)
    : EncodedAudioChunkData(aData), mParent(aParent) {}

nsIGlobalObject* EncodedAudioChunk::GetParentObject() const {
  AssertIsOnOwningThread();

  return mParent.get();
}

JSObject* EncodedAudioChunk::WrapObject(JSContext* aCx,
                                        JS::Handle<JSObject*> aGivenProto) {
  AssertIsOnOwningThread();

  return EncodedAudioChunk_Binding::Wrap(aCx, this, aGivenProto);
}

// https://w3c.github.io/webcodecs/#encodedaudiochunk-constructors
/* static */
already_AddRefed<EncodedAudioChunk> EncodedAudioChunk::Constructor(
    const GlobalObject& aGlobal, const EncodedAudioChunkInit& aInit,
    ErrorResult& aRv) {
  nsCOMPtr<nsIGlobalObject> global = do_QueryInterface(aGlobal.GetAsSupports());
  if (!global) {
    aRv.Throw(NS_ERROR_FAILURE);
    return nullptr;
  }

  auto buffer = ProcessTypedArrays(
      aInit.mData,
      [&](const Span<uint8_t>& aData,
          JS::AutoCheckCannotGC&&) -> RefPtr<MediaAlignedByteBuffer> {
        // Make sure it's in uint32_t's range.
        CheckedUint32 byteLength(aData.Length());
        if (!byteLength.isValid()) {
          aRv.Throw(NS_ERROR_INVALID_ARG);
          return nullptr;
        }
        if (aData.Length() == 0) {
          LOGW("Buffer for constructing EncodedAudioChunk is empty!");
        }
        RefPtr<MediaAlignedByteBuffer> buf = MakeRefPtr<MediaAlignedByteBuffer>(
            aData.Elements(), aData.Length());

        // Instead of checking *buf, size comparision is used to allow
        // constructing a zero-sized EncodedAudioChunk.
        if (!buf || buf->Size() != aData.Length()) {
          aRv.Throw(NS_ERROR_OUT_OF_MEMORY);
          return nullptr;
        }
        return buf;
      });

  RefPtr<EncodedAudioChunk> chunk(new EncodedAudioChunk(
      global, buffer.forget(), aInit.mType, aInit.mTimestamp,
      OptionalToMaybe(aInit.mDuration)));
  return aRv.Failed() ? nullptr : chunk.forget();
}

EncodedAudioChunkType EncodedAudioChunk::Type() const {
  AssertIsOnOwningThread();

  return mType;
}

int64_t EncodedAudioChunk::Timestamp() const {
  AssertIsOnOwningThread();

  return mTimestamp;
}

Nullable<uint64_t> EncodedAudioChunk::GetDuration() const {
  AssertIsOnOwningThread();
  return MaybeToNullable(mDuration);
}

uint32_t EncodedAudioChunk::ByteLength() const {
  AssertIsOnOwningThread();
  MOZ_ASSERT(mBuffer);

  return static_cast<uint32_t>(mBuffer->Length());
}

// https://w3c.github.io/webcodecs/#dom-encodedaudiochunk-copyto
void EncodedAudioChunk::CopyTo(
    const MaybeSharedArrayBufferViewOrMaybeSharedArrayBuffer& aDestination,
    ErrorResult& aRv) {
  AssertIsOnOwningThread();

  ProcessTypedArraysFixed(aDestination, [&](const Span<uint8_t>& aData) {
    if (mBuffer->Size() > aData.size_bytes()) {
      aRv.ThrowTypeError(
          "Destination ArrayBuffer smaller than source EncodedAudioChunk");
      return;
    }

    PodCopy(aData.data(), mBuffer->Data(), mBuffer->Size());
  });
}

// https://w3c.github.io/webcodecs/#ref-for-deserialization-steps
/* static */
JSObject* EncodedAudioChunk::ReadStructuredClone(
    JSContext* aCx, nsIGlobalObject* aGlobal, JSStructuredCloneReader* aReader,
    const EncodedAudioChunkData& aData) {
  JS::Rooted<JS::Value> value(aCx, JS::NullValue());
  // To avoid a rooting hazard error from returning a raw JSObject* before
  // running the RefPtr destructor, RefPtr needs to be destructed before
  // returning the raw JSObject*, which is why the RefPtr<EncodedAudioChunk> is
  // created in the scope below. Otherwise, the static analysis infers the
  // RefPtr cannot be safely destructed while the unrooted return JSObject* is
  // on the stack.
  {
    auto frame = MakeRefPtr<EncodedAudioChunk>(aGlobal, aData);
    if (!GetOrCreateDOMReflector(aCx, frame, &value) || !value.isObject()) {
      return nullptr;
    }
  }
  return value.toObjectOrNull();
}

// https://w3c.github.io/webcodecs/#ref-for-serialization-steps
bool EncodedAudioChunk::WriteStructuredClone(
    JSStructuredCloneWriter* aWriter, StructuredCloneHolder* aHolder) const {
  AssertIsOnOwningThread();

  // Indexing the chunk and send the index to the receiver.
  const uint32_t index =
      static_cast<uint32_t>(aHolder->EncodedAudioChunks().Length());
  // The serialization is limited to the same process scope so it's ok to
  // serialize a reference instead of a copy.
  aHolder->EncodedAudioChunks().AppendElement(EncodedAudioChunkData(*this));
  return !NS_WARN_IF(
      !JS_WriteUint32Pair(aWriter, SCTAG_DOM_ENCODEDAUDIOCHUNK, index));
}

#undef LOGW
#undef LOGE
#undef LOG_INTERNAL

}  // namespace mozilla::dom