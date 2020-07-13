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

#include <cstring>

namespace translator {

// command specific helpers
namespace {

}  // namespace

StatusCode ReadCapacity10ToNvme(absl::Span<const uint8_t> raw_scsi,
                         nvme::GenericQueueEntryCmd& identify_ns,
                         nvme::GenericQueueEntryCmd& identify_ctrl,
                         scsi::LunAddress lun) {
  scsi::ReadCapacity10Command cmd = {};
  if (!ReadValue(raw_scsi, cmd)) {
    DebugLog("Malformed ReadCapacity10 Command");
    return StatusCode::kInvalidInput;
  };

  // Allocate prp & assign to command
  uint64_t ns_prp = AllocPages(1);
  if (!ns_prp) return StatusCode::kFailure;

  identify_ns = nvme::GenericQueueEntryCmd{
      .opc = static_cast<uint8_t>(nvme::AdminOpcode::kIdentify), .nsid = lun};
  identify_ns.dptr.prp.prp1 = ns_prp;
  identify_ns.cdw[0] = 0x0;  // cns val

  uint64_t ctrl_prp = AllocPages(1);
  if (!ctrl_prp) return StatusCode::kFailure;

  identify_ctrl = nvme::GenericQueueEntryCmd{
      .opc = static_cast<uint8_t>(nvme::AdminOpcode::kIdentify),
  };
  identify_ctrl.dptr.prp.prp1 = ctrl_prp;
  identify_ctrl.cdw[0] = 0x1;  // cns val
  return StatusCode::kSuccess;
}

StatusCode ReadCapacity10ToScsi(
    absl::Span<const uint8_t> raw_scsi, absl::Span<uint8_t> buffer,
    absl::Span<const nvme::GenericQueueEntryCmd> nvme_cmds) {
  scsi::ReadCapacity10Command read_capacity_10_cmd = {};

  if (!ReadValue(raw_scsi, read_capacity_10_cmd)) {
    DebugLog("Malformed ReadCapacity10 Command");
    return StatusCode::kInvalidInput;
  };

  uint8_t* ctrl_dptr = reinterpret_cast<uint8_t*>(nvme_cmds[0].dptr.prp.prp1);
  auto ctrl_span =
      absl::MakeSpan(ctrl_dptr, sizeof(nvme::IdentifyControllerData));

  uint8_t* ns_dptr = reinterpret_cast<uint8_t*>(nvme_cmds[1].dptr.prp.prp1);
  auto ns_span = absl::MakeSpan(ns_dptr, sizeof(nvme::IdentifyNamespace));

  nvme::IdentifyControllerData identify_ctrl = {};
  if (!ReadValue(ctrl_span, identify_ctrl)) {
    DebugLog("Malformed IdentifyController Data");
    return StatusCode::kInvalidInput;
  };

  nvme::IdentifyNamespace identify_ns = {};
  if (!ReadValue(ns_span, identify_ns)) {
    DebugLog("Malformed IdentifyNamespace Data");
    return StatusCode::kInvalidInput;
  };

  // nsid should come from Namespace
  uint32_t nsid = nvme_cmds[1].nsid;

  return StatusCode::kSuccess;
}

};  // namespace translator
