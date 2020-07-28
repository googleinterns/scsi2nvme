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

namespace translator {

StatusCode ReadCapacity10ToNvme(Span<const uint8_t> raw_scsi,
                                nvme::GenericQueueEntryCmd& identify_ns,
                                uint32_t nsid, Allocation& allocation) {
  scsi::ReadCapacity10Command cmd = {};
  if (!ReadValue(raw_scsi, cmd)) {
    DebugLog("Malformed ReadCapacity10 Command - Error in reading to buffer");
    return StatusCode::kInvalidInput;
  };

  if (cmd.control_byte.naca == 1) {
    DebugLog("Malformed ReadCapacity10 Command - Invalid NACA bit");
    return StatusCode::kInvalidInput;
  }

  // Allocate prp & assign to command
  auto read_status = allocation.SetPages(1, 0);
  if (read_status != StatusCode::kSuccess) {
    return read_status;
  }

  identify_ns = nvme::GenericQueueEntryCmd{
      .opc = static_cast<uint8_t>(nvme::AdminOpcode::kIdentify), .nsid = nsid};
  identify_ns.dptr.prp.prp1 = allocation.data_addr;
  identify_ns.cdw[0] =
      0x0;  // Controller or Namespace Structure (CNS): This field specifies the
            // information to be returned to the host.

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
          identify_ns->nsze > 0xffffffff ? 0xffffffff : identify_ns->nsze,
      .block_length = identify_ns->lbaf[identify_ns->flbas.format].lbads,
  };

  WriteValue(result, buffer);
  return StatusCode::kSuccess;
}

};  // namespace translator
