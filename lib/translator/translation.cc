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

#include "common.h"

namespace translator {

BeginResponse Translation::Begin(absl::Span<const uint8_t> scsi_cmd,
                                 scsi_defs::LunAddress lun) {
  BeginResponse response = {};
  response.status = ApiStatus::kSuccess;
  if (this->pipeline_status != StatusCode::kUninitialized) {
    DebugLog("Invalid use of API: Begin called before complete or abort");
    response.status = ApiStatus::kFailure;
    return response;
  }

  // Verify buffer is large enough to contain opcode (one byte)
  if (scsi_cmd.size() < 1) {
    DebugLog("Empty SCSI Buffer");
    this->pipeline_status = StatusCode::kFailure;
    return response;
  }

  this->pipeline_status = StatusCode::kSuccess;
  this->scsi_cmd = scsi_cmd;

  scsi_defs::OpCode opc = static_cast<scsi_defs::OpCode>(scsi_cmd[0]);
  switch (opc) {
    case scsi_defs::OpCode::kInquiry:
      return response;
    default:
      DebugLog("Bad OpCode: %#x", static_cast<uint8_t>(opc));
      this->pipeline_status = StatusCode::kFailure;
      return response;
  }
}

ApiStatus Translation::Complete(
    absl::Span<const nvme_defs::GenericQueueEntryCpl> cpl_data,
    absl::Span<uint8_t> buffer) {
  if (this->pipeline_status == StatusCode::kUninitialized) {
    DebugLog("Invalid use of API: Complete called before Begin");
    return ApiStatus::kFailure;
  }
  if (this->pipeline_status == StatusCode::kFailure) {
    // TODO fill buffer with SCSI CHECK CONDITION response
    return ApiStatus::kSuccess;
  }

  this->pipeline_status =
      StatusCode::kUninitialized;  // Reset for next interaction

  scsi_defs::OpCode opc = static_cast<scsi_defs::OpCode>(this->scsi_cmd[0]);
  switch (opc) {
    case scsi_defs::OpCode::kInquiry:
      return ApiStatus::kSuccess;
  }
}

absl::Span<const nvme_defs::GenericQueueEntryCmd> Translation::GetNvmeCmds() {
  return absl::MakeSpan(this->nvme_cmds, this->nvme_cmd_count);
}

void Translation::AbortPipeline() {
  this->pipeline_status = StatusCode::kUninitialized;
  this->nvme_cmd_count = 0;
  // TODO free allocated PRPs
}

};  // namespace translator
