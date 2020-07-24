// Copyright 2020 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "lib/translator/read_capacity_10.h"

#include "gtest/gtest.h"

namespace {

class ReadCapacity10Test : public ::testing::Test {
 protected:
  scsi::ReadCapacity10Command read_capacity_10_cmd_;
  nvme::GenericQueueEntryCmd identify_cmds_[1];
  translator::Span<nvme::GenericQueueEntryCmd> nvme_cmds_;
  translator::Span<const uint8_t> scsi_cmd_;
  nvme::IdentifyNamespace identify_ns_;

  uint8_t buffer_[200];

  // Per-test-suite set-up.
  // Called before the first test in this test suite.
  static void SetUpTestSuite() {
    auto alloc_callback = [](uint16_t count) -> uint64_t {
      // TODO: fix count == 0
      // EXPECT_EQ(1, count);
      return 1337;
    };
    void (*dealloc_callback)(uint64_t, uint16_t) = nullptr;
    translator::SetAllocPageCallbacks(alloc_callback, dealloc_callback);
  }

  // You can define per-test set-up logic as usual.
  void SetUp() override {
    identify_ns_ = {};
    SetNamespace(&identify_ns_);

    read_capacity_10_cmd_ = {};
    SetCommand();
    nvme_cmds_ = identify_cmds_;
    memset(buffer_, 0, sizeof(buffer_));
  }

  void SetCommand() {
    const uint8_t* cmd_ptr =
        reinterpret_cast<const uint8_t*>(&read_capacity_10_cmd_);
    scsi_cmd_ = translator::Span<const uint8_t>(
        cmd_ptr, sizeof(scsi::ReadCapacity10Command));
  }

