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
    DebugLog("buffer is empty or nullptr\n");
    return StatusCode::kInvalidInput;
  }

  scsi_defs::OpCode opcode;
  StatusCode opcode_status = MakeScsiOpcode(raw_cmd[0], opcode);
  if (opcode_status != StatusCode::kSuccess) {
    return opcode_status;
  }
  if (opcode != scsi_defs::OpCode::kInquiry) {
    const char* expected_cmd_str =
        ScsiOpcodeToString(scsi_defs::OpCode::kInquiry);
    const char* cmd_str = ScsiOpcodeToString(opcode);
    DebugLog("invalid opcode. expected %s got %s.",
            expected_cmd_str, cmd_str);
    return StatusCode::kInvalidInput;
  }

  // TODO: check invalid parameters in scsi_command
  cmd = *reinterpret_cast<const scsi_defs::InquiryCommand *>(&raw_cmd[1]);
  return StatusCode::kSuccess;
}

scsi_defs::InquiryData TranslateStandardInquiryResponse(
    const nvme_defs::IdentifyControllerData &identify_controller_data,
    const nvme_defs::IdentifyNamespace &identify_namespace_data) {
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

  // Shall be set to “NVMe” followed by 4 spaces: “NVMe “
  strncpy(result.vendor_identification, kNvmeVendorIdentification.data(), 8);

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

  if (idx >= 0) {
    DebugLog("less than four characters set\n");
  }
  return result;
}

scsi_defs::InquiryData BuildStandardInquiry() {
  // Identify controller results
  nvme_defs::IdentifyControllerData identify_controller_data;
  // Identify namespace results
  nvme_defs::IdentifyNamespace identify_namespace_data;
  return TranslateStandardInquiryResponse(identify_controller_data,
                                          identify_namespace_data);
}

scsi_defs::SupportedVitalProductData BuildSupportedVpdPages() {
  // TODO: write this after SupportedVitalProductData in the return buffer when
  // we agree on how to return SCSI responses
  scsi_defs::PageCode supported_page_list[7] = {
      scsi_defs::PageCode::kSupportedVpd,
      scsi_defs::PageCode::kUnitSerialNumber,
      scsi_defs::PageCode::kDeviceIdentification,
      scsi_defs::PageCode::kExtended,
      scsi_defs::PageCode::kBlockLimitsVpd,
      scsi_defs::PageCode::kBlockDeviceCharacteristicsVpd,
      scsi_defs::PageCode::kLogicalBlockProvisioningVpd};

  return scsi_defs::SupportedVitalProductData{
      // Shall be set to 5 indicating number of items supported Vpd pages list
      // requires. NOTE: document says to set this to 5 but there are 7
      // entries....
      .page_length = 5,
  };
}

scsi_defs::UnitSerialNumber TranslateUnitSerialNumberVpdResponse(
    const nvme_defs::IdentifyNamespace &identify_namespace_data) {
  scsi_defs::UnitSerialNumber result = scsi_defs::UnitSerialNumber();
  result.peripheral_qualifier =
      scsi_defs::PeripheralQualifier::kPeripheralDeviceConnected;
  result.peripheral_device_type =
      scsi_defs::PeripheralDeviceType::kDirectAccessBlock;
  result.page_code = 0x80;

  // TODO: write this after scsi response in buffer.
  char* product_serial_number[100];

  // check if nonzero
  bool nguid_nz =
      identify_namespace_data.nguid[0] || identify_namespace_data.nguid[1];
  bool eui64_nz = identify_namespace_data.eui64;
  if (eui64_nz) {
    if (nguid_nz) {
      // 6.1.3.1.1
      result.page_length = 40;

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

      memcpy(product_serial_number, formatted_hex_string, result.page_length);
    } else {
      // 6.1.3.1.2
      result.page_length = 20;

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

      memcpy(product_serial_number, formatted_hex_string, result.page_length);
    }
  } else {
    if (nguid_nz) {
      // 6.1.3.1.1
      result.page_length = 40;

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

      memcpy(product_serial_number, formatted_hex_string, result.page_length);
    } else {
      // 6.1.3.1.3
      // valid for NVMe 1.0 devices only
      // TODO?
    }
  }

  return result;
}

scsi_defs::UnitSerialNumber BuildUnitSerialNumberVpd() {
  nvme_defs::IdentifyNamespace identify_namespace_data;
  return TranslateUnitSerialNumberVpdResponse(identify_namespace_data);
}

// TODO: write return value to a buffer
// Main logic engine for the Inquiry command
void translate(absl::Span<const uint8_t> raw_cmd) {
  scsi_defs::InquiryCommand cmd;
  StatusCode status = RawToScsiCommand(raw_cmd, cmd);
  if (status != StatusCode::kSuccess) return;

  if (cmd.evpd) {
    switch (cmd.page_code) {
      case scsi_defs::PageCode::kSupportedVpd: {
        // Return Supported Vpd Pages data page to application client, refer
        // to 6.1.2.
        scsi_defs::SupportedVitalProductData result = BuildSupportedVpdPages();

        break;
      }
      case scsi_defs::PageCode::kUnitSerialNumber: {
        // Return Unit Serial Number data page toapplication client.
        // Referto 6.1.3.
        scsi_defs::UnitSerialNumber result = BuildUnitSerialNumberVpd();
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
    scsi_defs::InquiryData result = BuildStandardInquiry();
  }
  return;
}

};  // namespace translator
