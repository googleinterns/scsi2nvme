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

#include "lib/translator/write.h"
#include <byteswap.h>
#include <netinet/in.h>
#include "lib/translator/common.h"

#include "gtest/gtest.h"

// Tests Write Command
namespace {

constexpr uint32_t kWrite6Lba = 0xFFFFF;
constexpr uint8_t kWrite6TransferLength = 0xFF;

constexpr uint32_t kLba = 0xFFFFFFFF;
constexpr uint16_t kTransferLength = 0xFFFF;
constexpr uint8_t kValidWriteProtect = 0b010;
constexpr uint8_t kPrInfo = 0b0011;
constexpr uint8_t kInvalidWriteProtect = 0b111;
constexpr bool kFua = 0b1;

constexpr uint64_t kWrite16Lba = 0xFFFFFFFFFFFFFFFF;
constexpr uint32_t kNsid = 0x1234abcd;
constexpr uint32_t kPageSize = 4096;
constexpr uint32_t kLbaSize = 512;

class WriteTest : public ::testing::Test {
 protected:
  // Per-test-suite set-up.
  // Called before the first test in this test suite.
  static void SetUpTestSuite() {
    // Mocks AllocPages to not return a null (0) value
    auto alloc_callback = [](uint16_t count) -> uint64_t {
      if (count != 0) {
        return 1337;
      } else {
        return 0;
      }
    };
    void (*dealloc_callback)(uint64_t, uint16_t) = nullptr;
    translator::SetAllocPageCallbacks(alloc_callback, dealloc_callback);
  }
};

uint32_t BuildCdw12(uint16_t tl, uint8_t prinfo, bool fua) {
  uint32_t cdw12 = tl - 1 | prinfo << 26 | fua << 30;
  return cdw12;
}

TEST(Write6Command, ShouldReturnInvalidStatusCode) {
  nvme::GenericQueueEntryCmd nvme_cmd;
  translator::Allocation allocation = {};

  uint8_t write6_cmd[sizeof(scsi::Write6Command) - 1];
  translator::StatusCode status_code = translator::Write6ToNvme(
      write6_cmd, nvme_cmd, allocation, kNsid, kPageSize, kLbaSize);
  EXPECT_EQ(status_code, translator::StatusCode::kInvalidInput);
}

TEST(Write10Command, ShouldReturnInvalidStatusCode) {
  nvme::GenericQueueEntryCmd nvme_cmd;
  translator::Allocation allocation = {};
  uint8_t write10_cmd[sizeof(scsi::Write10Command) - 1];
  translator::StatusCode status_code = translator::Write10ToNvme(
      write10_cmd, nvme_cmd, allocation, kNsid, kPageSize, kLbaSize);
  EXPECT_EQ(status_code, translator::StatusCode::kInvalidInput);
}

TEST(Write12Command, ShouldReturnInvalidStatusCode) {
  nvme::GenericQueueEntryCmd nvme_cmd;
  translator::Allocation allocation = {};

  uint8_t write12_cmd[sizeof(scsi::Write12Command) - 1];
  translator::StatusCode status_code = translator::Write12ToNvme(
      write12_cmd, nvme_cmd, allocation, kNsid, kPageSize, kLbaSize);
  EXPECT_EQ(status_code, translator::StatusCode::kInvalidInput);
}

TEST(Write16Command, ShouldReturnInvalidStatusCode) {
  nvme::GenericQueueEntryCmd nvme_cmd;
  translator::Allocation allocation = {};

  uint8_t write16_cmd[sizeof(scsi::Write16Command) - 1];
  translator::StatusCode status_code = translator::Write16ToNvme(
      write16_cmd, nvme_cmd, allocation, kNsid, kPageSize, kLbaSize);
  EXPECT_EQ(status_code, translator::StatusCode::kInvalidInput);
}

TEST_F(WriteTest, Write6ShouldReturnValidStatusCode) {
  uint8_t network_endian_lba_1 = 0x1;
  uint8_t network_endian_lba_2 = htons(0x1234);
  scsi::Write6Command cmd = {.logical_block_address_1 = network_endian_lba_1,
                             .logical_block_address = network_endian_lba_2,
                             .transfer_length = kWrite6TransferLength};

  uint8_t scsi_cmd[sizeof(scsi::Write6Command)];

  translator::WriteValue(cmd, scsi_cmd);
  nvme::GenericQueueEntryCmd nvme_cmd;
  translator::Allocation allocation = {};

  translator::StatusCode status_code = translator::Write6ToNvme(
      scsi_cmd, nvme_cmd, allocation, kNsid, kPageSize, kLbaSize);
  EXPECT_EQ(translator::StatusCode::kSuccess, status_code);
}

TEST_F(WriteTest, Write10ShouldReturnValidStatusCode) {
  uint32_t network_lba = htonl(kLba);
  uint16_t network_transfer_length = htons(kTransferLength);

  scsi::Write10Command cmd = {.fua = kFua,
                              .wr_protect = kValidWriteProtect,
                              .logical_block_address = network_lba,
                              .transfer_length = network_transfer_length};

  uint8_t scsi_cmd[sizeof(scsi::Write10Command)];
  translator::WriteValue(cmd, scsi_cmd);
  nvme::GenericQueueEntryCmd nvme_cmd;
  translator::Allocation allocation = {};

  translator::StatusCode status_code = translator::Write10ToNvme(
      scsi_cmd, nvme_cmd, allocation, kNsid, kPageSize, kLbaSize);

  EXPECT_EQ(translator::StatusCode::kSuccess, status_code);
}

TEST_F(WriteTest, Write12ShouldReturnValidStatusCode) {
  uint32_t network_lba = htonl(kLba);
  uint16_t network_transfer_length = htons(kTransferLength);

  scsi::Write12Command cmd = {.fua = kFua,
                              .wr_protect = kValidWriteProtect,
                              .logical_block_address = network_lba,
                              .transfer_length = network_transfer_length};

  uint8_t scsi_cmd[sizeof(scsi::Write12Command)];
  translator::WriteValue(cmd, scsi_cmd);
  nvme::GenericQueueEntryCmd nvme_cmd;
  translator::Allocation allocation = {};

  translator::StatusCode status_code = translator::Write12ToNvme(
      scsi_cmd, nvme_cmd, allocation, kNsid, kPageSize, kLbaSize);
  EXPECT_EQ(translator::StatusCode::kSuccess, status_code);
}

TEST_F(WriteTest, Write16ShouldReturnValidStatusCode) {
  uint64_t network_lba = translator::htonll(kWrite16Lba);
  uint16_t network_transfer_length = htons(kTransferLength);

  scsi::Write16Command cmd = {.fua = kFua,
                              .wr_protect = kValidWriteProtect,
                              .logical_block_address = network_lba,
                              .transfer_length = network_transfer_length};

  uint8_t scsi_cmd[sizeof(scsi::Write16Command)];
  translator::WriteValue(cmd, scsi_cmd);
  nvme::GenericQueueEntryCmd nvme_cmd;
  translator::Allocation allocation = {};

  translator::StatusCode status_code = translator::Write16ToNvme(
      scsi_cmd, nvme_cmd, allocation, kNsid, kPageSize, kLbaSize);
  EXPECT_EQ(status_code, translator::StatusCode::kSuccess);
}

TEST_F(WriteTest, Write6ShouldBuildCorrectNvmeCommandStruct) {
  uint8_t network_endian_lba_1 = 0x1;
  uint8_t network_endian_lba_2 = htons(0x1234);
  scsi::Write6Command cmd = {.logical_block_address_1 = network_endian_lba_1,
                             .logical_block_address = network_endian_lba_2,
                             .transfer_length = kWrite6TransferLength};

  uint8_t scsi_cmd[sizeof(scsi::Write6Command)];
  translator::WriteValue(cmd, scsi_cmd);

  nvme::GenericQueueEntryCmd nvme_cmd;
  translator::Allocation allocation = {};
  translator::StatusCode status_code = translator::Write6ToNvme(
      scsi_cmd, nvme_cmd, allocation, kNsid, kPageSize, kLbaSize);

  uint32_t expected_lba_value =
      (network_endian_lba_1 << 16) | ntohs(network_endian_lba_2);
  uint32_t expected_cdw12 = translator::htoll(kWrite6TransferLength - 1);

  EXPECT_EQ(status_code, translator::StatusCode::kSuccess);
  EXPECT_EQ(nvme_cmd.cdw[0], expected_lba_value);
  EXPECT_EQ(nvme_cmd.opc, (uint8_t)nvme::NvmOpcode::kWrite);
  EXPECT_EQ(nvme_cmd.psdt, 0);
  EXPECT_EQ(nvme_cmd.cdw[2], expected_cdw12);
}

TEST_F(WriteTest, Write10ShouldBuildCorrectNvmeCommandStruct) {
  uint32_t network_lba = htonl(kLba);
  uint16_t network_transfer_length = htons(kTransferLength);
  scsi::Write10Command cmd = {.fua = kFua,
                              .wr_protect = kValidWriteProtect,
                              .logical_block_address = network_lba,
                              .transfer_length = network_transfer_length};

  uint8_t scsi_cmd[sizeof(scsi::Write10Command)];
  translator::WriteValue(cmd, scsi_cmd);

  nvme::GenericQueueEntryCmd nvme_cmd;
  translator::Allocation allocation = {};
  translator::StatusCode status_code = translator::Write10ToNvme(
      scsi_cmd, nvme_cmd, allocation, kNsid, kPageSize, kLbaSize);

  uint32_t expected_cdw12 = BuildCdw12(kTransferLength, kPrInfo, kFua);
  uint32_t expected_cdw10 = translator::htoll(kLba);

  EXPECT_EQ(status_code, translator::StatusCode::kSuccess);
  EXPECT_EQ(nvme_cmd.cdw[0], expected_cdw10);
  EXPECT_EQ(nvme_cmd.opc, (uint8_t)nvme::NvmOpcode::kWrite);
  EXPECT_EQ(nvme_cmd.psdt, 0);
  EXPECT_EQ(nvme_cmd.cdw[2], expected_cdw12);
}

TEST_F(WriteTest, Write12ShouldBuildCorrectNvmeCommandStruct) {
  uint32_t network_lba = htonl(kLba);
  uint16_t network_transfer_length = htons(kTransferLength);
  scsi::Write12Command cmd = {.fua = kFua,
                              .wr_protect = kValidWriteProtect,
                              .logical_block_address = network_lba,
                              .transfer_length = network_transfer_length};

  uint8_t scsi_cmd[sizeof(scsi::Write12Command)];
  translator::WriteValue(cmd, scsi_cmd);

  nvme::GenericQueueEntryCmd nvme_cmd;
  translator::Allocation allocation = {};
  translator::StatusCode status_code = translator::Write12ToNvme(
      scsi_cmd, nvme_cmd, allocation, kNsid, kPageSize, kLba);

  uint32_t expected_cdw12 = BuildCdw12(kTransferLength, kPrInfo, kFua);
  uint32_t expected_cdw10 = translator::htoll(kLba);

  EXPECT_EQ(status_code, translator::StatusCode::kSuccess);
  EXPECT_EQ(nvme_cmd.cdw[0], expected_cdw10);
  EXPECT_EQ(nvme_cmd.opc, (uint8_t)nvme::NvmOpcode::kWrite);
  EXPECT_EQ(nvme_cmd.psdt, 0);
  EXPECT_EQ(nvme_cmd.cdw[2], expected_cdw12);
}

TEST_F(WriteTest, Write16ShouldBuildCorrectNvmeCommandStruct) {
  uint64_t network_lba = translator::htolll(kWrite16Lba);
  uint16_t network_transfer_length = htons(kTransferLength);

  scsi::Write16Command cmd = {.fua = kFua,
                              .wr_protect = kValidWriteProtect,
                              .logical_block_address = network_lba,
                              .transfer_length = network_transfer_length};

  uint8_t scsi_cmd[sizeof(scsi::Write16Command)];
  translator::WriteValue(cmd, scsi_cmd);

  nvme::GenericQueueEntryCmd nvme_cmd;
  translator::Allocation allocation = {};
  translator::StatusCode status_code = translator::Write16ToNvme(
      scsi_cmd, nvme_cmd, allocation, kNsid, kPageSize, kLba);

  uint32_t expected_cdw10 = translator::htoll(kWrite16Lba);
  uint32_t expected_cdw11 = translator::htoll(kWrite16Lba >> 32);
  uint32_t expected_cdw12 = BuildCdw12(kTransferLength, kPrInfo, kFua);

  EXPECT_EQ(status_code, translator::StatusCode::kSuccess);
  EXPECT_EQ(nvme_cmd.cdw[0], expected_cdw10);
  EXPECT_EQ(nvme_cmd.cdw[1], expected_cdw11);
  EXPECT_EQ(nvme_cmd.opc, (uint8_t)nvme::NvmOpcode::kWrite);
  EXPECT_EQ(nvme_cmd.psdt, 0);
  EXPECT_EQ(nvme_cmd.cdw[2], expected_cdw12);
}

TEST_F(WriteTest, Write10ShoudlFailOnWrongProtectBit) {
  scsi::Write10Command cmd = {.fua = kFua,
                              .wr_protect = kInvalidWriteProtect,
                              .logical_block_address = kLba,
                              .transfer_length = kTransferLength};

  uint8_t scsi_cmd[sizeof(scsi::Write10Command)];
  translator::WriteValue(cmd, scsi_cmd);

  nvme::GenericQueueEntryCmd nvme_cmd;
  translator::Allocation allocation = {};
  translator::StatusCode status_code = translator::Write10ToNvme(
      scsi_cmd, nvme_cmd, allocation, kNsid, kPageSize, kLbaSize);

  EXPECT_EQ(status_code, translator::StatusCode::kFailure);
}

TEST_F(WriteTest, Write12ShoudlFailOnWrongProtectBit) {
  uint32_t network_lba = htonl(kLba);
  uint16_t network_transfer_length = htons(kTransferLength);
  scsi::Write12Command cmd = {.fua = kFua,
                              .wr_protect = kInvalidWriteProtect,
                              .logical_block_address = network_lba,
                              .transfer_length = network_transfer_length};

  uint8_t scsi_cmd[sizeof(scsi::Write12Command)];
  translator::WriteValue(cmd, scsi_cmd);

  nvme::GenericQueueEntryCmd nvme_cmd;
  translator::Allocation allocation = {};
  translator::StatusCode status_code = translator::Write12ToNvme(
      scsi_cmd, nvme_cmd, allocation, kNsid, kPageSize, kLbaSize);

  EXPECT_EQ(status_code, translator::StatusCode::kFailure);
}

TEST_F(WriteTest, Write16ShoudlFailOnWrongProtectBit) {
  uint64_t network_lba = translator::htolll(kWrite16Lba);
  uint16_t network_transfer_length = htons(kTransferLength);

  scsi::Write16Command cmd = {.fua = kFua,
                              .wr_protect = kInvalidWriteProtect,
                              .logical_block_address = network_lba,
                              .transfer_length = network_transfer_length};

  uint8_t scsi_cmd[sizeof(scsi::Write16Command)];
  translator::WriteValue(cmd, scsi_cmd);

  nvme::GenericQueueEntryCmd nvme_cmd;
  translator::Allocation allocation = {};
  translator::StatusCode status_code = translator::Write16ToNvme(
      scsi_cmd, nvme_cmd, allocation, kNsid, kPageSize, kLba);

  EXPECT_EQ(status_code, translator::StatusCode::kFailure);
}

TEST_F(WriteTest, Write6ShouldWrite256BlocksOnZeroTransferLength) {
  uint8_t network_endian_lba_1 = 0x1;
  uint8_t network_endian_lba_2 = htons(0x1234);
  uint8_t transfer_length = 0;
  scsi::Write6Command cmd = {.logical_block_address_1 = network_endian_lba_1,
                             .logical_block_address = network_endian_lba_2,
                             .transfer_length = transfer_length};

  uint8_t scsi_cmd[sizeof(scsi::Write6Command)];
  translator::WriteValue(cmd, scsi_cmd);

  nvme::GenericQueueEntryCmd nvme_cmd;
  translator::Allocation allocation = {};
  translator::StatusCode status_code = translator::Write6ToNvme(
      scsi_cmd, nvme_cmd, allocation, kNsid, kPageSize, kLbaSize);

  uint8_t expected_transfer_length = 256;
  uint32_t expected_cdw12 = translator::htoll(expected_transfer_length - 1);

  EXPECT_EQ(nvme_cmd.cdw[2], expected_cdw12);
}

TEST_F(WriteTest, Write10ShouldWriteFailOnZeroTransferLength) {
  uint32_t network_lba = htonl(kLba);
  uint16_t transfer_length = 0;
  scsi::Write10Command cmd = {.fua = kFua,
                              .wr_protect = kValidWriteProtect,
                              .logical_block_address = network_lba,
                              .transfer_length = transfer_length};

  uint8_t scsi_cmd[sizeof(scsi::Write10Command)];
  translator::WriteValue(cmd, scsi_cmd);

  nvme::GenericQueueEntryCmd nvme_cmd;
  translator::Allocation allocation = {};
  translator::StatusCode status_code = translator::Write10ToNvme(
      scsi_cmd, nvme_cmd, allocation, kNsid, kPageSize, kLbaSize);

  EXPECT_EQ(status_code, translator::StatusCode::kNoTranslation);
}

TEST_F(WriteTest, Write12ShouldWriteFailOnZeroTransferLength) {
  uint32_t network_lba = htonl(kLba);
  uint16_t transfer_length = 0;
  scsi::Write12Command cmd = {.fua = kFua,
                              .wr_protect = kValidWriteProtect,
                              .logical_block_address = network_lba,
                              .transfer_length = transfer_length};

  uint8_t scsi_cmd[sizeof(scsi::Write12Command)];
  translator::WriteValue(cmd, scsi_cmd);

  nvme::GenericQueueEntryCmd nvme_cmd;
  translator::Allocation allocation = {};
  translator::StatusCode status_code = translator::Write12ToNvme(
      scsi_cmd, nvme_cmd, allocation, kNsid, kPageSize, kLbaSize);

  EXPECT_EQ(status_code, translator::StatusCode::kNoTranslation);
}

TEST_F(WriteTest, Write16ShouldWriteFailOnZeroTransferLength) {
  uint64_t network_lba = translator::htolll(kWrite16Lba);
  uint16_t transfer_length = 0;

  scsi::Write16Command cmd = {.fua = kFua,
                              .wr_protect = kValidWriteProtect,
                              .logical_block_address = network_lba,
                              .transfer_length = transfer_length};

  uint8_t scsi_cmd[sizeof(scsi::Write16Command)];
  translator::WriteValue(cmd, scsi_cmd);

  nvme::GenericQueueEntryCmd nvme_cmd;
  translator::Allocation allocation = {};
  translator::StatusCode status_code = translator::Write16ToNvme(
      scsi_cmd, nvme_cmd, allocation, kNsid, kPageSize, kLba);

  EXPECT_EQ(status_code, translator::StatusCode::kNoTranslation);
}
}  // namespace
