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
#include "maintenance_in.h"
#include "mode_sense.h"
#include "read.h"
#include "read_capacity_10.h"
#include "report_luns.h"
#include "request_sense.h"
#include "status.h"
#include "synchronize_cache.h"
#include "unmap.h"
#include "verify.h"

namespace translator {

constexpr uint32_t kPageSize = 4096;
constexpr uint32_t kLbaSize = 4096;

BeginResponse Translation::Begin(Span<const uint8_t> scsi_cmd,
                                 Span<const uint8_t> buffer_out,
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
    DebugLog("Empty SCSI Command Buffer");
    pipeline_status_ = StatusCode::kFailure;
    return response;
  }

  pipeline_status_ = StatusCode::kSuccess;
  scsi_cmd_ = scsi_cmd;
  DebugLog("Translating command %s with opcode %#x",
           ScsiOpcodeToString((scsi::OpCode)(scsi_cmd[0])), scsi_cmd[0]);
  DebugLog("LUN: %u", lun);
  uint32_t nsid = static_cast<uint32_t>(lun) + 1;
  Span<const uint8_t> scsi_cmd_no_op = scsi_cmd.subspan(1);
  scsi::OpCode opc = static_cast<scsi::OpCode>(scsi_cmd[0]);
  switch (opc) {
    case scsi::OpCode::kInquiry:
      pipeline_status_ =
          InquiryToNvme(scsi_cmd_no_op, nvme_wrappers_[0], nvme_wrappers_[1],
                        response.alloc_len, nsid, allocations_);
      nvme_cmd_count_ = 2;
      break;
    case scsi::OpCode::kUnmap:
      pipeline_status_ = UnmapToNvme(scsi_cmd_no_op, buffer_out,
                                     nvme_wrappers_[0], allocations_[0], nsid);
      nvme_cmd_count_ = 1;
    case scsi::OpCode::kModeSense6:
      pipeline_status_ =
          ModeSense6ToNvme(scsi_cmd_no_op, nvme_wrappers_, allocations_[0],
                           nsid, nvme_cmd_count_, response.alloc_len);
      break;
    case scsi::OpCode::kModeSense10:
      pipeline_status_ =
          ModeSense10ToNvme(scsi_cmd_no_op, nvme_wrappers_, allocations_[0],
                            nsid, nvme_cmd_count_, response.alloc_len);
    case scsi::OpCode::kMaintenanceIn:
      // ReportSupportedOpCodes is the only supported MaintenanceIn command
      pipeline_status_ =
          ValidateReportSupportedOpCodes(scsi_cmd_no_op, response.alloc_len);
      nvme_cmd_count_ = 0;
    case scsi::OpCode::kReportLuns:
      pipeline_status_ = ReportLunsToNvme(scsi_cmd_no_op, nvme_wrappers_[0],
                                          allocations_[0], response.alloc_len);
      nvme_cmd_count_ = 1;
      break;
    case scsi::OpCode::kReadCapacity10:
      pipeline_status_ =
          ReadCapacity10ToNvme(scsi_cmd_no_op, nvme_wrappers_[0], nsid,
                               allocations_[0], response.alloc_len);
      nvme_cmd_count_ = 1;
      break;
    case scsi::OpCode::kRequestSense:
      pipeline_status_ = RequestSenseToNvme(scsi_cmd_no_op, response.alloc_len);
      break;
    case scsi::OpCode::kRead6:
      pipeline_status_ =
          Read6ToNvme(scsi_cmd_no_op, nvme_wrappers_[0], allocations_[0], nsid,
                      kPageSize, kLbaSize, response.alloc_len);
      nvme_cmd_count_ = 1;
      break;
    case scsi::OpCode::kRead10:
      pipeline_status_ =
          Read10ToNvme(scsi_cmd_no_op, nvme_wrappers_[0], allocations_[0], nsid,
                       kPageSize, kLbaSize, response.alloc_len);
      nvme_cmd_count_ = 1;
      break;
    case scsi::OpCode::kRead12:
      pipeline_status_ =
          Read12ToNvme(scsi_cmd_no_op, nvme_wrappers_[0], allocations_[0], nsid,
                       kPageSize, kLbaSize, response.alloc_len);
      nvme_cmd_count_ = 1;
      break;
    case scsi::OpCode::kRead16:
      pipeline_status_ =
          Read16ToNvme(scsi_cmd_no_op, nvme_wrappers_[0], allocations_[0], nsid,
                       kPageSize, kLbaSize, response.alloc_len);
      nvme_cmd_count_ = 1;
      break;
    case scsi::OpCode::kSync10:
      SynchronizeCache10ToNvme(nvme_wrappers_[0], nsid);
      pipeline_status_ = StatusCode::kSuccess;
      nvme_cmd_count_ = 1;
      break;
    case scsi::OpCode::kVerify10:
      pipeline_status_ = VerifyToNvme(scsi_cmd_no_op, nvme_wrappers_[0]);
      nvme_cmd_count_ = 1;
      break;
    case scsi::OpCode::kTestUnitReady:
      // Always return NVMe device is ready
      // The implementation of actually querying readiness of NVMe device does
      // not fit with our Library and engine design and is of little use
      pipeline_status_ = StatusCode::kSuccess;
      break;
    default:
      DebugLog("Bad OpCode: %#x", static_cast<uint8_t>(opc));
      pipeline_status_ = StatusCode::kFailure;
      break;
  }

