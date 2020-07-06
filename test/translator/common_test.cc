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

#include "absl/types/span.h"
#include "gtest/gtest.h"
#include "lib/scsi_defs.h"

namespace {

/*
   Tests the logging methods
*/

TEST(Common, ShouldCorrectlyCallback) {
  const char* buf = "Testing%d";
  auto callback = [](const char* buf) {
    const char* expected_buf = "Testing123";
    for (uint32_t i = 0; i < 11; ++i) {
      EXPECT_EQ(expected_buf[i], buf[i]);
    }
  };
  translator::SetDebugCallback(callback);
  translator::DebugLog(buf, 123);
}

TEST(Common, ShouldNotMemcpy) {
  scsi_defs::Read6Command cmd;
  uint8_t buffer[1];

  auto span = absl::MakeSpan(buffer, sizeof(scsi_defs::Read6Command) + 1);
  bool result = translator::ReadValue(span, cmd);
  EXPECT_FALSE(result);

  span = absl::MakeSpan(buffer, sizeof(scsi_defs::Read6Command) - 1);
  result = translator::ReadValue(span, cmd);
  EXPECT_FALSE(result);
}

TEST(Common, ShouldCorrectlyMemcpy) {
  scsi_defs::ControlByte cb = {};
  uint8_t buffer[1] = {0b11000100};

  auto span = absl::MakeSpan(buffer, sizeof(scsi_defs::ControlByte));
  bool result = translator::ReadValue(span, cb);
  EXPECT_TRUE(result);
  EXPECT_EQ(0b00, cb.obsolete);
  EXPECT_EQ(0b1, cb.naca);
  EXPECT_EQ(0b000, cb.reserved);
  EXPECT_EQ(0b11, cb.vendor_specific);
}

}  // namespace
