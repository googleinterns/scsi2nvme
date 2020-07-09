// Copyright 2020 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef LIB_TRANSLATOR_COMMON_H
#define LIB_TRANSLATOR_COMMON_H

#include <cstring>
#include <type_traits>

#include "absl/types/span.h"
#include "lib/scsi_defs.h"
#include "third_party/spdk_defs/nvme_defs.h"
#include <cstring>
#include <type_traits>


namespace translator {

enum class StatusCode {
  kSuccess,
  kUninitialized,
  kInvalidInput,
  kNoTranslation,
  kFailure
};

void DebugLog(const char* format, ...);

// Max consecutive pages required by NVMe PRP list is 512
void* AllocPages(uint16_t count);

void DeallocPages(void* pages_ptr, uint16_t count);

void SetDebugCallback(void (*callback)(const char*));

void SetAllocPageCallbacks(void* (*alloc_callback)(uint16_t),
                           void (*dealloc_callback)(void*, uint16_t));

template <typename T>
bool ReadValue(absl::Span<const uint8_t> data, T& out) {
  static_assert(std::is_pod_v<T>, "Only supports POD types");
  if (sizeof(T) > data.size()) return false;
  memcpy(&out, data.data(), sizeof(T));
  return true;
}

template <typename T>
bool WriteValue(const T& data, absl::Span<uint8_t> out) {
  static_assert(std::is_pod_v<T>, "Only supports POD types");
  if (sizeof(T) > out.size()) return false;
  memcpy(out.data(), &data, sizeof(T));
  return true;
}

}  // namespace translator

#endif
