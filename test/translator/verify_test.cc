// Copyright 2020 Google LLC//
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

#include "lib/translator/verify.h"
#include <byteswap.h>
#include <netinet/in.h>

#include "gtest/gtest.h"

// Tests

namespace {
TEST(Verify, BasicSuccess) {
  nvme::GenericQueueEntryCmd nvme_cmd;
  uint32_t lba = 0x12345;
  const scsi::Verify10Command verify_cmd = {
      .verification_length = htons(1),
      .control_byte = {.naca = 0},
  };
  const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&verify_cmd);
  translator::Span<const uint8_t> scsi_cmd =
      translator::Span(ptr, sizeof(scsi::Verify10Command));
  EXPECT_EQ(translator::VerifyToNvme(scsi_cmd, nvme_cmd),
            translator::StatusCode::kSuccess);
}

TEST(Verify, NoOp) {
  nvme::GenericQueueEntryCmd nvme_cmd;
  uint32_t lba = 0x12345;
  const scsi::Verify10Command verify_cmd = {
      .verification_length = htons(0),
      .control_byte = {.naca = 0},
  };
  const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&verify_cmd);
  translator::Span<const uint8_t> scsi_cmd =
      translator::Span(ptr, sizeof(scsi::Verify10Command));
  EXPECT_EQ(translator::VerifyToNvme(scsi_cmd, nvme_cmd),
            translator::StatusCode::kNoTranslation);
}
TEST(Verify, BadBuffer) {
  nvme::GenericQueueEntryCmd nvme_cmd;
  uint32_t lba = 0x12345;
  const scsi::Verify10Command verify_cmd = {.control_byte = {.naca = 0}};
  const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&verify_cmd);
  translator::Span<const uint8_t> scsi_cmd = translator::Span(ptr, sizeof(1));
  EXPECT_EQ(translator::VerifyToNvme(scsi_cmd, nvme_cmd),
            translator::StatusCode::kInvalidInput);
}

TEST(Verify, BadControlByteNaca) {
  nvme::GenericQueueEntryCmd nvme_cmd;
  uint32_t lba = 0x12345;
  const scsi::Verify10Command verify_cmd = {.verification_length = 1,
                                            .control_byte = {.naca = 1}};
  const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&verify_cmd);
  translator::Span<const uint8_t> scsi_cmd =
      translator::Span(ptr, sizeof(scsi::Verify10Command));
  EXPECT_EQ(translator::VerifyToNvme(scsi_cmd, nvme_cmd),
            translator::StatusCode::kInvalidInput);
}

TEST(Verify, Protect000) {
  nvme::GenericQueueEntryCmd nvme_cmd;
  uint32_t lba = 0x12345;
  const scsi::Verify10Command verify_cmd = {
      .bytchk = 0,
      .vr_protect = 0b000,
      .logical_block_address = htonl(lba),
      .verification_length = htons(1),
      .control_byte = {.naca = 0},
  };
  const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&verify_cmd);
  translator::Span<const uint8_t> scsi_cmd =
      translator::Span(ptr, sizeof(scsi::Verify10Command));
  EXPECT_EQ(translator::VerifyToNvme(scsi_cmd, nvme_cmd),
            translator::StatusCode::kSuccess);

  uint8_t prchk = 0b111;
  uint8_t pr_info = 0b1000 | prchk;
  EXPECT_EQ(nvme_cmd.cdw[0], translator::htoll(lba));
  EXPECT_EQ(nvme_cmd.cdw[1], 0);
  EXPECT_EQ(nvme_cmd.cdw[2],
            translator::htoll((ntohs(verify_cmd.verification_length) - 1) |
                              ((pr_info) << 26)));
}

