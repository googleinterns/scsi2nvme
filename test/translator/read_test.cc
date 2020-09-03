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

#include <byteswap.h>
#include <netinet/in.h>

#include "gtest/gtest.h"

namespace {

// Tests the translator::Read6, Read10, Read12, Read16 functions

constexpr uint8_t kRdProtect = 0b101;
constexpr uint8_t kPrinfo = 0b0111;  // expected transformation of kRdProtect
constexpr uint8_t kUnsupportedRdProtect = 0b111;
constexpr uint8_t kFua = 0b1;
constexpr uint32_t kNsid = 0x1a2b3c4d;
constexpr uint32_t kLbaSize = 64;
constexpr uint32_t kHostTransferLen = 50;

uint8_t buffer_in[256 * kLbaSize];  // Buffer large enough for all tests

class ReadTest : public ::testing::Test {
 protected:
  // Per-test-suite set-up.
  // Called before the first test in this test suite.
  static void SetUpTestSuite() {
    // Mocks AllocPages to not return a null (0) value
    auto alloc_callback = [](uint32_t page_size, uint16_t count) -> uint64_t {
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

TEST_F(ReadTest, Read6ToNvmeShouldReturnInvalidInputStatus) {
  uint32_t alloc_len = 0;
  uint8_t scsi_cmd[sizeof(scsi::Read6Command) - 1];
  translator::NvmeCmdWrapper nvme_wrapper;
  translator::Allocation allocation = {};

  translator::StatusCode status_code =
      translator::Read6ToNvme(scsi_cmd, nvme_wrapper, allocation, kNsid,
                              kLbaSize, buffer_in, alloc_len);
  EXPECT_EQ(translator::StatusCode::kInvalidInput, status_code);
}

TEST_F(ReadTest, Read6ToNvmeShouldReturnCorrectTranslation) {
  uint32_t alloc_len = 0;
  uint8_t network_endian_lba_1 = 0x1a;
  uint16_t network_endian_lba_2 = htons(0x2b3c);
  uint32_t cdw10 = 0x1a2b3c;

  uint32_t cdw12 = translator::htoll(kHostTransferLen - 1);

  scsi::Read6Command cmd = {
      .logical_block_address_1 = network_endian_lba_1,
      .logical_block_address_2 = network_endian_lba_2,
      .transfer_length = kHostTransferLen,
  };
  uint8_t scsi_cmd[sizeof(scsi::Read6Command)];
  translator::WriteValue(cmd, scsi_cmd);
  translator::NvmeCmdWrapper nvme_wrapper;
  translator::Allocation allocation = {};

  translator::StatusCode status_code =
      translator::Read6ToNvme(scsi_cmd, nvme_wrapper, allocation, kNsid,
                              kLbaSize, buffer_in, alloc_len);

  EXPECT_EQ(translator::StatusCode::kSuccess, status_code);
  EXPECT_EQ((uint8_t)nvme::NvmOpcode::kRead, nvme_wrapper.cmd.opc);
  EXPECT_EQ(0, nvme_wrapper.cmd.psdt);
  EXPECT_EQ(kNsid, nvme_wrapper.cmd.nsid);
  EXPECT_EQ(cdw10, nvme_wrapper.cmd.cdw[0]);
  EXPECT_EQ(0, nvme_wrapper.cmd.cdw[1]);
  EXPECT_EQ(cdw12, nvme_wrapper.cmd.cdw[2]);
  EXPECT_EQ(false, nvme_wrapper.is_admin);
  EXPECT_EQ(kHostTransferLen * kLbaSize, alloc_len);
  EXPECT_EQ(alloc_len, nvme_wrapper.buffer_len);
}

TEST_F(ReadTest, Read6ToNvmeShouldRead256BlocksForZeroTransferLen) {
  uint32_t alloc_len = 0;
  uint8_t network_endian_lba_1 = 0x1a;
  uint16_t network_endian_lba_2 = htons(0x2b3c);
  uint32_t cdw10 = 0x1a2b3c;

  uint32_t cdw12 = translator::htoll(255);

  scsi::Read6Command cmd = {
      .logical_block_address_1 = network_endian_lba_1,
      .logical_block_address_2 = network_endian_lba_2,
      .transfer_length = 0,
  };
  uint8_t scsi_cmd[sizeof(scsi::Read6Command)];
  translator::WriteValue(cmd, scsi_cmd);
  translator::NvmeCmdWrapper nvme_wrapper;
  translator::Allocation allocation = {};

  translator::StatusCode status_code =
      translator::Read6ToNvme(scsi_cmd, nvme_wrapper, allocation, kNsid,
                              kLbaSize, buffer_in, alloc_len);

  EXPECT_EQ(translator::StatusCode::kSuccess, status_code);
  EXPECT_EQ((uint8_t)nvme::NvmOpcode::kRead, nvme_wrapper.cmd.opc);
  EXPECT_EQ(0, nvme_wrapper.cmd.psdt);
  EXPECT_EQ(kNsid, nvme_wrapper.cmd.nsid);
  EXPECT_EQ(cdw10, nvme_wrapper.cmd.cdw[0]);
  EXPECT_EQ(0, nvme_wrapper.cmd.cdw[1]);
  EXPECT_EQ(cdw12, nvme_wrapper.cmd.cdw[2]);
  EXPECT_EQ(false, nvme_wrapper.is_admin);
  EXPECT_EQ(256 * kLbaSize, alloc_len);
  EXPECT_EQ(alloc_len, nvme_wrapper.buffer_len);
}

TEST_F(ReadTest, Read10ToNvmeShouldReturnInvalidInputStatus) {
  uint32_t alloc_len = 0;
  uint8_t scsi_cmd[sizeof(scsi::Read10Command) - 1];
  translator::NvmeCmdWrapper nvme_wrapper;

  translator::Allocation allocation = {};
  translator::StatusCode status_code =
      translator::Read10ToNvme(scsi_cmd, nvme_wrapper, allocation, kNsid,
                               kLbaSize, buffer_in, alloc_len);
  EXPECT_EQ(translator::StatusCode::kInvalidInput, status_code);
}

TEST_F(ReadTest, Read10ToNvmeShouldReturnCorrectTranslation) {
  uint32_t alloc_len = 0;
  uint32_t network_endian_lba = 0x1a2b3c4d;
  uint32_t cdw10 = bswap_32(network_endian_lba);

  uint32_t cdw12 =
      translator::htoll((kHostTransferLen - 1) | kPrinfo << 26 | kFua << 30);

  scsi::Read10Command cmd = {
      .fua = kFua,
      .rd_protect = kRdProtect,
      .logical_block_address = network_endian_lba,
      .transfer_length = htons(static_cast<uint16_t>(kHostTransferLen)),
  };
  uint8_t scsi_cmd[sizeof(scsi::Read10Command)];
  translator::WriteValue(cmd, scsi_cmd);
  translator::NvmeCmdWrapper nvme_wrapper;
  translator::Allocation allocation = {};

  translator::StatusCode status_code =
      translator::Read10ToNvme(scsi_cmd, nvme_wrapper, allocation, kNsid,
                               kLbaSize, buffer_in, alloc_len);

  EXPECT_EQ(translator::StatusCode::kSuccess, status_code);
  EXPECT_EQ((uint8_t)nvme::NvmOpcode::kRead, nvme_wrapper.cmd.opc);
  EXPECT_EQ(0, nvme_wrapper.cmd.psdt);
  EXPECT_EQ(kNsid, nvme_wrapper.cmd.nsid);
  EXPECT_EQ(cdw10, nvme_wrapper.cmd.cdw[0]);
  EXPECT_EQ(0, nvme_wrapper.cmd.cdw[1]);
  EXPECT_EQ(cdw12, nvme_wrapper.cmd.cdw[2]);
  EXPECT_EQ(false, nvme_wrapper.is_admin);
  EXPECT_EQ(kHostTransferLen * kLbaSize, alloc_len);
  EXPECT_EQ(alloc_len, nvme_wrapper.buffer_len);
}

TEST_F(ReadTest, Read12ToNvmeShouldReturnInvalidInputStatus) {
  uint32_t alloc_len = 0;
  uint8_t scsi_cmd[sizeof(scsi::Read12Command) - 1];
  translator::NvmeCmdWrapper nvme_wrapper;
  translator::Allocation allocation = {};

  translator::StatusCode status_code =
      translator::Read12ToNvme(scsi_cmd, nvme_wrapper, allocation, kNsid,
                               kLbaSize, buffer_in, alloc_len);
  EXPECT_EQ(translator::StatusCode::kInvalidInput, status_code);
}

TEST_F(ReadTest, Read12ToNvmeShouldReturnCorrectTranslation) {
  uint32_t alloc_len = 0;
  uint32_t network_endian_lba = 0x1a2b3c4d;
  uint32_t cdw10 = bswap_32(network_endian_lba);

  uint32_t cdw12 =
      translator::htoll((kHostTransferLen - 1) | kPrinfo << 26 | kFua << 30);

  scsi::Read12Command cmd = {
      .fua = kFua,
      .rd_protect = kRdProtect,
      .logical_block_address = network_endian_lba,
      .transfer_length = htonl(static_cast<uint32_t>(kHostTransferLen)),
  };
  uint8_t scsi_cmd[sizeof(scsi::Read12Command)];
  translator::WriteValue(cmd, scsi_cmd);
  translator::NvmeCmdWrapper nvme_wrapper;
  translator::Allocation allocation = {};

  translator::StatusCode status_code =
      translator::Read12ToNvme(scsi_cmd, nvme_wrapper, allocation, kNsid,
                               kLbaSize, buffer_in, alloc_len);

  EXPECT_EQ(translator::StatusCode::kSuccess, status_code);
  EXPECT_EQ((uint8_t)nvme::NvmOpcode::kRead, nvme_wrapper.cmd.opc);
  EXPECT_EQ(0, nvme_wrapper.cmd.psdt);
  EXPECT_EQ(kNsid, nvme_wrapper.cmd.nsid);
  EXPECT_EQ(cdw10, nvme_wrapper.cmd.cdw[0]);
  EXPECT_EQ(0, nvme_wrapper.cmd.cdw[1]);
  EXPECT_EQ(cdw12, nvme_wrapper.cmd.cdw[2]);
  EXPECT_EQ(false, nvme_wrapper.is_admin);
  EXPECT_EQ(kHostTransferLen * kLbaSize, alloc_len);
  EXPECT_EQ(alloc_len, nvme_wrapper.buffer_len);
}

TEST_F(ReadTest, Read16ToNvmeShouldReturnInvalidInputStatus) {
  uint32_t alloc_len = 0;
  uint8_t scsi_cmd[sizeof(scsi::Read16Command) - 1];
  translator::NvmeCmdWrapper nvme_wrapper;
  translator::Allocation allocation = {};

  translator::StatusCode status_code =
      translator::Read16ToNvme(scsi_cmd, nvme_wrapper, allocation, kNsid,
                               kLbaSize, buffer_in, alloc_len);
  EXPECT_EQ(translator::StatusCode::kInvalidInput, status_code);
}

TEST_F(ReadTest, Read16ToNvmeLongTransferLengthShouldReturnInvalidInputStatus) {
  uint32_t alloc_len = 0;
  uint64_t network_endian_lba = 0x1a2b3c4d5e6f7f8f;
  uint32_t transfer_len = 0xffff + 1;

  scsi::Read16Command cmd = {.fua = kFua,
                             .rd_protect = kRdProtect,
                             .logical_block_address = network_endian_lba,
                             .transfer_length = htonl(transfer_len)};
  uint8_t scsi_cmd[sizeof(scsi::Read16Command)];
  translator::WriteValue(cmd, scsi_cmd);
  translator::NvmeCmdWrapper nvme_wrapper;
  translator::Allocation allocation = {};

  translator::StatusCode status_code =
      translator::Read16ToNvme(scsi_cmd, nvme_wrapper, allocation, kNsid,
                               kLbaSize, buffer_in, alloc_len);

  EXPECT_EQ(translator::StatusCode::kInvalidInput, status_code);
}

TEST_F(ReadTest, Read16ToNvmeShouldReturnCorrectTranslation) {
  uint32_t alloc_len = 0;
  uint64_t host_endian_lba = 0x1a2b3c4d5e6f7f8f;
  uint64_t network_endian_lba = translator::htonll(host_endian_lba);
  uint32_t cdw10 = translator::htoll(host_endian_lba);
  uint32_t cdw11 = translator::htoll(host_endian_lba >> 32);

  uint32_t cdw12 =
      translator::htoll((kHostTransferLen - 1) | kPrinfo << 26 | kFua << 30);
  scsi::Read16Command cmd = {
      .fua = kFua,
      .rd_protect = kRdProtect,
      .logical_block_address = network_endian_lba,
      .transfer_length = htonl(static_cast<uint32_t>(kHostTransferLen))};
  uint8_t scsi_cmd[sizeof(scsi::Read16Command)];
  translator::WriteValue(cmd, scsi_cmd);
  translator::NvmeCmdWrapper nvme_wrapper;
  translator::Allocation allocation = {};

  translator::StatusCode status_code =
      translator::Read16ToNvme(scsi_cmd, nvme_wrapper, allocation, kNsid,
                               kLbaSize, buffer_in, alloc_len);

  EXPECT_EQ(translator::StatusCode::kSuccess, status_code);
  EXPECT_EQ((uint8_t)nvme::NvmOpcode::kRead, nvme_wrapper.cmd.opc);
  EXPECT_EQ(0, nvme_wrapper.cmd.psdt);
  EXPECT_EQ(kNsid, nvme_wrapper.cmd.nsid);
  EXPECT_EQ(cdw10, nvme_wrapper.cmd.cdw[0]);
  EXPECT_EQ(cdw11, nvme_wrapper.cmd.cdw[1]);
  EXPECT_EQ(cdw12, nvme_wrapper.cmd.cdw[2]);
  EXPECT_EQ(false, nvme_wrapper.is_admin);
  EXPECT_EQ(kHostTransferLen * kLbaSize, alloc_len);
  EXPECT_EQ(alloc_len, nvme_wrapper.buffer_len);
}

TEST_F(ReadTest, NonRead6ToNvmeShouldReturnNoTranslationForZeroTransferLen) {
  uint32_t alloc_len = 0;
  scsi::Read10Command cmd = {
      .fua = kFua,
      .rd_protect = kUnsupportedRdProtect,
      .logical_block_address = 100,
      .transfer_length = 0,
  };
  uint8_t scsi_cmd[sizeof(scsi::Read10Command)];
  translator::WriteValue(cmd, scsi_cmd);
  translator::NvmeCmdWrapper nvme_wrapper;
  translator::Allocation allocation = {};

  translator::StatusCode status_code =
      translator::Read10ToNvme(scsi_cmd, nvme_wrapper, allocation, kNsid,
                               kLbaSize, buffer_in, alloc_len);
  EXPECT_EQ(translator::StatusCode::kNoTranslation, status_code);
}

TEST_F(ReadTest, ShouldReturnInvalidInputStatusForUnsupportedRdprotect) {
  uint32_t alloc_len = 0;
  scsi::Read10Command cmd = {
      .fua = kFua,
      .rd_protect = kUnsupportedRdProtect,
      .logical_block_address = 100,
      .transfer_length = htons(static_cast<uint16_t>(kHostTransferLen)),
  };
  uint8_t scsi_cmd[sizeof(scsi::Read10Command)];
  translator::WriteValue(cmd, scsi_cmd);
  translator::NvmeCmdWrapper nvme_wrapper;
  translator::Allocation allocation = {};

  translator::StatusCode status_code =
      translator::Read10ToNvme(scsi_cmd, nvme_wrapper, allocation, kNsid,
                               kLbaSize, buffer_in, alloc_len);
  EXPECT_EQ(translator::StatusCode::kInvalidInput, status_code);
}

TEST_F(ReadTest, InsufficientBufferShouldReturnFailure) {
  uint32_t alloc_len = 0;
  uint32_t host_transfer_length = 16;
  uint32_t network_transfer_length = htonl(host_transfer_length);
  uint32_t transfer_length_bytes = host_transfer_length * kLbaSize;

  uint8_t small_buffer[1];

  scsi::Read12Command cmd = {
      .fua = kFua,
      .rd_protect = kRdProtect,
      .logical_block_address = 0xffffffff,
      .transfer_length = network_transfer_length,
  };
  uint8_t scsi_cmd[sizeof(scsi::Read12Command)];
  translator::WriteValue(cmd, scsi_cmd);
  translator::NvmeCmdWrapper nvme_wrapper;
  translator::Allocation allocation = {};

  translator::StatusCode status_code =
      translator::Read12ToNvme(scsi_cmd, nvme_wrapper, allocation, kNsid,
                               kLbaSize, small_buffer, alloc_len);

  ASSERT_EQ(translator::StatusCode::kFailure, status_code);
}

TEST_F(ReadTest, ShouldSuccessfullyReadToScsiDataInBuffer) {
  uint32_t alloc_len = 0;
  uint32_t transfer_length_bytes = kHostTransferLen * kLbaSize;

  scsi::Read12Command cmd = {
      .fua = kFua,
      .rd_protect = kRdProtect,
      .logical_block_address = 0xffffffff,
      .transfer_length = htonl(kHostTransferLen),
  };
  uint8_t scsi_cmd[sizeof(scsi::Read12Command)];
  translator::WriteValue(cmd, scsi_cmd);
  translator::NvmeCmdWrapper nvme_wrapper;
  translator::Allocation allocation = {};

  translator::StatusCode status_code =
      translator::Read12ToNvme(scsi_cmd, nvme_wrapper, allocation, kNsid,
                               kLbaSize, buffer_in, alloc_len);
  ASSERT_EQ(translator::StatusCode::kSuccess, status_code);
  ASSERT_EQ(transfer_length_bytes, alloc_len);

  // Act as the NVMe driver and write to the PRP pointer
  uint8_t* data = reinterpret_cast<uint8_t*>(nvme_wrapper.cmd.dptr.prp.prp1);
  for (int i = 0; i < alloc_len; ++i) {
    data[i] = i % 256;
  }

  // Check values were written to buffer_in
  for (int i = 0; i < alloc_len; ++i) {
    ASSERT_EQ(buffer_in[i], data[i]);
  }
}

}  // namespace
