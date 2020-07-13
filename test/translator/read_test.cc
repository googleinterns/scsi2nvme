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

namespace {

/*
   Tests the translator::Read6, Read10, Read12, Read16, Read32 functions
*/

// Used by Read10, Read12, Read16, Read32
constexpr uint8_t kRdProtect = 0b101;
constexpr uint8_t kPrinfo = 0b0111;  // expected transformation of kRdProtect
constexpr uint8_t kUnsupportedRdProtect = 0b111;
constexpr uint8_t kFua = 0b1;

// Used by Read32
constexpr uint32_t kEilbrt = 1234;
constexpr uint16_t kElbat = 5678;
constexpr uint16_t kLbatm = 9999;

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

uint32_t BuildCdw13(uint16_t tl, uint8_t prinfo, bool fua) {
  uint32_t cdw13 = 0;
  cdw13 = tl;
  cdw13 |= (prinfo << 26);
  cdw13 |= (fua << 30);
  return cdw13;
}

TEST_F(ReadTest, Read6ShouldReturnInvalidInputStatus) {
  uint8_t scsi_cmd[sizeof(scsi::Read6Command) - 1];
  nvme::GenericQueueEntryCmd nvme_cmd;
  translator::Allocation allocation;

  translator::StatusCode status_code =
      translator::Read6ToNvme(scsi_cmd, nvme_cmd, allocation);
  EXPECT_EQ(translator::StatusCode::kInvalidInput, status_code);
}

TEST_F(ReadTest, Read6ShouldReturnCorrectTranslation) {
  uint32_t lba = (uint32_t)pow(2, 21) - 1;
  uint8_t transfer_len = 255;
  uint8_t nlb = transfer_len - 1;
  scsi::Read6Command cmd = {
      .logical_block_address = lba,
      .transfer_length = transfer_len,
  };
  uint8_t scsi_cmd[sizeof(scsi::Read6Command)];
  translator::WriteValue(cmd, scsi_cmd);
  nvme::GenericQueueEntryCmd nvme_cmd;
  translator::Allocation allocation;

  translator::StatusCode status_code =
      translator::Read6ToNvme(scsi_cmd, nvme_cmd, allocation);

  EXPECT_EQ(translator::StatusCode::kSuccess, status_code);
  EXPECT_EQ((uint8_t)nvme::NvmOpcode::kRead, nvme_cmd.opc);
  EXPECT_EQ(0, nvme_cmd.psdt);
  EXPECT_EQ(lba, nvme_cmd.cdw[0]);
  EXPECT_EQ(nlb, nvme_cmd.cdw[2]);
}

TEST_F(ReadTest, Read6ShouldRead256BlocksForZeroTransferLen) {
  uint32_t lba = (uint32_t)pow(2, 21) - 1;
  scsi::Read6Command cmd = {
      .logical_block_address = lba,
      .transfer_length = 0,
  };
  uint8_t scsi_cmd[sizeof(scsi::Read6Command)];
  translator::WriteValue(cmd, scsi_cmd);
  nvme::GenericQueueEntryCmd nvme_cmd;
  translator::Allocation allocation;

  translator::StatusCode status_code =
      translator::Read6ToNvme(scsi_cmd, nvme_cmd, allocation);

  EXPECT_EQ(translator::StatusCode::kSuccess, status_code);
  EXPECT_EQ((uint8_t)nvme::NvmOpcode::kRead, nvme_cmd.opc);
  EXPECT_EQ(0, nvme_cmd.psdt);
  EXPECT_EQ(lba, nvme_cmd.cdw[0]);
  EXPECT_EQ(255, nvme_cmd.cdw[2]);
}

TEST_F(ReadTest, Read10ShouldReturnInvalidInputStatus) {
  uint8_t scsi_cmd[sizeof(scsi::Read10Command) - 1];
  nvme::GenericQueueEntryCmd nvme_cmd;
  translator::Allocation allocation;

  translator::StatusCode status_code =
      translator::Read10ToNvme(scsi_cmd, nvme_cmd, allocation);
  EXPECT_EQ(translator::StatusCode::kInvalidInput, status_code);
}

TEST_F(ReadTest, Read10ShouldReturnCorrectTranslation) {
  uint32_t lba = (uint32_t)pow(2, 32) - 1;
  uint16_t transfer_len = (uint32_t)pow(2, 16) - 1;
  uint16_t nlb = transfer_len - 1;
  scsi::Read10Command cmd = {
      .rdprotect = kRdProtect,
      .fua = kFua,
      .logical_block_address = lba,
      .transfer_length = transfer_len,
  };
  uint8_t scsi_cmd[sizeof(scsi::Read10Command)];
  translator::WriteValue(cmd, scsi_cmd);
  nvme::GenericQueueEntryCmd nvme_cmd;
  translator::Allocation allocation;

  translator::StatusCode status_code =
      translator::Read10ToNvme(scsi_cmd, nvme_cmd, allocation);

  EXPECT_EQ(translator::StatusCode::kSuccess, status_code);
  EXPECT_EQ((uint8_t)nvme::NvmOpcode::kRead, nvme_cmd.opc);
  EXPECT_EQ(0, nvme_cmd.psdt);
  EXPECT_EQ(lba, nvme_cmd.cdw[0]);
  EXPECT_EQ(BuildCdw13(nlb, kPrinfo, kFua), nvme_cmd.cdw[2]);
}

TEST_F(ReadTest, Read12ShouldReturnInvalidInputStatus) {
  uint8_t scsi_cmd[sizeof(scsi::Read12Command) - 1];
  nvme::GenericQueueEntryCmd nvme_cmd;
  translator::Allocation allocation;

  translator::StatusCode status_code =
      translator::Read12ToNvme(scsi_cmd, nvme_cmd, allocation);
  EXPECT_EQ(translator::StatusCode::kInvalidInput, status_code);
}

TEST_F(ReadTest, Read12ShouldReturnCorrectTranslation) {
  uint32_t lba = (uint32_t)pow(2, 32) - 1;
  uint16_t transfer_len = (uint32_t)pow(2, 16) - 1;
  uint16_t nlb = transfer_len - 1;
  scsi::Read12Command cmd = {
      .rdprotect = kRdProtect,
      .fua = kFua,
      .logical_block_address = lba,
      .transfer_length = transfer_len,
  };
  uint8_t scsi_cmd[sizeof(scsi::Read12Command)];
  translator::WriteValue(cmd, scsi_cmd);
  nvme::GenericQueueEntryCmd nvme_cmd;
  translator::Allocation allocation;

  translator::StatusCode status_code =
      translator::Read12ToNvme(scsi_cmd, nvme_cmd, allocation);

  EXPECT_EQ(translator::StatusCode::kSuccess, status_code);
  EXPECT_EQ((uint8_t)nvme::NvmOpcode::kRead, nvme_cmd.opc);
  EXPECT_EQ(0, nvme_cmd.psdt);
  EXPECT_EQ(lba, nvme_cmd.cdw[0]);
  EXPECT_EQ(BuildCdw13(nlb, kPrinfo, kFua), nvme_cmd.cdw[2]);
}

TEST_F(ReadTest, Read16ShouldReturnInvalidInputStatus) {
  uint8_t scsi_cmd[sizeof(scsi::Read16Command) - 1];
  nvme::GenericQueueEntryCmd nvme_cmd;
  translator::Allocation allocation;

  translator::StatusCode status_code =
      translator::Read16ToNvme(scsi_cmd, nvme_cmd, allocation);
  EXPECT_EQ(translator::StatusCode::kInvalidInput, status_code);
}

TEST_F(ReadTest, Read16ShouldReturnCorrectTranslation) {
  uint32_t lba = (uint32_t)pow(2, 64) - 1;
  uint16_t transfer_len = (uint32_t)pow(2, 32) - 1;
  uint16_t nlb = transfer_len - 1;
  scsi::Read16Command cmd = {
      .rdprotect = kRdProtect,
      .fua = kFua,
      .logical_block_address = lba,
      .transfer_length = transfer_len,
  };
  uint8_t scsi_cmd[sizeof(scsi::Read16Command)];
  translator::WriteValue(cmd, scsi_cmd);
  nvme::GenericQueueEntryCmd nvme_cmd;
  translator::Allocation allocation;

  translator::StatusCode status_code =
      translator::Read16ToNvme(scsi_cmd, nvme_cmd, allocation);

  EXPECT_EQ(translator::StatusCode::kSuccess, status_code);
  EXPECT_EQ((uint8_t)nvme::NvmOpcode::kRead, nvme_cmd.opc);
  EXPECT_EQ(0, nvme_cmd.psdt);
  EXPECT_EQ(lba, nvme_cmd.cdw[0]);
  EXPECT_EQ(BuildCdw13(nlb, kPrinfo, kFua), nvme_cmd.cdw[2]);
}

TEST_F(ReadTest, Read32ShouldReturnInvalidInputStatus) {
  uint8_t scsi_cmd[sizeof(scsi::Read32Command) - 1];
  nvme::GenericQueueEntryCmd nvme_cmd;
  translator::Allocation allocation;

  translator::StatusCode status_code =
      translator::Read32ToNvme(scsi_cmd, nvme_cmd, allocation);
  EXPECT_EQ(translator::StatusCode::kInvalidInput, status_code);
}

TEST_F(ReadTest, Read32ShouldReturnCorrectTranslation) {
  uint64_t lba = (uint64_t)(pow(2, 34) + 8);
  uint32_t transfer_len = (uint32_t)pow(2, 32) - 1;
  uint16_t nlb = transfer_len - 1;
  scsi::Read32Command cmd = {
      .rdprotect = kRdProtect,
      .fua = kFua,
      .logical_block_address = lba,
      .eilbrt = kEilbrt,
      .elbat = kElbat,
      .lbatm = kLbatm,
      .transfer_length = transfer_len,
  };
  uint8_t scsi_cmd[sizeof(scsi::Read32Command)];
  translator::WriteValue(cmd, scsi_cmd);
  nvme::GenericQueueEntryCmd nvme_cmd;
  translator::Allocation allocation;

  translator::StatusCode status_code =
      translator::Read32ToNvme(scsi_cmd, nvme_cmd, allocation);

  EXPECT_EQ(translator::StatusCode::kSuccess, status_code);
  EXPECT_EQ((uint8_t)nvme::NvmOpcode::kRead, nvme_cmd.opc);
  EXPECT_EQ(0, nvme_cmd.psdt);
  EXPECT_EQ(8, nvme_cmd.cdw[0]);
  EXPECT_EQ(BuildCdw13(nlb, kPrinfo, kFua), nvme_cmd.cdw[2]);
  EXPECT_EQ(kEilbrt, nvme_cmd.cdw[4]);
  EXPECT_EQ(kElbat | (kLbatm << 16), nvme_cmd.cdw[5]);
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
  translator::Allocation allocation;

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
  translator::Allocation allocation;

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
  translator::Allocation allocation;

  translator::StatusCode status_code =
      translator::Read6ToNvme(scsi_cmd, nvme_cmd, allocation);
  EXPECT_EQ(translator::StatusCode::kFailure, status_code);
}

}  // namespace
