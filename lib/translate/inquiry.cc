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
namespace inquiry {
// Creates and validates a Inquiry Command struct
std::optional<scsi_defs::InquiryCommand> RawToScsiCommand(
    absl::Span<const uint32_t> raw_cmd) {
  if (raw_cmd.empty()) {
    printf("buffer is empty or nullptr\n");
    return std::nullopt;
  }

  uint8_t opcode = raw_cmd[0];
  if (static_cast<scsi_defs::OpCode>(opcode) != scsi_defs::OpCode::kInquiry) {
    printf("invalid opcode. expected %ux. got %ux.",
           static_cast<uint8_t>(scsi_defs::OpCode::kInquiry), opcode);
    return std::nullopt;
  }

  // printf("opcode is good, casting\n");
  // TODO: check invalid parameters in scsi_command
  return std::make_optional(
      *reinterpret_cast<const scsi_defs::InquiryCommand*>(&raw_cmd[1]));
}

// Executes the NVME Identify Controller command
nvme_defs::IdentifyControllerData NvmeIdentifyController() {
  // TODO
  return nvme_defs::IdentifyControllerData();
}

// Executes the NVME Identify Namespace command
nvme_defs::IdentifyNamespace NvmeIdentifyNamespace() {
  // TODO
  return nvme_defs::IdentifyNamespace();
}

scsi_defs::InquiryData TranslateStandardInquiryResponse(
    const nvme_defs::IdentifyControllerData &identify_controller_data,
    const nvme_defs::IdentifyNamespace &identify_namespace_data) {
  // SCSI Inquiry Standard Result
  // https://www.nvmexpress.org/wp-content/uploads/NVM-Express-SCSI-Translation-Reference-1_1-Gold.pdf
  // Section 6.1.1
  scsi_defs::InquiryData result = scsi_defs::InquiryData();
  result.peripheral_qualifier = static_cast<scsi_defs::PeripheralQualifier>(0);
  result.peripheral_device_type =
      static_cast<scsi_defs::PeripheralDeviceType>(0);
  result.rmb = 0;
  result.version = static_cast<scsi_defs::Version>(0x6);
  result.normaca = 0;
  result.hisup = 0;
  result.response_data_format =
      static_cast<scsi_defs::ResponseDataFormat>(0b10);
  result.additional_length = 0x1f;
  result.sccs = 0;
  result.acc = 0;
  result.tpgs = static_cast<scsi_defs::TPGS>(0);
  result.third_party_copy = 0;
  result.protect = (identify_namespace_data.dps.pit == 0 &&
                    identify_namespace_data.dps.md_start == 0)
                       ? 0
                       : 1;
  result.encserv = 0;
  result.multip = 0;
  result.addr_16 = 0;
  result.wbus_16 = 0;
  result.sync = 0;
  result.cmdque = 1;

  // Shall be set to “NVMe” followed by 4 spaces: “NVMe “
  char NVME_VENDOR_IDENTIFICATION[9] = "NVMe    ";
  strncpy(result.vendor_identification, NVME_VENDOR_IDENTIFICATION, 8);

  // Shall be set to the first 16 bytes of the Model Number (MN) field within
  // the Identify Controller Data Structure
  memcpy(result.product_identification, identify_controller_data.mn, 16);

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
  if (idx >= 0) printf("less than four characters set\n");
  return result;
}
scsi_defs::InquiryData BuildStandardInquiry() {
  // Identify controller results
  nvme_defs::IdentifyControllerData identify_controller_data =
      NvmeIdentifyController();
  // Identify namespace results
  nvme_defs::IdentifyNamespace identify_namespace_data =
      NvmeIdentifyNamespace();
  return TranslateStandardInquiryResponse(identify_controller_data,
                                             identify_namespace_data);
}

scsi_defs::SupportedVitalProductData BuildSupportedVpdPages() {
  scsi_defs::SupportedVitalProductData result =
      scsi_defs::SupportedVitalProductData();
  result.peripheral_qualifier =
      scsi_defs::PeripheralQualifier::kPeripheralDeviceConnected;
  result.peripheral_device_type =
      scsi_defs::PeripheralDeviceType::kDirectAccessBlock;
  result.page_code = 0;

  // Shall be set to 5 indicating number of items supported Vpd pages list
  // requires. NOTE: document says to set this to 5 but there are 7 entries....
  result.page_length = 5;

  result.supported_page_list[0] = 0x00;
  result.supported_page_list[1] = 0x80;
  result.supported_page_list[2] = 0x83;
  result.supported_page_list[3] = 0x86;
  result.supported_page_list[4] = 0xB0;
  result.supported_page_list[5] = 0xB1;
  result.supported_page_list[6] = 0xB2;
  return result;
}

scsi_defs::UnitSerialNumber TranslateUnitSerialNumberVpdResponse(
    const nvme_defs::IdentifyNamespace &identify_namespace_data) {
  scsi_defs::UnitSerialNumber result = scsi_defs::UnitSerialNumber();
  result.peripheral_qualifier =
      scsi_defs::PeripheralQualifier::kPeripheralDeviceConnected;
  result.peripheral_device_type =
      scsi_defs::PeripheralDeviceType::kDirectAccessBlock;
  result.page_code = 0x80;

  // check if nonzero
  bool nguid_nz =
      identify_namespace_data.nguid[0] || identify_namespace_data.nguid[1];
  bool eui64_nz = identify_namespace_data.eui64;
  if (eui64_nz) {
    if (nguid_nz) {
      // 6.1.3.1.1
      result.page_length = 40;

      // convert 128-bit number into hex string
      char hex_string[33];
      sprintf(hex_string, "%08llx", identify_namespace_data.nguid[0]);
      sprintf(&hex_string[16], "%08llx", identify_namespace_data.nguid[1]);

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

      memcpy(result.product_serial_number, formatted_hex_string,
             result.page_length);
    } else {
      // 6.1.3.1.2
      result.page_length = 20;

      // convert 128-bit number into hex string
      char hex_string[17];
      sprintf(hex_string, "%08llx", identify_namespace_data.eui64);

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

      memcpy(result.product_serial_number, formatted_hex_string,
             result.page_length);
    }
  } else {
    if (nguid_nz) {
      // 6.1.3.1.1
      result.page_length = 40;

      // convert 128-bit number into hex string
      char hex_string[33];
      sprintf(hex_string, "%08llx", identify_namespace_data.nguid[0]);
      sprintf(&hex_string[16], "%08llx", identify_namespace_data.nguid[1]);

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

      memcpy(result.product_serial_number, formatted_hex_string,
             result.page_length);
    } else {
      // 6.1.3.1.3
      // valid for NVMe 1.0 devices only
      // TODO?
    }
  }

  return result;
}

scsi_defs::UnitSerialNumber BuildUnitSerialNumberVpd() {
  nvme_defs::IdentifyNamespace identify_namespace_data =
      NvmeIdentifyNamespace();
  return TranslateUnitSerialNumberVpdResponse(identify_namespace_data);
}

// TODO: write return value to a buffer
// Main logic engine for the Inquiry command
void translate(absl::Span<const uint32_t> raw_cmd) {
  std::optional<scsi_defs::InquiryCommand> opt_cmd =
      RawToScsiCommand(raw_cmd);
  if (!opt_cmd.has_value()) return;

  scsi_defs::InquiryCommand cmd = opt_cmd.value();
  if (cmd.evpd) {
    switch (cmd.page_code) {
      // Shall be supported by returning Supported Vpd Pages data page to
      // application client, refer to 6.1.2.
      case 0x00: {
        scsi_defs::SupportedVitalProductData result =
            BuildSupportedVpdPages();
        break;
      }
      // Shall be supported by returning Unit Serial Number data page to
      // application client. Refer to 6.1.3.
      case 0x80: {
        scsi_defs::UnitSerialNumber result = BuildUnitSerialNumberVpd();
        break;
      }
      // Shall be supported by returning Device Identification data page to
      // application client, refer to 6.1.4
      case 0x83:
        break;
      // May optionally be supported by returning Extended INQUIRY data page to
      // application client, refer to 6.1.5.
      case 0x86:
        break;
      // Shall be supported by returning Block Device Characteristics Vpd Page
      // to application client, refer to 6.1.7.
      case 0xB1:
        break;
      // Command may be terminated with CHECK CONDITION status, ILLEGAL REQUEST
      // sense key, and ILLEGAL FIELD IN CDB additional sense code
      default:
        break;
    }
  } else {
    // Shall be supported by returning Standard INQUIRY Data to application
    // client
    scsi_defs::InquiryData result = BuildStandardInquiry();
  }
  return;
}
};  // namespace inquiry