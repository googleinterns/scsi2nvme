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

#include <netinet/in.h>

#include "gtest/gtest.h"

namespace {

// Tests the translator::Read6, Read10, Read12, Read16 functions

constexpr uint8_t kRdProtect = 0b101;
constexpr uint8_t kPrinfo = 0b0111;  // expected transformation of kRdProtect
constexpr uint8_t kUnsupportedRdProtect = 0b111;
constexpr uint8_t kFua = 0b1;
constexpr uint16_t kTransferLen = 0x1a2b;
constexpr uint16_t kNlb = 0x1a2a;
constexpr uint32_t kCdw12 = 0x5c001a2a;  // based on kNlb, kFua, kPrinfo
constexpr uint32_t kNsid = 0x1a2b3c4d;

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

TEST_F(ReadTest, Read6ToNvmeShouldReturnInvalidInputStatus) {
  uint8_t scsi_cmd[sizeof(scsi::Read6Command) - 1];
  nvme::GenericQueueEntryCmd nvme_cmd;
  translator::Allocation allocation = {};

  translator::StatusCode status_code =
      translator::Read6ToNvme(scsi_cmd, nvme_cmd, allocation, kNsid);
  EXPECT_EQ(translator::StatusCode::kInvalidInput, status_code);
}

TEST_F(ReadTest, Read6ToNvmeShouldReturnCorrectTranslation) {
  uint32_t network_endian_lba = 0x1a2b3c;  // max 21 bits
  uint32_t host_endian_lba = ntohl(network_endian_lba);

  uint8_t transfer_len = 0x1b;
  uint16_t nlb = 0x1a;

  scsi::Read6Command cmd = {
      .logical_block_address = network_endian_lba,
      .transfer_length = transfer_len,
  };
  uint8_t scsi_cmd[sizeof(scsi::Read6Command)];
  translator::WriteValue(cmd, scsi_cmd);
  nvme::GenericQueueEntryCmd nvme_cmd;
  translator::Allocation allocation = {};

  translator::StatusCode status_code =
      translator::Read6ToNvme(scsi_cmd, nvme_cmd, allocation, kNsid);

  EXPECT_EQ(translator::StatusCode::kSuccess, status_code);
  EXPECT_EQ((uint8_t)nvme::NvmOpcode::kRead, nvme_cmd.opc);
  EXPECT_EQ(0, nvme_cmd.psdt);
  EXPECT_EQ(kNsid, nvme_cmd.nsid);
  EXPECT_EQ(host_endian_lba, nvme_cmd.cdw[0]);
  EXPECT_EQ(0, nvme_cmd.cdw[1]);
  EXPECT_EQ(nlb, nvme_cmd.cdw[2]);
}

TEST_F(ReadTest, Read6ToNvmeShouldRead256BlocksForZeroTransferLen) {
  uint32_t network_endian_lba = 0x1a2b3e;  // max 21 bits
  uint32_t host_endian_lba = ntohl(network_endian_lba);

  scsi::Read6Command cmd = {
      .logical_block_address = network_endian_lba,
      .transfer_length = 0,
  };
  uint8_t scsi_cmd[sizeof(scsi::Read6Command)];
  translator::WriteValue(cmd, scsi_cmd);
  nvme::GenericQueueEntryCmd nvme_cmd;
  translator::Allocation allocation = {};

  translator::StatusCode status_code =
      translator::Read6ToNvme(scsi_cmd, nvme_cmd, allocation, kNsid);

  EXPECT_EQ(translator::StatusCode::kSuccess, status_code);
  EXPECT_EQ((uint8_t)nvme::NvmOpcode::kRead, nvme_cmd.opc);
  EXPECT_EQ(0, nvme_cmd.psdt);
  EXPECT_EQ(kNsid, nvme_cmd.nsid);
  EXPECT_EQ(host_endian_lba, nvme_cmd.cdw[0]);
  EXPECT_EQ(0, nvme_cmd.cdw[1]);
  EXPECT_EQ(255, nvme_cmd.cdw[2]);
}

TEST_F(ReadTest, Read10ToNvmeShouldReturnInvalidInputStatus) {
  uint8_t scsi_cmd[sizeof(scsi::Read10Command) - 1];
  nvme::GenericQueueEntryCmd nvme_cmd;
  translator::Allocation allocation = {};

  translator::StatusCode status_code =
      translator::Read10ToNvme(scsi_cmd, nvme_cmd, allocation, kNsid);
  EXPECT_EQ(translator::StatusCode::kInvalidInput, status_code);
}

TEST_F(ReadTest, Read10ToNvmeShouldReturnCorrectTranslation) {
  uint32_t network_endian_lba = 0x1a2b3c4d;
  uint32_t host_endian_lba = ntohl(network_endian_lba);

  scsi::Read10Command cmd = {
      .rd_protect = kRdProtect,
      .fua = kFua,
      .logical_block_address = network_endian_lba,
      .transfer_length = kTransferLen,
  };
  uint8_t scsi_cmd[sizeof(scsi::Read10Command)];
  translator::WriteValue(cmd, scsi_cmd);
  nvme::GenericQueueEntryCmd nvme_cmd;
  translator::Allocation allocation = {};

  translator::StatusCode status_code =
      translator::Read10ToNvme(scsi_cmd, nvme_cmd, allocation, kNsid);

  EXPECT_EQ(translator::StatusCode::kSuccess, status_code);
  EXPECT_EQ((uint8_t)nvme::NvmOpcode::kRead, nvme_cmd.opc);
  EXPECT_EQ(0, nvme_cmd.psdt);
  EXPECT_EQ(kNsid, nvme_cmd.nsid);
  EXPECT_EQ(host_endian_lba, nvme_cmd.cdw[0]);
  EXPECT_EQ(0, nvme_cmd.cdw[1]);
  EXPECT_EQ(kCdw12, nvme_cmd.cdw[2]);
}

TEST_F(ReadTest, Read12ToNvmeShouldReturnInvalidInputStatus) {
  uint8_t scsi_cmd[sizeof(scsi::Read12Command) - 1];
  nvme::GenericQueueEntryCmd nvme_cmd;
  translator::Allocation allocation = {};

  translator::StatusCode status_code =
      translator::Read12ToNvme(scsi_cmd, nvme_cmd, allocation, kNsid);
  EXPECT_EQ(translator::StatusCode::kInvalidInput, status_code);
}

TEST_F(ReadTest, Read12ToNvmeShouldReturnCorrectTranslation) {
  uint32_t network_endian_lba = 0x1a2b3c4d;
  uint32_t host_endian_lba = ntohl(network_endian_lba);

  scsi::Read12Command cmd = {
      .rd_protect = kRdProtect,
      .fua = kFua,
      .logical_block_address = network_endian_lba,
      .transfer_length = kTransferLen,
  };
  uint8_t scsi_cmd[sizeof(scsi::Read12Command)];
  translator::WriteValue(cmd, scsi_cmd);
  nvme::GenericQueueEntryCmd nvme_cmd;
  translator::Allocation allocation = {};

  translator::StatusCode status_code =
      translator::Read12ToNvme(scsi_cmd, nvme_cmd, allocation, kNsid);

  EXPECT_EQ(translator::StatusCode::kSuccess, status_code);
  EXPECT_EQ((uint8_t)nvme::NvmOpcode::kRead, nvme_cmd.opc);
  EXPECT_EQ(0, nvme_cmd.psdt);
  EXPECT_EQ(kNsid, nvme_cmd.nsid);
  EXPECT_EQ(host_endian_lba, nvme_cmd.cdw[0]);
  EXPECT_EQ(0, nvme_cmd.cdw[1]);
  EXPECT_EQ(kCdw12, nvme_cmd.cdw[2]);
}

TEST_F(ReadTest, Read16ToNvmeShouldReturnInvalidInputStatus) {
  uint8_t scsi_cmd[sizeof(scsi::Read16Command) - 1];
  nvme::GenericQueueEntryCmd nvme_cmd;
  translator::Allocation allocation = {};

  translator::StatusCode status_code =
      translator::Read16ToNvme(scsi_cmd, nvme_cmd, allocation, kNsid);
  EXPECT_EQ(translator::StatusCode::kInvalidInput, status_code);
}

TEST_F(ReadTest, Read16ToNvmeLongTransferLengthShouldReturnInvalidInputStatus) {
  uint64_t network_endian_lba = 0x1a2b3c4d5e6f7f8f;
  uint32_t transfer_length = 0xffff + 1;

  scsi::Read16Command cmd = {.rd_protect = kRdProtect,
                             .fua = kFua,
                             .logical_block_address = network_endian_lba,
                             .transfer_length = transfer_length};
  uint8_t scsi_cmd[sizeof(scsi::Read16Command)];
  translator::WriteValue(cmd, scsi_cmd);
  nvme::GenericQueueEntryCmd nvme_cmd;
  translator::Allocation allocation = {};

  translator::StatusCode status_code =
      translator::Read16ToNvme(scsi_cmd, nvme_cmd, allocation, kNsid);

  EXPECT_EQ(translator::StatusCode::kInvalidInput, status_code);
}

TEST_F(ReadTest, Read16ToNvmeShouldReturnCorrectTranslation) {
  uint64_t network_endian_lba = 0x1a2b3c4d5e6f7f8f;
  uint64_t host_endian_lba = translator::ntohll(network_endian_lba);
  uint32_t cdw10 = host_endian_lba;
  uint32_t cdw11 = host_endian_lba >> 32;

  scsi::Read16Command cmd = {.rd_protect = kRdProtect,
                             .fua = kFua,
                             .logical_block_address = network_endian_lba,
                             .transfer_length = kTransferLen};
  uint8_t scsi_cmd[sizeof(scsi::Read16Command)];
  translator::WriteValue(cmd, scsi_cmd);
  nvme::GenericQueueEntryCmd nvme_cmd;
  translator::Allocation allocation = {};

  translator::StatusCode status_code =
      translator::Read16ToNvme(scsi_cmd, nvme_cmd, allocation, kNsid);

  EXPECT_EQ(translator::StatusCode::kSuccess, status_code);
  EXPECT_EQ((uint8_t)nvme::NvmOpcode::kRead, nvme_cmd.opc);
  EXPECT_EQ(0, nvme_cmd.psdt);
  EXPECT_EQ(kNsid, nvme_cmd.nsid);
  EXPECT_EQ(cdw10, nvme_cmd.cdw[0]);
  EXPECT_EQ(cdw11, nvme_cmd.cdw[1]);
  EXPECT_EQ(kCdw12, nvme_cmd.cdw[2]);
}

TEST_F(ReadTest, NonRead6ToNvmeShouldReturnNoTranslationForZeroTransferLen) {
  scsi::Read10Command cmd = {
      .rd_protect = kUnsupportedRdProtect,
      .fua = kFua,
      .logical_block_address = 100,
      .transfer_length = 0,
  };
  uint8_t scsi_cmd[sizeof(scsi::Read10Command)];
  translator::WriteValue(cmd, scsi_cmd);
  nvme::GenericQueueEntryCmd nvme_cmd;
  translator::Allocation allocation = {};

  translator::StatusCode status_code =
      translator::Read10ToNvme(scsi_cmd, nvme_cmd, allocation, kNsid);
  EXPECT_EQ(translator::StatusCode::kNoTranslation, status_code);
}

TEST_F(ReadTest, ShouldReturnInvalidInputStatusForUnsupportedRdprotect) {
  scsi::Read10Command cmd = {
      .rd_protect = kUnsupportedRdProtect,
      .fua = kFua,
      .logical_block_address = 100,
      .transfer_length = 255,
  };
  uint8_t scsi_cmd[sizeof(scsi::Read10Command)];
  translator::WriteValue(cmd, scsi_cmd);
  nvme::GenericQueueEntryCmd nvme_cmd;
  translator::Allocation allocation = {};

  translator::StatusCode status_code =
      translator::Read10ToNvme(scsi_cmd, nvme_cmd, allocation, kNsid);
  EXPECT_EQ(translator::StatusCode::kInvalidInput, status_code);
}

// Not under TEST_F(ReadTest, ...) because it overrides the
// alloc_callback behaviour that's required before all other tests
TEST(ReadTestNullAllocPages, ShouldReturnFailureStatus) {
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
      translator::Read6ToNvme(scsi_cmd, nvme_cmd, allocation, kNsid);
  EXPECT_EQ(translator::StatusCode::kFailure, status_code);
}

}  // namespace
