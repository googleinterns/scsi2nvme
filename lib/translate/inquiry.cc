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
StatusCode RawToScsiCommand(absl::Span<const uint32_t> raw_cmd,
                            scsi_defs::InquiryCommand &cmd) {
  if (raw_cmd.empty()) {
    char debug_buffer[100];
    sprintf(debug_buffer, "buffer is empty or nullptr\n");
    if (debug_callback != NULL) debug_callback(debug_buffer);
    return StatusCode::kInvalidInput;
  }

  scsi_defs::OpCode opcode;
  StatusCode opcode_status = MakeScsiOpcode(raw_cmd[0], opcode);
  if (opcode_status != StatusCode::kSuccess) {
    return opcode_status;
  }
  if (opcode != scsi_defs::OpCode::kInquiry) {
    char debug_buffer[100];
    const char *expected_cmd_str =
        ScsiOpcodeToString(scsi_defs::OpCode::kInquiry);
    const char *cmd_str = ScsiOpcodeToString(opcode);
    sprintf(debug_buffer, "invalid opcode. expected %s got %s.",
            expected_cmd_str, cmd_str);
    if (debug_callback != NULL) debug_callback(debug_buffer);

    return StatusCode::kInvalidInput;
  }

  // TODO: check invalid parameters in scsi_command
  cmd = *reinterpret_cast<const scsi_defs::InquiryCommand *>(&raw_cmd[1]);
  return StatusCode::kSuccess;
}

// TODO: Executes the NVME Identify Controller command
nvme_defs::IdentifyControllerData NvmeIdentifyController() {
  return nvme_defs::IdentifyControllerData();
}

// TODO: Executes the NVME Identify Namespace command
nvme_defs::IdentifyNamespace NvmeIdentifyNamespace() {
  return nvme_defs::IdentifyNamespace();
}

scsi_defs::InquiryData TranslateStandardInquiryResponse(
    const nvme_defs::IdentifyControllerData &identify_controller_data,
    const nvme_defs::IdentifyNamespace &identify_namespace_data) {
  // SCSI Inquiry Standard Result
  // https://www.nvmexpress.org/wp-content/uploads/NVM-Express-SCSI-Translation-Reference-1_1-Gold.pdf
  // Section 6.1.1
  scsi_defs::InquiryData result = scsi_defs::InquiryData();
  result.peripheral_qualifier =
      scsi_defs::PeripheralQualifier::kPeripheralDeviceConnected;
  result.peripheral_device_type =
      scsi_defs::PeripheralDeviceType::kDirectAccessBlock;
  result.rmb = 0;
  result.version = scsi_defs::Version::kSpc4;
  result.normaca = 0;
  result.hisup = 0;
  result.response_data_format = scsi_defs::ResponseDataFormat::kCompliant;
  result.additional_length = 0x1f;
  result.sccs = 0;
  result.acc = 0;
  result.tpgs = scsi_defs::TPGS::kNotSupported;
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
    char debug_buffer[100];
    sprintf(debug_buffer, "less than four characters set\n");
    if (debug_callback != NULL) debug_callback(debug_buffer);
  }
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
  return scsi_defs::SupportedVitalProductData {
  .peripheral_qualifier =
      scsi_defs::PeripheralQualifier::kPeripheralDeviceConnected,
  .peripheral_device_type =
      scsi_defs::PeripheralDeviceType::kDirectAccessBlock,
  .page_code = 0,

  // Shall be set to 5 indicating number of items supported Vpd pages list
  // requires. NOTE: document says to set this to 5 but there are 7 entries....
  .page_length = 5,

  .supported_page_list = {0x00, 0x80, 0x83,0x86,0xB0,0xB1,0xB2}
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

      memcpy(result.product_serial_number, formatted_hex_string,
             result.page_length);
    } else {
      // 6.1.3.1.2
      result.page_length = 20;

      // convert 128-bit number into hex string
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

      memcpy(result.product_serial_number, formatted_hex_string,
             result.page_length);
    }
  } else {
    if (nguid_nz) {
      // 6.1.3.1.1
      result.page_length = 40;

      // convert 128-bit number into hex string
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
        // Return Unit Serial Number data page toapplication client. Refer
        // to 6.1.3.
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
      case scsi_defs::PageCode::kReturnBlockDeviceCharacteristicsVpd:
        // TODO: Return Block Device Characteristics Vpd Page to application
        // client, refer to 6.1.7.
        break;
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