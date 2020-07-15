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
   Tests the translator::Read6, Read10, Read12, Read16, Read32 functions
*/

constexpr uint8_t kRdProtect = 0b101;
constexpr uint8_t kPrinfo = 0b0111;  // expected transformation of kRdProtect
constexpr uint8_t kUnsupportedRdProtect = 0b111;
constexpr uint8_t kFua = 0b1;
constexpr uint16_t kTransferLen = 0x1a2b;
constexpr uint16_t kNlb = 0x2a1a;

class ReadTest : public ::testing::Test {
 protected:
  // Per-test-suite set-up.
  // Called before the first test in this test suite.
  static void SetUpTestSuite() {
    // Mocks AllocPages to not return a null (0) value
    auto alloc_callback = [](uint16_t count) -> uint64_t {
      EXPECT_EQ(1, count);
      return 1337;
    };
    void (*dealloc_callback)(uint64_t, uint16_t) = nullptr;
    translator::SetAllocPageCallbacks(alloc_callback, dealloc_callback);
  }
};

// Accounts for NVMe little endianness
uint32_t BuildCdw12(uint16_t nlb, uint8_t prinfo, bool fua) {
  return (nlb << 16) | (fua << 6) | (prinfo << 2);
}

TEST_F(ReadTest, Read6ShouldReturnInvalidInputStatus) {
  uint8_t scsi_cmd[sizeof(scsi::Read6Command) - 1];
  nvme::GenericQueueEntryCmd nvme_cmd;
  translator::Allocation allocation = {};

  translator::StatusCode status_code =
      translator::Read6ToNvme(scsi_cmd, nvme_cmd, allocation);
  EXPECT_EQ(translator::StatusCode::kInvalidInput, status_code);
}

TEST_F(ReadTest, Read6ShouldReturnCorrectTranslation) {
  uint32_t lba = 0x1a2b3c;  // max 2^21 -1
  uint32_t cdw10 = 0x3c2b1a00;

  uint8_t transfer_len = 0x1b;
  uint16_t nlb = 0x1a00;

  scsi::Read6Command cmd = {
      .logical_block_address = lba,
      .transfer_length = transfer_len,
  };
  uint8_t scsi_cmd[sizeof(scsi::Read6Command)];
  translator::WriteValue(cmd, scsi_cmd);
  nvme::GenericQueueEntryCmd nvme_cmd;
  translator::Allocation allocation = {};

  translator::StatusCode status_code =
      translator::Read6ToNvme(scsi_cmd, nvme_cmd, allocation);

  EXPECT_EQ(translator::StatusCode::kSuccess, status_code);
  EXPECT_EQ((uint8_t)nvme::NvmOpcode::kRead, nvme_cmd.opc);
  EXPECT_EQ(0, nvme_cmd.psdt);
  EXPECT_EQ(cdw10, nvme_cmd.cdw[0]);
  EXPECT_EQ(0, nvme_cmd.cdw[1]);
  EXPECT_EQ(nlb, nvme_cmd.cdw[2]);
}

TEST_F(ReadTest, Read6ShouldRead256BlocksForZeroTransferLen) {
  uint32_t lba = 0x1a2b3c;  // max 2^21 - 1
  uint32_t cdw10 = 0x3c2b1a00;

  scsi::Read6Command cmd = {
      .logical_block_address = lba,
      .transfer_length = 0,
  };
  uint8_t scsi_cmd[sizeof(scsi::Read6Command)];
  translator::WriteValue(cmd, scsi_cmd);
  nvme::GenericQueueEntryCmd nvme_cmd;
  translator::Allocation allocation = {};

  translator::StatusCode status_code =
      translator::Read6ToNvme(scsi_cmd, nvme_cmd, allocation);

  EXPECT_EQ(translator::StatusCode::kSuccess, status_code);
  EXPECT_EQ((uint8_t)nvme::NvmOpcode::kRead, nvme_cmd.opc);
  EXPECT_EQ(0, nvme_cmd.psdt);
  EXPECT_EQ(cdw10, nvme_cmd.cdw[0]);
  EXPECT_EQ(0, nvme_cmd.cdw[1]);
  EXPECT_EQ(0xff00, nvme_cmd.cdw[2]);
}

TEST_F(ReadTest, Read10ShouldReturnInvalidInputStatus) {
  uint8_t scsi_cmd[sizeof(scsi::Read10Command) - 1];
  nvme::GenericQueueEntryCmd nvme_cmd;
  translator::Allocation allocation = {};

  translator::StatusCode status_code =
      translator::Read10ToNvme(scsi_cmd, nvme_cmd, allocation);
  EXPECT_EQ(translator::StatusCode::kInvalidInput, status_code);
}