TEST(Verify, Protect001) {
  nvme::GenericQueueEntryCmd nvme_cmd;
  uint32_t lba = 0x12345;
  const scsi::Verify10Command verify_cmd = {
      .bytchk = 0,
      .vr_protect = 0b001,
      .logical_block_address = htonl(lba),
      .verification_length = htons(1),
      .control_byte = {.naca = 0},
  };
  const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&verify_cmd);
  translator::Span<const uint8_t> scsi_cmd =
      translator::Span(ptr, sizeof(scsi::Verify10Command));
  EXPECT_EQ(translator::VerifyToNvme(scsi_cmd, nvme_cmd),
            translator::StatusCode::kSuccess);

  uint8_t prchk = 0b111;
  uint8_t pr_info = 0b1000 | prchk;
  EXPECT_EQ(nvme_cmd.cdw[0], translator::htoll(lba));
  EXPECT_EQ(nvme_cmd.cdw[1], 0);
  EXPECT_EQ(nvme_cmd.cdw[2],
            translator::htoll((ntohs(verify_cmd.verification_length) - 1) |
                              ((pr_info) << 26)));
}

TEST(Verify, Protect101) {
  nvme::GenericQueueEntryCmd nvme_cmd;
  uint32_t lba = 0x12345;
  const scsi::Verify10Command verify_cmd = {
      .bytchk = 0,
      .vr_protect = 0b101,
      .logical_block_address = htonl(lba),
      .verification_length = htons(1),
      .control_byte = {.naca = 0},
  };
  const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&verify_cmd);
  translator::Span<const uint8_t> scsi_cmd =
      translator::Span(ptr, sizeof(scsi::Verify10Command));
  EXPECT_EQ(translator::VerifyToNvme(scsi_cmd, nvme_cmd),
            translator::StatusCode::kSuccess);

  uint8_t prchk = 0b111;
  uint8_t pr_info = 0b1000 | prchk;
  EXPECT_EQ(nvme_cmd.cdw[0], translator::htoll(lba));
  EXPECT_EQ(nvme_cmd.cdw[1], 0);
  EXPECT_EQ(nvme_cmd.cdw[2],
            translator::htoll((ntohs(verify_cmd.verification_length) - 1) |
                              ((pr_info) << 26)));
}

TEST(Verify, Protect010) {
  nvme::GenericQueueEntryCmd nvme_cmd;
  uint32_t lba = 0x12345;
  const scsi::Verify10Command verify_cmd = {
      .bytchk = 0,
      .vr_protect = 0b010,
      .logical_block_address = htonl(lba),
      .verification_length = htons(1),
      .control_byte = {.naca = 0},
  };
  const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&verify_cmd);
  translator::Span<const uint8_t> scsi_cmd =
      translator::Span(ptr, sizeof(scsi::Verify10Command));
  EXPECT_EQ(translator::VerifyToNvme(scsi_cmd, nvme_cmd),
            translator::StatusCode::kSuccess);

  uint8_t prchk = 0b011;
  uint8_t pr_info = 0b1000 | prchk;
  EXPECT_EQ(nvme_cmd.cdw[0], translator::htoll(lba));
  EXPECT_EQ(nvme_cmd.cdw[1], 0);
  EXPECT_EQ(nvme_cmd.cdw[2],
            translator::htoll((ntohs(verify_cmd.verification_length) - 1) |
                              ((pr_info) << 26)));
}

TEST(Verify, Protect011) {
  nvme::GenericQueueEntryCmd nvme_cmd;
  uint32_t lba = 0x12345;
  const scsi::Verify10Command verify_cmd = {
      .bytchk = 0,
      .vr_protect = 0b011,
      .logical_block_address = htonl(lba),
      .verification_length = htons(1),
      .control_byte = {.naca = 0},
  };
  const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&verify_cmd);
  translator::Span<const uint8_t> scsi_cmd =
      translator::Span(ptr, sizeof(scsi::Verify10Command));
  EXPECT_EQ(translator::VerifyToNvme(scsi_cmd, nvme_cmd),
            translator::StatusCode::kSuccess);

  uint8_t prchk = 0b000;
  uint8_t pr_info = 0b1000 | prchk;
  EXPECT_EQ(nvme_cmd.cdw[0], translator::htoll(lba));
  EXPECT_EQ(nvme_cmd.cdw[1], 0);
  EXPECT_EQ(nvme_cmd.cdw[2],
            translator::htoll((ntohs(verify_cmd.verification_length) - 1) |
                              ((pr_info) << 26)));
}

