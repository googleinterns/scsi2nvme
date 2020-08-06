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

#include "verify.h"

#include <byteswap.h>
#include <netinet/in.h>

namespace translator {

namespace {
void BuildPrInfo(uint8_t bytchk, uint8_t vr_protect, uint8_t& pr_info) {
  uint8_t prchk = 0;
  if (bytchk == 0) {
    switch (vr_protect) {
      case 0b000:
      case 0b001:
      case 0b101:
        prchk = 0b111;
        break;
      case 0b010:
        prchk = 0b011;
        break;
      case 0b011:
        prchk = 0b000;
        break;
      case 0b100:
        prchk = 0b100;
        break;
    }
  } else if (bytchk == 1) {
    switch (vr_protect) {
      case 0b000:
        prchk = 0b111;
        break;
      case 0b001:
      case 0b010:
      case 0b011:
      case 0b100:
      case 0b101:
        prchk = 0;
        break;
    }
  }
  pr_info = 0b1000 | prchk;
}

}  // namespace

StatusCode VerifyToNvme(translator::Span<const uint8_t> scsi_cmd,
                        NvmeCmdWrapper& nvme_wrapper) {
  scsi::Verify10Command verify_cmd{};
  if (!ReadValue(scsi_cmd, verify_cmd)) {
    DebugLog("Malformed Verify Command - ReadValue Failure");
    return StatusCode::kInvalidInput;
  }

  // verification length of 0 is a no-op
  if (verify_cmd.verification_length == 0) {
    DebugLog("Verify Command is a No-Op");
    return StatusCode::kNoTranslation;
  }

  if (verify_cmd.control_byte.naca == 1) {
    DebugLog("Malformed Verify Command - Control Byte NACA is 0b1");
    return StatusCode::kInvalidInput;
  }

  // Support for VRPROTECT requires setting PRACT and PRCHK fields of the NVM
  // Express command

  uint8_t pr_info = 0;
  BuildPrInfo(verify_cmd.bytchk, verify_cmd.vr_protect, pr_info);

  // https://nvmexpress.org/wp-content/uploads/NVM-Express-1_4-2019.06.10-Ratified.pdf
  // Figure 355: Protection Information Field

  // Disable page out (DPO) specifies retention characteristics which are not
  // supported in NVM Express Bytchk -- Support requires translation to FUA
  // field of the NVM Express command.

  // NOTE: Bytchk does not seem to translate nicely. have defaulted FUA to off.

  // Group Number -- support unspecified
  nvme_wrapper.cmd = nvme::GenericQueueEntryCmd{
      .opc = static_cast<uint8_t>(nvme::NvmOpcode::kCompare),
      .cdw = {
          // Support requires translation to Starting LBA field of NVM Express
          // command.

          // Starting LBA (SLBA): This field indicates the 64-bit address of the
          // first logical block of data
          // to be verified as part of the operation. Command Dword 10 contains
          // bits 31:00; Command
          // Dword 11 contains bits 63: 32.
          __bswap_32(verify_cmd.logical_block_address),  // cdw 10
          0,

          // Support requires translation to Number of Logical Blocks (NLB)
          // field of the NVM Express command.
          // Number of Logical Blocks (NLB): This field indicates the number of
          // logical blocks to be
          // verified. This is a 0’s based value.

          // bits 15:00 is NLB
          // bits 29:26 is PRINFO
          htoll((ntohs(verify_cmd.verification_length) - 1) |
                ((pr_info) << 26)),  // cdw 12
      }};

  nvme_wrapper.is_admin = false;
  return StatusCode::kSuccess;
}

}  // namespace translator
