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
#include "inquiry.h"

#include "request_sense.h"

#include "read.h"

namespace translator {

BeginResponse Translation::Begin(Span<const uint8_t> scsi_cmd,
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

  uint32_t nsid = static_cast<uint32_t>(lun) + 1;
  Span<const uint8_t> scsi_cmd_no_op = scsi_cmd.subspan(1);
  scsi::OpCode opc = static_cast<scsi::OpCode>(scsi_cmd[0]);
  switch (opc) {
    case scsi::OpCode::kInquiry:
      pipeline_status_ =
          InquiryToNvme(scsi_cmd_no_op, nvme_cmds_[0], nvme_cmds_[1],
                        response.alloc_len, lun, allocations_);
      nvme_cmd_count_ = 2;
      break;
    case scsi::OpCode::kRequestSense:
      pipeline_status_ = RequestSenseToNvme(scsi_cmd_no_op, response.alloc_len);
      break;
    case scsi::OpCode::kRead6:
      pipeline_status_ =
          Read6ToNvme(scsi_cmd_no_op, nvme_cmds_, allocations_, nsid);
      nvme_cmd_count_ = 2;
      break;
    case scsi::OpCode::kRead10:
      pipeline_status_ =
          Read10ToNvme(scsi_cmd_no_op, nvme_cmds_, allocations_, nsid);
      nvme_cmd_count_ = 2;
      break;
    case scsi::OpCode::kRead12:
      pipeline_status_ =
          Read12ToNvme(scsi_cmd_no_op, nvme_cmds_, allocations_, nsid);
      nvme_cmd_count_ = 2;
      break;
    case scsi::OpCode::kRead16:
      pipeline_status_ =
          Read16ToNvme(scsi_cmd_no_op, nvme_cmds_, allocations_, nsid);
      nvme_cmd_count_ = 2;
      break;
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

ApiStatus Translation::Complete(Span<const nvme::GenericQueueEntryCpl> cpl_data,
                                Span<uint8_t> buffer) {
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
  Span<const uint8_t> scsi_cmd_no_op = scsi_cmd_.subspan(1);
  scsi::OpCode opc = static_cast<scsi::OpCode>(scsi_cmd_[0]);
  switch (opc) {
    case scsi::OpCode::kInquiry:
      pipeline_status_ = InquiryToScsi(scsi_cmd_no_op, buffer, GetNvmeCmds());
      ret = ApiStatus::kSuccess;
      break;
    case scsi::OpCode::kRequestSense:
      pipeline_status_ = RequestSenseToScsi(scsi_cmd_no_op, buffer);
      ret = ApiStatus::kSuccess;
      break;
  }
  AbortPipeline();
  return ret;
}

Span<const nvme::GenericQueueEntryCmd> Translation::GetNvmeCmds() {
  return Span(nvme_cmds_, nvme_cmd_count_);
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
