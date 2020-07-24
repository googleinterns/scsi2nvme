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
// limitations under the License.i// Copyright 2020 Google LLC

#include "common.h"

#ifdef __KERNEL__
#include <linux/byteorder/generic.h>
#else
#include <netinet/in.h>
#endif

#include <stdarg.h>
#include <stdio.h>

namespace translator {

static void (*debug_callback)(const char*);
static uint64_t (*alloc_pages_callback)(uint16_t);
static void (*dealloc_pages_callback)(uint64_t, uint16_t);

void DebugLog(const char* format, ...) {
  if (debug_callback == nullptr) return;
  char buffer[1024];
  va_list args;
  va_start(args, format);
  vsnprintf(buffer, 1024, format, args);
  debug_callback(buffer);
  va_end(args);
}

void SetDebugCallback(void (*callback)(const char*)) {
  debug_callback = callback;
}

// A return value of 0 is equivalent to nullptr.
uint64_t AllocPages(uint16_t count) {
  if (alloc_pages_callback == nullptr) return 0;
  return alloc_pages_callback(count);
}

void DeallocPages(uint64_t pages_ptr, uint16_t count) {
  if (dealloc_pages_callback == nullptr) return;
  dealloc_pages_callback(pages_ptr, count);
}

void SetAllocPageCallbacks(uint64_t (*alloc_callback)(uint16_t),
                           void (*dealloc_callback)(uint64_t, uint16_t)) {
  alloc_pages_callback = alloc_callback;
  dealloc_pages_callback = dealloc_callback;
}

bool IsLittleEndian() {
  static uint32_t test_val = 42;
  // Check first byte to determine endianness
  if (*reinterpret_cast<const char*>(&test_val) == test_val)  // little endian
    return true;
  return false;  // big endian
}

uint64_t htonll(uint64_t value) {
  if (IsLittleEndian()) {  // little endian
    const uint32_t high_bits = htonl(static_cast<uint32_t>(value >> 32));
    const uint32_t low_bits =
        htonl(static_cast<uint32_t>(value & 0xFFFFFFFFLL));

    return (static_cast<uint64_t>(low_bits) << 32) | high_bits;
  } else {  // big endian
    return value;
  }
}

StatusCode Allocation::SetPages(uint16_t data_page_count,
                                uint16_t mdata_page_count) {
  if ((data_page_count != 0 && this->data_addr != 0) ||
      (mdata_page_count != 0 && this->mdata_addr != 0)) {
    DebugLog("Trying to override data that has not been flushed");
    return StatusCode::kFailure;
  }

  this->data_page_count = data_page_count;
  this->data_addr = AllocPages(data_page_count);
  this->mdata_page_count = mdata_page_count;
  this->mdata_addr = AllocPages(mdata_page_count);

  if ((data_page_count != 0 && this->data_addr == 0) ||
      (mdata_page_count != 0 && this->mdata_addr == 0)) {
    DebugLog("Error when requesting memory");
    return StatusCode::kFailure;
  }

  return StatusCode::kSuccess;
}

const char* ScsiOpcodeToString(scsi::OpCode opcode) {
  switch (opcode) {
    case scsi::OpCode::kTestUnitReady:
      return "kTestUnitReady";
    case scsi::OpCode::kRequestSense:
      return "kRequestSense";
    case scsi::OpCode::kRead6:
      return "kRead6";
    case scsi::OpCode::kWrite6:
      return "kWrite6";
    case scsi::OpCode::kInquiry:
      return "kInquiry";
    case scsi::OpCode::kReserve6:
      return "kReserve6";
    case scsi::OpCode::kRelease6:
      return "kRelease6";
    case scsi::OpCode::kModeSense6:
      return "kModeSense6";
    case scsi::OpCode::kStartStopUnit:
      return "kStartStopUnit";
    case scsi::OpCode::kDoPreventAllowMediumRemoval:
      return "kDoPreventAllowMediumRemoval";
    case scsi::OpCode::kReadCapacity10:
      return "kReadCapacity10";
    case scsi::OpCode::kRead10:
      return "kRead10";
    case scsi::OpCode::kWrite10:
      return "kWrite10";
    case scsi::OpCode::kVerify10:
      return "kVerify10";
    case scsi::OpCode::kSync10:
      return "kSync10";
    case scsi::OpCode::kUnmap:
      return "kUnmap";
    case scsi::OpCode::kReadToc:
      return "kReadToc";
    case scsi::OpCode::kModeSense10:
      return "kModeSense10";
    case scsi::OpCode::kPersistentReserveIn:
      return "kPersistentReserveIn";
    case scsi::OpCode::kPersistentReserveOut:
      return "kPersistentReserveOut";

    // same opcode
    case scsi::OpCode::kRead32:
      return "kRead32 / kWrite32 /  kVerify32";

    case scsi::OpCode::kRead16:
      return "kRead16";
    case scsi::OpCode::kWrite16:
      return "kWrite16";
    case scsi::OpCode::kVerify16:
      return "kVerify16";
    case scsi::OpCode::kSync16:
      return "kSync16";
    case scsi::OpCode::kServiceActionIn:
      return "kServiceActionIn";
    case scsi::OpCode::kReportLuns:
      return "kReportLuns";
    case scsi::OpCode::kMaintenanceIn:
      return "kMaintenanceIn";
    case scsi::OpCode::kRead12:
      return "kRead12";
    case scsi::OpCode::kWrite12:
      return "kWrite12";
    case scsi::OpCode::kVerify12:
      return "kVerify12";
    default:
      return "INVALID_OPCODE";
  }
}

}  // namespace translator
