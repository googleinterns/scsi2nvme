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

#include "gtest/gtest.h"

namespace {

/*
   Tests the translator::Read6, Read10, Read12, Read16, Read 32 functions
*/

constexpr uint32_t kLba = 100;
constexpr uint16_t kTl = 255;
constexpr uint8_t kRdProtect = 0b101;
constexpr uint8_t kPrinfo = 0b0111;  // expected transformation of kRdProtect
constexpr uint8_t kUnsupportedRdProtect = 0b111;
constexpr uint8_t kFua = 0b1;
constexpr uint32_t kEilbrt = 1234;
constexpr uint16_t kElbat = 5678;
constexpr uint16_t kLbatm = 9999;

class ReadTest : public ::testing::Test {
 protected:
  // Per-test-suite set-up.
  // Called before the first test in this test suite.
  static void SetUpTestSuite() {
    auto alloc_callback = [](uint16_t count) -> void* {
      EXPECT_EQ(1, count);
      return reinterpret_cast<void*>(1337);
    };
    void (*dealloc_callback)(void*, uint16_t) = nullptr;
    translator::SetAllocPageCallbacks(alloc_callback, dealloc_callback);
  }
};

uint32_t BuildCdw13(uint16_t tl, uint8_t prinfo, bool fua) {
  uint32_t cdw13 = 0;
  cdw13 = tl;
  cdw13 |= (prinfo << 26);
  cdw13 |= (fua << 30);
  return cdw13;
}

TEST_F(ReadTest, Read6ShouldReturnInvalidInputStatus) {
  uint8_t raw_cmd[sizeof(scsi_defs::Read6Command) - 1];
  nvme_defs::GenericQueueEntryCmd nvme_cmd;

  translator::StatusCode sc = translator::Read6ToNvme( raw_cmd, nvme_cmd);
  EXPECT_EQ(translator::StatusCode::kInvalidInput, sc);
}

TEST_F(ReadTest, Read6ShouldReturnCorrectTranslation) {
  scsi_defs::Read6Command cmd = {
      .logical_block_address = kLba,
      .transfer_length = kTl,
  };
  uint8_t raw_cmd[sizeof(scsi_defs::Read6Command)];
  translator::WriteValue(cmd, raw_cmd);
  nvme_defs::GenericQueueEntryCmd nvme_cmd;

  translator::StatusCode sc = translator::Read6ToNvme( raw_cmd, nvme_cmd);
  EXPECT_EQ(translator::StatusCode::kSuccess, sc);
  EXPECT_EQ((uint8_t)nvme_defs::NvmOpcode::kRead, nvme_cmd.opc);
  EXPECT_EQ(0, nvme_cmd.psdt);
  EXPECT_EQ(kLba, nvme_cmd.cdw[0]);
  EXPECT_EQ(kTl, nvme_cmd.cdw[2]);
}

TEST_F(ReadTest, Read10ShouldReturnInvalidInputStatus) {
  uint8_t raw_cmd[sizeof(scsi_defs::Read10Command) - 1];
  nvme_defs::GenericQueueEntryCmd nvme_cmd;

  translator::StatusCode sc = translator::Read10ToNvme( raw_cmd,
nvme_cmd); EXPECT_EQ(translator::StatusCode::kInvalidInput, sc);
}

TEST_F(ReadTest, Read10ShouldReturnCorrectTranslation) {
  scsi_defs::Read10Command cmd = {
      .rdprotect = kRdProtect,
      .fua = kFua,
      .logical_block_address = kLba,
      .transfer_length = kTl,
  };
  uint8_t raw_cmd[sizeof(scsi_defs::Read10Command)];
  translator::WriteValue(cmd, raw_cmd);
  nvme_defs::GenericQueueEntryCmd nvme_cmd;

  translator::StatusCode sc =
      translator::Read10ToNvme( raw_cmd, nvme_cmd);

  EXPECT_EQ(translator::StatusCode::kSuccess, sc);
  EXPECT_EQ((uint8_t)nvme_defs::NvmOpcode::kRead, nvme_cmd.opc);
  EXPECT_EQ(0, nvme_cmd.psdt);
  EXPECT_EQ(kLba, nvme_cmd.cdw[0]);
  EXPECT_EQ(BuildCdw13(kTl, kPrinfo, kFua), nvme_cmd.cdw[2]);
}

TEST_F(ReadTest, Read12ShouldReturnInvalidInputStatus) {
  uint8_t raw_cmd[sizeof(scsi_defs::Read12Command) - 1];
  nvme_defs::GenericQueueEntryCmd nvme_cmd;

  translator::StatusCode sc = translator::Read12ToNvme(raw_cmd, nvme_cmd);
  EXPECT_EQ(translator::StatusCode::kInvalidInput, sc);
}

TEST_F(ReadTest, Read12ShouldReturnCorrectTranslation) {
  scsi_defs::Read12Command cmd = {
      .rdprotect = kRdProtect,
      .fua = kFua,
      .logical_block_address = kLba,
      .transfer_length = kTl,
  };
  uint8_t raw_cmd[sizeof(scsi_defs::Read12Command)];
  translator::WriteValue(cmd, raw_cmd);
  nvme_defs::GenericQueueEntryCmd nvme_cmd;

  translator::StatusCode sc = translator::Read12ToNvme(raw_cmd, nvme_cmd);

  EXPECT_EQ(translator::StatusCode::kSuccess, sc);
  EXPECT_EQ((uint8_t)nvme_defs::NvmOpcode::kRead, nvme_cmd.opc);
  EXPECT_EQ(0, nvme_cmd.psdt);
  EXPECT_EQ(kLba, nvme_cmd.cdw[0]);
  EXPECT_EQ(BuildCdw13(kTl, kPrinfo, kFua), nvme_cmd.cdw[2]);
}

TEST_F(ReadTest, Read16ShouldReturnInvalidInputStatus) {
  uint8_t raw_cmd[sizeof(scsi_defs::Read16Command) - 1];
  nvme_defs::GenericQueueEntryCmd nvme_cmd;

  translator::StatusCode sc = translator::Read16ToNvme(raw_cmd, nvme_cmd);
  EXPECT_EQ(translator::StatusCode::kInvalidInput, sc);
}

TEST_F(ReadTest, Read16ShouldReturnCorrectTranslation) {
  scsi_defs::Read16Command cmd = {
      .rdprotect = kRdProtect,
      .fua = kFua,
      .logical_block_address = kLba,
      .transfer_length = kTl,
  };
  uint8_t raw_cmd[sizeof(scsi_defs::Read16Command)];
  translator::WriteValue(cmd, raw_cmd);
  nvme_defs::GenericQueueEntryCmd nvme_cmd;

  translator::StatusCode sc = translator::Read16ToNvme(raw_cmd, nvme_cmd);

  EXPECT_EQ(translator::StatusCode::kSuccess, sc);
  EXPECT_EQ((uint8_t)nvme_defs::NvmOpcode::kRead, nvme_cmd.opc);
  EXPECT_EQ(0, nvme_cmd.psdt);
  EXPECT_EQ(kLba, nvme_cmd.cdw[0]);
  EXPECT_EQ(BuildCdw13(kTl, kPrinfo, kFua), nvme_cmd.cdw[2]);
}

TEST_F(ReadTest, Read32ShouldReturnInvalidInputStatus) {
  uint8_t raw_cmd[sizeof(scsi_defs::Read32Command) - 1];
  nvme_defs::GenericQueueEntryCmd nvme_cmd;

  translator::StatusCode sc = translator::Read32ToNvme(raw_cmd, nvme_cmd);
  EXPECT_EQ(translator::StatusCode::kInvalidInput, sc);
}

TEST_F(ReadTest, Read32ShouldReturnCorrectTranslation) {
  scsi_defs::Read32Command cmd = {
      .rdprotect = kRdProtect,
      .fua = kFua,
      .logical_block_address = kLba,
      .eilbrt = kEilbrt,
      .elbat = kElbat,
      .lbatm = kLbatm,
      .transfer_length = kTl,
  };
  uint8_t raw_cmd[sizeof(scsi_defs::Read32Command)];
  translator::WriteValue(cmd, raw_cmd);
  nvme_defs::GenericQueueEntryCmd nvme_cmd;

  translator::StatusCode sc = translator::Read32ToNvme(raw_cmd, nvme_cmd);

  EXPECT_EQ(translator::StatusCode::kSuccess, sc);
  EXPECT_EQ((uint8_t)nvme_defs::NvmOpcode::kRead, nvme_cmd.opc);
  EXPECT_EQ(0, nvme_cmd.psdt);
  EXPECT_EQ(kLba, nvme_cmd.cdw[0]);
  EXPECT_EQ(BuildCdw13(kTl, kPrinfo, kFua), nvme_cmd.cdw[2]);
  EXPECT_EQ(kEilbrt, nvme_cmd.cdw[4]);
  EXPECT_EQ(kElbat | (kLbatm << 16), nvme_cmd.cdw[5]);
}

TEST_F(ReadTest, ShouldReturnInvalidInputStatusForUnsupportedRdprotect) {
  scsi_defs::Read10Command cmd = {
      .rdprotect = kUnsupportedRdProtect,
      .fua = kFua,
      .logical_block_address = kLba,
      .transfer_length = kTl,
  };
  uint8_t raw_cmd[sizeof(scsi_defs::Read10Command)];
  translator::WriteValue(cmd, raw_cmd);
  nvme_defs::GenericQueueEntryCmd nvme_cmd;

  translator::StatusCode sc = translator::Read10ToNvme(raw_cmd, nvme_cmd);

  EXPECT_EQ(translator::StatusCode::kInvalidInput, sc);
}
}  // namespace
