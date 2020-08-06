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

#include "maintenance_in.h"

#include "lib/scsi.h"

namespace translator {

StatusCode ValidateReportSupportedOpCodes(Span<const uint8_t> scsi_cmd,
                                          uint32_t& alloc_len) {
  scsi::ReportOpCodesCommand report_cmd = {};
  if (!ReadValue(scsi_cmd, report_cmd)) {
    DebugLog("Malformed Report Supported OpCodes command");
    return StatusCode::kInvalidInput;
  }

  if (report_cmd.requested_op_code !=
          static_cast<uint8_t>(scsi::OpCode::kWriteSame16) ||
      report_cmd.reporting_options != 0b001) {
    // Project specifications only require supporting ReportSupportedOpCodes for
    // WriteSame16
    DebugLog("Only supporting ReportSupportedOpCodes for WriteSame16");
    return StatusCode::kInvalidInput;
  }

  alloc_len = sizeof(scsi::OneCommandParamData);

  return StatusCode::kSuccess;
}

void WriteReportSupportedOpCodesResult(Span<uint8_t> buffer) {
  scsi::OneCommandParamData data = {
      // device does not support the requested command
      // all data after byte 1 is undefined
      .support = 0b001};

  WriteValue(data, buffer);
}

}  // namespace translator
