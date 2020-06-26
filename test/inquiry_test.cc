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

#include <stdlib.h>

#include "gtest/gtest.h"
#include "lib/translate/inquiry.h"

// Tests

namespace {

TEST(TranslteInquiryRawToScsi, empty) {
  absl::Span<const uint8_t> raw_cmd;
  scsi_defs::InquiryCommand result_cmd;
  translator::StatusCode status =
      translator::RawToScsiCommand(raw_cmd, result_cmd);
  ASSERT_NE(status, translator::StatusCode::kSuccess);
}

TEST(TranslteInquiryRawToScsi, wrongOp) {
  const uint8_t buf = 4;
  absl::Span<const uint8_t> raw_cmd = absl::MakeSpan(&buf, 1);
  scsi_defs::InquiryCommand result_cmd;
  translator::StatusCode status =
      translator::RawToScsiCommand(raw_cmd, result_cmd);
  ASSERT_NE(status, translator::StatusCode::kSuccess);
}

TEST(TranslteInquiryRawToScsi, defaultSuccess) {
  uint32_t sz = 1 + sizeof(scsi_defs::InquiryCommand);
  uint8_t *buf = (uint8_t *)malloc(4 * sz);
  buf[0] = static_cast<uint8_t>(scsi_defs::OpCode::kInquiry);

  scsi_defs::InquiryCommand *cmd = (scsi_defs::InquiryCommand *)&buf[1];
  *cmd = scsi_defs::InquiryCommand();
  absl::Span<const uint8_t> raw_cmd = absl::MakeSpan(buf, sz);
  ASSERT_FALSE(raw_cmd.empty());

  scsi_defs::InquiryCommand result_cmd;
  translator::StatusCode status =
      translator::RawToScsiCommand(raw_cmd, result_cmd);
  ASSERT_EQ(status, translator::StatusCode::kSuccess);

  ASSERT_EQ(result_cmd.reserved, 0);
  ASSERT_EQ(result_cmd.obsolete, 0);
  ASSERT_EQ(result_cmd.evpd, 0);
  ASSERT_EQ(result_cmd.allocation_length, 0);
}

// TODO: test invalid parameters in scsi_command

TEST(TranslteInquiryRawToScsi, customSuccess) {
  uint32_t sz = 1 + sizeof(scsi_defs::InquiryCommand);
  uint8_t *buf = (uint8_t *)malloc(4 * sz);
  buf[0] = static_cast<uint8_t>(scsi_defs::OpCode::kInquiry);

  scsi_defs::InquiryCommand *cmd = (scsi_defs::InquiryCommand *)&buf[1];
  *cmd = scsi_defs::InquiryCommand();
  cmd->evpd = 1;
  cmd->allocation_length = 29;

  absl::Span<const uint8_t> raw_cmd = absl::MakeSpan(buf, sz);
  ASSERT_FALSE(raw_cmd.empty());

  scsi_defs::InquiryCommand result_cmd;
  translator::StatusCode status =
      translator::RawToScsiCommand(raw_cmd, result_cmd);
  ASSERT_EQ(status, translator::StatusCode::kSuccess);

  ASSERT_EQ(result_cmd.reserved, 0);
  ASSERT_EQ(result_cmd.obsolete, 0);
  ASSERT_EQ(result_cmd.evpd, 1);
  ASSERT_EQ(result_cmd.allocation_length, 29);
}

TEST(TranslateStandardInquiryResponse, Success) {
  nvme_defs::IdentifyNamespace ns_data = nvme_defs::IdentifyNamespace();
  nvme_defs::IdentifyControllerData ctrl_data =
      nvme_defs::IdentifyControllerData();

  ctrl_data.mn[0] = 0x42;
  ctrl_data.mn[15] = 0x28;

  ctrl_data.fr[0] = 'a';
  ctrl_data.fr[1] = ' ';
  ctrl_data.fr[2] = 'b';
  ctrl_data.fr[3] = 'c';
  ctrl_data.fr[4] = ' ';
  ctrl_data.fr[5] = ' ';
  ctrl_data.fr[6] = ' ';
  ctrl_data.fr[7] = 'd';

  scsi_defs::InquiryData result =
      translator::TranslateStandardInquiryResponse(ctrl_data, ns_data);
  ASSERT_EQ(result.peripheral_qualifier,
            static_cast<scsi_defs::PeripheralQualifier>(0));
  ASSERT_EQ(result.peripheral_device_type,
            static_cast<scsi_defs::PeripheralDeviceType>(0));
  ASSERT_EQ(result.rmb, 0);
  ASSERT_EQ(result.version, static_cast<scsi_defs::Version>(0x6));
  ASSERT_EQ(result.normaca, 0);
  ASSERT_EQ(result.hisup, 0);
  ASSERT_EQ(result.response_data_format,
            static_cast<scsi_defs::ResponseDataFormat>(0b10));
  ASSERT_EQ(result.additional_length, 0x1f);
  ASSERT_EQ(result.sccs, 0);
  ASSERT_EQ(result.acc, 0);
  ASSERT_EQ(result.tpgs, static_cast<scsi_defs::TPGS>(0));
  ASSERT_EQ(result.third_party_copy, 0);
  ASSERT_EQ(result.protect,
            (ns_data.dps.pit == 0 && ns_data.dps.md_start == 0) ? 0 : 1);
  ASSERT_EQ(result.encserv, 0);
  ASSERT_EQ(result.multip, 0);
  ASSERT_EQ(result.addr_16, 0);
  ASSERT_EQ(result.wbus_16, 0);
  ASSERT_EQ(result.sync, 0);
  ASSERT_EQ(result.cmdque, 1);

  char *c = result.vendor_identification;
  ASSERT_EQ(c[0], 'N');
  ASSERT_EQ(c[1], 'V');
  ASSERT_EQ(c[2], 'M');
  ASSERT_EQ(c[3], 'e');
  ASSERT_EQ(c[4], ' ');
  ASSERT_EQ(c[5], ' ');
  ASSERT_EQ(c[6], ' ');
  ASSERT_EQ(c[7], ' ');

  for (int i = 0; i < 16; i++) {
    ASSERT_EQ(result.product_identification[i], ctrl_data.mn[i]);
  }

  ASSERT_EQ(result.product_revision_level[0], 'a');
  ASSERT_EQ(result.product_revision_level[1], 'b');
  ASSERT_EQ(result.product_revision_level[2], 'c');
  ASSERT_EQ(result.product_revision_level[3], 'd');
}

// TODO: relies on nvme exec function impl
TEST(TranslateStandardInquiryResponse, badControllerData) {}

// TODO: relies on nvme exec function impl
TEST(TranslateStandardInquiryResponse, badNamespaceData) {}

// TODO: relies on nvme exec function impl
TEST(TranslateStandardInquiryResponse, noData) {}

TEST(SupportedVpdPages, Success) {
  scsi_defs::SupportedVitalProductData result =
      translator::BuildSupportedVpdPages();

  ASSERT_EQ(result.peripheral_qualifier,
            scsi_defs::PeripheralQualifier::kPeripheralDeviceConnected);
  ASSERT_EQ(result.peripheral_device_type,
            scsi_defs::PeripheralDeviceType::kDirectAccessBlock);
  ASSERT_EQ(result.page_code, 0);
  ASSERT_EQ(result.page_length, 5);

  ASSERT_EQ(result.supported_page_list[0], scsi_defs::PageCode::kSupportedVpd);
  ASSERT_EQ(result.supported_page_list[1],
            scsi_defs::PageCode::kUnitSerialNumber);
  ASSERT_EQ(result.supported_page_list[2],
            scsi_defs::PageCode::kDeviceIdentification);
  ASSERT_EQ(result.supported_page_list[3], scsi_defs::PageCode::kExtended);
  ASSERT_EQ(result.supported_page_list[4],
            scsi_defs::PageCode::kBlockLimitsVpd);
  ASSERT_EQ(result.supported_page_list[5],
            scsi_defs::PageCode::kBlockDeviceCharacteristicsVpd);
  ASSERT_EQ(result.supported_page_list[6],
            scsi_defs::PageCode::kLogicalBlockProvisioningVpd);
}

TEST(TranslateUnitSerialNumberVpd, eui64) {
  nvme_defs::IdentifyNamespace identify_namespace_data =
      nvme_defs::IdentifyNamespace();

  identify_namespace_data.eui64 = 0x123456789abcdefa;
  identify_namespace_data.nguid[0] = 0;
  identify_namespace_data.nguid[1] = 0;

  scsi_defs::UnitSerialNumber result =
      translator::TranslateUnitSerialNumberVpdResponse(identify_namespace_data);
  ASSERT_EQ(result.peripheral_qualifier,
            scsi_defs::PeripheralQualifier::kPeripheralDeviceConnected);
  ASSERT_EQ(result.peripheral_device_type,
            scsi_defs::PeripheralDeviceType::kDirectAccessBlock);
  ASSERT_EQ(result.page_code, 0x80);
  ASSERT_EQ(result.page_length, 20);

  char formatted_hex_string[21] = "1234_5678_9abc_defa.";
  for (int i = 0; i < 20; i++) {
    ASSERT_EQ(result.product_serial_number[i], formatted_hex_string[i]);
  }
}

TEST(TranslateUnitSerialNumberVpd, nguid) {
  nvme_defs::IdentifyNamespace identify_namespace_data =
      nvme_defs::IdentifyNamespace();

  identify_namespace_data.eui64 = 0;
  identify_namespace_data.nguid[0] = 0x123456789abcdefa;
  identify_namespace_data.nguid[1] = 0x123456789abcdefa;

  scsi_defs::UnitSerialNumber result =
      translator::TranslateUnitSerialNumberVpdResponse(identify_namespace_data);
  ASSERT_EQ(result.peripheral_qualifier,
            scsi_defs::PeripheralQualifier::kPeripheralDeviceConnected);
  ASSERT_EQ(result.peripheral_device_type,
            scsi_defs::PeripheralDeviceType::kDirectAccessBlock);
  ASSERT_EQ(result.page_code, 0x80);
  ASSERT_EQ(result.page_length, 40);
  char formatted_hex_string[41] = "1234_5678_9abc_defa_1234_5678_9abc_defa.";

  ASSERT_STREQ((char *)result.product_serial_number, formatted_hex_string);
}

TEST(TranslateUnitSerialNumberVpd, both) {
  nvme_defs::IdentifyNamespace identify_namespace_data =
      nvme_defs::IdentifyNamespace();

  identify_namespace_data.eui64 = 0x123456789abcdefa;
  identify_namespace_data.nguid[0] = 0x123456789abcdefa;
  identify_namespace_data.nguid[1] = 0x123456789abcdefa;

  scsi_defs::UnitSerialNumber result =
      translator::TranslateUnitSerialNumberVpdResponse(identify_namespace_data);
  ASSERT_EQ(result.peripheral_qualifier,
            scsi_defs::PeripheralQualifier::kPeripheralDeviceConnected);
  ASSERT_EQ(result.peripheral_device_type,
            scsi_defs::PeripheralDeviceType::kDirectAccessBlock);
  ASSERT_EQ(result.page_code, 0x80);
  ASSERT_EQ(result.page_length, 40);
  char formatted_hex_string[41] = "1234_5678_9abc_defa_1234_5678_9abc_defa.";

  ASSERT_STREQ((char *)result.product_serial_number, formatted_hex_string);
}
}  // namespace
