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

#include "unmap.h"

#ifdef __KERNEL__
#include <linux/byteorder/generic.h>
#else
#include <netinet/in.h>
#endif

#include <byteswap.h>

namespace translator {

// Section 5.6
// https://www.nvmexpress.org/wp-content/uploads/NVM-Express-SCSI-Translation-Reference-1_1-Gold.pdf
StatusCode UnmapToNvme(Span<const uint8_t> scsi_cmd,
                       Span<const uint8_t> buffer_out,
                       NvmeCmdWrapper& nvme_wrapper, uint32_t page_size,
                       uint32_t nsid, Allocation& allocation) {
  scsi::UnmapCommand unmap_cmd;
  if (!ReadValue(scsi_cmd, unmap_cmd)) {
    DebugLog("Malformed unmap command");
    return StatusCode::kFailure;
  }
  if (ntohs(unmap_cmd.param_list_length) < sizeof(scsi::UnmapParamList)) {
    DebugLog("Insufficient unmap parameter list length");
    return StatusCode::kFailure;
  }
  if (unmap_cmd.anchor == 1) {
    DebugLog("Unsupported unmap anchor request");
    return StatusCode::kNoTranslation;
  }

  // Copy unmap parameter list into memory
  scsi::UnmapParamList param_list;
  if (!ReadValue(buffer_out, param_list)) {
    DebugLog("Malformed unmap parameter list");
    return StatusCode::kFailure;
  }
  buffer_out = buffer_out.subspan(sizeof(scsi::UnmapParamList));

  // Ensure that block descriptor data length & span length align
  uint16_t bd_data_length = ntohs(param_list.block_desc_data_length);
  if (buffer_out.size() < bd_data_length) {
    DebugLog("Block descriptor list length reported longer than buffer");
    return StatusCode::kFailure;
  }
  uint32_t data_length_remainder =
      bd_data_length % sizeof(scsi::UnmapBlockDescriptor);
  if (data_length_remainder != 0) {
    DebugLog("Non-divisible unmap block descriptor data length %u",
             bd_data_length);
    return StatusCode::kInvalidInput;
  }
  uint32_t block_descriptor_count =
      bd_data_length / sizeof(scsi::UnmapBlockDescriptor);
  if (block_descriptor_count == 0 || block_descriptor_count > 256) {
    DebugLog("Unsupported unmap block descriptor count %u",
             block_descriptor_count);
    return StatusCode::kNoTranslation;
  }

  // Copy block descriptors into structures
  scsi::UnmapBlockDescriptor block_descriptors[block_descriptor_count];
  for (uint32_t i = 0; i < block_descriptor_count; ++i) {
    if (!ReadValue(buffer_out, block_descriptors[i])) {
      DebugLog("Failed to read unmap block descriptor");
      return StatusCode::kFailure;
    }
    buffer_out = buffer_out.subspan(sizeof(block_descriptors[i]));
  }

  uint16_t num_pages = 1;
  // Copy block descriptor data into allocated nvme data buffer
  if (allocation.SetPages(page_size, num_pages, 0) == StatusCode::kFailure)
    return StatusCode::kFailure;
  uint8_t* dmr_ptr = reinterpret_cast<uint8_t*>(allocation.data_addr);
  Span<uint8_t> dmr_span(
      dmr_ptr, sizeof(nvme::DatasetManagmentRange) * block_descriptor_count);
  for (uint32_t i = 0; i < block_descriptor_count; ++i) {
    nvme::DatasetManagmentRange* dme =
        SafePointerCastWrite<nvme::DatasetManagmentRange>(dmr_span);
    if (dme == nullptr) {
      DebugLog("Failed to cast dataset managment pointer");
      return StatusCode::kFailure;
    }
    dme->lba = bswap_64(block_descriptors[i].logical_block_addr);
    dme->lb_count = bswap_32(block_descriptors[i].logical_block_count);
    dmr_span = dmr_span.subspan(sizeof(nvme::DatasetManagmentRange));
  }

  // Create NVMe command
  uint8_t bd_count_byte = static_cast<uint8_t>(block_descriptor_count - 1);
  nvme::DatasetManagementCmd dataset_cmd = {
      .opc = static_cast<uint8_t>(nvme::NvmOpcode::kDatasetManagement),
      .nsid = nsid,
      .nr = bd_count_byte,  // (1's based -> 0's based)
      .ad = 1};
  dataset_cmd.dptr.prp.prp1 = allocation.data_addr,
  memcpy(&nvme_wrapper.cmd, &dataset_cmd, sizeof(nvme_wrapper.cmd));

  nvme_wrapper.buffer_len = page_size * num_pages;
  nvme_wrapper.is_admin = true;
  static_assert(sizeof(nvme_wrapper.cmd) == sizeof(dataset_cmd));

  return StatusCode::kSuccess;
}

}  // namespace translator
