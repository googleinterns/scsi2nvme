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

#include "../lib/translate/inquiry.h"

#include <stdlib.h>

#include "gtest/gtest.h"

// Tests

namespace {

TEST(translteInquiryRawToSCSI, empty) {
    absl::Span<const uint32_t> raw_cmd;
    std::optional<scsi_defs::InquiryCommand> result = inquiry::raw_cmd_to_scsi_command(raw_cmd);
    ASSERT_FALSE(result.has_value());
}

TEST(translteInquiryRawToSCSI, wrongOp) {
    const uint32_t buf = 4;
    absl::Span<const uint32_t> raw_cmd = absl::MakeSpan(&buf, 1);
    std::optional<scsi_defs::InquiryCommand> result = inquiry::raw_cmd_to_scsi_command(raw_cmd);
    ASSERT_FALSE(result.has_value());
}

TEST(translteInquiryRawToSCSI, defaultSuccess) {
    uint32_t sz = 1+sizeof(scsi_defs::InquiryCommand);
    uint32_t * buf = (uint32_t*)malloc(4 * sz);
    buf[0] = static_cast<uint32_t>(scsi_defs::OpCode::kInquiry);

    scsi_defs::InquiryCommand * cmd = (scsi_defs::InquiryCommand *) &buf[1];
    *cmd = scsi_defs::InquiryCommand();
    absl::Span<const uint32_t> raw_cmd = absl::MakeSpan(buf, sz);
    ASSERT_FALSE(raw_cmd.empty());

    std::optional<scsi_defs::InquiryCommand> result = inquiry::raw_cmd_to_scsi_command(raw_cmd);
    ASSERT_TRUE(result.has_value());

    scsi_defs::InquiryCommand result_cmd = result.value();
    ASSERT_EQ(result_cmd.reserved, 0);
    ASSERT_EQ(result_cmd.obsolete, 0);
    ASSERT_EQ(result_cmd.evpd, 0);
    ASSERT_EQ(result_cmd.page_code, 0);
    ASSERT_EQ(result_cmd.allocation_length, 0);
}

// TODO: test invalid parameters in scsi_command
TEST(translteInquiryRawToSCSI, customSuccess) {
    uint32_t sz = 1+sizeof(scsi_defs::InquiryCommand);
    uint32_t * buf = (uint32_t*)malloc(4 * sz);
    buf[0] = static_cast<uint32_t>(scsi_defs::OpCode::kInquiry);

    scsi_defs::InquiryCommand * cmd = (scsi_defs::InquiryCommand *) &buf[1];
    *cmd = scsi_defs::InquiryCommand();
    cmd->evpd = 1;
    cmd->page_code = 4;
    cmd->allocation_length = 29;

    absl::Span<const uint32_t> raw_cmd = absl::MakeSpan(buf, sz);
    ASSERT_FALSE(raw_cmd.empty());

    std::optional<scsi_defs::InquiryCommand> result = inquiry::raw_cmd_to_scsi_command(raw_cmd);
    ASSERT_TRUE(result.has_value());

    scsi_defs::InquiryCommand result_cmd = result.value();
    ASSERT_EQ(result_cmd.reserved, 0);
    ASSERT_EQ(result_cmd.obsolete, 0);
    ASSERT_EQ(result_cmd.evpd, 1);
    ASSERT_EQ(result_cmd.page_code, 4);
    ASSERT_EQ(result_cmd.allocation_length, 29);
}

TEST(translateStandardInquiryResponse, Success) {
    nvme_defs::IdentifyNamespace ns_data = nvme_defs::IdentifyNamespace();
    nvme_defs::IdentifyControllerData ctrl_data = nvme_defs::IdentifyControllerData();

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

    scsi_defs::InquiryData result = inquiry::translate_standard_inquiry_response(ctrl_data, ns_data);
    ASSERT_EQ(result.peripheral_qualifier, static_cast<scsi_defs::PeripheralQualifier>(0));
    ASSERT_EQ(result.peripheral_device_type, static_cast<scsi_defs::PeripheralDeviceType>(0));
    ASSERT_EQ(result.rmb, 0);
    ASSERT_EQ(result.version, static_cast<scsi_defs::Version>(0x6));
    ASSERT_EQ(result.normaca, 0);
    ASSERT_EQ(result.hisup, 0);
    ASSERT_EQ(result.response_data_format, static_cast<scsi_defs::ResponseDataFormat>(0b10));
    ASSERT_EQ(result.additional_length, 0x1f);
    ASSERT_EQ(result.sccs, 0);
    ASSERT_EQ(result.acc, 0);
    ASSERT_EQ(result.tpgs, static_cast<scsi_defs::TPGS>(0));
    ASSERT_EQ(result.third_party_copy, 0);
    ASSERT_EQ(result.protect, (ns_data.dps.pit == 0 && ns_data.dps.md_start == 0) ? 0 : 1);
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

    for(int i = 0; i < 16; i++) {
        ASSERT_EQ(result.product_identification[i], ctrl_data.mn[i]);
    }

    ASSERT_EQ(result.product_revision_level[0], 'a');
    ASSERT_EQ(result.product_revision_level[1], 'b');
    ASSERT_EQ(result.product_revision_level[2], 'c');
    ASSERT_EQ(result.product_revision_level[3], 'd');
}

// TODO?
TEST(translateStandardInquiryResponse, badControllerData) {

}
// TODO?
TEST(translateStandardInquiryResponse, badNamespaceData) {

}
// TODO?
TEST(translateStandardInquiryResponse, noData) {

}

TEST(supportedVPDPages, Success) {
    scsi_defs::SupportedVitalProductData result = inquiry::build_supported_vpd_pages();

    ASSERT_EQ(result.peripheral_qualifier , scsi_defs::PeripheralQualifier::kPeripheralDeviceConnected);
    ASSERT_EQ(result.peripheral_device_type , scsi_defs::PeripheralDeviceType::kDirectAccessBlock);
    ASSERT_EQ(result.page_code , 0);
    ASSERT_EQ(result.page_length , 5);
    ASSERT_EQ(result.supported_page_list[0] , 0x00);
    ASSERT_EQ(result.supported_page_list[1] , 0x80);
    ASSERT_EQ(result.supported_page_list[2] , 0x83);
    ASSERT_EQ(result.supported_page_list[3] , 0x86);
    ASSERT_EQ(result.supported_page_list[4] , 0xB0);
    ASSERT_EQ(result.supported_page_list[5] , 0xB1);
    ASSERT_EQ(result.supported_page_list[6] , 0xB2);
}

TEST(translateUnitSerialNumberVPD, eui64) {
    nvme_defs::IdentifyNamespace identify_namespace_data = nvme_defs::IdentifyNamespace();

    identify_namespace_data.eui64 = 0x123456789abcdefa;
    identify_namespace_data.nguid[0] = 0;
    identify_namespace_data.nguid[1] = 0;

    scsi_defs::UnitSerialNumber result = inquiry::translate_unit_serial_number_vpd_response(identify_namespace_data);
    ASSERT_EQ(result.peripheral_qualifier, scsi_defs::PeripheralQualifier::kPeripheralDeviceConnected);
    ASSERT_EQ(result.peripheral_device_type, scsi_defs::PeripheralDeviceType::kDirectAccessBlock);
    ASSERT_EQ(result.page_code, 0x80);
    ASSERT_EQ(result.page_length, 20);

    char formatted_hex_string[21] = "1234_5678_9abc_defa.";
    for(int i = 0; i < 20; i++) {
        ASSERT_EQ(result.product_serial_number[i], formatted_hex_string[i]);
    }
}



TEST(translateUnitSerialNumberVPD, nguid) {
    nvme_defs::IdentifyNamespace identify_namespace_data = nvme_defs::IdentifyNamespace();

    identify_namespace_data.eui64 = 0;
    identify_namespace_data.nguid[0] = 0x123456789abcdefa;
    identify_namespace_data.nguid[1] = 0x123456789abcdefa;

    scsi_defs::UnitSerialNumber result = inquiry::translate_unit_serial_number_vpd_response(identify_namespace_data);
    ASSERT_EQ(result.peripheral_qualifier, scsi_defs::PeripheralQualifier::kPeripheralDeviceConnected);
    ASSERT_EQ(result.peripheral_device_type, scsi_defs::PeripheralDeviceType::kDirectAccessBlock);
    ASSERT_EQ(result.page_code, 0x80);
    ASSERT_EQ(result.page_length, 40);
    char formatted_hex_string[41] = "1234_5678_9abc_defa_1234_5678_9abc_defa.";
    
    ASSERT_STREQ((char*)result.product_serial_number, formatted_hex_string);
}

TEST(translateUnitSerialNumberVPD, both) {
    nvme_defs::IdentifyNamespace identify_namespace_data = nvme_defs::IdentifyNamespace();

    identify_namespace_data.eui64 = 0x123456789abcdefa;
    identify_namespace_data.nguid[0] = 0x123456789abcdefa;
    identify_namespace_data.nguid[1] = 0x123456789abcdefa;

    scsi_defs::UnitSerialNumber result = inquiry::translate_unit_serial_number_vpd_response(identify_namespace_data);
    ASSERT_EQ(result.peripheral_qualifier, scsi_defs::PeripheralQualifier::kPeripheralDeviceConnected);
    ASSERT_EQ(result.peripheral_device_type, scsi_defs::PeripheralDeviceType::kDirectAccessBlock);
    ASSERT_EQ(result.page_code, 0x80);
    ASSERT_EQ(result.page_length, 40);
    char formatted_hex_string[41] = "1234_5678_9abc_defa_1234_5678_9abc_defa.";
    
    ASSERT_STREQ((char*)result.product_serial_number, formatted_hex_string);
}
}

