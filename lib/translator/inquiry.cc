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

#include "inquiry.h"
#include "string.h"

namespace translator {

// Creates and validates a Inquiry Command struct
StatusCode RawToScsiCommand(absl::Span<const uint8_t> raw_cmd,
                            scsi_defs::InquiryCommand &cmd) {
  if (raw_cmd.empty()) {
    DebugLog("buffer is empty or nullptr");
    return StatusCode::kInvalidInput;
  }

  uint8_t raw_opcode = raw_cmd[0];
  if (raw_opcode > 0xaf) {
    DebugLog("raw opcode is out of range: %x", raw_opcode);
    return StatusCode::kInvalidInput;
  }

  scsi_defs::OpCode opcode = static_cast<scsi_defs::OpCode>(raw_opcode);

  if (opcode != scsi_defs::OpCode::kInquiry) {
    absl::string_view expected_cmd_str =
        ScsiOpcodeToString(scsi_defs::OpCode::kInquiry);
    absl::string_view cmd_str = ScsiOpcodeToString(opcode);
    DebugLog("wrong opcode. expected %s got %s.", expected_cmd_str, cmd_str);
    return StatusCode::kInvalidInput;
  }

  // TODO: check invalid parameters in scsi_command
  cmd = *reinterpret_cast<const scsi_defs::InquiryCommand *>(&raw_cmd[1]);
  return StatusCode::kSuccess;
}

void TranslateStandardInquiry(
    const nvme_defs::IdentifyControllerData &identify_controller_data,
    const nvme_defs::IdentifyNamespace &identify_namespace_data,
    absl::Span<uint8_t> buffer) {
  // SCSI Inquiry Standard Result
  // https://www.nvmexpress.org/wp-content/uploads/NVM-Express-SCSI-Translation-Reference-1_1-Gold.pdf
  // Section 6.1.1
  scsi_defs::InquiryData result = scsi_defs::InquiryData{
      .version = scsi_defs::Version::kSpc4,
      .response_data_format = scsi_defs::ResponseDataFormat::kCompliant,
      .additional_length = 0x1f,
      .tpgs = scsi_defs::TPGS::kNotSupported,
      .protect = (identify_namespace_data.dps.pit == 0 &&
                  identify_namespace_data.dps.md_start == 0)
                     ? 0
                     : 1,

      .cmdque = 1};

  // Shall be set to “NVMe” followed by 4 spaces: “NVMe    “
  // Vendor Identification is not null terminated.
  memcpy(result.vendor_identification, kNvmeVendorIdentification.data(),
         kNvmeVendorIdentification.size());

  // Shall be set to the first 16 bytes of the Model Number (MN) field within
  // the Identify Controller Data Structure
  memcpy(result.product_identification, identify_controller_data.mn,
         sizeof(result.product_identification));

  /*
  Shall be set to the last 4 ASCII graphic characters in the range of 21h-7Eh
  (i.e. last 4 non-space characters) of the Firmware Revision (FR) field within
  the Identify Controller Data Structure.
  */
  int idx = 3;
  for (int i = 7; i >= 0; i--) {
    if (identify_controller_data.fr[i] != ' ' &&
        identify_controller_data.fr[i] >= 0x21 &&
        identify_controller_data.fr[i] <= 0x7e) {
      result.product_revision_level[idx] = identify_controller_data.fr[i];
      if (idx-- == 0) break;
    }
  }

  memcpy(buffer.data(), &result, sizeof(result));
}

void TranslateSupportedVpdPages(absl::Span<uint8_t> buf) {
  scsi_defs::PageCode supported_page_list[7] = {
      scsi_defs::PageCode::kSupportedVpd,
      scsi_defs::PageCode::kUnitSerialNumber,
      scsi_defs::PageCode::kDeviceIdentification,
      scsi_defs::PageCode::kExtended,
      scsi_defs::PageCode::kBlockLimitsVpd,
      scsi_defs::PageCode::kBlockDeviceCharacteristicsVpd,
      scsi_defs::PageCode::kLogicalBlockProvisioningVpd};

  scsi_defs::SupportedVitalProductData result =
      scsi_defs::SupportedVitalProductData{
          // Shall be set to 5 indicating number of items supported Vpd pages
          // list
          // requires. NOTE: document says to set this to 5 but there are 7
          // entries....
          .page_length = sizeof(supported_page_list),
      };
  memcpy(buf.data(), &result, sizeof(result));
  memcpy(&buf[sizeof(result)], &supported_page_list,
         sizeof(supported_page_list));
}

void TranslateUnitSerialNumberVpd(
    const nvme_defs::IdentifyControllerData &identify_controller_data,
    const nvme_defs::IdentifyNamespace &identify_namespace_data, uint32_t nsid,
    absl::Span<uint8_t> buffer) {
  scsi_defs::UnitSerialNumber result = scsi_defs::UnitSerialNumber();
  result.peripheral_qualifier =
      scsi_defs::PeripheralQualifier::kPeripheralDeviceConnected;
  result.peripheral_device_type =
      scsi_defs::PeripheralDeviceType::kDirectAccessBlock;
  result.page_code = scsi_defs::PageCode::kUnitSerialNumber;

  char product_serial_number[100];

  const size_t kNguidLen = 40, kEui64Len = 20, kV1SerialLen = 30;

  // check if nonzero
  bool nguid_nz =
      identify_namespace_data.nguid[0] || identify_namespace_data.nguid[1];
  bool eui64_nz = identify_namespace_data.eui64;
  if (eui64_nz) {
    if (nguid_nz) {
      // 6.1.3.1.1
      result.page_length = kNguidLen;

      // convert 128-bit number into hex string, 64-bits at a time
      char hex_string[33];
      sprintf(hex_string, "%08lx", identify_namespace_data.nguid[0]);
      sprintf(&hex_string[16], "%08lx", identify_namespace_data.nguid[1]);

      // insert _ and . in the correct positions
      char formatted_hex_string[41];
      for (int i = 4; i < result.page_length; i += 5) {
        formatted_hex_string[i] = '_';
      }
      formatted_hex_string[result.page_length - 1] = '.';

      // insert numbers
      int pos = 0;
      for (int i = 0; i < result.page_length - 1; i++) {
        if (formatted_hex_string[i] != '_') {
          formatted_hex_string[i] = hex_string[pos++];
        }
      }

      memcpy(&product_serial_number, formatted_hex_string, result.page_length);
    } else {
      // 6.1.3.1.2
      result.page_length = kEui64Len;

      // convert 64-bit number into hex string
      char hex_string[17];
      sprintf(hex_string, "%08lx", identify_namespace_data.eui64);

      // insert _ and . in the correct positions
      char formatted_hex_string[21];
      for (int i = 4; i < result.page_length; i += 5) {
        formatted_hex_string[i] = '_';
      }
      formatted_hex_string[result.page_length - 1] = '.';

      // insert numbers
      int pos = 0;
      for (int i = 0; i < result.page_length - 1; i++) {
        if (formatted_hex_string[i] != '_') {
          formatted_hex_string[i] = hex_string[pos++];
        }
      }

      memcpy(&product_serial_number, formatted_hex_string, result.page_length);
    }
  } else {
    if (nguid_nz) {
      // 6.1.3.1.1
      result.page_length = kNguidLen;

      // convert 128-bit number into hex string, 64-bits at a time
      char hex_string[33];
      sprintf(hex_string, "%08lx", identify_namespace_data.nguid[0]);
      sprintf(&hex_string[16], "%08lx", identify_namespace_data.nguid[1]);

      // insert _ and . in the correct positions
      char formatted_hex_string[41];
      for (int i = 4; i < result.page_length; i += 5) {
        formatted_hex_string[i] = '_';
      }
      formatted_hex_string[result.page_length - 1] = '.';

      // insert numbers
      int pos = 0;
      for (int i = 0; i < result.page_length - 1; i++) {
        if (formatted_hex_string[i] != '_') {
          formatted_hex_string[i] = hex_string[pos++];
        }
      }

      memcpy(&product_serial_number, formatted_hex_string, result.page_length);
    } else {
      // 6.1.3.1.3
      // valid for NVMe 1.0 devices only

      // PAGE LENGTH should be set to 30 indicating the page length in bytes.
      result.page_length = kV1SerialLen;

      // PRODUCT SERIAL NUMBER should be formed as follows:
      // Bits 239:80 20 bytes of Serial Number (bytes 23:04 of Identify
      // Controller data structure)
      static_assert(sizeof(identify_controller_data.sn) == 20);
      memcpy(&product_serial_number, identify_controller_data.sn,
             sizeof(identify_controller_data.sn));

      // Bits 79:72 ASCII representation of “_”
      product_serial_number[sizeof(identify_controller_data.sn)] = '_';

      // Bits 71:08 ASCII representation of 32 bit Namespace Identifier (NSID)
      memcpy(&product_serial_number[sizeof(identify_controller_data.sn) + 1],
             &nsid, sizeof(nsid));

      // Bits 07:00 ASCII representation of “.”
      product_serial_number[kV1SerialLen - 1] = '.';
    }
  }
  memcpy(buffer.data(), &result, sizeof(result));
  memcpy(&buffer[sizeof(result)], &product_serial_number, result.page_length);
}

// Main logic engine for the Inquiry command
void translate(absl::Span<const uint8_t> raw_cmd, absl::Span<uint8_t> buffer) {
  scsi_defs::InquiryCommand cmd;
  StatusCode status = RawToScsiCommand(raw_cmd, cmd);
  if (status != StatusCode::kSuccess) return;

  nvme_defs::IdentifyControllerData identify_controller_data;
  nvme_defs::IdentifyNamespace identify_namespace_data;

  if (cmd.evpd) {
    switch (cmd.page_code) {
      case scsi_defs::PageCode::kSupportedVpd: {
        // Return Supported Vpd Pages data page to application client, refer
        // to 6.1.2.
        TranslateSupportedVpdPages(buffer);
        break;
      }
      case scsi_defs::PageCode::kUnitSerialNumber: {
        // Return Unit Serial Number data page toapplication client.
        // Referto 6.1.3.

        // TODO: get nsid from Genric Command
        uint32_t nsid = 0x123;
        TranslateUnitSerialNumberVpd(identify_controller_data,
                                     identify_namespace_data, nsid, buffer);
        break;
      }
      case scsi_defs::PageCode::kDeviceIdentification:
        // TODO: Return Device Identification data page toapplication client,
        // refer to 6.1.4
        break;
      case scsi_defs::PageCode::kExtended:
        // TODO: May optionally be supported by returning Extended INQUIRY data
        // page toapplication client, refer to 6.1.5.
        break;
      case scsi_defs::PageCode::kBlockLimitsVpd:
        // TODO: May be supported by returning Block Limits VPD data page to
        // application client, refer to 6.1.6.
        break;
      case scsi_defs::PageCode::kBlockDeviceCharacteristicsVpd:
        // TODO: Return Block Device Characteristics Vpd Page to application
        // client, refer to 6.1.7.
        break;
      case scsi_defs::PageCode::kLogicalBlockProvisioningVpd:
      // May be supported by returning Logical Block Provisioning VPD Page to
      // application client, refer to 6.1.8.
      default:
        // TODO: Command may be terminated with CHECK CONDITION status, ILLEGAL
        // REQUEST dense key, and ILLEGAL FIELD IN CDB additional sense code
        break;
    }
  } else {
    // Return Standard INQUIRY Data to application client
    TranslateStandardInquiry(identify_controller_data, identify_namespace_data,
                             buffer);
  }
  return;
}

};  // namespace translator
