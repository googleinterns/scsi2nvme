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

StatusCode Allocation::SetPages(uint16_t data_page_count,
                                uint16_t mdata_page_count) {
  this->data_page_count = data_page_count;
  this->data_addr = AllocPages(data_page_count);
  this->mdata_page_count = mdata_page_count;
  this->mdata_addr = AllocPages(mdata_page_count);

  if (this->data_addr == 0 || this->mdata_addr == 0) {
    DebugLog("Error when requesting a page of memory");
    return StatusCode::kFailure;
  }

  return StatusCode::kSuccess;
}

absl::string_view ScsiOpcodeToString(scsi_defs::OpCode opcode) {
  switch (opcode) {
    case scsi_defs::OpCode::kTestUnitReady:
      return "kTestUnitReady";
    case scsi_defs::OpCode::kRequestSense:
      return "kRequestSense";
    case scsi_defs::OpCode::kRead6:
      return "kRead6";
    case scsi_defs::OpCode::kWrite6:
      return "kWrite6";
    case scsi_defs::OpCode::kInquiry:
      return "kInquiry";
    case scsi_defs::OpCode::kReserve6:
      return "kReserve6";
    case scsi_defs::OpCode::kRelease6:
      return "kRelease6";
    case scsi_defs::OpCode::kModeSense6:
      return "kModeSense6";
    case scsi_defs::OpCode::kStartStopUnit:
      return "kStartStopUnit";
    case scsi_defs::OpCode::kDoPreventAllowMediumRemoval:
      return "kDoPreventAllowMediumRemoval";
    case scsi_defs::OpCode::kReadCapacity10:
      return "kReadCapacity10";
    case scsi_defs::OpCode::kRead10:
      return "kRead10";
    case scsi_defs::OpCode::kWrite10:
      return "kWrite10";
    case scsi_defs::OpCode::kVerify10:
      return "kVerify10";
    case scsi_defs::OpCode::kSync10:
      return "kSync10";
    case scsi_defs::OpCode::kUnmap:
      return "kUnmap";
    case scsi_defs::OpCode::kReadToc:
      return "kReadToc";
    case scsi_defs::OpCode::kModeSense10:
      return "kModeSense10";
    case scsi_defs::OpCode::kPersistentReserveIn:
      return "kPersistentReserveIn";
    case scsi_defs::OpCode::kPersistentReserveOut:
      return "kPersistentReserveOut";

    // same opcode
    case scsi_defs::OpCode::kRead32:
      return "kRead32 / kWrite32 /  kVerify32";

    case scsi_defs::OpCode::kRead16:
      return "kRead16";
    case scsi_defs::OpCode::kWrite16:
      return "kWrite16";
    case scsi_defs::OpCode::kVerify16:
      return "kVerify16";
    case scsi_defs::OpCode::kSync16:
      return "kSync16";
    case scsi_defs::OpCode::kServiceActionIn:
      return "kServiceActionIn";
    case scsi_defs::OpCode::kReportLuns:
      return "kReportLuns";
    case scsi_defs::OpCode::kMaintenanceIn:
      return "kMaintenanceIn";
    case scsi_defs::OpCode::kRead12:
      return "kRead12";
    case scsi_defs::OpCode::kWrite12:
      return "kWrite12";
    case scsi_defs::OpCode::kVerify12:
      return "kVerify12";
    default:
      return "INVALID_OPCODE";
  }
}

}  // namespace translator
