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

#include "lib/translator/mode_sense.h"

#include "gtest/gtest.h"

#include <netinet/in.h>

namespace {

// Tests mode sense to nvme translation

TEST(TranslateModeSenseToNvme, ShouldReturnNoCommands) {
  translator::Span<translator::NvmeCmdWrapper> nvme_wrappers;
  translator::Allocation allocation = {};
  uint32_t nsid = 1;
  uint32_t cmd_count = 0;
  uint32_t alloc_len = 0;
  scsi::ModeSense6Command ms6_cmd = {
      .dbd = 1,
      .page_code = scsi::ModePageCode::kPowerConditionMode,
      .alloc_length = 50,
  };
  translator::Span<uint8_t> scsi_cmd(reinterpret_cast<uint8_t*>(&ms6_cmd),
                                     sizeof(ms6_cmd));

  translator::StatusCode status_code = translator::ModeSense6ToNvme(
      scsi_cmd, nvme_wrappers, allocation, nsid, cmd_count, alloc_len);

  EXPECT_EQ(translator::StatusCode::kSuccess, status_code);
  EXPECT_EQ(cmd_count, 0);
  EXPECT_EQ(alloc_len, 50);
}

TEST(TranslateModeSenseToNvme, ShouldReturnGetFeatures) {
  translator::NvmeCmdWrapper nvme_wrapper;
  translator::Span<translator::NvmeCmdWrapper> nvme_wrappers(&nvme_wrapper, 1);
  translator::Allocation allocation = {};
  uint32_t nsid = 32;
  uint32_t cmd_count = 0;
  uint32_t alloc_len = 0;
  scsi::ModeSense6Command ms6_cmd = {
      .dbd = 1,
      .page_code = scsi::ModePageCode::kCacheMode,
      .pc = scsi::PageControl::kDefault,
      .alloc_length = 25};
  translator::Span<uint8_t> scsi_cmd(reinterpret_cast<uint8_t*>(&ms6_cmd),
                                     sizeof(ms6_cmd));

  translator::StatusCode status_code = translator::ModeSense6ToNvme(
      scsi_cmd, nvme_wrappers, allocation, nsid, cmd_count, alloc_len);

  EXPECT_EQ(translator::StatusCode::kSuccess, status_code);
  EXPECT_EQ(cmd_count, 1);
  EXPECT_EQ(alloc_len, 25);
  EXPECT_EQ(true, nvme_wrappers[0].is_admin);

  nvme::GetFeaturesCmd* nvme_cmd_ptr =
      reinterpret_cast<nvme::GetFeaturesCmd*>(&(nvme_wrapper.cmd));

  EXPECT_EQ(static_cast<uint8_t>(nvme::AdminOpcode::kGetFeatures),
            nvme_cmd_ptr->opc);
  EXPECT_EQ(nsid, nvme_cmd_ptr->nsid);
  EXPECT_EQ(nvme::FeatureSelect::kDefault, nvme_cmd_ptr->sel);
  EXPECT_EQ(nvme::FeatureType::kVolatileWriteCache, nvme_cmd_ptr->fid);
}

TEST(TranslateModeSenseToNvme, ShouldReturnDbdCommands) {
  auto alloc_callback = [](uint16_t count) -> uint64_t { return 2323; };
  void (*dealloc_callback)(uint64_t, uint16_t) = nullptr;
  translator::SetAllocPageCallbacks(alloc_callback, dealloc_callback);

  translator::NvmeCmdWrapper nvme_wrappers[2];
  translator::Allocation allocation = {};
  uint32_t nsid = 32;
  uint32_t cmd_count = 0;
  uint32_t alloc_len = 0;
  scsi::ModeSense6Command ms6_cmd = {
      .dbd = 0,
      .page_code = scsi::ModePageCode::kCacheMode,
      .alloc_length = 25};
  translator::Span<uint8_t> scsi_cmd(reinterpret_cast<uint8_t*>(&ms6_cmd),
                                     sizeof(ms6_cmd));

  translator::StatusCode status_code = translator::ModeSense6ToNvme(
      scsi_cmd, nvme_wrappers, allocation, nsid, cmd_count, alloc_len);

  EXPECT_EQ(translator::StatusCode::kSuccess, status_code);
  EXPECT_EQ(cmd_count, 2);
  EXPECT_EQ(alloc_len, 25);

  EXPECT_EQ(2323, allocation.data_addr);
  EXPECT_EQ(1, allocation.data_page_count);

  EXPECT_EQ(static_cast<uint8_t>(nvme::AdminOpcode::kIdentify),
            nvme_wrappers[0].cmd.opc);
  EXPECT_EQ(nsid, nvme_wrappers[0].cmd.nsid);
  EXPECT_EQ(2323, nvme_wrappers[0].cmd.dptr.prp.prp1);
  EXPECT_EQ(0x0, nvme_wrappers[0].cmd.cdw[0]);
  EXPECT_EQ(true, nvme_wrappers[0].is_admin);

  EXPECT_EQ(static_cast<uint8_t>(nvme::AdminOpcode::kGetFeatures),
            nvme_wrappers[1].cmd.opc);
  EXPECT_EQ(true, nvme_wrappers[1].is_admin);
}

// Tests mode sense 6 to scsi translation

TEST(TranslateModeSense6ToScsi, ShouldReturnCorrectBuffer) {
  uint8_t expected_buffer_size =
      sizeof(scsi::ModeParameter6Header) +
      sizeof(scsi::ShortLbaBlockDescriptor) + sizeof(scsi::CachingModePage) +
      sizeof(scsi::ControlModePage) + sizeof(scsi::PowerConditionModePage);

  // Create identify cmd
  nvme::GenericQueueEntryCmd identify_cmd = {};
  uint8_t expected_bd_factor = 5;
  nvme::IdentifyNamespace id_ns = {.ncap = 400};
  id_ns.flbas.format = 3;
  id_ns.lbaf[3].lbads = expected_bd_factor;
  identify_cmd.dptr.prp.prp1 = reinterpret_cast<uint64_t>(&id_ns);

  // Create SCSI command
  scsi::ModeSense6Command ms6_cmd = {
      .dbd = 0,
      .page_code = scsi::ModePageCode::kAllSupportedModes,
      .alloc_length = expected_buffer_size,
  };
  translator::Span<uint8_t> scsi_cmd(reinterpret_cast<uint8_t*>(&ms6_cmd),
                                     sizeof(ms6_cmd));

  // Define misc params
  uint32_t get_features_result = 0b01;
  uint8_t buffer[expected_buffer_size];
  translator::Span<uint8_t> span_buf(buffer, expected_buffer_size);

  translator::StatusCode status_code = translator::ModeSense6ToScsi(
      scsi_cmd, identify_cmd, get_features_result, span_buf);

  EXPECT_EQ(translator::StatusCode::kSuccess, status_code);

  // Validate header
  scsi::ModeParameter6Header header;
  translator::ReadValue(span_buf, header);
  // Subtract size of mode_data_length variable (1 byte)
  EXPECT_EQ(expected_buffer_size - 1, header.mode_data_length);
  EXPECT_EQ(0, header.medium_type);
  EXPECT_EQ(0, header.wp);
  EXPECT_EQ(0x1, header.dpofua);
  EXPECT_EQ(sizeof(scsi::ShortLbaBlockDescriptor), header.bdl);
  span_buf = span_buf.subspan(sizeof(header));

  // Validate block descriptor
  scsi::ShortLbaBlockDescriptor block_descriptor;
  translator::ReadValue(span_buf, block_descriptor);
  EXPECT_EQ(id_ns.ncap, ntohl(block_descriptor.number_of_blocks));
  uint32_t lbl = block_descriptor.logical_block_length;
  if (translator::IsLittleEndian()) lbl <<= 8;
  EXPECT_EQ(1 << expected_bd_factor, ntohl(lbl));
  span_buf = span_buf.subspan(sizeof(block_descriptor));

  // Validate Cache Page
  scsi::CachingModePage cache_mode_page;
  translator::ReadValue(span_buf, cache_mode_page);
  EXPECT_EQ(scsi::ModePageCode::kCacheMode, cache_mode_page.page_code);
  EXPECT_EQ(get_features_result, cache_mode_page.wce);
  span_buf = span_buf.subspan(sizeof(cache_mode_page));

  // Validate Control Page
  scsi::ControlModePage control_mode_page;
  translator::ReadValue(span_buf, control_mode_page);
  EXPECT_EQ(scsi::ModePageCode::kControlMode, control_mode_page.page_code);
  span_buf = span_buf.subspan(sizeof(control_mode_page));

  // Validate Power Condition Page
  scsi::PowerConditionModePage power_condition_mode_page;
  translator::ReadValue(span_buf, power_condition_mode_page);
  EXPECT_EQ(scsi::ModePageCode::kPowerConditionMode,
            power_condition_mode_page.page_code);
}

// Tests mode sense 10 to scsi translation

TEST(TranslateModeSense10ToScsi, ShouldReturnCorrectBuffer) {
  uint8_t expected_buffer_size =
      sizeof(scsi::ModeParameter10Header) +
      sizeof(scsi::LongLbaBlockDescriptor) + sizeof(scsi::CachingModePage) +
      sizeof(scsi::ControlModePage) + sizeof(scsi::PowerConditionModePage);

  // Create identify cmd
  nvme::GenericQueueEntryCmd identify_cmd = {};
  uint8_t expected_bd_factor = 10;
  nvme::IdentifyNamespace id_ns = {.ncap = 400};
  id_ns.flbas.format = 3;
  id_ns.lbaf[3].lbads = expected_bd_factor;
  identify_cmd.dptr.prp.prp1 = reinterpret_cast<uint64_t>(&id_ns);

  // Create SCSI command
  scsi::ModeSense10Command ms10_cmd = {
      .dbd = 0,
      .llbaa = true,
      .page_code = scsi::ModePageCode::kAllSupportedModes,
      .alloc_length = expected_buffer_size};
  translator::Span<uint8_t> scsi_cmd(reinterpret_cast<uint8_t*>(&ms10_cmd),
                                     sizeof(ms10_cmd));

  // Define misc params
  uint32_t get_features_result = 0b00;
  uint8_t buffer[expected_buffer_size];
  translator::Span<uint8_t> span_buf(buffer, expected_buffer_size);

  translator::StatusCode status_code = translator::ModeSense10ToScsi(
      scsi_cmd, identify_cmd, get_features_result, span_buf);

  EXPECT_EQ(translator::StatusCode::kSuccess, status_code);

  // Validate header
  scsi::ModeParameter10Header header;
  translator::ReadValue(span_buf, header);
  // Subtract size of mode_data_length variable (2 bytes)
  EXPECT_EQ(expected_buffer_size - 2, htons(header.mode_data_length));
  EXPECT_EQ(0, header.medium_type);
  EXPECT_EQ(0, header.wp);
  EXPECT_EQ(0x1, header.dpofua);
  EXPECT_EQ(sizeof(scsi::LongLbaBlockDescriptor), ntohs(header.bdl));
  span_buf = span_buf.subspan(sizeof(header));

  // Validate block descriptor
  scsi::LongLbaBlockDescriptor block_descriptor;
  translator::ReadValue(span_buf, block_descriptor);
  EXPECT_EQ(id_ns.ncap, translator::ntohll(block_descriptor.number_of_blocks));
  EXPECT_EQ(1 << expected_bd_factor,
            htonl(block_descriptor.logical_block_length));
  span_buf = span_buf.subspan(sizeof(block_descriptor));

  // Validate Cache Page
  scsi::CachingModePage cache_mode_page;
  translator::ReadValue(span_buf, cache_mode_page);
  EXPECT_EQ(scsi::ModePageCode::kCacheMode, cache_mode_page.page_code);
  EXPECT_EQ(get_features_result, cache_mode_page.wce);
  span_buf = span_buf.subspan(sizeof(cache_mode_page));

  // Validate Control Page
  scsi::ControlModePage control_mode_page;
  translator::ReadValue(span_buf, control_mode_page);
  EXPECT_EQ(scsi::ModePageCode::kControlMode, control_mode_page.page_code);
  span_buf = span_buf.subspan(sizeof(control_mode_page));

  // Validate Power Condition Page
  scsi::PowerConditionModePage power_condition_mode_page;
  translator::ReadValue(span_buf, power_condition_mode_page);
  EXPECT_EQ(scsi::ModePageCode::kPowerConditionMode,
            power_condition_mode_page.page_code);
}

}  // namespace
