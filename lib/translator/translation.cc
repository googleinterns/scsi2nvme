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

#include "translation.h"
#include "write.h"

namespace translator {

BeginResponse Translation::Begin(absl::Span<const uint8_t> scsi_cmd,
                                 scsi::LunAddress lun) {
  BeginResponse response = {};
  response.status = ApiStatus::kSuccess;
  if (pipeline_status_ != StatusCode::kUninitialized) {
    DebugLog("Invalid use of API: Begin called before complete or abort");
    response.status = ApiStatus::kFailure;
    return response;
  }

  // Verify buffer is large enough to contain opcode (one byte)
  if (scsi_cmd.empty()) {
    DebugLog("Empty SCSI Buffer");
    pipeline_status_ = StatusCode::kFailure;
    return response;
  }

  pipeline_status_ = StatusCode::kSuccess;
  scsi_cmd_ = scsi_cmd;

  absl::Span<const uint8_t> scsi_cmd_no_op = scsi_cmd.subspan(1);
  scsi::OpCode opc = static_cast<scsi::OpCode>(scsi_cmd[0]);
  switch (opc) {
    case scsi::OpCode::kInquiry:
      break;
    case scsi::OpCode::kWrite6:
      pipeline_status_ = translator::Write6ToNvme(scsi_cmd, nvme_cmds_[0], allocations_[0]);
      nvme_cmd_count_ = 1;
    case scsi::OpCode::kWrite10:
      pipeline_status_ = translator::Write10ToNvme(scsi_cmd, nvme_cmds_[0], allocations_[0]);
      nvme_cmd_count_ = 1;
    case scsi::OpCode::kWrite12:
      pipeline_status_ = translator::Write12ToNvme(scsi_cmd, nvme_cmds_[0], allocations_[0]);
      nvme_cmd_count_ = 1;
    case scsi::OpCode::kWrite16:
      pipeline_status_ = translator::Write16ToNvme(scsi_cmd, nvme_cmds_[0], allocations_[0]);
      nvme_cmd_count_ = 1; 
    default:
      DebugLog("Bad OpCode: %#x", static_cast<uint8_t>(opc));
      pipeline_status_ = StatusCode::kFailure;
      break;
  }

  if (pipeline_status_ != StatusCode::kSuccess) {
    nvme_cmd_count_ = 0;
  }
  return response;
}

ApiStatus Translation::Complete(
    absl::Span<const nvme::GenericQueueEntryCpl> cpl_data,
    absl::Span<uint8_t> buffer) {
  if (pipeline_status_ == StatusCode::kUninitialized) {
    DebugLog("Invalid use of API: Complete called before Begin");
    return ApiStatus::kFailure;
  }
  if (pipeline_status_ == StatusCode::kFailure) {
    // TODO fill buffer with SCSI CHECK CONDITION response
    AbortPipeline();
    return ApiStatus::kSuccess;
  }

  // Switch cases should not return
  ApiStatus ret;
  scsi::OpCode opc = static_cast<scsi::OpCode>(scsi_cmd_[0]);
  switch (opc) {
    case scsi::OpCode::kInquiry:
      ret = ApiStatus::kSuccess;
      break;
  }
  AbortPipeline();
  return ret;
}

absl::Span<const nvme::GenericQueueEntryCmd> Translation::GetNvmeCmds() {
  return absl::MakeSpan(nvme_cmds_, nvme_cmd_count_);
}

void Translation::AbortPipeline() {
  pipeline_status_ = StatusCode::kUninitialized;
  nvme_cmd_count_ = 0;
  FlushMemory();
}

void Translation::FlushMemory() {
  for (uint32_t i = 0; i < nvme_cmd_count_; ++i) {
    if (allocations_[i].data_addr != 0) {
      DeallocPages(allocations_[i].data_addr, allocations_[i].data_page_count);
      allocations_[i].data_addr = 0;
    }
    if (allocations_[i].mdata_addr != 0) {
      DeallocPages(allocations_[i].mdata_addr,
                   allocations_[i].mdata_page_count);
      allocations_[i].mdata_addr = 0;
    }
  }
}

};  // namespace translator