TEST_F(ReadTest, Read10ShouldReturnCorrectTranslation) {
  uint32_t lba = 0x1a2b3c4d;
  uint32_t cdw10 = 0x4d3c2b1a;

  scsi::Read10Command cmd = {
      .rdprotect = kRdProtect,
      .fua = kFua,
      .logical_block_address = lba,
      .transfer_length = kTransferLen,
  };
  uint8_t scsi_cmd[sizeof(scsi::Read10Command)];
  translator::WriteValue(cmd, scsi_cmd);
  nvme::GenericQueueEntryCmd nvme_cmd;
  translator::Allocation allocation = {};

  translator::StatusCode status_code =
      translator::Read10ToNvme(scsi_cmd, nvme_cmd, allocation);

  EXPECT_EQ(translator::StatusCode::kSuccess, status_code);
  EXPECT_EQ((uint8_t)nvme::NvmOpcode::kRead, nvme_cmd.opc);
  EXPECT_EQ(0, nvme_cmd.psdt);
  EXPECT_EQ(cdw10, nvme_cmd.cdw[0]);
  EXPECT_EQ(0, nvme_cmd.cdw[1]);
  EXPECT_EQ(BuildCdw12(kNlb, kPrinfo, kFua), nvme_cmd.cdw[2]);
}

TEST_F(ReadTest, Read12ShouldReturnInvalidInputStatus) {
  uint8_t scsi_cmd[sizeof(scsi::Read12Command) - 1];
  nvme::GenericQueueEntryCmd nvme_cmd;
  translator::Allocation allocation = {};

  translator::StatusCode status_code =
      translator::Read12ToNvme(scsi_cmd, nvme_cmd, allocation);
  EXPECT_EQ(translator::StatusCode::kInvalidInput, status_code);
}

TEST_F(ReadTest, Read12ShouldReturnCorrectTranslation) {
  uint32_t lba = 0x1a2b3c4d;
  uint32_t cdw10 = 0x4d3c2b1a;

  scsi::Read12Command cmd = {
      .rdprotect = kRdProtect,
      .fua = kFua,
      .logical_block_address = lba,
      .transfer_length = kTransferLen,
  };
  uint8_t scsi_cmd[sizeof(scsi::Read12Command)];
  translator::WriteValue(cmd, scsi_cmd);
  nvme::GenericQueueEntryCmd nvme_cmd;
  translator::Allocation allocation = {};

  translator::StatusCode status_code =
      translator::Read12ToNvme(scsi_cmd, nvme_cmd, allocation);

  EXPECT_EQ(translator::StatusCode::kSuccess, status_code);
  EXPECT_EQ((uint8_t)nvme::NvmOpcode::kRead, nvme_cmd.opc);
  EXPECT_EQ(0, nvme_cmd.psdt);
  EXPECT_EQ(cdw10, nvme_cmd.cdw[0]);
  EXPECT_EQ(0, nvme_cmd.cdw[1]);
  EXPECT_EQ(BuildCdw12(kNlb, kPrinfo, kFua), nvme_cmd.cdw[2]);
}

TEST_F(ReadTest, Read16ShouldReturnInvalidInputStatus) {
  uint8_t scsi_cmd[sizeof(scsi::Read16Command) - 1];
  nvme::GenericQueueEntryCmd nvme_cmd;
  translator::Allocation allocation = {};

  translator::StatusCode status_code =
      translator::Read16ToNvme(scsi_cmd, nvme_cmd, allocation);
  EXPECT_EQ(translator::StatusCode::kInvalidInput, status_code);
}

TEST_F(ReadTest, Read16ShouldReturnCorrectTranslation) {
  uint64_t lba = 0x1a2b3c4d5e6f7f8f;
  uint32_t cdw10 = 0x8f7f6f5e;
  uint32_t cdw11 = 0x4d3c2b1a;

  scsi::Read16Command cmd = {
      .rdprotect = kRdProtect,
      .fua = kFua,
      .logical_block_address = lba,
      .transfer_length = kTransferLen,
  };
  uint8_t scsi_cmd[sizeof(scsi::Read16Command)];
  translator::WriteValue(cmd, scsi_cmd);
  nvme::GenericQueueEntryCmd nvme_cmd;
  translator::Allocation allocation = {};

  translator::StatusCode status_code =
      translator::Read16ToNvme(scsi_cmd, nvme_cmd, allocation);

  EXPECT_EQ(translator::StatusCode::kSuccess, status_code);
  EXPECT_EQ((uint8_t)nvme::NvmOpcode::kRead, nvme_cmd.opc);
  EXPECT_EQ(0, nvme_cmd.psdt);
  EXPECT_EQ(cdw10, nvme_cmd.cdw[0]);
  EXPECT_EQ(cdw11, nvme_cmd.cdw[1]);
  EXPECT_EQ(BuildCdw12(kNlb, kPrinfo, kFua), nvme_cmd.cdw[2]);
}

