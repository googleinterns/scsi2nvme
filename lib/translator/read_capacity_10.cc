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

#include "read_capacity_10.h"

#ifdef __KERNEL__
#include <linux/byteorder/generic.h>
#else
#include <netinet/in.h>
#endif

namespace translator {

StatusCode ReadCapacity10ToNvme(Span<const uint8_t> raw_scsi,
                                NvmeCmdWrapper& wrapper, uint32_t page_size,
                                uint32_t nsid, Allocation& allocation,
                                uint32_t& alloc_len) {
  scsi::ReadCapacity10Command cmd = {};
  if (!ReadValue(raw_scsi, cmd)) {
    DebugLog("Malformed ReadCapacity10 Command - Error in reading to buffer");
    return StatusCode::kInvalidInput;
  };

  if (cmd.control_byte.naca == 1) {
    DebugLog("Malformed ReadCapacity10 Command - Invalid NACA bit");
    return StatusCode::kInvalidInput;
  }

  // READ CAPACITY (10) requests that the device
  // server transfer 8 bytes of parameter data describing the capacity
  // and medium format of the direct-access block device to the data-in buffer.
  alloc_len = 8;

  uint16_t num_pages = 1;
  // Allocate prp & assign to command
  auto read_status = allocation.SetPages(page_size, num_pages, 0);
  if (read_status != StatusCode::kSuccess) {
    return read_status;
  }

  wrapper.cmd = nvme::GenericQueueEntryCmd{
      .opc = static_cast<uint8_t>(nvme::AdminOpcode::kIdentify), .nsid = nsid};
  wrapper.cmd.dptr.prp.prp1 = allocation.data_addr;
  wrapper.cmd.cdw[0] =
      0x0;  // Controller or Namespace Structure (CNS): This field specifies the
            // information to be returned to the host.

  wrapper.buffer_len = page_size * num_pages;
  wrapper.is_admin = true;
  return StatusCode::kSuccess;
}

StatusCode ReadCapacity10ToScsi(
    Span<uint8_t> buffer, const nvme::GenericQueueEntryCmd& gen_identify_ns) {
  uint8_t* ns_dptr = reinterpret_cast<uint8_t*>(gen_identify_ns.dptr.prp.prp1);
  Span<uint8_t> ns_span(ns_dptr, sizeof(nvme::IdentifyNamespace));

  const nvme::IdentifyNamespace* identify_ns =
      SafePointerCastRead<nvme::IdentifyNamespace>(ns_span);
  if (identify_ns == nullptr) {
    DebugLog("Identify namespace structure failed to cast");
    return StatusCode::kFailure;
  }

  scsi::ReadCapacity10Data result = {
      .returned_logical_block_address =
          htonl(ltohll(identify_ns->nsze) > 0xffffffff
                    ? 0xffffffff
                    : static_cast<uint32_t>(ltohll(identify_ns->nsze))),
  };

  uint8_t lbads = identify_ns->lbaf[identify_ns->flbas.format].lbads;
  if (lbads < 9) {
    DebugLog("lbads value smaller than 9 is not supported");
    return StatusCode::kFailure;
  } else if (lbads > 31) {
    DebugLog(
        "lbads exceeds type limit of "
        "scsi::ReadCapacity10Data.block_length");
    return StatusCode::kFailure;
  }
  result.block_length = htonl(static_cast<uint32_t>(1 << lbads));

  if (!WriteValue(result, buffer)) {
    DebugLog("Error writing Read Capacity 10 Data to buffer");
    return StatusCode::kFailure;
  }
  return StatusCode::kSuccess;
}

};  // namespace translator
