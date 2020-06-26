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

#ifndef COMMON_H
#define COMMON_H

#include <stdarg.h>
#include <stdio.h>

#include "common.h"

namespace translator {

void DebugLog(const char *format, ...) {
  if (debug_callback == nullptr) return;
  char buffer[1024];
  va_list args;
  va_start(args, format);
  vsnprintf(buffer, 1024, format, args);
  debug_callback(buffer);
  va_end(args);
}

void SetDebugCallback(void (*callback)(const char *)) {
  debug_callback = callback;
}

StatusCode MakeScsiOpcode(uint8_t val, scsi_defs::OpCode &opcode) {
  if (val > 0xaf) {
    char debug_buffer[100];
    sprintf(debug_buffer, "invalid opcode. %ux is out of range.", val);
    if (debug_callback != NULL) debug_callback(debug_buffer);
    return StatusCode::kInvalidInput;
  }
  opcode = static_cast<scsi_defs::OpCode>(val);
  return StatusCode::kSuccess;
}

const char *ScsiOpcodeToString(scsi_defs::OpCode opcode) {
  uint32_t opcode_val = static_cast<uint32_t>(opcode);
  switch (opcode_val) {
    case 0x0:
      return "kTestUnitReady";
      break;
    case 0x3:
      return "kRequestSense";
      break;
    case 0x08:
      return "kRead6";
      break;
    case 0x0a:
      return "kWrite6";
      break;
    case 0x12:
      return "kInquiry";
      break;
    case 0x16:
      return "kReserve6";
      break;
    case 0x17:
      return "kRelease6";
      break;
    case 0x1a:
      return "kModeSense6";
      break;
    case 0x1b:
      return "kStartStopUnit";
      break;
    case 0x1e:
      return "kDoPreventAllowMediumRemoval";
      break;
    case 0x25:
      return "kReadCapacity10";
      break;
    case 0x28:
      return "kRead10";
      break;
    case 0x2a:
      return "kWrite10";
      break;
    case 0x2f:
      return "kVerify10";
      break;
    case 0x35:
      return "kSync10";
      break;
    case 0x42:
      return "kUnmap";
      break;
    case 0x43:
      return "kReadToc";
      break;
    case 0x5a:
      return "kModeSense10";
      break;
    case 0x5e:
      return "kPersistentReserveIn";
      break;
    case 0x5f:
      return "kPersistentReserveOut";
      break;
    case 0x7f:
      return "kRead32 / kWrite32 / kVerify32";
      break;
    case 0x88:
      return "kRead16";
      break;
    case 0x8a:
      return "kWrite16";
      break;
    case 0x8f:
      return "kVerify16";
      break;
    case 0x91:
      return "kSync16";
      break;
    case 0x9e:
      return "kServiceActionIn";
      break;
    case 0xa0:
      return "kReportLuns";
      break;
    case 0xa3:
      return "kMaintenanceIn";
      break;
    case 0xa8:
      return "kRead12";
      break;
    case 0xaa:
      return "kWrite12";
      break;
    case 0xaf:
      return "kVerify12";
      break;
    default:
      return "INVALID_OPCODE";
      break;
  }
}

}  // namespace translator
#endif