  void SetNamespace(nvme::IdentifyNamespace* identify_ns_) {
    identify_cmds_[0].dptr.prp.prp1 = reinterpret_cast<uint64_t>(identify_ns_);
  }
};

TEST_F(ReadCapacity10Test, ToNvmeSuccess) {
  translator::Allocation allocation{};
  EXPECT_EQ(translator::ReadCapacity10ToNvme(scsi_cmd_, identify_cmds_[0], 1,
                                             allocation),
            translator::StatusCode::kSuccess);
}

TEST_F(ReadCapacity10Test, ToNvmeBadBuffer) {
  uint8_t bad_buffer[1];
  translator::Allocation allocation{};
  EXPECT_EQ(translator::ReadCapacity10ToNvme(bad_buffer, identify_cmds_[0], 1,
                                             allocation),
            translator::StatusCode::kInvalidInput);
}

TEST_F(ReadCapacity10Test, ToNvmeBadControlByteNaca) {
  read_capacity_10_cmd_.control_byte.naca = 1;
  translator::Allocation allocation{};
  EXPECT_EQ(translator::ReadCapacity10ToNvme(scsi_cmd_, identify_cmds_[0], 1,
                                             allocation),
            translator::StatusCode::kInvalidInput);
}

TEST_F(ReadCapacity10Test, Success) {
  identify_ns_.nsze = 0;
  identify_ns_.flbas.format = 0;
  identify_ns_.lbaf[identify_ns_.flbas.format].lbads = 0;
  EXPECT_EQ(translator::StatusCode::kSuccess,
            translator::ReadCapacity10ToScsi(buffer_, nvme_cmds_[0]));
  scsi::ReadCapacity10Data result = {};
  ASSERT_TRUE(translator::ReadValue(buffer_, result));
  EXPECT_EQ(result.returned_logical_block_address,
            identify_ns_.nsze > 0xffffffff ? 0xffffffff : identify_ns_.nsze);
  EXPECT_EQ(result.block_length,
            identify_ns_.lbaf[identify_ns_.flbas.format].lbads);
}

TEST_F(ReadCapacity10Test, NszeNonzero) {
  identify_ns_.nsze = 1;
  identify_ns_.flbas.format = 0;
  identify_ns_.lbaf[identify_ns_.flbas.format].lbads = 0;
  EXPECT_EQ(translator::StatusCode::kSuccess,
            translator::ReadCapacity10ToScsi(buffer_, nvme_cmds_[0]));
  scsi::ReadCapacity10Data result = {};
  ASSERT_TRUE(translator::ReadValue(buffer_, result));
  EXPECT_EQ(result.returned_logical_block_address,
            identify_ns_.nsze > 0xffffffff ? 0xffffffff : identify_ns_.nsze);
  EXPECT_EQ(result.block_length,
            identify_ns_.lbaf[identify_ns_.flbas.format].lbads);
}

TEST_F(ReadCapacity10Test, NszeLarge) {
  identify_ns_.nsze = 0xffffffffffff;
  identify_ns_.flbas.format = 0;
  identify_ns_.lbaf[identify_ns_.flbas.format].lbads = 0;
  EXPECT_EQ(translator::StatusCode::kSuccess,
            translator::ReadCapacity10ToScsi(buffer_, nvme_cmds_[0]));
  scsi::ReadCapacity10Data result = {};
  ASSERT_TRUE(translator::ReadValue(buffer_, result));
  EXPECT_EQ(result.returned_logical_block_address,
            identify_ns_.nsze > 0xffffffff ? 0xffffffff : identify_ns_.nsze);
  EXPECT_EQ(result.block_length,
            identify_ns_.lbaf[identify_ns_.flbas.format].lbads);
}

TEST_F(ReadCapacity10Test, NszeLimit) {
  identify_ns_.nsze = 0xffffffff;
  identify_ns_.flbas.format = 0;
  identify_ns_.lbaf[identify_ns_.flbas.format].lbads = 0;
  EXPECT_EQ(translator::StatusCode::kSuccess,
            translator::ReadCapacity10ToScsi(buffer_, nvme_cmds_[0]));
  scsi::ReadCapacity10Data result = {};
  ASSERT_TRUE(translator::ReadValue(buffer_, result));
  EXPECT_EQ(result.returned_logical_block_address,
            identify_ns_.nsze > 0xffffffff ? 0xffffffff : identify_ns_.nsze);
  EXPECT_EQ(result.block_length,
            identify_ns_.lbaf[identify_ns_.flbas.format].lbads);
}

TEST_F(ReadCapacity10Test, BlocklengthNonzeo) {
  identify_ns_.nsze = 0;
  identify_ns_.flbas.format = 0;
  identify_ns_.lbaf[identify_ns_.flbas.format].lbads = 1;
  EXPECT_EQ(translator::StatusCode::kSuccess,
            translator::ReadCapacity10ToScsi(buffer_, nvme_cmds_[0]));
  scsi::ReadCapacity10Data result = {};
  ASSERT_TRUE(translator::ReadValue(buffer_, result));
  EXPECT_EQ(result.returned_logical_block_address,
            identify_ns_.nsze > 0xffffffff ? 0xffffffff : identify_ns_.nsze);
  EXPECT_EQ(result.block_length,
            identify_ns_.lbaf[identify_ns_.flbas.format].lbads);
}
TEST_F(ReadCapacity10Test, BlocklengthLimit) {
  identify_ns_.nsze = 0;
  identify_ns_.flbas.format = 0;
  identify_ns_.lbaf[identify_ns_.flbas.format].lbads = 0xff;
  EXPECT_EQ(translator::StatusCode::kSuccess,
            translator::ReadCapacity10ToScsi(buffer_, nvme_cmds_[0]));
  scsi::ReadCapacity10Data result = {};
  ASSERT_TRUE(translator::ReadValue(buffer_, result));
  EXPECT_EQ(result.returned_logical_block_address,
            identify_ns_.nsze > 0xffffffff ? 0xffffffff : identify_ns_.nsze);
  EXPECT_EQ(result.block_length,
            identify_ns_.lbaf[identify_ns_.flbas.format].lbads);
}

}  // namespace
