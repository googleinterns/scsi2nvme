// Copyright 2020 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "lib/translator/read.h"

#include <cmath>

#include "gtest/gtest.h"
#include "lib/translator/common.h"

namespace {

/*
   Tests the translator::read functions 
*/

constexpr uint32_t kNsid = 1234;
constexpr uint32_t kLba = 100;
constexpr uint16_t kShortTl = 255;
constexpr uint16_t kNumCommands = 4;
constexpr uint32_t kLongTl =
    kShortTl + (uint32_t)pow(2, 16) * (kNumCommands - 1);
constexpr uint8_t kRdProtect = 0b101;
constexpr uint8_t kPrinfo = 0b0111;  // expected transformation of kRdProtect
constexpr uint8_t kFua = 0b1;

scsi_defs::ControlByte BuildControlByte() {
  // 0b10010101
  return scsi_defs::ControlByte{
      .obsolete = 0b01,
      .naca = 0b1,
      .reserved = 0b010,
      .vendor_specific = 0b10,
  };
}

uint32_t BuildCdw13(uint16_t tl, uint8_t prinfo, bool fua) {
  uint32_t cdw13 = 0;
  cdw13 = tl;
  cdw13 |= (prinfo << 26);
  cdw13 |= (fua << 30);
  return cdw13;
}
/**
TEST(Read6, ShouldReturnInvalidInputStatus) {
  uint8_t raw_cmd[sizeof(scsi_defs::Read6Command) - 1];
  nvme_defs::GenericQueueEntryCmd nvme_cmd = {};

  translator::StatusCode sc = translator::Read6ToNvme(kNsid, raw_cmd, nvme_cmd);
  EXPECT_EQ(translator::StatusCode::kInvalidInput, sc);
}

TEST(Read6, ShouldReturnCorrect) {
  scsi_defs::Read6Command cmd = {
      .logical_block_address = kLba,
      .transfer_length = kShortTl,
      .control_byte = BuildControlByte(),
  };
  uint8_t raw_cmd[sizeof(scsi_defs::Read6Command)];
  translator::WriteValue(cmd, raw_cmd);
  nvme_defs::GenericQueueEntryCmd nvme_cmd = {};

  translator::StatusCode sc = translator::Read6ToNvme(kNsid, raw_cmd, nvme_cmd);
  EXPECT_EQ(translator::StatusCode::kSuccess, sc);
  EXPECT_EQ((uint8_t)nvme_defs::NvmOpcode::kRead, nvme_cmd.opc);
  EXPECT_EQ(kNsid, nvme_cmd.nsid);
  EXPECT_EQ(0, nvme_cmd.psdt);
  // EXPECT_TRUE( nvme_cmd.mptr != nullptr);
  // EXPECT_TRUE(nvme_cmd.dptr.prp.prp1 != nullptr);
  EXPECT_EQ(kLba, nvme_cmd.cdw[0]);
  EXPECT_EQ(kShortTl, nvme_cmd.cdw[2]);
}

TEST(Read10, ShouldReturnInvalidInputStatus) {
  uint8_t raw_cmd[sizeof(scsi_defs::Read10Command) - 1];
  nvme_defs::GenericQueueEntryCmd nvme_cmd = {};

  translator::StatusCode sc = translator::Read10ToNvme(kNsid, raw_cmd,
nvme_cmd); EXPECT_EQ(translator::StatusCode::kInvalidInput, sc);
}

TEST(Read10, ShouldReturnCorrect) {
  scsi_defs::Read10Command cmd = {
      .rd_protect = kRdProtect,
      .fua = kFua,
      .logical_block_address = kLba,
      .transfer_length = kShortTl,
      .control_byte = BuildControlByte(),
  };
  uint8_t raw_cmd[sizeof(scsi_defs::Read10Command)];
  translator::WriteValue(cmd, raw_cmd);
  nvme_defs::GenericQueueEntryCmd nvme_cmd = {};

  translator::StatusCode sc =
      translator::Read10ToNvme(kNsid, raw_cmd, nvme_cmd);

  EXPECT_EQ(translator::StatusCode::kSuccess, sc);
  EXPECT_EQ((uint8_t)nvme_defs::NvmOpcode::kRead, nvme_cmd.opc);
  EXPECT_EQ(kNsid, nvme_cmd.nsid);
  EXPECT_EQ(0, nvme_cmd.psdt);
  // EXPECT_TRUE( nvme_cmd.mptr != nullptr);
  // EXPECT_TRUE(nvme_cmd.dptr.prp.prp1 != nullptr);
  EXPECT_EQ(kLba, nvme_cmd.cdw[0]);
  EXPECT_EQ(BuildCdw13(kShortTl, kPrinfo, kFua), nvme_cmd.cdw[2]);
}

TEST(Read12, ShouldReturnInvalidInputStatus) {
  uint8_t raw_cmd[sizeof(scsi_defs::Read12Command) - 1];
  uint32_t nvme_cmd_count = 0;
  nvme_defs::GenericQueueEntryCmd nvme_cmds[nvme_cmd_count];

  translator::StatusCode sc = translator::Read12ToNvme(kNsid, raw_cmd,
nvme_cmd_count, nvme_cmds); EXPECT_EQ(translator::StatusCode::kInvalidInput,
sc);
}
*/

TEST(Read12, ShouldReturnCorrect) {
  scsi_defs::Read12Command cmd = {
      .rd_protect = kRdProtect,
      .fua = kFua,
      .logical_block_address = kLba,
      .transfer_length = kLongTl,
      .control_byte = BuildControlByte(),
  };
  uint8_t raw_cmd[sizeof(scsi_defs::Read12Command)];
  translator::WriteValue(cmd, raw_cmd);
  uint32_t nvme_cmd_count = 0;  // expect to be changed
  nvme_defs::GenericQueueEntryCmd nvme_cmds[nvme_cmd_count];

  translator::StatusCode sc =
      translator::Read12ToNvme(kNsid, raw_cmd, nvme_cmd_count, nvme_cmds);

  EXPECT_EQ(translator::StatusCode::kSuccess, sc);
  EXPECT_EQ(kNumCommands, nvme_cmd_count);

  for (int i = 0; i < kNumCommands; i++) {
    nvme_defs::GenericQueueEntryCmd nvme_cmd = nvme_cmds[i];
    uint16_t tl = (i == 0) ? kShortTl : (uint16_t)pow(2, 16);

    EXPECT_EQ((uint8_t)nvme_defs::NvmOpcode::kRead, nvme_cmd.opc);
    EXPECT_EQ(kNsid, nvme_cmd.nsid);
    EXPECT_EQ(0, nvme_cmd.psdt);
    EXPECT_EQ(kLba, nvme_cmd.cdw[0]);

    // EXPECT_EQ(BuildCdw13(kShortTl, kPrinfo, kFua), nvme_cmd.cdw[2]);
  }
}

}  // namespace
