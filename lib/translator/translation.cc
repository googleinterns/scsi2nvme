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

StatusCode Translation::ScsiToNvme(absl::Span<const uint8_t> scsi_cmd,
                                   const ScsiContext& scsi_context) {
  this->pipeline_status = StatusCode::kSuccess;
  // Verify buffer is large enough to contain opcode (one byte)
  if (scsi_cmd.size() < 1) {
    DebugLog("Empty SCSI Buffer");
    this->pipeline_status = StatusCode::kFailure;
    return StatusCode::kFailure;
  }

  this->scsi_cmd = scsi_cmd;
  this->scsi_context = scsi_context;

  scsi_defs::OpCode opc = static_cast<scsi_defs::OpCode>(scsi_cmd[0]);
  switch (opc) {
    case scsi_defs::OpCode::kInquiry:
      return StatusCode::kSuccess;
    default:
      DebugLog("Bad OpCode");
      this->pipeline_status = StatusCode::kFailure;
      return StatusCode::kNoTranslation;
  }
}

StatusCode Translation::NvmeToScsi(const NvmeCompletionData& cpl_data,
                                   void* data_in) {
  if (this->pipeline_status == StatusCode::kFailure) {
    // TODO fill data-in buffer with SCSI failure response
    return StatusCode::kFailure;
  }

  if (scsi_cmd.size() < 1) {
    DebugLog("Empty SCSI Buffer");
    return StatusCode::kFailure;
  }
  if (data_in == nullptr) {
    DebugLog("Null data_in buffer");
    return StatusCode::kFailure;
  }

  scsi_defs::OpCode opc = static_cast<scsi_defs::OpCode>(this->scsi_cmd[0]);
  switch (opc) {
    case scsi_defs::OpCode::kInquiry:
      return StatusCode::kSuccess;
    default:
      DebugLog("Bad OpCode");
      return StatusCode::kNoTranslation;
  }
}

absl::Span<const nvme_defs::GenericQueueEntryCmd> Translation::GetNvmeCmds() {
  return absl::MakeSpan(this->nvme_cmds, this->nvme_cmd_count);
}

};  // namespace translator