  if (pipeline_status_ != StatusCode::kSuccess) {
    FlushMemory();
    nvme_cmd_count_ = 0;
  }
  return response;
}

CompleteResponse Translation::Complete(
    Span<const nvme::GenericQueueEntryCpl> cpl_data, Span<uint8_t> buffer_in,
    Span<uint8_t> sense_buffer) {
  CompleteResponse resp = {};
  if (pipeline_status_ == StatusCode::kUninitialized) {
    DebugLog("Invalid use of API: Complete called before Begin");
    resp.status = ApiStatus::kFailure;
    return resp;
  }

  if (cpl_data.size() != nvme_cmd_count_) {
    DebugLog(
        "Invalid use of API, completion count %u does not equal command count "
        "%u",
        cpl_data.size(), nvme_cmd_count_);
    AbortPipeline();
    resp.status = ApiStatus::kFailure;
    return resp;
  }

  if (pipeline_status_ != StatusCode::kSuccess) {
    ScsiStatus scsi_status = {
        .status = scsi::Status::kCheckCondition,
        .sense_key = scsi::SenseKey::kIllegalRequest,
        .asc = scsi::AdditionalSenseCode::kInvalidFieldInCdb,
        .ascq = scsi::AdditionalSenseCodeQualifier::kNoAdditionalSenseInfo};
    FillSenseBuffer(sense_buffer, scsi_status);
    AbortPipeline();
    resp.status = ApiStatus::kSuccess;
    resp.scsi_status = scsi_status.status;
    return resp;
  }

  // Verify NVMe commands completed successfully. If not translate error.
  for (uint32_t i = 0; i < cpl_data.size(); ++i) {
    const nvme::GenericQueueEntryCpl& cpl_entry = cpl_data[i];
    const nvme::CplStatus& cpl_status = cpl_entry.cpl_status;
    ScsiStatus scsi_status = StatusToScsi(cpl_status.sct, cpl_status.sc);
    if (scsi_status.status != scsi::Status::kGood) {
      FillSenseBuffer(sense_buffer, scsi_status);
      AbortPipeline();
      resp.status = ApiStatus::kSuccess;
      resp.scsi_status = scsi_status.status;
      return resp;
    }
  }

  // Switch cases should not return
  resp.status = ApiStatus::kSuccess;
  Span<const uint8_t> scsi_cmd_no_op = scsi_cmd_.subspan(1);
  scsi::OpCode opc = static_cast<scsi::OpCode>(scsi_cmd_[0]);
  switch (opc) {
    case scsi::OpCode::kVerify10:
      // VerifyToScsi() is not needed
      break;
    case scsi::OpCode::kInquiry:
      pipeline_status_ =
          InquiryToScsi(scsi_cmd_no_op, buffer_in, nvme_wrappers_[0].cmd,
                        nvme_wrappers_[1].cmd);
      break;
    case scsi::OpCode::kModeSense6:
      // TODO: Update this when the cpl_data interface is finalized
      ModeSense6ToScsi(scsi_cmd_no_op, nvme_wrappers_[0].cmd, cpl_data[0].cdw0,
                       buffer_in);
      break;
    case scsi::OpCode::kModeSense10:
      // TODO: Update this when the cpl_data interface is finalized
      ModeSense10ToScsi(scsi_cmd_no_op, nvme_wrappers_[0].cmd, cpl_data[0].cdw0,
                        buffer_in);
      break;
    case scsi::OpCode::kMaintenanceIn:
      // ReportSupportedOpCodes is the only supported MaintenanceIn command
      WriteReportSupportedOpCodesResult(buffer_in);
      break;
    case scsi::OpCode::kReportLuns:
      pipeline_status_ = ReportLunsToScsi(nvme_wrappers_[0].cmd, buffer_in);
      break;
    case scsi::OpCode::kUnmap:
      pipeline_status_ = StatusCode::kSuccess;
      break;
    case scsi::OpCode::kReadCapacity10:
      pipeline_status_ = ReadCapacity10ToScsi(buffer_in, nvme_wrappers_[0].cmd);
      break;
    case scsi::OpCode::kRequestSense:
      pipeline_status_ = RequestSenseToScsi(scsi_cmd_no_op, buffer_in);
      break;
    case scsi::OpCode::kRead6:
    case scsi::OpCode::kRead10:
    case scsi::OpCode::kRead12:
    case scsi::OpCode::kRead16:
      pipeline_status_ = ReadToScsi(buffer_in, nvme_wrappers_[0].cmd, kLbaSize);
      break;
    case scsi::OpCode::kSync10:
      // No command specific response data to translate
      pipeline_status_ = StatusCode::kSuccess;
      break;
    case scsi::OpCode::kTestUnitReady:
      break;
    default:
      DebugLog(
          "Invalid opcode case reached: %u. Please contact SCSI2NVMe devs.",
          scsi_cmd_[0]);
      break;
  }
  if (pipeline_status_ != StatusCode::kSuccess) {
    // TODO fill buffer with SCSI CHECK CONDITION response
    DebugLog("Failed to translate back to SCSI");
  }
  AbortPipeline();
  return resp;
}

Span<const NvmeCmdWrapper> Translation::GetNvmeWrappers() {
  return Span<NvmeCmdWrapper>(nvme_wrappers_, nvme_cmd_count_);
}

void Translation::AbortPipeline() {
  pipeline_status_ = StatusCode::kUninitialized;
  FlushMemory();
  nvme_cmd_count_ = 0;
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
