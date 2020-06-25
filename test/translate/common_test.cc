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

#include "lib/translate/common.h"

#include "gtest/gtest.h"

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

}  // namespace
