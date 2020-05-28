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

#include "lib/scsi_defs.h"

#include "gtest/gtest.h"
#include "absl/base/casts.h"
namespace {

  /*
    Tests the ControlByte class
  */

  TEST(ControlByteClass, ShouldReturnCorrectControlByte) {
    uint8_t cbValue = 0b11000100;
    scsi_defs::ControlByte cb = absl::bit_cast<scsi_defs::ControlByte>(cbValue);

    EXPECT_EQ(0b00, cb.obsolete);
    EXPECT_EQ(0b1, cb.naca);
    EXPECT_EQ(0b000, cb.reserved);
    EXPECT_EQ(0b11, cb.vendor_specific);
  }

} // anonymous namespace for testing
