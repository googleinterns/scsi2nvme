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

#include "lib/translator/common.h"

#include "gtest/gtest.h"

namespace {

/*
   Tests translation class
*/

TEST(Translation, ShouldReturnUnknownOpCode) {
  translator::Translation translation;
  uint8_t opc = 233;
  absl::Span<const uint8_t> scsi_cmd = absl::MakeSpan(&opc, 1);
  translator::StatusCode sc = translation.Begin(scsi_cmd, 0);
  EXPECT_EQ(translator::StatusCode::kNoTranslation, sc);
}

TEST(Translation, ShouldReturnSuccess) {
  translator::Translation translation;
  uint8_t opc = static_cast<uint8_t>(scsi_defs::OpCode::kInquiry);
  absl::Span<const uint8_t> scsi_cmd = absl::MakeSpan(&opc, 1);
  translator::StatusCode sc = translation.Begin(scsi_cmd, 0);
  EXPECT_EQ(translator::StatusCode::kSuccess, sc);
}

}  // namespace
