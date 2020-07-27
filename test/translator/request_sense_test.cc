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

#include "lib/translator/request_sense.h"

#include "gtest/gtest.h"

namespace {
TEST(RequestSense, ToNvmeSuccess) {
  const scsi::RequestSenseCommand request_sense_cmd = {
      .allocation_length = 100,
      .control_byte = {.naca = 0},
  };
  uint32_t allocation_length = 0;
  const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&request_sense_cmd);
  translator::Span<const uint8_t> scsi_cmd(ptr,
                                           sizeof(scsi::RequestSenseCommand));
  EXPECT_EQ(translator::RequestSenseToNvme(scsi_cmd, allocation_length),
            translator::StatusCode::kSuccess);
  EXPECT_EQ(allocation_length, request_sense_cmd.allocation_length);
}

TEST(RequestSense, ToNvmeBadBuffer) {
  const scsi::RequestSenseCommand request_sense_cmd = {
      .allocation_length = 100,
      .control_byte = {.naca = 0},

  };
  uint32_t allocation_length = 0;
  const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&request_sense_cmd);
  translator::Span<const uint8_t> scsi_cmd(ptr, sizeof(1));
  EXPECT_EQ(translator::RequestSenseToNvme(scsi_cmd, allocation_length),
            translator::StatusCode::kInvalidInput);
  EXPECT_EQ(allocation_length, 0);
}

TEST(RequestSense, ToScsiBadBuffer) {
  const scsi::RequestSenseCommand request_sense_cmd{};
  uint8_t buf[100];
  translator::Span<uint8_t> buffer = buf;
  const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&request_sense_cmd);
  translator::Span<const uint8_t> scsi_cmd(ptr, sizeof(1));
  EXPECT_EQ(translator::RequestSenseToScsi(scsi_cmd, buffer),
            translator::StatusCode::kInvalidInput);
}

TEST(RequestSense, ToNvmeBadControlByteNaca) {
  const scsi::RequestSenseCommand request_sense_cmd = {
      .allocation_length = 100,
      .control_byte = {.naca = 1},

  };
  uint32_t allocation_length = 0;
  const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&request_sense_cmd);
  translator::Span<const uint8_t> scsi_cmd =
      translator::Span(ptr, sizeof(scsi::RequestSenseCommand));
  EXPECT_EQ(translator::RequestSenseToNvme(scsi_cmd, allocation_length),
            translator::StatusCode::kInvalidInput);
  EXPECT_EQ(allocation_length, 0);
}

TEST(RequestSense, ToScsiDescriptor) {
  uint8_t buf[100];
  translator::Span<uint8_t> buffer = buf;
  const scsi::RequestSenseCommand request_sense_cmd = {
      .desc = 1, .allocation_length = 100, .control_byte = {.naca = 0}};
  const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&request_sense_cmd);
  translator::Span<const uint8_t> scsi_cmd =
      translator::Span(ptr, sizeof(scsi::RequestSenseCommand));
  translator::StatusCode status =
      translator::RequestSenseToScsi(scsi_cmd, buffer);
  EXPECT_EQ(status, translator::StatusCode::kSuccess);

  scsi::DescriptorFormatSenseData result{};
  ASSERT_TRUE(translator::ReadValue(buf, result));

  EXPECT_EQ(result.response_code, scsi::SenseResponse::kCurrentDescriptorError);
  EXPECT_EQ(result.additional_sense_code,
            scsi::AdditionalSenseCode::kNoAdditionalSenseInfo);
}

TEST(RequestSense, ToScsiFixed) {
  uint8_t buf[100];
  translator::Span<uint8_t> buffer = buf;
  const scsi::RequestSenseCommand request_sense_cmd = {
      .desc = 0, .allocation_length = 100, .control_byte = {.naca = 0}};
  const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&request_sense_cmd);
  translator::Span<const uint8_t> scsi_cmd =
      translator::Span(ptr, sizeof(scsi::RequestSenseCommand));
  translator::StatusCode status =
      translator::RequestSenseToScsi(scsi_cmd, buffer);
  EXPECT_EQ(status, translator::StatusCode::kSuccess);

  scsi::FixedFormatSenseData result{};
  ASSERT_TRUE(translator::ReadValue(buf, result));

  EXPECT_EQ(result.response_code, scsi::SenseResponse::kCurrentFixedError);
  EXPECT_EQ(result.additional_sense_length, 0x0);
  EXPECT_EQ(result.command_specific_info, 0x0);
  EXPECT_EQ(result.additional_sense_code,
            scsi::AdditionalSenseCode::kNoAdditionalSenseInfo);
}
}  // namespace
