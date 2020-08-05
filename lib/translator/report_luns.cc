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

#include "report_luns.h"

#ifdef __KERNEL__
#include <linux/byteorder/generic.h>
#else
#include <netinet/in.h>
#endif

#include <byteswap.h>

namespace translator {

namespace {

// Return the number of valid namespace IDs in a namespace ID list.
uint32_t GetNsListLength(const nvme::IdentifyNamespaceList& ns_list) {
  uint32_t size = 0;
  for (uint32_t i = 0; i < nvme::kIdentifyNsListMaxLength; ++i) {
    DebugLog("Potential NSID: %u", ns_list.ids[i]);
    if (ns_list.ids[i] == 0) break;
    ++size;
  }
  return size;
}

}  // namespace

// Section 4.5
// https://www.nvmexpress.org/wp-content/uploads/NVM-Express-SCSI-Translation-Reference-1_1-Gold.pdf
StatusCode ReportLunsToNvme(Span<const uint8_t> scsi_cmd,
                            nvme::GenericQueueEntryCmd& identify_cmd,
                            Allocation& allocation, uint32_t& alloc_len) {
  // Cast scsi_cmd to ReportLunsCommand
  scsi::ReportLunsCommand rl_cmd;
  if (!ReadValue(scsi_cmd, rl_cmd)) {
    DebugLog("Malformed ReportLuns command");
    return StatusCode::kInvalidInput;
  }
  if (rl_cmd.select_report != scsi::SelectReport::kRestrictedMethods &&
      rl_cmd.select_report != scsi::SelectReport::kWellKnown &&
      rl_cmd.select_report != scsi::SelectReport::kAllLogical) {
    DebugLog("Invalid report luns select report %u", rl_cmd.select_report);
    return StatusCode::kInvalidInput;
  }

  // Assign allocation length for downstream use
  alloc_len = ntohl(rl_cmd.alloc_length);

  // Assign generic params to NVMe command
  identify_cmd = {};
  identify_cmd.opc = static_cast<uint8_t>(nvme::AdminOpcode::kIdentify);
  identify_cmd.cdw[0] = 0x10;  // Set CNS to return namespace ID list

  // Allocate prp & assign to command
  if (allocation.SetPages(1, 0) == StatusCode::kFailure)
    return StatusCode::kFailure;
  identify_cmd.dptr.prp.prp1 = allocation.data_addr;
  return StatusCode::kSuccess;
}

// Section 6.6
// https://www.nvmexpress.org/wp-content/uploads/NVM-Express-SCSI-Translation-Reference-1_1-Gold.pdf
StatusCode ReportLunsToScsi(const nvme::GenericQueueEntryCmd& identify_cmd,
                            Span<uint8_t> buffer) {
  if (buffer.size() < sizeof(scsi::ReportLunsParamData)) {
    DebugLog("Insufficient buffer size");
    return StatusCode::kFailure;
  }

  // Get ns_list and lun count from NVMe dptr
  uint8_t* ns_dptr = reinterpret_cast<uint8_t*>(identify_cmd.dptr.prp.prp1);
  Span<uint8_t> ns_span(ns_dptr, sizeof(nvme::IdentifyNamespaceList));
  const nvme::IdentifyNamespaceList* ns_list;
  ns_list = SafePointerCastRead<nvme::IdentifyNamespaceList>(ns_span);
  if (ns_list == nullptr) {
    DebugLog("Namespace pointer was null");
    return StatusCode::kFailure;
  }

  // TODO: TEMP TESTING.
  //uint32_t lun_count = GetNsListLength(*ns_list);
  uint32_t lun_count = 1;

  // Define scsi response structure and determine attributes
  uint32_t allocated_list_bytes =
      buffer.size() - sizeof(scsi::ReportLunsParamData);
  scsi::ReportLunsParamData rlpd = {};
  rlpd.list_byte_length = htonl(lun_count * sizeof(scsi::LunAddress));
  if (ntohl(rlpd.list_byte_length) > allocated_list_bytes) {
    uint32_t lbl = allocated_list_bytes -
                   (allocated_list_bytes % sizeof(scsi::LunAddress));
    rlpd.list_byte_length = htonl(lbl);
    lun_count = lbl / sizeof(scsi::LunAddress);
  }
  
  DebugLog("LUN Count: %u", lun_count);

  // Write response to buffer
  if (!WriteValue(rlpd, buffer)) {
    DebugLog("Buffer not large enough for report luns response header");
    return StatusCode::kSuccess;
  }
  /*uint32_t lun_offset = sizeof(scsi::ReportLunsParamData);
  for (uint32_t i = 0; i < lun_count; ++i) {
    scsi::LunAddress lun = htonll(static_cast<scsi::LunAddress>(
        ltohl(ns_list->ids[i]) - 1));  // LUN must start @ 0 via SAM spec
    DebugLog("Reporting LUN: %u", ntohll(lun));
    if (!WriteValue(
            lun, buffer.subspan(lun_offset + i * sizeof(scsi::LunAddress)))) {
      DebugLog("Truncating report luns response at position %u", i);
      return StatusCode::kSuccess;
    }
  }*/
  // TODO: TEMP TESTING
  scsi::LunAddress lun = htonll(0);
  WriteValue(lun, buffer);

  DebugLog("Lun List Length: %u", ntohl(rlpd.list_byte_length));

  return StatusCode::kSuccess;
}

};  // namespace translator
