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

#include <cstdlib>

#include "gtest/gtest.h"
#include "lib/translator/inquiry.h"

// Tests
namespace {

class InquiryTest : public ::testing::Test {
 protected:
  scsi::InquiryCommand inquiry_cmd_;
  nvme::GenericQueueEntryCmd identify_cmds_[2];
  absl::Span<const uint8_t> scsi_cmd_;
  absl::Span<nvme::GenericQueueEntryCmd> nvme_cmds_;
  nvme::IdentifyControllerData identify_ctrl_;
  nvme::IdentifyNamespace identify_ns_;
  uint8_t buffer_[200];

  // Per-test-suite set-up.
  // Called before the first test in this test suite.
  static void SetUpTestSuite() {
    auto alloc_callback = [](uint16_t count) -> uint64_t {
      // EXPECT_EQ(1, count);
      return 1337;
    };
    void (*dealloc_callback)(uint64_t, uint16_t) = nullptr;
    translator::SetAllocPageCallbacks(alloc_callback, dealloc_callback);
  }

  // You can define per-test set-up logic as usual.
  virtual void SetUp() {
    identify_ctrl_ = {};
    identify_ns_ = {};
    SetController(&identify_ctrl_);
    SetNamespace(&identify_ns_);

    inquiry_cmd_ = {};
    SetCommand();
    nvme_cmds_ = identify_cmds_;
    memset(buffer_, 0, sizeof(buffer_));
  }

  void SetCommand() {
    const uint8_t* cmd_ptr = reinterpret_cast<const uint8_t*>(&inquiry_cmd_);
    scsi_cmd_ = absl::MakeSpan(cmd_ptr, sizeof(scsi::InquiryCommand));
  }

  void SetController(nvme::IdentifyControllerData* identify_ctrl_) {
    identify_cmds_[0].dptr.prp.prp1 =
        reinterpret_cast<uint64_t>(identify_ctrl_);
  }

