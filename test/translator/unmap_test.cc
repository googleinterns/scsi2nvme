// Copyright 2020 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "lib/translator/unmap.h"

#include "gtest/gtest.h"

#include <netinet/in.h>

// Tests

namespace {

constexpr uint32_t kPageSize = 4096;

TEST(TranslateUnmap, ShouldFillBufferCorrectly) {
  // Define basic test variables
  uint32_t descriptor_count = 3;
  uint32_t nsid = 323;
  uint32_t addr_offset = 500;
  uint32_t count_offset = 7;
  uint16_t data_length = sizeof(scsi::UnmapParamList) +
                         descriptor_count * sizeof(scsi::UnmapBlockDescriptor);
  // Create command structures and buffers
  uint8_t scsi_cmd_buffer[sizeof(scsi::UnmapCommand)];
  scsi::UnmapCommand unmap_cmd = {.param_list_length = htons(data_length)};
  uint8_t buf_out[data_length];
  scsi::UnmapParamList param_list = {};
  param_list.data_length =
      htons(data_length - 2);  // total bytes - bytes in field
  param_list.block_desc_data_length =
      htons(data_length - sizeof(scsi::UnmapParamList));
  scsi::UnmapBlockDescriptor descriptors[descriptor_count];
  for (uint32_t i = 0; i < descriptor_count; ++i) {
    descriptors[i].logical_block_addr = translator::htonll(i + addr_offset);
    descriptors[i].logical_block_count = htonl(i + count_offset);
  }
  // Copy buffers to spans
  translator::Span<uint8_t> scsi_cmd(scsi_cmd_buffer,
                                     sizeof(scsi::UnmapCommand));
  translator::WriteValue(unmap_cmd, scsi_cmd);
  translator::Span<uint8_t> buffer_out(buf_out, data_length);
  translator::WriteValue(param_list, buffer_out);
  auto buf_descriptor_list = buffer_out.subspan(sizeof(param_list));
  for (uint32_t i = 0; i < descriptor_count; ++i) {
    translator::WriteValue(descriptors[i], buf_descriptor_list);
    buf_descriptor_list = buf_descriptor_list.subspan(sizeof(descriptors[i]));
  }

  translator::NvmeCmdWrapper nvme_wrapper = {};
  translator::Allocation allocation = {};

  auto alloc_callback = [](uint32_t page_size, uint16_t count) -> uint64_t {
    void* ptr = malloc(4096);  // Minimum page size of 4096 bytes
    return reinterpret_cast<uint64_t>(ptr);
  };
  void (*dealloc_callback)(uint64_t, uint16_t) = nullptr;
  translator::SetAllocPageCallbacks(alloc_callback, dealloc_callback);

  // Run function we're testing
  translator::StatusCode status_code = translator::UnmapToNvme(
      scsi_cmd, buffer_out, nvme_wrapper, kPageSize, nsid, allocation);

  // Validate outputs
  ASSERT_EQ(status_code, translator::StatusCode::kSuccess);

  EXPECT_EQ(true, nvme_wrapper.is_admin);

  EXPECT_EQ(nsid, nvme_wrapper.cmd.nsid);
  EXPECT_EQ(descriptor_count - 1, nvme_wrapper.cmd.cdw[0]);
  EXPECT_EQ(0b100, nvme_wrapper.cmd.cdw[1]);

  EXPECT_NE(0, allocation.data_addr);
  EXPECT_EQ(1, allocation.data_page_count);

  nvme::DatasetManagmentRange* dmr_ptr =
      reinterpret_cast<nvme::DatasetManagmentRange*>(
          nvme_wrapper.cmd.dptr.prp.prp1);
  for (uint32_t i = 0; i < descriptor_count; ++i) {
    EXPECT_EQ(i + addr_offset, translator::ltohl(dmr_ptr[i].lba));
    EXPECT_EQ(i + count_offset, translator::ltohl(dmr_ptr[i].lb_count));
  }
}

}  // namespace
