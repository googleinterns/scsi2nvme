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

#include "lib/translator/status.h"

#include "absl/base/casts.h"
#include "gtest/gtest.h"

// Tests

namespace {

TEST(TranslateGenericCommandStatus, ShouldReturnCorrectStatus) {
  uint8_t status_code_type = 0x0;  // kGeneric
  uint8_t status_code = 0x0;       // kSuccess;
  translator::ScsiStatus result =
      translator::StatusToScsi(status_code_type, status_code);

  EXPECT_EQ(result.status, scsi::Status::kGood);
  EXPECT_EQ(result.sense_key, scsi::SenseKey::kNoSense);
  EXPECT_EQ(result.asc, scsi::AdditionalSenseCode::kNoAdditionalSenseInfo);
  EXPECT_EQ(result.ascq,
            scsi::AdditionalSenseCodeQualifier::kNoAdditionalSenseInfo);
}

TEST(TranslateUnsupportedGenericCommandStatus, ShouldReturnNullptr) {
  uint8_t status_code_type = 0x0;  // kGeneric
  uint8_t status_code = 0x03;      // kCommandIdConflict
  translator::ScsiStatus result =
      translator::StatusToScsi(status_code_type, status_code);

  EXPECT_EQ(result.status, scsi::Status::kCheckCondition);
  EXPECT_EQ(result.sense_key, scsi::SenseKey::kNoSense);
  EXPECT_EQ(result.asc, scsi::AdditionalSenseCode::kNoAdditionalSenseInfo);
  EXPECT_EQ(result.ascq,
            scsi::AdditionalSenseCodeQualifier::kNoAdditionalSenseInfo);
}

TEST(TranslateCommandSpecificStatus, ShouldReturnCorrectStatus) {
  uint8_t status_code_type = 0x1;  // kCommandSpecific
  uint8_t status_code = 0x0a;      // kInvalidFormat;
  translator::ScsiStatus result =
      translator::StatusToScsi(status_code_type, status_code);

  EXPECT_EQ(result.status, scsi::Status::kCheckCondition);
  EXPECT_EQ(result.sense_key, scsi::SenseKey::kIllegalRequest);
  EXPECT_EQ(result.asc, scsi::AdditionalSenseCode::kFormatCommandFailed);
  EXPECT_EQ(result.ascq,
            scsi::AdditionalSenseCodeQualifier::kFormatCommandFailed);
}

TEST(TranslateUnsupportedCommandSpecificStatus, ShouldReturnNullptr) {
  uint8_t status_code_type = 0x01;  // kGeneric
  uint8_t status_code = 0x0d;       // kFeatureIdNotSaveable;
  translator::ScsiStatus result =
      translator::StatusToScsi(status_code_type, status_code);

  EXPECT_EQ(result.status, scsi::Status::kCheckCondition);
  EXPECT_EQ(result.sense_key, scsi::SenseKey::kNoSense);
  EXPECT_EQ(result.asc, scsi::AdditionalSenseCode::kNoAdditionalSenseInfo);
  EXPECT_EQ(result.ascq,
            scsi::AdditionalSenseCodeQualifier::kNoAdditionalSenseInfo);
}

TEST(TranslateMediaErrorStatus, ShouldReturnCorrectStatus) {
  uint8_t status_code_type = 0x2;  // kCommandSpecific
  uint8_t status_code = 0x81;      // kInvalidFormat;
  translator::ScsiStatus result =
      translator::StatusToScsi(status_code_type, status_code);

  EXPECT_EQ(result.status, scsi::Status::kCheckCondition);
  EXPECT_EQ(result.sense_key, scsi::SenseKey::kMediumError);
  EXPECT_EQ(result.asc, scsi::AdditionalSenseCode::kUnrecoveredReadError);
  EXPECT_EQ(result.ascq,
            scsi::AdditionalSenseCodeQualifier::kUnrecoveredReadError);
}

TEST(TranslateUnsupportedMediaErrorStatus, ShouldReturnNullptr) {
  uint8_t status_code_type = 0x02;  // kMediaError
  uint8_t status_code = 0x87;       // kDeallocatedOrUnwrittenBlock
  translator::ScsiStatus result =
      translator::StatusToScsi(status_code_type, status_code);

  EXPECT_EQ(result.status, scsi::Status::kCheckCondition);
  EXPECT_EQ(result.sense_key, scsi::SenseKey::kNoSense);
  EXPECT_EQ(result.asc, scsi::AdditionalSenseCode::kNoAdditionalSenseInfo);
  EXPECT_EQ(result.ascq,
            scsi::AdditionalSenseCodeQualifier::kNoAdditionalSenseInfo);
}

TEST(TranslateUnsupportedStatusCodeType, ShouldReturnNullptr) {
  translator::ScsiStatus result = translator::StatusToScsi(6, 0);

  EXPECT_EQ(result.status, scsi::Status::kCheckCondition);
  EXPECT_EQ(result.sense_key, scsi::SenseKey::kNoSense);
  EXPECT_EQ(result.asc, scsi::AdditionalSenseCode::kNoAdditionalSenseInfo);
  EXPECT_EQ(result.ascq,
            scsi::AdditionalSenseCodeQualifier::kNoAdditionalSenseInfo);
}

}  // namespace