TEST_F(ReadTest, Read32ShouldReturnInvalidInputStatus) {
  uint8_t scsi_cmd[sizeof(scsi::Read32Command) - 1];
  nvme::GenericQueueEntryCmd nvme_cmd;
  translator::Allocation allocation = {};

  translator::StatusCode status_code =
      translator::Read32ToNvme(scsi_cmd, nvme_cmd, allocation);
  EXPECT_EQ(translator::StatusCode::kInvalidInput, status_code);
}

TEST_F(ReadTest, Read32ShouldReturnCorrectTranslation) {
  uint64_t lba = 0x1a2b3c4d5e6f7f8f;
  uint32_t cdw10 = 0x8f7f6f5e;
  uint32_t cdw11 = 0x4d3c2b1a;

  uint32_t eilbrt = 0x1234;
  uint32_t cdw14 = 0x34120000;

  uint16_t elbat = 0x5678;
  uint16_t lbatm = 0x90;
  uint32_t cdw15 = 0x78569000;

  scsi::Read32Command cmd = {
      .rdprotect = kRdProtect,
      .fua = kFua,
      .logical_block_address = lba,
      .eilbrt = eilbrt,
      .elbat = elbat,
      .lbatm = lbatm,
      .transfer_length = kTransferLen,
  };
  uint8_t scsi_cmd[sizeof(scsi::Read32Command)];
  translator::WriteValue(cmd, scsi_cmd);
  nvme::GenericQueueEntryCmd nvme_cmd;
  translator::Allocation allocation = {};

  translator::StatusCode status_code =
      translator::Read32ToNvme(scsi_cmd, nvme_cmd, allocation);

  EXPECT_EQ(translator::StatusCode::kSuccess, status_code);
  EXPECT_EQ((uint8_t)nvme::NvmOpcode::kRead, nvme_cmd.opc);
  EXPECT_EQ(0, nvme_cmd.psdt);
  EXPECT_EQ(cdw10, nvme_cmd.cdw[0]);
  EXPECT_EQ(cdw11, nvme_cmd.cdw[1]);
  EXPECT_EQ(BuildCdw12(kNlb, kPrinfo, kFua), nvme_cmd.cdw[2]);
  EXPECT_EQ(cdw14, nvme_cmd.cdw[4]);
  EXPECT_EQ(cdw15, nvme_cmd.cdw[5]);
}

TEST_F(ReadTest, NonRead6ShouldReturnNoTranslationForZeroTransferLen) {
  scsi::Read10Command cmd = {
      .rdprotect = kUnsupportedRdProtect,
      .fua = kFua,
      .logical_block_address = 100,
      .transfer_length = 0,
  };
  uint8_t scsi_cmd[sizeof(scsi::Read10Command)];
  translator::WriteValue(cmd, scsi_cmd);
  nvme::GenericQueueEntryCmd nvme_cmd;
  translator::Allocation allocation = {};

  translator::StatusCode status_code =
      translator::Read10ToNvme(scsi_cmd, nvme_cmd, allocation);
  EXPECT_EQ(translator::StatusCode::kNoTranslation, status_code);
}

TEST_F(ReadTest, ShouldReturnInvalidInputStatusForUnsupportedRdprotect) {
  scsi::Read10Command cmd = {
      .rdprotect = kUnsupportedRdProtect,
      .fua = kFua,
      .logical_block_address = 100,
      .transfer_length = 255,
  };
  uint8_t scsi_cmd[sizeof(scsi::Read10Command)];
  translator::WriteValue(cmd, scsi_cmd);
  nvme::GenericQueueEntryCmd nvme_cmd;
  translator::Allocation allocation = {};

  translator::StatusCode status_code =
      translator::Read10ToNvme(scsi_cmd, nvme_cmd, allocation);
  EXPECT_EQ(translator::StatusCode::kInvalidInput, status_code);
}

TEST_F(ReadTest, ShouldReturnFailureStatusForNullAllocPages) {
  auto alloc_callback = [](uint16_t count) -> uint64_t {
    EXPECT_EQ(1, count);
    return 0;
  };
  void (*dealloc_callback)(uint64_t, uint16_t) = nullptr;
  translator::SetAllocPageCallbacks(alloc_callback, dealloc_callback);

  scsi::Read6Command cmd = {
      .logical_block_address = 100,
      .transfer_length = 255,
  };
  uint8_t scsi_cmd[sizeof(scsi::Read6Command)];
  translator::WriteValue(cmd, scsi_cmd);
  nvme::GenericQueueEntryCmd nvme_cmd;
  translator::Allocation allocation = {};

  translator::StatusCode status_code =
      translator::Read6ToNvme(scsi_cmd, nvme_cmd, allocation);
  EXPECT_EQ(translator::StatusCode::kFailure, status_code);
}

}  // namespace
