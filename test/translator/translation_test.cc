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

#include "lib/translator/translation.h"

#include "gtest/gtest.h"

namespace {

/*
   Tests translation class
*/

TEST(Translation, ShouldHandleUnknownOpcode) {
  translator::Translation translation = {};
  uint8_t opc = 233;
  translator::Span<const uint8_t> scsi_cmd = translator::Span(&opc, 1);
  translator::BeginResponse resp = translation.Begin(scsi_cmd, 0);
  EXPECT_EQ(translator::ApiStatus::kSuccess, resp.status);
}

TEST(Translation, ShouldReturnInquirySuccess) {
  translator::Translation translation = {};
  uint8_t opc = static_cast<uint8_t>(scsi::OpCode::kInquiry);
  translator::Span<const uint8_t> scsi_cmd = translator::Span(&opc, 1);
  translator::BeginResponse resp = translation.Begin(scsi_cmd, 0);
  EXPECT_EQ(translator::ApiStatus::kSuccess, resp.status);
}

TEST(Translation, ShouldFailInvalidPipeline) {
  translator::Translation translation = {};
  translator::Span<const nvme::GenericQueueEntryCpl> cpl_data;
  translator::Span<uint8_t> buffer_in;
  translator::Span<uint8_t> sense_buffer;
  translator::CompleteResponse status =
      translation.Complete(cpl_data, buffer_in, sense_buffer);
  EXPECT_EQ(translator::ApiStatus::kFailure, status.status);
}

TEST(Translation, ShouldReturnSenseData) {
  translator::Translation translation = {};

  uint8_t opc = 255;  // Unsupported opcode
  translator::Span<const uint8_t> scsi_cmd = translator::Span(&opc, 1);
  translator::BeginResponse resp = translation.Begin(scsi_cmd, 0);

  ASSERT_EQ(translator::ApiStatus::kSuccess, resp.status);

  translator::Span<const nvme::GenericQueueEntryCpl> cpl_data;
  translator::Span<uint8_t> buffer_in;
  scsi::DescriptorFormatSenseData dfsd = {};
  uint8_t* dfsd_buf = reinterpret_cast<uint8_t*>(&dfsd);
  translator::Span<uint8_t> sense_buffer(dfsd_buf, sizeof(dfsd));
  translator::CompleteResponse cpl_resp =
      translation.Complete(cpl_data, buffer_in, sense_buffer);

  ASSERT_EQ(translator::ApiStatus::kSuccess, cpl_resp.status);

  scsi::DescriptorFormatSenseData expected_dfsd = {
      .response_code = scsi::SenseResponse::kCurrentDescriptorError,
      .sense_key = scsi::SenseKey::kIllegalRequest,
      .additional_sense_code = scsi::AdditionalSenseCode::kInvalidFieldInCdb,
      .additional_sense_code_qualifier =
          scsi::AdditionalSenseCodeQualifier::kNoAdditionalSenseInfo,
      .additional_sense_length = 0};
  EXPECT_EQ(expected_dfsd.response_code, dfsd.response_code);
  EXPECT_EQ(expected_dfsd.sense_key, dfsd.sense_key);
  EXPECT_EQ(expected_dfsd.additional_sense_code, dfsd.additional_sense_code);
  EXPECT_EQ(expected_dfsd.additional_sense_code_qualifier,
            dfsd.additional_sense_code_qualifier);
  EXPECT_EQ(expected_dfsd.additional_sense_length,
            dfsd.additional_sense_length);

  EXPECT_EQ(scsi::Status::kCheckCondition, cpl_resp.scsi_status);
}

TEST(Translation, ShouldReturnEmptyCmdSpan) {
  translator::Translation translation = {};
  translator::Span<const translator::NvmeCmdWrapper> nvme_wrappers =
      translation.GetNvmeWrappers();
  EXPECT_EQ(0, nvme_wrappers.size());
}

}  // namespace
