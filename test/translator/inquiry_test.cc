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

#include "lib/translator/inquiry.h"

#include <netinet/in.h>

#include "gtest/gtest.h"

// Tests
namespace {

constexpr uint64_t kSampleIdentifierData = 0x12345678;
constexpr uint8_t kIdentifierLengthNGUID = 0x10;
constexpr uint8_t kIdentifierLengthEUI64 = 0x8;
constexpr uint8_t kPageSize = 4096;

class InquiryTest : public ::testing::Test {
 protected:
  scsi::InquiryCommand inquiry_cmd_;
  nvme::GenericQueueEntryCmd identify_cmds_[2];
  translator::Span<const uint8_t> scsi_cmd_;
  translator::NvmeCmdWrapper nvme_wrappers_[2];
  nvme::IdentifyControllerData identify_ctrl_;
  nvme::IdentifyNamespace identify_ns_;
  uint8_t buffer_[200];

  // Per-test-suite set-up.
  // Called before the first test in this test suite.
  static void SetUpTestSuite() {
    auto alloc_callback = [](uint32_t page_size, uint16_t count) -> uint64_t {
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

    SetCommand();
    nvme_wrappers_[0].cmd = identify_cmds_[0];
    nvme_wrappers_[1].cmd = identify_cmds_[1];
    memset(buffer_, 0, sizeof(buffer_));
  }

  void SetCommand() {
    const uint8_t* cmd_ptr = reinterpret_cast<const uint8_t*>(&inquiry_cmd_);
    scsi_cmd_ = translator::Span(cmd_ptr, sizeof(scsi::InquiryCommand));
  }

  void SetNamespace(nvme::IdentifyNamespace* identify_ns_) {
    identify_cmds_[0].dptr.prp.prp1 = reinterpret_cast<uint64_t>(identify_ns_);
  }

  void SetController(nvme::IdentifyControllerData* identify_ctrl_) {
    identify_cmds_[1].dptr.prp.prp1 =
        reinterpret_cast<uint64_t>(identify_ctrl_);
  }
};

TEST_F(InquiryTest, InquiryToNvme) {
  inquiry_cmd_.allocation_length = htons(4096);
  uint32_t nsid = 0x123;
  uint32_t alloc_len;
  translator::Allocation allocations[2] = {{}};

  translator::StatusCode status =
      translator::InquiryToNvme(scsi_cmd_, nvme_wrappers_[0], nvme_wrappers_[1],
                                kPageSize, nsid, allocations, alloc_len);

  EXPECT_EQ(status, translator::StatusCode::kSuccess);

  EXPECT_EQ(alloc_len, 4096);

  // identify_ns
  EXPECT_EQ(nvme_wrappers_[0].cmd.opc,
            static_cast<uint8_t>(nvme::AdminOpcode::kIdentify));
  EXPECT_EQ(nvme_wrappers_[0].cmd.nsid, nsid);
  EXPECT_NE(nvme_wrappers_[0].cmd.dptr.prp.prp1, 0);
  EXPECT_EQ(nvme_wrappers_[0].cmd.cdw[0], 0);
  EXPECT_EQ(nvme_wrappers_[0].is_admin, true);
  EXPECT_EQ(nvme_wrappers_[0].buffer_len, kPageSize * 1);

  // identify controller
  EXPECT_EQ(nvme_wrappers_[1].cmd.opc,
            static_cast<uint8_t>(nvme::AdminOpcode::kIdentify));
  EXPECT_EQ(nvme_wrappers_[1].cmd.nsid, 0);
  EXPECT_NE(nvme_wrappers_[1].cmd.dptr.prp.prp1, 0);
  EXPECT_EQ(nvme_wrappers_[1].cmd.cdw[0], 1);
  EXPECT_EQ(nvme_wrappers_[1].is_admin, true);
  EXPECT_EQ(nvme_wrappers_[1].buffer_len, kPageSize * 1);
}

TEST_F(InquiryTest, InquiryToNvmeFailRead) {
  inquiry_cmd_.allocation_length = htons(4096);
  uint32_t nsid = 0x123;
  uint32_t alloc_len;
  translator::Allocation allocations[2] = {};

  uint8_t bad_buffer[1] = {};
  translator::StatusCode status = translator::InquiryToNvme(
      bad_buffer, nvme_wrappers_[0], nvme_wrappers_[1], kPageSize, nsid,
      allocations, alloc_len);

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

  translator::StatusCode status = translator::InquiryToScsi(
      scsi_cmd_, buffer_, nvme_wrappers_[0].cmd, nvme_wrappers_[1].cmd);
  EXPECT_EQ(status, translator::StatusCode::kSuccess);

  scsi::InquiryData result{};
  ASSERT_TRUE(translator::ReadValue(buffer_, result));

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

  translator::StatusCode status = translator::InquiryToScsi(
      scsi_cmd_, buffer_, nvme_wrappers_[0].cmd, nvme_wrappers_[1].cmd);

  EXPECT_EQ(status, translator::StatusCode::kSuccess);

  scsi::SupportedVitalProductData result{};
  ASSERT_TRUE(translator::ReadValue(buffer_, result));

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
  translator::Span<uint8_t> span_buffer = buffer_;
  ASSERT_TRUE(translator::ReadValue(
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

  identify_ns_.eui64 = translator::htolll(0x123456789abcdefa);
  identify_ns_.nguid[0] = 0;
  identify_ns_.nguid[1] = 0;

  translator::StatusCode status = translator::InquiryToScsi(
      scsi_cmd_, buffer_, nvme_wrappers_[0].cmd, nvme_wrappers_[1].cmd);
  EXPECT_EQ(status, translator::StatusCode::kSuccess);

  scsi::UnitSerialNumber result{};
  ASSERT_TRUE(translator::ReadValue(buffer_, result));

  EXPECT_EQ(result.peripheral_qualifier,
            scsi::PeripheralQualifier::kPeripheralDeviceConnected);
  EXPECT_EQ(result.peripheral_device_type,
            scsi::PeripheralDeviceType::kDirectAccessBlock);
  EXPECT_EQ(result.page_code, scsi::PageCode::kUnitSerialNumber);
  EXPECT_EQ(result.page_length, 20);

  translator::Span<uint8_t> span_buffer = buffer_;
  uint8_t product_serial_number[20];
  ASSERT_TRUE(
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

  translator::StatusCode status = translator::InquiryToScsi(
      scsi_cmd_, buffer_, nvme_wrappers_[0].cmd, nvme_wrappers_[1].cmd);
  EXPECT_EQ(status, translator::StatusCode::kSuccess);

  scsi::UnitSerialNumber result{};
  ASSERT_TRUE(translator::ReadValue(buffer_, result));

  EXPECT_EQ(result.peripheral_qualifier,
            scsi::PeripheralQualifier::kPeripheralDeviceConnected);
  EXPECT_EQ(result.peripheral_device_type,
            scsi::PeripheralDeviceType::kDirectAccessBlock);
  EXPECT_EQ(result.page_code, scsi::PageCode::kUnitSerialNumber);
  EXPECT_EQ(result.page_length, 40);
  char formatted_hex_string[41] = "1234_5678_9abc_defa_1234_5678_9abc_defa.";
  translator::Span<uint8_t> span_buffer = buffer_;
  uint8_t product_serial_number[40];
  ASSERT_TRUE(
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

  translator::StatusCode status = translator::InquiryToScsi(
      scsi_cmd_, buffer_, nvme_wrappers_[0].cmd, nvme_wrappers_[1].cmd);
  EXPECT_EQ(status, translator::StatusCode::kSuccess);

  scsi::UnitSerialNumber result{};
  ASSERT_TRUE(translator::ReadValue(buffer_, result));

  EXPECT_EQ(result.peripheral_qualifier,
            scsi::PeripheralQualifier::kPeripheralDeviceConnected);
  EXPECT_EQ(result.peripheral_device_type,
            scsi::PeripheralDeviceType::kDirectAccessBlock);
  EXPECT_EQ(result.page_code, scsi::PageCode::kUnitSerialNumber);
  EXPECT_EQ(result.page_length, 40);
  char formatted_hex_string[41] = "1234_5678_9abc_defa_1234_5678_9abc_defa.";

  translator::Span<uint8_t> span_buffer = buffer_;
  uint8_t product_serial_number[40];
  ASSERT_TRUE(
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
  nvme_wrappers_[0].cmd.nsid = 0xaaaaaaaa;

  translator::StatusCode status = translator::InquiryToScsi(
      scsi_cmd_, buffer_, nvme_wrappers_[0].cmd, nvme_wrappers_[1].cmd);
  EXPECT_EQ(status, translator::StatusCode::kSuccess);

  scsi::UnitSerialNumber result{};
  ASSERT_TRUE(translator::ReadValue(buffer_, result));

  EXPECT_EQ(result.peripheral_qualifier,
            scsi::PeripheralQualifier::kPeripheralDeviceConnected);
  EXPECT_EQ(result.peripheral_device_type,
            scsi::PeripheralDeviceType::kDirectAccessBlock);
  EXPECT_EQ(result.page_code, scsi::PageCode::kUnitSerialNumber);
  EXPECT_EQ(result.page_length, 30);

  translator::Span<uint8_t> span_buffer = buffer_;
  uint8_t product_serial_number[30];
  ASSERT_TRUE(
      translator::ReadValue(span_buffer.subspan(sizeof(scsi::UnitSerialNumber)),
                            product_serial_number));

  char formatted_hex_string[31] = "12345abcde12345abcde_aaaaaaaa.";
  for (int i = 0; i < 30; ++i) {
    EXPECT_EQ(product_serial_number[i], formatted_hex_string[i]);
  }
}

TEST_F(InquiryTest, BlockLimitsVpd) {
  inquiry_cmd_ = scsi::InquiryCommand{
      .evpd = 1, .page_code = scsi::PageCode::kBlockLimitsVpd};

  identify_ctrl_.mdts = 0;
  identify_ctrl_.fuses.compare_and_write = 0;
  identify_ctrl_.oncs.dsm = 0;

  translator::StatusCode status = translator::InquiryToScsi(
      scsi_cmd_, buffer_, nvme_wrappers_[0].cmd, nvme_wrappers_[1].cmd);
  EXPECT_EQ(status, translator::StatusCode::kSuccess);

  scsi::BlockLimitsVpd result{};
  ASSERT_TRUE(translator::ReadValue(buffer_, result));

  uint32_t max_transfer_length =
      identify_ctrl_.mdts ? 1 << identify_ctrl_.mdts : 0;

  EXPECT_EQ(result.page_code, scsi::PageCode::kBlockLimitsVpd);
  EXPECT_EQ(result.page_length, 0x003c);
  EXPECT_EQ(result.max_compare_write_length,
            identify_ctrl_.fuses.compare_and_write ? max_transfer_length : 0);
  EXPECT_EQ(result.max_transfer_length, max_transfer_length);
  EXPECT_EQ(result.max_unmap_lba_count, identify_ctrl_.oncs.dsm);
  EXPECT_EQ(result.max_unmap_block_descriptor_count,
            identify_ctrl_.oncs.dsm ? 0x0100 : 0);
}

TEST_F(InquiryTest, BlockLimitsVpdMdts) {
  inquiry_cmd_ = scsi::InquiryCommand{
      .evpd = 1, .page_code = scsi::PageCode::kBlockLimitsVpd};

  identify_ctrl_.mdts = 5;
  identify_ctrl_.fuses.compare_and_write = 0;
  identify_ctrl_.oncs.dsm = 0;

  translator::StatusCode status = translator::InquiryToScsi(
      scsi_cmd_, buffer_, nvme_wrappers_[0].cmd, nvme_wrappers_[1].cmd);
  EXPECT_EQ(status, translator::StatusCode::kSuccess);

  scsi::BlockLimitsVpd result{};
  ASSERT_TRUE(translator::ReadValue(buffer_, result));

  uint32_t max_transfer_length =
      identify_ctrl_.mdts ? 1 << identify_ctrl_.mdts : 0;

  EXPECT_EQ(result.page_code, scsi::PageCode::kBlockLimitsVpd);
  EXPECT_EQ(result.page_length, 0x003c);
  EXPECT_EQ(result.max_compare_write_length, 0);
  EXPECT_EQ(result.max_transfer_length, htonl(max_transfer_length));
  EXPECT_EQ(result.max_unmap_lba_count, 0);
  EXPECT_EQ(result.max_unmap_block_descriptor_count, 0);
}

TEST_F(InquiryTest, BlockLimitsVpdFuse) {
  inquiry_cmd_ = scsi::InquiryCommand{
      .evpd = 1, .page_code = scsi::PageCode::kBlockLimitsVpd};

  identify_ctrl_.mdts = 0;
  identify_ctrl_.fuses.compare_and_write = 1;
  identify_ctrl_.oncs.dsm = 0;

  translator::StatusCode status = translator::InquiryToScsi(
      scsi_cmd_, buffer_, nvme_wrappers_[0].cmd, nvme_wrappers_[1].cmd);
  EXPECT_EQ(status, translator::StatusCode::kSuccess);

  scsi::BlockLimitsVpd result{};
  ASSERT_TRUE(translator::ReadValue(buffer_, result));

  uint32_t max_transfer_length =
      identify_ctrl_.mdts ? 1 << identify_ctrl_.mdts : 0;

  EXPECT_EQ(result.page_code, scsi::PageCode::kBlockLimitsVpd);
  EXPECT_EQ(result.page_length, 0x003c);
  EXPECT_EQ(result.max_compare_write_length,
            identify_ctrl_.fuses.compare_and_write ? max_transfer_length : 0);
  EXPECT_EQ(result.max_transfer_length, max_transfer_length);
  EXPECT_EQ(result.max_unmap_lba_count, identify_ctrl_.oncs.dsm);
  EXPECT_EQ(result.max_unmap_block_descriptor_count,
            identify_ctrl_.oncs.dsm ? 0x0100 : 0);
}

TEST_F(InquiryTest, BlockLimitsVpdDsm) {
  inquiry_cmd_ = scsi::InquiryCommand{
      .evpd = 1, .page_code = scsi::PageCode::kBlockLimitsVpd};

  identify_ctrl_.mdts = 0;
  identify_ctrl_.fuses.compare_and_write = 0;
  identify_ctrl_.oncs.dsm = 1;

  translator::StatusCode status = translator::InquiryToScsi(
      scsi_cmd_, buffer_, nvme_wrappers_[0].cmd, nvme_wrappers_[1].cmd);
  EXPECT_EQ(status, translator::StatusCode::kSuccess);

  scsi::BlockLimitsVpd result{};
  ASSERT_TRUE(translator::ReadValue(buffer_, result));

  uint32_t max_transfer_length =
      identify_ctrl_.mdts ? 1 << identify_ctrl_.mdts : 0;

  EXPECT_EQ(result.page_code, scsi::PageCode::kBlockLimitsVpd);
  EXPECT_EQ(result.page_length, 0x003c);
  EXPECT_EQ(result.max_compare_write_length, 0);
  EXPECT_EQ(result.max_transfer_length, htonl(max_transfer_length));
  EXPECT_EQ(result.max_unmap_lba_count,
            htonl(static_cast<uint32_t>(identify_ctrl_.oncs.dsm)));
  EXPECT_EQ(result.max_unmap_block_descriptor_count, htonl(0x0100));
}

TEST_F(InquiryTest, BlockLimitsVpdMdtsFuse) {
  inquiry_cmd_ = scsi::InquiryCommand{
      .evpd = 1, .page_code = scsi::PageCode::kBlockLimitsVpd};

  identify_ctrl_.mdts = 5;
  identify_ctrl_.fuses.compare_and_write = 1;
  identify_ctrl_.oncs.dsm = 0;

  translator::StatusCode status = translator::InquiryToScsi(
      scsi_cmd_, buffer_, nvme_wrappers_[0].cmd, nvme_wrappers_[1].cmd);
  EXPECT_EQ(status, translator::StatusCode::kSuccess);

  scsi::BlockLimitsVpd result{};
  ASSERT_TRUE(translator::ReadValue(buffer_, result));

  uint32_t max_transfer_length =
      identify_ctrl_.mdts ? 1 << identify_ctrl_.mdts : 0;

  EXPECT_EQ(result.page_code, scsi::PageCode::kBlockLimitsVpd);
  EXPECT_EQ(result.page_length, 0x003c);
  EXPECT_EQ(result.max_compare_write_length, max_transfer_length);
  EXPECT_EQ(result.max_transfer_length, htonl(max_transfer_length));
  EXPECT_EQ(result.max_unmap_lba_count, 0);
  EXPECT_EQ(result.max_unmap_block_descriptor_count, 0);
}

TEST_F(InquiryTest, BlockLimitsVpdMdtsFuseLarge) {
  inquiry_cmd_ = scsi::InquiryCommand{
      .evpd = 1, .page_code = scsi::PageCode::kBlockLimitsVpd};

  identify_ctrl_.mdts = 10;
  identify_ctrl_.fuses.compare_and_write = 1;
  identify_ctrl_.oncs.dsm = 0;

  translator::StatusCode status = translator::InquiryToScsi(
      scsi_cmd_, buffer_, nvme_wrappers_[0].cmd, nvme_wrappers_[1].cmd);
  EXPECT_EQ(status, translator::StatusCode::kSuccess);

  scsi::BlockLimitsVpd result{};
  ASSERT_TRUE(translator::ReadValue(buffer_, result));

  uint32_t max_transfer_length =
      identify_ctrl_.mdts ? 1 << identify_ctrl_.mdts : 0;

  // TODO: put in common?
  const uint8_t kMaxCompareWriteLen = 255;

  EXPECT_EQ(result.page_code, scsi::PageCode::kBlockLimitsVpd);
  EXPECT_EQ(result.page_length, 0x003c);
  EXPECT_EQ(result.max_compare_write_length, kMaxCompareWriteLen);
  EXPECT_EQ(result.max_transfer_length, htonl(max_transfer_length));
  EXPECT_EQ(result.max_unmap_lba_count, 0);
  EXPECT_EQ(result.max_unmap_block_descriptor_count, 0);
}

TEST_F(InquiryTest, BlockLimitsVpdMdtsFuseVeryLarge) {
  inquiry_cmd_ = scsi::InquiryCommand{
      .evpd = 1, .page_code = scsi::PageCode::kBlockLimitsVpd};

  identify_ctrl_.mdts = 65;
  identify_ctrl_.fuses.compare_and_write = 1;
  identify_ctrl_.oncs.dsm = 0;

  translator::StatusCode status = translator::InquiryToScsi(
      scsi_cmd_, buffer_, nvme_wrappers_[0].cmd, nvme_wrappers_[1].cmd);
  EXPECT_EQ(status, translator::StatusCode::kSuccess);

  scsi::BlockLimitsVpd result{};
  ASSERT_TRUE(translator::ReadValue(buffer_, result));

  // TODO: put in common?
  const uint8_t kMaxCompareWriteLen = 255;

  EXPECT_EQ(result.page_code, scsi::PageCode::kBlockLimitsVpd);
  EXPECT_EQ(result.page_length, 0x003c);
  EXPECT_EQ(result.max_compare_write_length, kMaxCompareWriteLen);

  uint32_t largest_transfer = 1 << 16;
  EXPECT_EQ(result.max_transfer_length, htonl(largest_transfer));
  EXPECT_EQ(result.max_unmap_lba_count, 0);
  EXPECT_EQ(result.max_unmap_block_descriptor_count, 0);
}

TEST_F(InquiryTest, BlockLimitsVpdMdtsOncs) {
  inquiry_cmd_ = scsi::InquiryCommand{
      .evpd = 1, .page_code = scsi::PageCode::kBlockLimitsVpd};

  identify_ctrl_.mdts = 5;
  identify_ctrl_.fuses.compare_and_write = 0;
  identify_ctrl_.oncs.dsm = 1;

  translator::StatusCode status = translator::InquiryToScsi(
      scsi_cmd_, buffer_, nvme_wrappers_[0].cmd, nvme_wrappers_[1].cmd);
  EXPECT_EQ(status, translator::StatusCode::kSuccess);

  scsi::BlockLimitsVpd result{};
  ASSERT_TRUE(translator::ReadValue(buffer_, result));

  uint32_t max_transfer_length =
      identify_ctrl_.mdts ? 1 << identify_ctrl_.mdts : 0;

  EXPECT_EQ(result.page_code, scsi::PageCode::kBlockLimitsVpd);
  EXPECT_EQ(result.page_length, 0x003c);
  EXPECT_EQ(result.max_compare_write_length, 0);
  EXPECT_EQ(result.max_transfer_length, htonl(max_transfer_length));
  EXPECT_EQ(result.max_unmap_lba_count,
            htonl(static_cast<uint32_t>(identify_ctrl_.oncs.dsm)));
  EXPECT_EQ(result.max_unmap_block_descriptor_count, htonl(0x0100));
}

TEST_F(InquiryTest, BlockLimitsVpdFuseOncs) {
  inquiry_cmd_ = scsi::InquiryCommand{
      .evpd = 1, .page_code = scsi::PageCode::kBlockLimitsVpd};

  identify_ctrl_.mdts = 0;
  identify_ctrl_.fuses.compare_and_write = 1;
  identify_ctrl_.oncs.dsm = 1;

  translator::StatusCode status = translator::InquiryToScsi(
      scsi_cmd_, buffer_, nvme_wrappers_[0].cmd, nvme_wrappers_[1].cmd);
  EXPECT_EQ(status, translator::StatusCode::kSuccess);

  scsi::BlockLimitsVpd result{};
  ASSERT_TRUE(translator::ReadValue(buffer_, result));

  uint32_t max_transfer_length =
      identify_ctrl_.mdts ? 1 << identify_ctrl_.mdts : 0;

  EXPECT_EQ(result.page_code, scsi::PageCode::kBlockLimitsVpd);
  EXPECT_EQ(result.page_length, 0x003c);
  EXPECT_EQ(result.max_compare_write_length, max_transfer_length);
  EXPECT_EQ(result.max_transfer_length, htonl(max_transfer_length));
  EXPECT_EQ(result.max_unmap_lba_count, htonl(identify_ctrl_.oncs.dsm));
  EXPECT_EQ(result.max_unmap_block_descriptor_count, htonl(0x0100));
}

TEST_F(InquiryTest, BlockLimitsVpdMdtsFuseOncs) {
  inquiry_cmd_ = scsi::InquiryCommand{
      .evpd = 1, .page_code = scsi::PageCode::kBlockLimitsVpd};

  identify_ctrl_.mdts = 5;
  identify_ctrl_.fuses.compare_and_write = 1;
  identify_ctrl_.oncs.dsm = 1;

  translator::StatusCode status = translator::InquiryToScsi(
      scsi_cmd_, buffer_, nvme_wrappers_[0].cmd, nvme_wrappers_[1].cmd);
  EXPECT_EQ(status, translator::StatusCode::kSuccess);

  scsi::BlockLimitsVpd result{};
  ASSERT_TRUE(translator::ReadValue(buffer_, result));

  uint32_t max_transfer_length =
      identify_ctrl_.mdts ? 1 << identify_ctrl_.mdts : 0;

  EXPECT_EQ(result.page_code, scsi::PageCode::kBlockLimitsVpd);
  EXPECT_EQ(result.page_length, 0x003c);
  EXPECT_EQ(result.max_compare_write_length, max_transfer_length);
  EXPECT_EQ(result.max_transfer_length, htonl(max_transfer_length));
  EXPECT_EQ(result.max_unmap_lba_count, htonl(identify_ctrl_.oncs.dsm));
  EXPECT_EQ(result.max_unmap_block_descriptor_count, htonl(0x0100));
}

TEST_F(InquiryTest, LogicalBlockProvisioningVpd) {
  inquiry_cmd_.evpd = 1;
  inquiry_cmd_.page_code = scsi::PageCode::kLogicalBlockProvisioningVpd;

  translator::StatusCode status = translator::InquiryToScsi(
      scsi_cmd_, buffer_, nvme_wrappers_[0].cmd, nvme_wrappers_[1].cmd);
  EXPECT_EQ(status, translator::StatusCode::kSuccess);

  scsi::LogicalBlockProvisioningVpd result{};
  ASSERT_TRUE(translator::ReadValue(buffer_, result));

  bool ad = identify_ctrl_.oncs.dsm;

  EXPECT_EQ(result.page_code, scsi::PageCode::kLogicalBlockProvisioningVpd);
  EXPECT_EQ(result.lbprz, ad);
  EXPECT_EQ(result.anc_sup, 0);
  EXPECT_EQ(result.provisioning_type, 0);
  EXPECT_EQ(result.lbpu, 0);
}

TEST_F(InquiryTest, LogicalBlockProvisioningVpdDsm) {
  inquiry_cmd_.evpd = 1;
  inquiry_cmd_.page_code = scsi::PageCode::kLogicalBlockProvisioningVpd;

  identify_ctrl_.oncs.dsm = 1;
  translator::StatusCode status = translator::InquiryToScsi(
      scsi_cmd_, buffer_, nvme_wrappers_[0].cmd, nvme_wrappers_[1].cmd);
  EXPECT_EQ(status, translator::StatusCode::kSuccess);

  scsi::LogicalBlockProvisioningVpd result{};
  ASSERT_TRUE(translator::ReadValue(buffer_, result));

  bool ad = identify_ctrl_.oncs.dsm;

  EXPECT_EQ(result.page_code, scsi::PageCode::kLogicalBlockProvisioningVpd);
  EXPECT_EQ(result.lbprz, ad);
  EXPECT_EQ(result.anc_sup, 0);
  EXPECT_EQ(result.provisioning_type, 1);
  EXPECT_EQ(result.lbpu, 1);
}

TEST_F(InquiryTest, LogicalBlockProvisioningVpdThinprov) {
  inquiry_cmd_.evpd = 1;
  inquiry_cmd_.page_code = scsi::PageCode::kLogicalBlockProvisioningVpd;

  identify_ns_.nsfeat.thin_prov = 1;

  translator::StatusCode status = translator::InquiryToScsi(
      scsi_cmd_, buffer_, nvme_wrappers_[0].cmd, nvme_wrappers_[1].cmd);
  EXPECT_EQ(status, translator::StatusCode::kSuccess);

  scsi::LogicalBlockProvisioningVpd result{};
  ASSERT_TRUE(translator::ReadValue(buffer_, result));

  bool ad = identify_ctrl_.oncs.dsm;

  EXPECT_EQ(result.page_code, scsi::PageCode::kLogicalBlockProvisioningVpd);
  EXPECT_EQ(result.lbprz, ad);
  EXPECT_EQ(result.anc_sup, 0);
  EXPECT_EQ(result.provisioning_type, 0);
  EXPECT_EQ(result.lbpu, 0);
}

TEST_F(InquiryTest, LogicalBlockProvisioningVpdAdThinprov) {
  inquiry_cmd_.evpd = 1;
  inquiry_cmd_.page_code = scsi::PageCode::kLogicalBlockProvisioningVpd;

  identify_ctrl_.oncs.dsm = 1;
  identify_ns_.nsfeat.thin_prov = 1;

  translator::StatusCode status = translator::InquiryToScsi(
      scsi_cmd_, buffer_, nvme_wrappers_[0].cmd, nvme_wrappers_[1].cmd);
  EXPECT_EQ(status, translator::StatusCode::kSuccess);

  scsi::LogicalBlockProvisioningVpd result{};
  ASSERT_TRUE(translator::ReadValue(buffer_, result));

  bool ad = identify_ctrl_.oncs.dsm;

  EXPECT_EQ(result.page_code, scsi::PageCode::kLogicalBlockProvisioningVpd);
  EXPECT_EQ(result.lbprz, ad);
  EXPECT_EQ(result.anc_sup, 0);
  EXPECT_EQ(result.provisioning_type, 2);
  EXPECT_EQ(result.lbpu, 1);
}

TEST_F(InquiryTest, TranslateDeviceIdentificationVPDBuildCorrectStructs) {
  inquiry_cmd_ = scsi::InquiryCommand{
      .evpd = 1, .page_code = scsi::PageCode::kUnitSerialNumber};

  identify_ns_.eui64 = 0;
  identify_ns_.nguid[0] = kSampleIdentifierData;
  identify_ns_.nguid[1] = kSampleIdentifierData;
  inquiry_cmd_.evpd = 1;
  inquiry_cmd_.page_code = scsi::PageCode::kDeviceIdentification;

  translator::StatusCode status = translator::InquiryToScsi(
      scsi_cmd_, buffer_, nvme_wrappers_[0].cmd, nvme_wrappers_[1].cmd);
  EXPECT_EQ(status, translator::StatusCode::kSuccess);

  scsi::DeviceIdentificationVpd device_identification_vpd{};
  ASSERT_TRUE(translator::ReadValue(buffer_, device_identification_vpd));

  EXPECT_EQ(device_identification_vpd.peripheral_qualifier,
            scsi::PeripheralQualifier::kPeripheralDeviceConnected);

  EXPECT_EQ(device_identification_vpd.peripheral_device_type,
            scsi::PeripheralDeviceType::kDirectAccessBlock);

  EXPECT_EQ(device_identification_vpd.page_code,
            scsi::PageCode::kDeviceIdentification);
}

TEST_F(InquiryTest, DeviceIdentificationBuildsCorrectIdentificationDescriptor) {
  inquiry_cmd_ = scsi::InquiryCommand{
      .evpd = 1, .page_code = scsi::PageCode::kUnitSerialNumber};

  identify_ns_.eui64 = 0;
  identify_ns_.nguid[0] = kSampleIdentifierData;
  identify_ns_.nguid[1] = kSampleIdentifierData;
  inquiry_cmd_.evpd = 1;
  inquiry_cmd_.page_code = scsi::PageCode::kDeviceIdentification;

  translator::StatusCode status = translator::InquiryToScsi(
      scsi_cmd_, buffer_, nvme_wrappers_[0].cmd, nvme_wrappers_[1].cmd);
  EXPECT_EQ(status, translator::StatusCode::kSuccess);

  scsi::IdentificationDescriptor identification_descriptor{};
  translator::Span<uint8_t> span_buf(
      &buffer_[sizeof(scsi::DeviceIdentificationVpd)],
      sizeof(scsi::IdentificationDescriptor));
  ASSERT_TRUE(translator::ReadValue(span_buf, identification_descriptor));

  EXPECT_EQ(identification_descriptor.protocol_identifier,
            scsi::ProtocolIdentifier::kFibreChannel);
  EXPECT_EQ(identification_descriptor.code_set, scsi::CodeSet::kBinary);
  EXPECT_EQ(identification_descriptor.protocol_identifier_valid, false);
  EXPECT_EQ(identification_descriptor.association,
            scsi::Association::kPhysicalDevice);
  EXPECT_EQ(identification_descriptor.identifier_type,
            scsi::IdentifierType::kEUI64);
}

TEST_F(InquiryTest, DeviceIdentificationVpdUsesNGUID) {
  inquiry_cmd_ = scsi::InquiryCommand{
      .evpd = 1, .page_code = scsi::PageCode::kUnitSerialNumber};

  identify_ns_.eui64 = 0;
  identify_ns_.nguid[0] = kSampleIdentifierData;
  identify_ns_.nguid[1] = kSampleIdentifierData;
  inquiry_cmd_.evpd = 1;
  inquiry_cmd_.page_code = scsi::PageCode::kDeviceIdentification;

  translator::StatusCode status = translator::InquiryToScsi(
      scsi_cmd_, buffer_, nvme_wrappers_[0].cmd, nvme_wrappers_[1].cmd);
  EXPECT_EQ(status, translator::StatusCode::kSuccess);

  scsi::IdentificationDescriptor identification_descriptor{};
  translator::Span<uint8_t> span_buf(
      &buffer_[sizeof(scsi::DeviceIdentificationVpd)],
      sizeof(scsi::IdentificationDescriptor));
  ASSERT_TRUE(translator::ReadValue(span_buf, identification_descriptor));

  EXPECT_EQ(identification_descriptor.identifier_length,
            kIdentifierLengthNGUID);

  // test whether each field of the nguid array has correct data. Each field is
  // 64 bit wide.
  for (int i = 0; i < sizeof(identify_ns_.nguid) / sizeof(uint64_t); i++) {
    uint64_t nguid_data = 0;
    translator::Span<uint8_t> span_buf_data(
        &buffer_[sizeof(scsi::DeviceIdentificationVpd) +
                 sizeof(scsi::IdentificationDescriptor) + i * sizeof(uint64_t)],
        sizeof(uint64_t));
    ASSERT_TRUE(translator::ReadValue(span_buf_data, nguid_data));
    EXPECT_EQ(nguid_data, kSampleIdentifierData);
  }
}

TEST_F(InquiryTest, IdentificationDescriptorUsesEUI64) {
  identify_ns_.eui64 = 0x12345678;
  identify_ns_.nguid[0] = 0;
  identify_ns_.nguid[1] = 0;
  inquiry_cmd_.evpd = 1;
  inquiry_cmd_.page_code = scsi::PageCode::kDeviceIdentification;

  translator::StatusCode status = translator::InquiryToScsi(
      scsi_cmd_, buffer_, nvme_wrappers_[0].cmd, nvme_wrappers_[1].cmd);
  EXPECT_EQ(status, translator::StatusCode::kSuccess);

  scsi::IdentificationDescriptor identification_descriptor{};
  translator::Span<uint8_t> span_buf(
      &buffer_[sizeof(scsi::DeviceIdentificationVpd)],
      sizeof(scsi::IdentificationDescriptor));
  ASSERT_TRUE(translator::ReadValue(span_buf, identification_descriptor));

  uint64_t eui64_data = 0;
  translator::Span<uint8_t> span_buf_data(
      &buffer_[sizeof(scsi::DeviceIdentificationVpd) +
               sizeof(scsi::IdentificationDescriptor)],
      8);
  ASSERT_TRUE(translator::ReadValue(span_buf_data, eui64_data));
  EXPECT_EQ(eui64_data, kSampleIdentifierData);
  EXPECT_EQ(identification_descriptor.identifier_length,
            kIdentifierLengthEUI64);
}

TEST_F(InquiryTest, FailsOnControllerNullPointer) {
  nvme_wrappers_[0].cmd.dptr.prp.prp1 = 0;

  translator::StatusCode status = translator::InquiryToScsi(
      scsi_cmd_, buffer_, nvme_wrappers_[0].cmd, nvme_wrappers_[1].cmd);
  ASSERT_EQ(status, translator::StatusCode::kFailure);
}

TEST_F(InquiryTest, FailsOnNamespaceNullPointer) {
  nvme_wrappers_[1].cmd.dptr.prp.prp1 = 0;

  translator::StatusCode status = translator::InquiryToScsi(
      scsi_cmd_, buffer_, nvme_wrappers_[0].cmd, nvme_wrappers_[1].cmd);
  ASSERT_EQ(status, translator::StatusCode::kFailure);
}

TEST_F(InquiryTest, ExtndedInquiryDataVpdShouldReturnCorrectSPTValue) {
  inquiry_cmd_.evpd = 1;
  inquiry_cmd_.page_code = scsi::PageCode::kExtended;

  identify_ns_.dpc.pit1 = identify_ns_.dpc.pit2 = 0;
  identify_ns_.dpc.pit3 = 1;

  // dpc = 0b001
  translator::StatusCode status = translator::InquiryToScsi(
      scsi_cmd_, buffer_, nvme_wrappers_[0].cmd, nvme_wrappers_[1].cmd);

  scsi::ExtendedInquiryDataVpd extended_inquiry_data{};
  ASSERT_TRUE(translator::ReadValue(buffer_, extended_inquiry_data));

  EXPECT_EQ(extended_inquiry_data.spt, 0b000);

  // dpc = 0b010
  identify_ns_.dpc.pit2 = 1;
  identify_ns_.dpc.pit3 = 0;
  status = translator::InquiryToScsi(scsi_cmd_, buffer_, nvme_wrappers_[0].cmd,
                                     nvme_wrappers_[1].cmd);
  ASSERT_TRUE(translator::ReadValue(buffer_, extended_inquiry_data));

  EXPECT_EQ(extended_inquiry_data.spt, 0b010);

  // dpc = 0b011
  identify_ns_.dpc.pit3 = 1;

  status = translator::InquiryToScsi(scsi_cmd_, buffer_, nvme_wrappers_[0].cmd,
                                     nvme_wrappers_[1].cmd);

  ASSERT_TRUE(translator::ReadValue(buffer_, extended_inquiry_data));
  EXPECT_EQ(extended_inquiry_data.spt, 0b001);

  // dpc = 0b100
  identify_ns_.dpc.pit1 = 1;
  identify_ns_.dpc.pit2 = 0;
  identify_ns_.dpc.pit3 = 0;
  status = translator::InquiryToScsi(scsi_cmd_, buffer_, nvme_wrappers_[0].cmd,
                                     nvme_wrappers_[1].cmd);

  ASSERT_TRUE(translator::ReadValue(buffer_, extended_inquiry_data));
  EXPECT_EQ(extended_inquiry_data.spt, 0b100);

  // dpc = 0b101
  identify_ns_.dpc.pit3 = 1;
  status = translator::InquiryToScsi(scsi_cmd_, buffer_, nvme_wrappers_[0].cmd,
                                     nvme_wrappers_[1].cmd);

  ASSERT_TRUE(translator::ReadValue(buffer_, extended_inquiry_data));
  EXPECT_EQ(extended_inquiry_data.spt, 0b011);

  // dpc = 0b110
  identify_ns_.dpc.pit2 = 1;
  identify_ns_.dpc.pit3 = 0;
  status = translator::InquiryToScsi(scsi_cmd_, buffer_, nvme_wrappers_[0].cmd,
                                     nvme_wrappers_[1].cmd);

  ASSERT_TRUE(translator::ReadValue(buffer_, extended_inquiry_data));
  EXPECT_EQ(extended_inquiry_data.spt, 0b101);

  // dpc = 0b111
  identify_ns_.dpc.pit3 = 1;
  status = translator::InquiryToScsi(scsi_cmd_, buffer_, nvme_wrappers_[0].cmd,
                                     nvme_wrappers_[1].cmd);

  ASSERT_TRUE(translator::ReadValue(buffer_, extended_inquiry_data));
  EXPECT_EQ(extended_inquiry_data.spt, 0b111);
}

TEST_F(InquiryTest, ExtndedInquiryDataVpdShouldUseTheCorrectDPSValue) {
  inquiry_cmd_.evpd = 1;
  inquiry_cmd_.page_code = scsi::PageCode::kExtended;
  identify_ns_.dps.reserved4 = identify_ns_.dps.md_start =
      identify_ns_.dps.pit = 0;

  // dpc = 0b100
  identify_ns_.dpc.pit1 = 1;
  identify_ns_.dpc.pit2 = 0;
  identify_ns_.dpc.pit3 = 0;

  translator::StatusCode status = translator::InquiryToScsi(
      scsi_cmd_, buffer_, nvme_wrappers_[0].cmd, nvme_wrappers_[1].cmd);

  EXPECT_EQ(status, translator::StatusCode::kSuccess);
  scsi::ExtendedInquiryDataVpd extended_inquiry_data{};
  ASSERT_TRUE(translator::ReadValue(buffer_, extended_inquiry_data));

  EXPECT_EQ(extended_inquiry_data.grd_chk, 0b0);
  EXPECT_EQ(extended_inquiry_data.app_chk, 0b0);
  EXPECT_EQ(extended_inquiry_data.ref_chk, 0b0);

  identify_ns_.dps.reserved4 = 1;
  identify_ns_.dps.md_start = 1;
  identify_ns_.dps.pit = 1;

  status = translator::InquiryToScsi(scsi_cmd_, buffer_, nvme_wrappers_[0].cmd,
                                     nvme_wrappers_[1].cmd);
  EXPECT_EQ(status, translator::StatusCode::kSuccess);
  ASSERT_TRUE(translator::ReadValue(buffer_, extended_inquiry_data));

  EXPECT_EQ(extended_inquiry_data.grd_chk, 0b1);
  EXPECT_EQ(extended_inquiry_data.app_chk, 0b1);
  EXPECT_EQ(extended_inquiry_data.ref_chk, 0b1);
}

TEST_F(InquiryTest,
       ExtendedInquiryDataVpdShouldUseTheIdentifyControllerDataVWCBit) {
  inquiry_cmd_.evpd = 1;
  inquiry_cmd_.page_code = scsi::PageCode::kExtended;

  identify_ctrl_.vwc.present = 0;

  // dpc = 0b100
  identify_ns_.dpc.pit1 = 1;
  identify_ns_.dpc.pit2 = 0;
  identify_ns_.dpc.pit3 = 0;

  translator::StatusCode status = translator::InquiryToScsi(
      scsi_cmd_, buffer_, nvme_wrappers_[0].cmd, nvme_wrappers_[1].cmd);
  EXPECT_EQ(status, translator::StatusCode::kSuccess);
  scsi::ExtendedInquiryDataVpd extended_inquiry_data{};
  ASSERT_TRUE(translator::ReadValue(buffer_, extended_inquiry_data));

  EXPECT_EQ(extended_inquiry_data.v_sup, 0);

  identify_ctrl_.vwc.present = 1;

  status = translator::InquiryToScsi(scsi_cmd_, buffer_, nvme_wrappers_[0].cmd,
                                     nvme_wrappers_[1].cmd);
  EXPECT_EQ(status, translator::StatusCode::kSuccess);
  ASSERT_TRUE(translator::ReadValue(buffer_, extended_inquiry_data));

  EXPECT_EQ(extended_inquiry_data.v_sup, 1);
}

TEST_F(InquiryTest, ExtendedInquiryDataVpdShouldReturnCorrectDataPage) {
  inquiry_cmd_.evpd = 1;
  inquiry_cmd_.page_code = scsi::PageCode::kExtended;

  // adding mock values for testing purposes
  identify_ns_.dps.md_start = identify_ns_.dps.pit =
      identify_ns_.dps.reserved4 = 0;

  identify_ns_.dpc.pit1 = identify_ns_.dpc.pit2 = 0;
  identify_ns_.dpc.pit3 = 1;

  identify_ctrl_.vwc.present = 0;

  translator::StatusCode status = translator::InquiryToScsi(
      scsi_cmd_, buffer_, nvme_wrappers_[0].cmd, nvme_wrappers_[1].cmd);
  EXPECT_EQ(status, translator::StatusCode::kSuccess);
  scsi::ExtendedInquiryDataVpd extended_inquiry_data{};
  ASSERT_TRUE(translator::ReadValue(buffer_, extended_inquiry_data));

  EXPECT_EQ(extended_inquiry_data.peripheral_qualifer,
            scsi::PeripheralQualifier::kPeripheralDeviceConnected);

  EXPECT_EQ(extended_inquiry_data.peripheral_device_type,
            scsi::PeripheralDeviceType::kDirectAccessBlock);

  EXPECT_EQ(extended_inquiry_data.page_code, scsi::PageCode::kExtended);
  EXPECT_EQ(extended_inquiry_data.page_length,
            scsi::PageLength::kExtendedInquiryCommand);
  EXPECT_EQ(extended_inquiry_data.activate_microcode,
            scsi::ActivateMicrocode::kActivateAfterHardReset);
  EXPECT_EQ(extended_inquiry_data.spt, 0);
  EXPECT_EQ(extended_inquiry_data.grd_chk, 0);
  EXPECT_EQ(extended_inquiry_data.app_chk, 0);
  EXPECT_EQ(extended_inquiry_data.ref_chk, 0);
  EXPECT_EQ(extended_inquiry_data.uask_sup, 1);
  EXPECT_EQ(extended_inquiry_data.group_sup, 0);
  EXPECT_EQ(extended_inquiry_data.prior_sup, 0);
  EXPECT_EQ(extended_inquiry_data.headsup, 0);
  EXPECT_EQ(extended_inquiry_data.ordsup, 0);
  EXPECT_EQ(extended_inquiry_data.simpsup, 0);
  EXPECT_EQ(extended_inquiry_data.wu_sup, 0);
  EXPECT_EQ(extended_inquiry_data.crd_sup, 0);
  EXPECT_EQ(extended_inquiry_data.nv_sup, 0);
  EXPECT_EQ(extended_inquiry_data.v_sup, 0);
  EXPECT_EQ(extended_inquiry_data.p_i_i_sup, 0);
  EXPECT_EQ(extended_inquiry_data.luiclr, 1);
  EXPECT_EQ(extended_inquiry_data.r_sup, 0);
  EXPECT_EQ(extended_inquiry_data._reserved6, 0);
  EXPECT_EQ(extended_inquiry_data.multi_t_nexus_microcode_download, 0);
  EXPECT_EQ(extended_inquiry_data.extended_self_test_completion_minutes, 0);
  EXPECT_EQ(extended_inquiry_data.poa_sup, 0);
  EXPECT_EQ(extended_inquiry_data.hra_sup, 0);
  EXPECT_EQ(extended_inquiry_data.vsa_sup, 0);
  EXPECT_EQ(extended_inquiry_data.maximum_supported_sense_data_length, 0);
}

TEST_F(InquiryTest, BlockDeviceCharacteristicsVpdBuildsCorrectStruct) {
  inquiry_cmd_.evpd = 1;
  inquiry_cmd_.page_code = scsi::PageCode::kBlockDeviceCharacteristicsVpd;

  translator::StatusCode status = translator::InquiryToScsi(
      scsi_cmd_, buffer_, nvme_wrappers_[0].cmd, nvme_wrappers_[1].cmd);

  scsi::BlockDeviceCharacteristicsVpd result{};
  ASSERT_TRUE(translator::ReadValue(buffer_, result));

  EXPECT_EQ(result.peripheral_qualifier,
            scsi::PeripheralQualifier::kPeripheralDeviceConnected);
  EXPECT_EQ(result.peripheral_device_type,
            scsi::PeripheralDeviceType::kDirectAccessBlock);
  EXPECT_EQ(result.page_code, scsi::PageCode::kDeviceIdentification);
  EXPECT_EQ(result.page_length,
            scsi::PageLength::kBlockDeviceCharacteristicsVpd);
  EXPECT_EQ(result.medium_rotation_rate,
            scsi::MediumRotationRate::kNonRotatingMedium);
  EXPECT_EQ(result.nominal_form_factor, scsi::NominalFormFactor::kNotReported);
}

}  // namespace
