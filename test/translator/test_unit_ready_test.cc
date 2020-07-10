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

#include "lib/translator/test_unit_ready.h"

#include "absl/base/casts.h"
#include "gtest/gtest.h"

// Tests

namespace {
    TEST(TestUnitReady, Success) {
        const scsi::TestUnitReadyCommand test_unit_ready_cmd = {
            .control_byte = {.naca = 0}
        };
        const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&test_unit_ready_cmd);
        absl::Span<const uint8_t> scsi_cmd = absl::MakeSpan(ptr, sizeof(scsi::TestUnitReadyCommand));
        EXPECT_EQ(translator::TestUnitReadyToNvme(scsi_cmd), translator::StatusCode::kSuccess);
    }
    TEST(TestUnitReady, BadBuffer) {
        const scsi::TestUnitReadyCommand test_unit_ready_cmd = {
            .control_byte = {.naca = 0}
        };
        const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&test_unit_ready_cmd);
        absl::Span<const uint8_t> scsi_cmd = absl::MakeSpan(ptr, sizeof(1));
        EXPECT_EQ(translator::TestUnitReadyToNvme(scsi_cmd), translator::StatusCode::kInvalidInput);
    }
    TEST(TestUnitReady, BadControlByteNaca) {
        const scsi::TestUnitReadyCommand test_unit_ready_cmd = {
            .control_byte = {.naca = 1}
        };
        const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&test_unit_ready_cmd);
        absl::Span<const uint8_t> scsi_cmd = absl::MakeSpan(ptr, sizeof(scsi::TestUnitReadyCommand));
        EXPECT_EQ(translator::TestUnitReadyToNvme(scsi_cmd), translator::StatusCode::kInvalidInput);
    }
}