TEST(Verify, Protect100) {
  nvme::GenericQueueEntryCmd nvme_cmd;
  uint32_t lba = 0x12345;
  const scsi::Verify10Command verify_cmd = {
      .bytchk = 0,
      .vr_protect = 0b100,
      .logical_block_address = htonl(lba),
      .verification_length = htons(1),
      .control_byte = {.naca = 0},
  };
  const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&verify_cmd);
  translator::Span<const uint8_t> scsi_cmd =
      translator::Span(ptr, sizeof(scsi::Verify10Command));
  EXPECT_EQ(translator::VerifyToNvme(scsi_cmd, nvme_cmd),
            translator::StatusCode::kSuccess);

  uint8_t prchk = 0b100;
  uint8_t pr_info = 0b1000 | prchk;
  EXPECT_EQ(nvme_cmd.cdw[0], translator::htoll(lba));
  EXPECT_EQ(nvme_cmd.cdw[1], 0);
  EXPECT_EQ(nvme_cmd.cdw[2],
            translator::htoll((ntohs(verify_cmd.verification_length) - 1) |
                              ((pr_info) << 26)));
}

TEST(Verify, Protect000Bytchk1) {
  nvme::GenericQueueEntryCmd nvme_cmd;
  uint32_t lba = 0x12345;
  const scsi::Verify10Command verify_cmd = {
      .bytchk = 1,
      .vr_protect = 0b000,
      .logical_block_address = htonl(lba),
      .verification_length = htons(1),
      .control_byte = {.naca = 0},
  };
  const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&verify_cmd);
  translator::Span<const uint8_t> scsi_cmd =
      translator::Span(ptr, sizeof(scsi::Verify10Command));
  EXPECT_EQ(translator::VerifyToNvme(scsi_cmd, nvme_cmd),
            translator::StatusCode::kSuccess);

  uint8_t prchk = 0b111;
  uint8_t pr_info = 0b1000 | prchk;
  EXPECT_EQ(nvme_cmd.cdw[0], translator::htoll(lba));
  EXPECT_EQ(nvme_cmd.cdw[1], 0);
  EXPECT_EQ(nvme_cmd.cdw[2],
            translator::htoll((ntohs(verify_cmd.verification_length) - 1) |
                              ((pr_info) << 26)));
}
TEST(Verify, Protect001Bytchk1) {
  nvme::GenericQueueEntryCmd nvme_cmd;
  uint32_t lba = 0x12345;
  const scsi::Verify10Command verify_cmd = {
      .bytchk = 1,
      .vr_protect = 0b001,
      .logical_block_address = htonl(lba),
      .verification_length = htons(1),
      .control_byte = {.naca = 0},
  };
  const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&verify_cmd);
  translator::Span<const uint8_t> scsi_cmd =
      translator::Span(ptr, sizeof(scsi::Verify10Command));
  EXPECT_EQ(translator::VerifyToNvme(scsi_cmd, nvme_cmd),
            translator::StatusCode::kSuccess);

  uint8_t prchk = 0;
  uint8_t pr_info = 0b1000 | prchk;
  EXPECT_EQ(nvme_cmd.cdw[0], translator::htoll(lba));
  EXPECT_EQ(nvme_cmd.cdw[1], 0);
  EXPECT_EQ(nvme_cmd.cdw[2],
            translator::htoll((ntohs(verify_cmd.verification_length) - 1) |
                              ((pr_info) << 26)));
}

