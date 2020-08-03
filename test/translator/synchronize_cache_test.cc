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

#include "lib/translator/synchronize_cache.h"

#include "gtest/gtest.h"

namespace {

TEST(SynchronizeCache10Test, ShouldBuildNvmeFlushCommand) {
  nvme::GenericQueueEntryCmd nvme_cmd;
  uint32_t nsid = 0x12345;

  translator::SynchronizeCache10ToNvme(nvme_cmd, nsid);

  EXPECT_EQ(static_cast<uint8_t>(nvme::NvmOpcode::kFlush), nvme_cmd.opc);
  EXPECT_EQ(nsid, nvme_cmd.nsid);
}

}  // namespace
