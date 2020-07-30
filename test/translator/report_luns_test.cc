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

#include "lib/translator/report_luns.h"
#include "lib/translator/common.h"

#include "absl/base/casts.h"
#include "gtest/gtest.h"

#include <byteswap.h>
#include <netinet/in.h>

// Report Luns Translation Tests

namespace {

TEST(reportLunsToNvme, shouldReturnCorrectCommand) {
  nvme::GenericQueueEntryCmd nvme_cmd = {};
  scsi::ReportLunsCommand scsi_cmd = {};
  uint64_t expected_prp = 2323;
  uint32_t expected_alloc_len = 344;
  scsi_cmd.alloc_length = htonl(expected_alloc_len);

  uint8_t* buf_ptr = reinterpret_cast<uint8_t*>(&scsi_cmd);
  translator::Span<uint8_t> scsi_cmd_span(buf_ptr,
                                          sizeof(scsi::ReportLunsCommand));

  auto alloc_callback = [](uint16_t count) -> uint64_t { return 2323; };
  void (*dealloc_callback)(uint64_t, uint16_t) = nullptr;
  translator::SetAllocPageCallbacks(alloc_callback, dealloc_callback);

  translator::Allocation allocation = {};
  uint32_t actual_alloc_len;
  translator::StatusCode actual_status = translator::ReportLunsToNvme(
      scsi_cmd_span, nvme_cmd, allocation, actual_alloc_len);

  EXPECT_EQ(translator::StatusCode::kSuccess, actual_status);

  EXPECT_EQ(static_cast<uint8_t>(nvme::AdminOpcode::kIdentify), nvme_cmd.opc);
  EXPECT_EQ(expected_prp, nvme_cmd.dptr.prp.prp1);
  EXPECT_EQ(0x2, nvme_cmd.cdw[0]);

  EXPECT_EQ(2323, allocation.data_addr);
  EXPECT_EQ(1, allocation.data_page_count);
  EXPECT_EQ(expected_alloc_len, actual_alloc_len);
}

TEST(reportLunsToScsi, shouldFillBufferCorrectly) {
  nvme::IdentifyNamespaceList ns_list;
  uint32_t ns_list_size = 125;
  for (uint32_t i = 0; i < ns_list_size; ++i) {
    ns_list.ids[i] = bswap_32(htonl(i + 1));
  }
  nvme::GenericQueueEntryCmd identify_cmd = {};
  identify_cmd.dptr.prp.prp1 = reinterpret_cast<uint64_t>(&ns_list);

  uint32_t lun_list_byte_size = sizeof(scsi::LunAddress) * ns_list_size;
  uint32_t buf_len = sizeof(scsi::ReportLunsParamData) + lun_list_byte_size;
  uint8_t buffer[buf_len];
  translator::Span<uint8_t> span(buffer, buf_len);

  translator::StatusCode actual_status =
      translator::ReportLunsToScsi(identify_cmd, span);

  // Verify status code & byte length field
  EXPECT_EQ(translator::StatusCode::kSuccess, actual_status);
  scsi::ReportLunsParamData actual_response;
  translator::ReadValue(span, actual_response);
  EXPECT_EQ(lun_list_byte_size, ntohl(actual_response.list_byte_length));

  // Verify list contents
  scsi::LunAddress* lun_list;
  lun_list =
      reinterpret_cast<scsi::LunAddress*>(buffer + sizeof(actual_response));
  for (scsi::LunAddress i = 0; i < ns_list_size; ++i) {
    EXPECT_EQ(i, translator::ntohll(lun_list[i]));
  }
}

TEST(reportLunsToScsi, shouldFailNotEnoughMemory) {
  nvme::GenericQueueEntryCmd cmd_data;
  uint8_t* buf = nullptr;
  translator::Span<uint8_t> span(buf, 0);

  translator::StatusCode actualStatus =
      translator::ReportLunsToScsi(cmd_data, span);

  EXPECT_EQ(translator::StatusCode::kFailure, actualStatus);
}

TEST(reportLunsToScsi, shouldFailNullptr) {
  nvme::GenericQueueEntryCmd cmd_data = {};
  uint8_t buf[100];
  translator::Span<uint8_t> span = buf;
  cmd_data.dptr.prp.prp1 = 0;

  translator::StatusCode actualStatus =
      translator::ReportLunsToScsi(cmd_data, span);

  EXPECT_EQ(translator::StatusCode::kFailure, actualStatus);
}

}  // namespace