TEST(Verify, Protect010Bytchk1) {
  nvme::GenericQueueEntryCmd nvme_cmd;
  uint32_t lba = 0x12345;
  const scsi::Verify10Command verify_cmd = {
      .bytchk = 1,
      .vr_protect = 0b010,
      .logical_block_address = htonl(lba),
      .verification_length = htons(1),
      .control_byte = {.naca = 0},
  };
  const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&verify_cmd);
  translator::Span<const uint8_t> scsi_cmd =
      translator::Span(ptr, sizeof(scsi::Verify10Command));
  EXPECT_EQ(translator::VerifyToNvme(scsi_cmd, nvme_cmd),
            translator::StatusCode::kSuccess);

  uint8_t prchk = 0;
  uint8_t pr_info = 0b1000 | prchk;
  EXPECT_EQ(nvme_cmd.cdw[0], translator::htoll(lba));
  EXPECT_EQ(nvme_cmd.cdw[1], 0);
  EXPECT_EQ(nvme_cmd.cdw[2],
            translator::htoll((ntohs(verify_cmd.verification_length) - 1) |
                              ((pr_info) << 26)));
}

TEST(Verify, Protect011Bytchk1) {
  nvme::GenericQueueEntryCmd nvme_cmd;
  uint32_t lba = 0x12345;
  const scsi::Verify10Command verify_cmd = {
      .bytchk = 1,
      .vr_protect = 0b011,
      .logical_block_address = htonl(lba),
      .verification_length = htons(1),
      .control_byte = {.naca = 0},
  };
  const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&verify_cmd);
  translator::Span<const uint8_t> scsi_cmd =
      translator::Span(ptr, sizeof(scsi::Verify10Command));
  EXPECT_EQ(translator::VerifyToNvme(scsi_cmd, nvme_cmd),
            translator::StatusCode::kSuccess);

  uint8_t prchk = 0;
  uint8_t pr_info = 0b1000 | prchk;
  EXPECT_EQ(nvme_cmd.cdw[0], translator::htoll(lba));
  EXPECT_EQ(nvme_cmd.cdw[1], 0);
  EXPECT_EQ(nvme_cmd.cdw[2],
            translator::htoll((ntohs(verify_cmd.verification_length) - 1) |
                              ((pr_info) << 26)));
}

TEST(Verify, Protect100Bytchk1) {
  nvme::GenericQueueEntryCmd nvme_cmd;
  uint32_t lba = 0x12345;
  const scsi::Verify10Command verify_cmd = {
      .bytchk = 1,
      .vr_protect = 0b100,
      .logical_block_address = htonl(lba),
      .verification_length = htons(1),
      .control_byte = {.naca = 0},
  };
  const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&verify_cmd);
  translator::Span<const uint8_t> scsi_cmd =
      translator::Span(ptr, sizeof(scsi::Verify10Command));
  EXPECT_EQ(translator::VerifyToNvme(scsi_cmd, nvme_cmd),
            translator::StatusCode::kSuccess);

  uint8_t prchk = 0;
  uint8_t pr_info = 0b1000 | prchk;
  EXPECT_EQ(nvme_cmd.cdw[0], translator::htoll(lba));
  EXPECT_EQ(nvme_cmd.cdw[1], 0);
  EXPECT_EQ(nvme_cmd.cdw[2],
            translator::htoll((ntohs(verify_cmd.verification_length) - 1) |
                              ((pr_info) << 26)));
}

TEST(Verify, Protect101Bytchk1) {
  nvme::GenericQueueEntryCmd nvme_cmd;
  uint32_t lba = 0x12345;
  const scsi::Verify10Command verify_cmd = {
      .bytchk = 1,
      .vr_protect = 0b101,
      .logical_block_address = htonl(lba),
      .verification_length = htons(1),
      .control_byte = {.naca = 0},
  };
  const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&verify_cmd);
  translator::Span<const uint8_t> scsi_cmd =
      translator::Span(ptr, sizeof(scsi::Verify10Command));
  EXPECT_EQ(translator::VerifyToNvme(scsi_cmd, nvme_cmd),
            translator::StatusCode::kSuccess);

  uint8_t prchk = 0;
  uint8_t pr_info = 0b1000 | prchk;
  EXPECT_EQ(nvme_cmd.cdw[0], translator::htoll(lba));
  EXPECT_EQ(nvme_cmd.cdw[1], 0);
  EXPECT_EQ(nvme_cmd.cdw[2],
            translator::htoll((ntohs(verify_cmd.verification_length) - 1) |
                              ((pr_info) << 26)));
}
}  // namespace
