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

#include "lib/translator/maintenance_in.h"

#include "gtest/gtest.h"

namespace {

TEST(ReportSupportedOpCodes, InvalidOpCodeValidationFailure) {
  uint32_t alloc_len = 0;
  scsi::ReportOpCodesCommand cmd = {
      .requested_op_code = static_cast<uint8_t>(scsi::OpCode::kRead10)};
  uint8_t scsi_cmd[sizeof(scsi::ReportOpCodesCommand)];
  translator::WriteValue(cmd, scsi_cmd);

  translator::StatusCode status_code =
      translator::ValidateReportSupportedOpCodes(scsi_cmd, alloc_len);

  EXPECT_EQ(translator::StatusCode::kInvalidInput, status_code);
}

TEST(ReportSupportedOpCodes, InvalidReportingOptionsValidationFailure) {
  uint32_t alloc_len = 0;
  scsi::ReportOpCodesCommand cmd = {
      .reporting_options = 0,
      .requested_op_code = static_cast<uint8_t>(scsi::OpCode::kWriteSame16)};
  uint8_t scsi_cmd[sizeof(scsi::ReportOpCodesCommand)];
  translator::WriteValue(cmd, scsi_cmd);

  translator::StatusCode status_code =
      translator::ValidateReportSupportedOpCodes(scsi_cmd, alloc_len);

  EXPECT_EQ(translator::StatusCode::kInvalidInput, status_code);
}

TEST(ReportSupportedOpCodes, ValidationSuccess) {
  uint32_t alloc_len = 0;
  scsi::ReportOpCodesCommand cmd = {
      .reporting_options = 0b001,
      .requested_op_code = static_cast<uint8_t>(scsi::OpCode::kWriteSame16)};
  uint8_t scsi_cmd[sizeof(scsi::ReportOpCodesCommand)];
  translator::WriteValue(cmd, scsi_cmd);

  translator::StatusCode status_code =
      translator::ValidateReportSupportedOpCodes(scsi_cmd, alloc_len);

  EXPECT_EQ(translator::StatusCode::kSuccess, status_code);
  EXPECT_EQ(sizeof(scsi::OneCommandParamData), alloc_len);
}

TEST(ReportSupportedOpCodes, WriteResultSuccess) {
  uint8_t buffer[256] = {};  // Sufficiently large buffer

  translator::WriteReportSupportedOpCodesResult(buffer);

  EXPECT_EQ(1, buffer[1]);
}

}  // namespace
