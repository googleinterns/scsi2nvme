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

#include "lib/scsi.h"

namespace translator {

// Vendor Identification shall be set to "NVMe" followed by 4 spaces: "NVMe    "
// This value is not null terminated and should be size 8
constexpr char kNvmeVendorIdentification[] = "NVMe    ";
static_assert(strlen(kNvmeVendorIdentification) == 8);

// The maximum amplification ratio of any supported SCSI:NVMe translation
constexpr int kMaxCommandRatio = 3;

// Reports the status of a translation for internal use
enum class StatusCode {
  kSuccess,
  kUninitialized,
  kInvalidInput,
  kNoTranslation,
  kFailure
};

struct Allocation {
  uint64_t data_addr;         // Start of data buffer
  uint16_t data_page_count;   // Number of data pages
  uint64_t mdata_addr;        // Start of metadata buffer
  uint16_t mdata_page_count;  // Number of metadata pages

  // Sets [m]data_page_count, calls AllocPages([m]data_page_count),
  // and returns StatusCode based on whether AllocPages was successful
  StatusCode SetPages(uint16_t data_page_count, uint16_t mdata_page_count);
};

void DebugLog(const char* format, ...);

// Max consecutive pages required by NVMe PRP list is 512
uint64_t AllocPages(uint16_t count);

void DeallocPages(uint64_t pages_ptr, uint16_t count);

void SetDebugCallback(void (*callback)(const char*));

void SetAllocPageCallbacks(uint64_t (*alloc_callback)(uint16_t),
                           void (*dealloc_callback)(uint64_t, uint16_t));

// Returns true if system is little endian.
// Returns false if system is big endian.
bool IsLittleEndian();

// Host to Network endianness transformation for uint64_t
// Network endianness is always big endian
// Converts value to big endian if Host is little endian
// No op if Host is big endian
uint64_t htonll(uint64_t value);

// Network to Host endianness transformation for uint64_t
// Network endianness is always big endian
// Converts value to little endian if Host is little endian
// No op if Host is big endian
#define ntohll htonll

// does not return string_view for compatibility with DebugLog
const char* ScsiOpcodeToString(scsi::OpCode opcode);

template <typename T>
class Span {
 public:
  Span() : ptr_(nullptr), len_(0) {}
  Span(T* ptr, size_t len) : ptr_(ptr), len_(len) {}
  template <size_t N>
  Span(T (&a)[N]) : Span(a, N) {}
  template <typename Y, typename = std::enable_if<std::is_same<
                            typename std::decay<T*>::type, Y*>::value>>
  Span(const Span<Y>& ref) : Span(ref.data(), ref.size()) {}
  T* data() const { return ptr_; }
  size_t size() const { return len_; }
  bool empty() { return len_ == 0; }
  T& operator[](size_t i) const { return *(ptr_ + i); }
  Span subspan(size_t pos, size_t len = npos) {
    if (pos >= len_) return Span<T>(nullptr, 0);
    if (len > (len_ - pos) || len == npos) len = len_ - pos;
    return Span<T>(ptr_ + pos, len);
  }

 private:
  T* ptr_;
  size_t len_;
  constexpr static size_t npos = -1;
};

template <typename T>
bool ReadValue(Span<const uint8_t> data, T& out) {
  static_assert(std::is_pod_v<T>, "Only supports POD types");
  if (sizeof(T) > data.size()) return false;
  memcpy(&out, data.data(), sizeof(T));
  return true;
}

template <typename T>
bool WriteValue(const T& data, Span<uint8_t> out) {
  static_assert(std::is_pod_v<T>, "Only supports POD types");
  if (sizeof(T) > out.size()) return false;
  memcpy(out.data(), &data, sizeof(T));
  return true;
}

}  // namespace translator
#endif
