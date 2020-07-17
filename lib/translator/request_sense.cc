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

#include "request_sense.h"

namespace translator {

namespace {  // anonymous namespace for helper functions
void TranslateDescriptorSenseData(absl::Span<uint8_t> buffer) {
  scsi::DescriptorFormatSenseData result = {
      // Shall be set to 72h indicating current errors are returned.
      .response_code = 0x72,

      // Shall be set to NO ADDITIONAL SENSE INFORMATION (0) if device is in
      // power state 00h,
      // TODO: otherwise shall be set to LOW POWER CONDITION ON.
      .additional_sense_code =
          scsi::AdditionalSenseCode::kNoAdditionalSenseInfo,
  };
  WriteValue(result, buffer);
}

void TranslateFixedSenseData(absl::Span<uint8_t> buffer) {
  scsi::FixedFormatSenseData result = {
      // Shall be set to 70h indicating current errors are returned.
      .response_code = 0x70,

      // Shall indicate the number of bytes of additional sense data.
      // There is no additional sense data.
      .additional_sense_length = 0,

      // Shall contain information depending on command where the exception
      // condition occurred
      // TODO: This field informs about some specific errors (like "unit
      // attention") - we do not support this so we can always return pointer to
      // "fixed format sense data" that is just empty.
      .command_specific_info = 0,

      // Shall be set to NO ADDITIONAL SENSE INFORMATION (0) if device is in
      // power state 00h,
      // TODO: otherwise shall be set to LOW POWER CONDITION ON.
      .additional_sense_code =
          scsi::AdditionalSenseCode::kNoAdditionalSenseInfo,
  };
  WriteValue(result, buffer);
}

}  // namespace

StatusCode RequestSenseToNvme(absl::Span<const uint8_t> scsi_cmd,
                              uint32_t& allocation_length) {
  scsi::RequestSenseCommand request_sense_cmd{};
  if (!ReadValue(scsi_cmd, request_sense_cmd)) {
    DebugLog("Malformed RequestSense Command");
    return StatusCode::kInvalidInput;
  }

  if (request_sense_cmd.control_byte.naca == 1) {
    DebugLog("Malformed RequestSense Command");
    return StatusCode::kInvalidInput;
  }
  allocation_length = request_sense_cmd.allocation_length;
  return StatusCode::kSuccess;
}

StatusCode RequestSenseToScsi(absl::Span<const uint8_t> scsi_cmd,
                              absl::Span<uint8_t> buffer) {
  scsi::RequestSenseCommand request_sense_cmd{};
  if (!ReadValue(scsi_cmd, request_sense_cmd)) {
    DebugLog("Malformed RequestSense Command");
    return StatusCode::kInvalidInput;
  }

  if (request_sense_cmd.desc == 1) {
    TranslateDescriptorSenseData(buffer);
  } else if (request_sense_cmd.desc == 0) {
    TranslateFixedSenseData(buffer);
  }
  return StatusCode::kSuccess;
}

}  // namespace translator