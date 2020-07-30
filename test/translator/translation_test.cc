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
  translator::Span<uint8_t> buffer;
  translator::ApiStatus status = translation.Complete(cpl_data, buffer);
  EXPECT_EQ(translator::ApiStatus::kFailure, status);
}

TEST(Translation, ShouldReturnEmptyCmdSpan) {
  translator::Translation translation = {};
  translator::Span<const nvme::GenericQueueEntryCmd> cmds =
      translation.GetNvmeCmds();
  EXPECT_EQ(0, cmds.size());
}

}  // namespace