  void SetNamespace(nvme::IdentifyNamespace* identify_ns_) {
    identify_cmds_[1].dptr.prp.prp1 = reinterpret_cast<uint64_t>(identify_ns_);
  }
};

TEST_F(InquiryTest, InquiryToNvme) {
  inquiry_cmd_.allocation_length = 4096;
  scsi::LunAddress lun = 0x123;
  uint32_t alloc_len;
  translator::Allocation allocations[2] = {{}};

  translator::StatusCode status = translator::InquiryToNvme(
      scsi_cmd_, nvme_cmds_[1], nvme_cmds_[0], alloc_len, lun, allocations);

  EXPECT_EQ(status, translator::StatusCode::kSuccess);

  EXPECT_EQ(alloc_len, inquiry_cmd_.allocation_length);

  EXPECT_EQ(nvme_cmds_[1].opc,
            static_cast<uint8_t>(nvme::AdminOpcode::kIdentify));
  EXPECT_EQ(nvme_cmds_[1].nsid, lun);
  EXPECT_NE(nvme_cmds_[1].dptr.prp.prp1, 0);
  EXPECT_EQ(nvme_cmds_[1].cdw[0], 0);

  EXPECT_EQ(nvme_cmds_[0].opc,
            static_cast<uint8_t>(nvme::AdminOpcode::kIdentify));
  EXPECT_EQ(nvme_cmds_[0].nsid, 0);
  EXPECT_NE(nvme_cmds_[0].dptr.prp.prp1, 0);
  EXPECT_EQ(nvme_cmds_[0].cdw[0], 1);
}

TEST_F(InquiryTest, InquiryToNvmeFailRead) {
  inquiry_cmd_.allocation_length = 4096;
  scsi::LunAddress lun = 0x123;
  uint32_t alloc_len;
  translator::Allocation allocations[2] = {};

  uint8_t bad_buffer_[1] = {};
  translator::StatusCode status = translator::InquiryToNvme(
      bad_buffer_, nvme_cmds_[1], nvme_cmds_[0], alloc_len, lun, allocations);

  EXPECT_EQ(status, translator::StatusCode::kInvalidInput);
}

TEST_F(InquiryTest, StandardInquiry) {
  identify_ctrl_.mn[0] = 0x42;
  identify_ctrl_.mn[15] = 0x28;

  identify_ctrl_.fr[0] = 'a';
  identify_ctrl_.fr[1] = ' ';
  identify_ctrl_.fr[2] = 'b';
  identify_ctrl_.fr[3] = 'c';
  identify_ctrl_.fr[4] = ' ';
  identify_ctrl_.fr[5] = ' ';
  identify_ctrl_.fr[6] = ' ';
  identify_ctrl_.fr[7] = 'd';

  translator::StatusCode status =
      translator::InquiryToScsi(scsi_cmd_, buffer_, nvme_cmds_);
  EXPECT_EQ(status, translator::StatusCode::kSuccess);

  scsi::InquiryData result{};
  EXPECT_TRUE(translator::ReadValue(buffer_, result));

  EXPECT_EQ(result.peripheral_qualifier,
            static_cast<scsi::PeripheralQualifier>(0));
  EXPECT_EQ(result.peripheral_device_type,
            static_cast<scsi::PeripheralDeviceType>(0));
  EXPECT_EQ(result.rmb, 0);
  EXPECT_EQ(result.version, static_cast<scsi::Version>(0x6));
  EXPECT_EQ(result.normaca, 0);
  EXPECT_EQ(result.hisup, 0);
  EXPECT_EQ(result.response_data_format,
            static_cast<scsi::ResponseDataFormat>(0b10));
  EXPECT_EQ(result.additional_length, 0x1f);
  EXPECT_EQ(result.sccs, 0);
  EXPECT_EQ(result.acc, 0);
  EXPECT_EQ(result.tpgs, static_cast<scsi::TPGS>(0));
  EXPECT_EQ(result.third_party_copy, 0);
  EXPECT_EQ(
      result.protect,
      (identify_ns_.dps.pit == 0 && identify_ns_.dps.md_start == 0) ? 0 : 1);
  EXPECT_EQ(result.encserv, 0);
  EXPECT_EQ(result.multip, 0);
  EXPECT_EQ(result.addr_16, 0);
  EXPECT_EQ(result.wbus_16, 0);
  EXPECT_EQ(result.sync, 0);
  EXPECT_EQ(result.cmdque, 1);

  char* c = result.vendor_identification;
  EXPECT_EQ(c[0], 'N');
  EXPECT_EQ(c[1], 'V');
  EXPECT_EQ(c[2], 'M');
  EXPECT_EQ(c[3], 'e');
  EXPECT_EQ(c[4], ' ');
  EXPECT_EQ(c[5], ' ');
  EXPECT_EQ(c[6], ' ');
  EXPECT_EQ(c[7], ' ');

  for (int i = 0; i < 16; i++) {
    EXPECT_EQ(result.product_identification[i], identify_ctrl_.mn[i]);
  }

  EXPECT_EQ(result.product_revision_level[0], 'a');
  EXPECT_EQ(result.product_revision_level[1], 'b');
  EXPECT_EQ(result.product_revision_level[2], 'c');
  EXPECT_EQ(result.product_revision_level[3], 'd');
}

TEST_F(InquiryTest, SupportedVpdPages) {
  inquiry_cmd_ = scsi::InquiryCommand{
      .evpd = 1, .page_code = scsi::PageCode::kSupportedVpd};

  translator::StatusCode status =
      translator::InquiryToScsi(scsi_cmd_, buffer_, nvme_cmds_);

  EXPECT_EQ(status, translator::StatusCode::kSuccess);

  scsi::SupportedVitalProductData result{};
  EXPECT_TRUE(translator::ReadValue(buffer_, result));

  EXPECT_EQ(result.peripheral_qualifier,
            scsi::PeripheralQualifier::kPeripheralDeviceConnected);
  EXPECT_EQ(result.peripheral_device_type,
            scsi::PeripheralDeviceType::kDirectAccessBlock);
  EXPECT_EQ(result.page_code, scsi::PageCode::kSupportedVpd);
  EXPECT_EQ(result.page_length, 7);

  scsi::PageCode supported_page_list[7] = {
      scsi::PageCode::kSupportedVpd,
      scsi::PageCode::kUnitSerialNumber,
      scsi::PageCode::kDeviceIdentification,
      scsi::PageCode::kExtended,
      scsi::PageCode::kBlockLimitsVpd,
      scsi::PageCode::kBlockDeviceCharacteristicsVpd,
      scsi::PageCode::kLogicalBlockProvisioningVpd};

  scsi::PageCode result_list[7];
  absl::Span<uint8_t> span_buffer = buffer_;
  EXPECT_TRUE(translator::ReadValue(
      span_buffer.subspan(sizeof(scsi::SupportedVitalProductData)),
      result_list));

  EXPECT_EQ(result_list[0], supported_page_list[0]);
  EXPECT_EQ(result_list[1], supported_page_list[1]);
  EXPECT_EQ(result_list[2], supported_page_list[2]);
  EXPECT_EQ(result_list[3], supported_page_list[3]);
  EXPECT_EQ(result_list[4], supported_page_list[4]);
  EXPECT_EQ(result_list[5], supported_page_list[5]);
  EXPECT_EQ(result_list[6], supported_page_list[6]);
}

TEST_F(InquiryTest, TranslateUnitSerialNumberVpdEui64) {
  inquiry_cmd_ = scsi::InquiryCommand{
      .evpd = 1, .page_code = scsi::PageCode::kUnitSerialNumber};

  identify_ns_.eui64 = 0x123456789abcdefa;
  identify_ns_.nguid[0] = 0;
  identify_ns_.nguid[1] = 0;

  translator::StatusCode status =
      translator::InquiryToScsi(scsi_cmd_, buffer_, nvme_cmds_);
  EXPECT_EQ(status, translator::StatusCode::kSuccess);

  scsi::UnitSerialNumber result{};
  EXPECT_TRUE(translator::ReadValue(buffer_, result));

  EXPECT_EQ(result.peripheral_qualifier,
            scsi::PeripheralQualifier::kPeripheralDeviceConnected);
  EXPECT_EQ(result.peripheral_device_type,
            scsi::PeripheralDeviceType::kDirectAccessBlock);
  EXPECT_EQ(result.page_code, scsi::PageCode::kUnitSerialNumber);
  EXPECT_EQ(result.page_length, 20);

  absl::Span<uint8_t> span_buffer = buffer_;
  uint8_t product_serial_number[20];
  EXPECT_TRUE(
      translator::ReadValue(span_buffer.subspan(sizeof(scsi::UnitSerialNumber)),
                            product_serial_number));

  char formatted_hex_string[21] = "1234_5678_9abc_defa.";
  for (int i = 0; i < 20; i++) {
    EXPECT_EQ(product_serial_number[i], formatted_hex_string[i]);
  }
}

TEST_F(InquiryTest, TranslateUnitSerialNumberVpdNguid) {
  inquiry_cmd_ = scsi::InquiryCommand{
      .evpd = 1, .page_code = scsi::PageCode::kUnitSerialNumber};

  identify_ns_.eui64 = 0;
  identify_ns_.nguid[0] = 0x123456789abcdefa;
  identify_ns_.nguid[1] = 0x123456789abcdefa;

  translator::StatusCode status =
      translator::InquiryToScsi(scsi_cmd_, buffer_, nvme_cmds_);
  EXPECT_EQ(status, translator::StatusCode::kSuccess);

  scsi::UnitSerialNumber result{};
  EXPECT_TRUE(translator::ReadValue(buffer_, result));

  EXPECT_EQ(result.peripheral_qualifier,
            scsi::PeripheralQualifier::kPeripheralDeviceConnected);
  EXPECT_EQ(result.peripheral_device_type,
            scsi::PeripheralDeviceType::kDirectAccessBlock);
  EXPECT_EQ(result.page_code, scsi::PageCode::kUnitSerialNumber);
  EXPECT_EQ(result.page_length, 40);
  char formatted_hex_string[41] = "1234_5678_9abc_defa_1234_5678_9abc_defa.";
  absl::Span<uint8_t> span_buffer = buffer_;
  uint8_t product_serial_number[40];
  EXPECT_TRUE(
      translator::ReadValue(span_buffer.subspan(sizeof(scsi::UnitSerialNumber)),
                            product_serial_number));

  for (int i = 0; i < 40; ++i) {
    EXPECT_EQ(product_serial_number[i], formatted_hex_string[i]);
  }
}

TEST_F(InquiryTest, TranslateUnitSerialNumberVpdBoth) {
  inquiry_cmd_ = scsi::InquiryCommand{
      .evpd = 1, .page_code = scsi::PageCode::kUnitSerialNumber};

  identify_ns_.eui64 = 0x123456789abcdefa;
  identify_ns_.nguid[0] = 0x123456789abcdefa;
  identify_ns_.nguid[1] = 0x123456789abcdefa;

  translator::StatusCode status =
      translator::InquiryToScsi(scsi_cmd_, buffer_, nvme_cmds_);
  EXPECT_EQ(status, translator::StatusCode::kSuccess);

  scsi::UnitSerialNumber result{};
  EXPECT_TRUE(translator::ReadValue(buffer_, result));

  EXPECT_EQ(result.peripheral_qualifier,
            scsi::PeripheralQualifier::kPeripheralDeviceConnected);
  EXPECT_EQ(result.peripheral_device_type,
            scsi::PeripheralDeviceType::kDirectAccessBlock);
  EXPECT_EQ(result.page_code, scsi::PageCode::kUnitSerialNumber);
  EXPECT_EQ(result.page_length, 40);
  char formatted_hex_string[41] = "1234_5678_9abc_defa_1234_5678_9abc_defa.";

  absl::Span<uint8_t> span_buffer = buffer_;
  uint8_t product_serial_number[40];
  EXPECT_TRUE(
      translator::ReadValue(span_buffer.subspan(sizeof(scsi::UnitSerialNumber)),
                            product_serial_number));

  for (int i = 0; i < 40; ++i) {
    EXPECT_EQ(product_serial_number[i], formatted_hex_string[i]);
  }
}

TEST_F(InquiryTest, TranslateUnitSerialNumberVpdNone) {
  inquiry_cmd_ = scsi::InquiryCommand{
      .evpd = 1, .page_code = scsi::PageCode::kUnitSerialNumber};

  int8_t arr[20] = {'1', '2', '3', '4', '5', 'a', 'b', 'c', 'd', 'e',
                    '1', '2', '3', '4', '5', 'a', 'b', 'c', 'd', 'e'};
  memcpy(identify_ctrl_.sn, arr, 20);
  nvme_cmds_[1].nsid = 0xaaaaaaaa;

  translator::StatusCode status =
      translator::InquiryToScsi(scsi_cmd_, buffer_, nvme_cmds_);
  EXPECT_EQ(status, translator::StatusCode::kSuccess);

  scsi::UnitSerialNumber result{};
  EXPECT_TRUE(translator::ReadValue(buffer_, result));

  EXPECT_EQ(result.peripheral_qualifier,
            scsi::PeripheralQualifier::kPeripheralDeviceConnected);
  EXPECT_EQ(result.peripheral_device_type,
            scsi::PeripheralDeviceType::kDirectAccessBlock);
  EXPECT_EQ(result.page_code, scsi::PageCode::kUnitSerialNumber);
  EXPECT_EQ(result.page_length, 30);

  absl::Span<uint8_t> span_buffer = buffer_;
  uint8_t product_serial_number[30];
  EXPECT_TRUE(
      translator::ReadValue(span_buffer.subspan(sizeof(scsi::UnitSerialNumber)),
                            product_serial_number));

  char formatted_hex_string[31] = "12345abcde12345abcde_aaaaaaaa.";
  for (int i = 0; i < 30; ++i) {
    EXPECT_EQ(product_serial_number[i], formatted_hex_string[i]);
  }
}
}  // namespace