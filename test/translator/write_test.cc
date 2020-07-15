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

class WriteTest : public ::testing::Test {
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

uint32_t BuildCdw12(uint16_t tl, uint8_t prinfo, bool fua) {
  uint32_t cdw12 = 0;
  cdw12 |= tl | prinfo << 26 | fua << 30;
  return cdw12;
}

TEST(Write6Command, ShouldReturnInvalidStatusCode) {
  nvme::GenericQueueEntryCmd nvme_cmd;
  translator::Allocation allocation = {};

  uint8_t write6_cmd[sizeof(scsi::Write6Command) - 1];
  translator::StatusCode status_code =
      translator::Write6ToNvme(write6_cmd, nvme_cmd, allocation);
  EXPECT_EQ(status_code, translator::StatusCode::kInvalidInput);
}

TEST(Write10Command, ShouldReturnInvalidStatusCode) {
  nvme::GenericQueueEntryCmd nvme_cmd;
  translator::Allocation allocation = {};
  uint8_t write10_cmd[sizeof(scsi::Write10Command) - 1];
  translator::StatusCode status_code =
      translator::Write10ToNvme(write10_cmd, nvme_cmd, allocation);
  EXPECT_EQ(status_code, translator::StatusCode::kInvalidInput);
}

TEST(Write12Command, ShouldReturnInvalidStatusCode) {
  nvme::GenericQueueEntryCmd nvme_cmd;
  translator::Allocation allocation = {};

  uint8_t write12_cmd[sizeof(scsi::Write12Command) - 1];
  translator::StatusCode status_code =
      translator::Write12ToNvme(write12_cmd, nvme_cmd, allocation);
  EXPECT_EQ(status_code, translator::StatusCode::kInvalidInput);
}

TEST(Write16Command, ShouldReturnInvalidStatusCode) {
  nvme::GenericQueueEntryCmd nvme_cmd;
  translator::Allocation allocation = {};

  uint8_t write16_cmd[sizeof(scsi::Write16Command) - 1];
  translator::StatusCode status_code =
      translator::Write16ToNvme(write16_cmd, nvme_cmd, allocation);
  EXPECT_EQ(status_code, translator::StatusCode::kInvalidInput);
}

TEST_F(WriteTest, Write6ShouldReturnValidStatusCode) {
  scsi::Write6Command scsi_cmd = {.logical_block_address = kLba,
                                  .transfer_length = kWrite6TransferLength};

  uint8_t buf[sizeof(scsi::Write6Command)];
  absl::Span<uint8_t> span_buf =
      absl::MakeSpan(buf, sizeof(scsi::Write6Command));
  translator::WriteValue(scsi_cmd, span_buf);

  nvme::GenericQueueEntryCmd nvme_cmd;
  translator::Allocation allocation = {};

  translator::StatusCode status_code =
      translator::Write6ToNvme(span_buf, nvme_cmd, allocation);
  EXPECT_EQ(translator::StatusCode::kSuccess, status_code);
}

TEST_F(WriteTest, Write10ShouldReturnValidStatusCode) {
  scsi::Write10Command cmd = {.wr_protect = kValidWriteProtect,
                              .fua = kFua,
                              .logical_block_address = kLba,
                              .transfer_length = kTransferLength};

  uint8_t buf[sizeof(scsi::Write10Command)];
  absl::Span<uint8_t> span_buf =
      absl::MakeSpan(buf, sizeof(scsi::Write10Command));
  translator::WriteValue(cmd, span_buf);

  nvme::GenericQueueEntryCmd nvme_cmd;
  translator::Allocation allocation = {};

  translator::StatusCode status_code =
      translator::Write10ToNvme(span_buf, nvme_cmd, allocation);
  uint32_t cdw12 = BuildCdw12(kTransferLength, kPrInfo, kFua);

  EXPECT_EQ(translator::StatusCode::kSuccess, status_code);
}

TEST_F(WriteTest, Write12ShouldReturnValidStatusCode) {
  scsi::Write12Command scsi_cmd = {.wr_protect = kValidWriteProtect,
                                   .fua = kFua,
                                   .logical_block_address = kLba,
                                   .transfer_length = kTransferLength};

  uint8_t buf[sizeof(scsi::Write12Command)];
  absl::Span<uint8_t> span_buf =
      absl::MakeSpan(buf, sizeof(scsi::Write12Command));
  translator::WriteValue(scsi_cmd, span_buf);

  nvme::GenericQueueEntryCmd nvme_cmd;
  translator::Allocation allocation = {};

  translator::StatusCode status_code =
      translator::Write12ToNvme(span_buf, nvme_cmd, allocation);
  EXPECT_EQ(translator::StatusCode::kSuccess, status_code);
}

TEST_F(WriteTest, Write16ShouldReturnValidStatusCode) {
  scsi::Write16Command scsi_cmd = {.wr_protect = kValidWriteProtect,
                                   .fua = kFua,
                                   .logical_block_address = kWrite16Lba,
                                   .transfer_length = kTransferLength};

  uint8_t buf[sizeof(scsi::Write16Command)];
  absl::Span<uint8_t> span_buf =
      absl::MakeSpan(buf, sizeof(scsi::Write16Command));
  translator::WriteValue(scsi_cmd, span_buf);

  nvme::GenericQueueEntryCmd nvme_cmd;
  translator::Allocation allocation = {};

  translator::StatusCode status_code =
      translator::Write16ToNvme(span_buf, nvme_cmd, allocation);
  EXPECT_EQ(status_code, translator::StatusCode::kSuccess);
}

TEST_F(WriteTest, Write6ShouldBuildCorrectNvmeCommandStruct) {
  scsi::Write6Command scsi_cmd = {.logical_block_address = kWrite6Lba,
                                  .transfer_length = kWrite6TransferLength};

  uint8_t buf[sizeof(scsi::Write6Command)];
  absl::Span<uint8_t> span_buf =
      absl::MakeSpan(buf, sizeof(scsi::Write6Command));
  translator::WriteValue(scsi_cmd, span_buf);

  nvme::GenericQueueEntryCmd nvme_cmd;
  translator::Allocation allocation = {};
  translator::StatusCode status_code =
      translator::Write6ToNvme(span_buf, nvme_cmd, allocation);
  EXPECT_EQ(status_code, translator::StatusCode::kSuccess);
  EXPECT_EQ(nvme_cmd.cdw[0], kWrite6Lba);
  EXPECT_EQ(nvme_cmd.opc, (uint8_t)nvme::NvmOpcode::kWrite);
  EXPECT_EQ(nvme_cmd.psdt, 0);
}

TEST_F(WriteTest, Write10ShouldBuildCorrectNvmeCommandStruct) {
  scsi::Write10Command scsi_cmd = {.wr_protect = kValidWriteProtect,
                                   .fua = kFua,
                                   .logical_block_address = kLba,
                                   .transfer_length = kTransferLength};

  uint8_t buf[sizeof(scsi::Write10Command)];
  absl::Span<uint8_t> span_buf =
      absl::MakeSpan(buf, sizeof(scsi::Write10Command));
  translator::WriteValue(scsi_cmd, span_buf);

  nvme::GenericQueueEntryCmd nvme_cmd;
  translator::Allocation allocation = {};
  translator::StatusCode status_code =
      translator::Write10ToNvme(span_buf, nvme_cmd, allocation);

  uint32_t cdw12 = BuildCdw12(kTransferLength, kPrInfo, kFua);

  EXPECT_EQ(status_code, translator::StatusCode::kSuccess);
  EXPECT_EQ(nvme_cmd.cdw[0], kLba);
  EXPECT_EQ(nvme_cmd.opc, (uint8_t)nvme::NvmOpcode::kWrite);
  EXPECT_EQ(nvme_cmd.psdt, 0);
  EXPECT_EQ(nvme_cmd.cdw[2], cdw12);
}

TEST_F(WriteTest, Write12ShouldBuildCorrectNvmeCommandStruct) {
  scsi::Write12Command scsi_cmd = {.wr_protect = kValidWriteProtect,
                                   .fua = kFua,
                                   .logical_block_address = kLba,
                                   .transfer_length = kTransferLength};

  uint8_t buf[sizeof(scsi::Write12Command)];
  absl::Span<uint8_t> span_buf =
      absl::MakeSpan(buf, sizeof(scsi::Write12Command));
  translator::WriteValue(scsi_cmd, span_buf);

  nvme::GenericQueueEntryCmd nvme_cmd;
  translator::Allocation allocation = {};
  translator::StatusCode status_code =
      translator::Write12ToNvme(span_buf, nvme_cmd, allocation);

  uint32_t cdw12 = BuildCdw12(kTransferLength, kPrInfo, kFua);

  EXPECT_EQ(status_code, translator::StatusCode::kSuccess);
  EXPECT_EQ(nvme_cmd.cdw[0], kLba);
  EXPECT_EQ(nvme_cmd.opc, (uint8_t)nvme::NvmOpcode::kWrite);
  EXPECT_EQ(nvme_cmd.psdt, 0);
  EXPECT_EQ(nvme_cmd.cdw[2], cdw12);
}

TEST_F(WriteTest, Write16ShouldBuildCorrectNvmeCommandStruct) {
  scsi::Write16Command scsi_cmd = {.wr_protect = kValidWriteProtect,
                                   .fua = kFua,
                                   .logical_block_address = kWrite16Lba,
                                   .transfer_length = kTransferLength};

  uint8_t buf[sizeof(scsi::Write16Command)];
  absl::Span<uint8_t> span_buf =
      absl::MakeSpan(buf, sizeof(scsi::Write16Command));
  translator::WriteValue(scsi_cmd, span_buf);

  nvme::GenericQueueEntryCmd nvme_cmd;
  translator::Allocation allocation = {};
  translator::StatusCode status_code =
      translator::Write16ToNvme(span_buf, nvme_cmd, allocation);

  uint32_t cdw12 = BuildCdw12(kTransferLength, kPrInfo, kFua);

  EXPECT_EQ(status_code, translator::StatusCode::kSuccess);
  uint32_t cdw10 = kWrite16Lba;
  uint32_t cdw11 = kWrite16Lba >> 32;
  EXPECT_EQ(nvme_cmd.cdw[0], cdw10);
  EXPECT_EQ(nvme_cmd.cdw[1], cdw11);
  EXPECT_EQ(nvme_cmd.opc, (uint8_t)nvme::NvmOpcode::kWrite);
  EXPECT_EQ(nvme_cmd.psdt, 0);
  EXPECT_EQ(nvme_cmd.cdw[2], cdw12);
}

TEST_F(WriteTest, Write10ShoudlFailOnWrongProtectBit) {
  scsi::Write10Command scsi_cmd = {.wr_protect = kInvalidWriteProtect,
                                   .fua = kFua,
                                   .logical_block_address = kLba,
                                   .transfer_length = kTransferLength};

  uint8_t buf[sizeof(scsi::Write10Command)];
  absl::Span<uint8_t> span_buf =
      absl::MakeSpan(buf, sizeof(scsi::Write10Command));
  translator::WriteValue(scsi_cmd, span_buf);

  nvme::GenericQueueEntryCmd nvme_cmd;
  translator::Allocation allocation = {};
  translator::StatusCode status_code =
      translator::Write10ToNvme(span_buf, nvme_cmd, allocation);

  EXPECT_EQ(status_code, translator::StatusCode::kFailure);
}

TEST_F(WriteTest, Write12ShoudlFailOnWrongProtectBit) {
  scsi::Write12Command scsi_cmd = {.wr_protect = kInvalidWriteProtect,
                                   .fua = kFua,
                                   .logical_block_address = kLba,
                                   .transfer_length = kTransferLength};

  uint8_t buf[sizeof(scsi::Write12Command)];
  absl::Span<uint8_t> span_buf =
      absl::MakeSpan(buf, sizeof(scsi::Write12Command));
  translator::WriteValue(scsi_cmd, span_buf);

  nvme::GenericQueueEntryCmd nvme_cmd;
  translator::Allocation allocation = {};
  translator::StatusCode status_code =
      translator::Write12ToNvme(span_buf, nvme_cmd, allocation);

  EXPECT_EQ(status_code, translator::StatusCode::kFailure);
}

TEST_F(WriteTest, Write16ShoudlFailOnWrongProtectBit) {
  scsi::Write16Command scsi_cmd = {.wr_protect = kInvalidWriteProtect,
                                   .fua = kFua,
                                   .logical_block_address = kWrite16Lba,
                                   .transfer_length = kTransferLength};

  uint8_t buf[sizeof(scsi::Write16Command)];
  absl::Span<uint8_t> span_buf =
      absl::MakeSpan(buf, sizeof(scsi::Write16Command));
  translator::WriteValue(scsi_cmd, span_buf);

  nvme::GenericQueueEntryCmd nvme_cmd;
  translator::Allocation allocation = {};
  translator::StatusCode status_code =
      translator::Write16ToNvme(span_buf, nvme_cmd, allocation);

  EXPECT_EQ(status_code, translator::StatusCode::kFailure);
}

}  // namespace
