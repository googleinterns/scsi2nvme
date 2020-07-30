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
#include "lib/scsi.h"

namespace {

// Tests the logging methods

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

  // necessary to "undo" the expected behavior of this test
  void (*debug_callback)(const char*) = nullptr;
  translator::SetDebugCallback(debug_callback);
}

TEST(Common, ShouldNotReadValueFromSpan) {
  scsi::Read6Command cmd;
  uint8_t buffer[sizeof(scsi::Read6Command) - 1];

  bool result = translator::ReadValue(buffer, cmd);
  ASSERT_FALSE(result);
}

TEST(Common, ShouldCorrectlyReadValueFromSpan) {
  scsi::ControlByte cb = {};
  uint8_t buffer[sizeof(scsi::ControlByte)] = {0b11000100};

  bool result = translator::ReadValue(buffer, cb);
  ASSERT_TRUE(result);
  EXPECT_EQ(0b00, cb.obsolete);
  EXPECT_EQ(0b1, cb.naca);
  EXPECT_EQ(0b000, cb.reserved);
  EXPECT_EQ(0b11, cb.vendor_specific);
}

TEST(Common, ShouldNotWriteValueToSpan) {
  scsi::Read6Command cmd;
  uint8_t buffer[sizeof(scsi::Read6Command) - 1];

  bool result = translator::WriteValue(cmd, buffer);
  EXPECT_FALSE(result);
}

TEST(Common, ShouldCorrectlyWriteValueToSpan) {
  scsi::ControlByte cb = {
      .obsolete = 0b00,
      .naca = 0b1,
      .reserved = 0b000,
      .vendor_specific = 0b11,
  };
  uint8_t buffer[sizeof(scsi::ControlByte)];

  bool result = translator::WriteValue(cb, buffer);
  EXPECT_TRUE(result);
  EXPECT_EQ(0b11000100, buffer[0]);
}

TEST(Common, ShouldBuildAllocationWithSuccessStatus) {
  translator::Allocation allocation = {};
  auto alloc_callback = [](uint16_t count) -> uint64_t {
    if (count == 1) {
      return 1337;
    }
    if (count == 3) {
      return 7331;
    }
    return 0;
  };
  void (*dealloc_callback)(uint64_t, uint16_t) = nullptr;
  translator::SetAllocPageCallbacks(alloc_callback, dealloc_callback);

  translator::StatusCode status_code = allocation.SetPages(1, 3);

  EXPECT_EQ(translator::StatusCode::kSuccess, status_code);
  EXPECT_EQ(1, allocation.data_page_count);
  EXPECT_EQ(1337, allocation.data_addr);
  EXPECT_EQ(3, allocation.mdata_page_count);
  EXPECT_EQ(7331, allocation.mdata_addr);
}

TEST(Common, ShouldBuildAllocationZeroPageCountaWithSuccessStatus) {
  translator::Allocation allocation = {};
  auto alloc_callback = [](uint16_t count) -> uint64_t {
    if (count == 2) {
      return 1337;
    }
    return 0;
  };
  void (*dealloc_callback)(uint64_t, uint16_t) = nullptr;
  translator::SetAllocPageCallbacks(alloc_callback, dealloc_callback);

  translator::StatusCode status_code = allocation.SetPages(2, 0);

  EXPECT_EQ(translator::StatusCode::kSuccess, status_code);
  EXPECT_EQ(2, allocation.data_page_count);
  EXPECT_EQ(1337, allocation.data_addr);
  EXPECT_EQ(0, allocation.mdata_page_count);
  EXPECT_EQ(0, allocation.mdata_addr);

  status_code = allocation.SetPages(0, 2);

  EXPECT_EQ(translator::StatusCode::kSuccess, status_code);
  EXPECT_EQ(0, allocation.data_page_count);
  EXPECT_EQ(0, allocation.data_addr);
  EXPECT_EQ(2, allocation.mdata_page_count);
  EXPECT_EQ(1337, allocation.mdata_addr);
}

TEST(Common, ShouldFailBuildAllocationWhenOverrdingMemory) {
  translator::Allocation allocation = {.data_addr = 1337, .mdata_addr = 0};

  translator::StatusCode status_code = allocation.SetPages(1, 1);
  EXPECT_EQ(translator::StatusCode::kFailure, status_code);
}

TEST(Common, ShouldFailBuildAllocationWhenAllocPageFails) {
  translator::Allocation allocation = {};
  auto alloc_callback = [](uint16_t count) -> uint64_t {
    EXPECT_EQ(1, count);
    return 0;
  };
  void (*dealloc_callback)(uint64_t, uint16_t) = nullptr;
  translator::SetAllocPageCallbacks(alloc_callback, dealloc_callback);

  translator::StatusCode status_code = allocation.SetPages(1, 1);
  EXPECT_EQ(translator::StatusCode::kFailure, status_code);
}

TEST(Common, SafePointerCastWrite) {
  uint32_t expected_val = 0x13292022;

  uint32_t write_buffer = 0;
  uint8_t* buf = reinterpret_cast<uint8_t*>(&write_buffer);
  translator::Span<uint8_t> span(buf, sizeof(uint32_t));

  uint32_t* val = translator::SafePointerCastWrite<uint32_t>(span);

  ASSERT_NE(nullptr, val);
  *val = expected_val;
  ASSERT_EQ(expected_val, write_buffer);
}

TEST(Common, SafePointerCastRead) {
  uint32_t expected_val = 0x13292022;
  uint8_t* buf = reinterpret_cast<uint8_t*>(&expected_val);
  translator::Span<uint8_t> span(buf, sizeof(uint32_t));

  const uint32_t* val = translator::SafePointerCastRead<uint32_t>(span);

  ASSERT_NE(nullptr, val);
  ASSERT_EQ(expected_val, *val);
}

TEST(Common, PointerCastReadInvalidSize) {
  uint32_t expected_val = 0;
  uint8_t* buf = reinterpret_cast<uint8_t*>(&expected_val);
  translator::Span<uint8_t> span(buf, 1);

  const uint32_t* val = translator::SafePointerCastRead<uint32_t>(span);

  ASSERT_EQ(nullptr, val);
}

TEST(Common, PointerCastWriteInvalidSize) {
  uint32_t expected_val = 0;
  uint8_t* buf = reinterpret_cast<uint8_t*>(&expected_val);
  translator::Span<uint8_t> span(buf, 3);

  uint32_t* val = translator::SafePointerCastWrite<uint32_t>(span);

  ASSERT_EQ(nullptr, val);
}

TEST(Common, PointerCastReadBadAlignment) {
  uint32_t expected_vals[2];
  uint8_t* buf = reinterpret_cast<uint8_t*>(&expected_vals);
  translator::Span<uint8_t> span(buf + 1, sizeof(uint32_t));

  const uint32_t* val = translator::SafePointerCastRead<uint32_t>(span);

  ASSERT_EQ(nullptr, val);
}

TEST(Common, PointerCastWriteBadAlignment) {
  uint32_t expected_vals[2];
  uint8_t* buf = reinterpret_cast<uint8_t*>(&expected_vals);
  translator::Span<uint8_t> span(buf + 1, sizeof(uint32_t));

  uint32_t* val = translator::SafePointerCastWrite<uint32_t>(span);

  ASSERT_EQ(nullptr, val);
}

}  // namespace
