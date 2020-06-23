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

#include "../lib/translate/inquiry.h"

#include <stdlib.h>

#include "gtest/gtest.h"

// Tests

namespace {

TEST(translteInquiryRawToSCSI, empty) {
    absl::Span<const uint32_t> raw_cmd;
    std::optional<scsi_defs::InquiryCommand> result = inquiry::translteInquiryRawToSCSI(raw_cmd);
    ASSERT_FALSE(result.has_value());
}

TEST(translteInquiryRawToSCSI, wrongOp) {
    const uint32_t buf = 4;
    absl::Span<const uint32_t> raw_cmd = absl::MakeSpan(&buf, 1);
    std::optional<scsi_defs::InquiryCommand> result = inquiry::translteInquiryRawToSCSI(raw_cmd);
    ASSERT_FALSE(result.has_value());
}

TEST(translteInquiryRawToSCSI, defaultHappyPath) {
    uint32_t sz = 1+sizeof(scsi_defs::InquiryCommand);
    uint32_t * buf = (uint32_t*)malloc(4 * sz);
    buf[0] = static_cast<uint32_t>(scsi_defs::OpCode::kInquiry);

    scsi_defs::InquiryCommand * cmd = (scsi_defs::InquiryCommand *) &buf[1];
    *cmd = scsi_defs::InquiryCommand();
    absl::Span<const uint32_t> raw_cmd = absl::MakeSpan(buf, sz);
    ASSERT_FALSE(raw_cmd.empty());

    std::optional<scsi_defs::InquiryCommand> result = inquiry::translteInquiryRawToSCSI(raw_cmd);
    ASSERT_TRUE(result.has_value());

    scsi_defs::InquiryCommand result_cmd = result.value();
    ASSERT_EQ(result_cmd.reserved, 0);
    ASSERT_EQ(result_cmd.obsolete, 0);
    ASSERT_EQ(result_cmd.evpd, 0);
    ASSERT_EQ(result_cmd.page_code, 0);
    ASSERT_EQ(result_cmd.allocation_length, 0);
}

// TODO: test invalid parameters in scsi_command
TEST(translteInquiryRawToSCSI, customHappyPath) {
    uint32_t sz = 1+sizeof(scsi_defs::InquiryCommand);
    uint32_t * buf = (uint32_t*)malloc(4 * sz);
    buf[0] = static_cast<uint32_t>(scsi_defs::OpCode::kInquiry);

    scsi_defs::InquiryCommand * cmd = (scsi_defs::InquiryCommand *) &buf[1];
    *cmd = scsi_defs::InquiryCommand();
    cmd->evpd = 1;
    cmd->page_code = 4;
    cmd->allocation_length = 29;

    absl::Span<const uint32_t> raw_cmd = absl::MakeSpan(buf, sz);
    ASSERT_FALSE(raw_cmd.empty());

    std::optional<scsi_defs::InquiryCommand> result = inquiry::translteInquiryRawToSCSI(raw_cmd);
    ASSERT_TRUE(result.has_value());

    scsi_defs::InquiryCommand result_cmd = result.value();
    ASSERT_EQ(result_cmd.reserved, 0);
    ASSERT_EQ(result_cmd.obsolete, 0);
    ASSERT_EQ(result_cmd.evpd, 1);
    ASSERT_EQ(result_cmd.page_code, 4);
    ASSERT_EQ(result_cmd.allocation_length, 29);
}
TEST(translateStandardInquiryResponse, happyPath) {

}
}