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

namespace inquiry {
    // Creates and validates a Inquiry Command struct
    scsi_defs::InquiryCommand raw_cmd_to_scsi_command(uint32_t* raw_cmd, int buffer_length) {
        if (buffer_length == 0 || raw_cmd == nullptr) {
            printf("buffer is empty or nullptr\n");
            // TODO: Optionals?
            // return nullptr;
        }

        uint8_t opcode = raw_cmd[0];
        if (static_cast<scsi_defs::OpCode>(opcode) != scsi_defs::OpCode::kInquiry) {
            printf("invalid opcode. expected %ux. got %ux.", static_cast<uint8_t>(scsi_defs::OpCode::kInquiry), opcode);
            // TODO: Optionals?
            // return nullptr;
        }

        // TODO: check invalid parameters in scsi_command
        return *reinterpret_cast<scsi_defs::InquiryCommand*>(raw_cmd[1]);
    }

    // Executes the NVME Identify Controller command
    nvme_defs::IdentifyControllerData nvme_identify_controller() {
        // TODO
        return nvme_defs::IdentifyControllerData();
    }
    // Executes the NVME Identify Namespace command
    nvme_defs::IdentifyNamespace nvme_identify_namespace() {
        // TODO
        return nvme_defs::IdentifyNamespace();
    }

    scsi_defs::InquiryData build_standard_inquiry_data() {
        // Identify controller results
        nvme_defs::IdentifyControllerData identify_controller_data = nvme_identify_controller();
        // Identify namespace results
        nvme_defs::IdentifyNamespace identify_namespace_data = nvme_identify_namespace();

        // SCSI Inquiry Standard Result
        // https://www.nvmexpress.org/wp-content/uploads/NVM-Express-SCSI-Translation-Reference-1_1-Gold.pdf
        // Section 6.1.1
        scsi_defs::InquiryData result = scsi_defs::InquiryData();
        result.peripheral_qualifier = static_cast<scsi_defs::PeripheralQualifier>(0);
        result.peripheral_device_type = static_cast<scsi_defs::PeripheralDeviceType>(0);
        result.rmb = 0;
        result.version = static_cast<scsi_defs::Version>(0x6);
        result.normaca = 0;
        result.hisup = 0;
        result.response_data_format = static_cast<scsi_defs::ResponseDataFormat>(0b10);
        result.additional_length = 0x1f;
        result.sccs = 0;
        result.acc = 0;
        result.tpgs = static_cast<scsi_defs::TPGS>(0);
        result.third_party_copy = 0;
        result.protect = (identify_namespace_data.dps.pit == 0 && identify_namespace_data.dps.md_start == 0) ? 0 : 1;
        result.encserv = 0;
        result.multip = 0;
        result.addr_16 = 0;
        result.wbus_16 = 0;
        result.sync = 0;
        result.cmdque = 1;

        // Shall be set to “NVMe” followed by 4 spaces: “NVMe “
        char NVME_VENDOR_IDENTIFICATION[9] = "NVMe    ";
        for(int i = 0; i < 8; i++) {
            result.vendor_identification <<= 8;
            result.vendor_identification |= NVME_VENDOR_IDENTIFICATION[i];
        }
        // printf("%lux\n", result.vendor_identification);

        // Shall be set to the first 16 bytes of the Model Number (MN) field within the Identify Controller Data Structure
        for(int i = 0; i < 16; i++) {
            result.product_identification[i] = identify_controller_data.mn[i];
        }

        // Shall be set to the first 4 bytes of the Firmware Revision (FR) field within the Identify Controller Data Structure
        for(int i = 0; i < 4; i++) {
            result.product_revision_level <<= 8;
            result.product_revision_level |= identify_controller_data.fr[i];
        }
        return result;
    }
    scsi_defs::SupportedVitalProductDataPages build_supported_vpd_pages_data()
    {
        scsi_defs::SupportedVitalProductDataPages result = scsi_defs::SupportedVitalProductDataPages();
        result.peripheral_qualifier = scsi_defs::PeripheralQualifier::kPeripheralDeviceConnected;
        result.peripheral_device_type = scsi_defs::PeripheralDeviceType::kDirectAccessBlock;

        // Shall be set to 5 indicating number of items supported VPD pages list requires.
        // NOTE: document says to set this to 5 but there are 7 entries....
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

    // TODO: figure out return value...
    // Main logic engine for the Inquiry command
    void translate(uint32_t* raw_cmd, int buffer_length) {
        scsi_defs::InquiryCommand cmd = raw_cmd_to_scsi_command(raw_cmd, buffer_length);
 
        if (cmd.evpd) {
            switch (cmd.page_code) {
                // Shall be supported by returning Supported VPD Pages data page to application client, refer to 6.1.2.
                case 0x00:
                 {
                    scsi_defs::SupportedVitalProductDataPages result = build_supported_vpd_pages_data();
                    break;
                 }
                // Shall be supported by returning Unit Serial Number data page to application client. Refer to 6.1.3.
                case 0x80:
                    break;
                // Shall be supported by returning Device Identification data page to application client, refer to 6.1.4
                case 0x83:
                    break;
                // May optionally be supported by returning Extended INQUIRY data page to application client, refer to 6.1.5.
                case 0x86:
                    break;
                // Shall be supported by returning Block Device Characteristics VPD Page to application client, refer to 6.1.7.
                case 0xB1:
                    break;
                // Command may be terminated with CHECK CONDITION status, ILLEGAL REQUEST sense key, and ILLEGAL FIELD IN CDB additional sense code
                default:
                    break;
            }
        }
        else {
            // Shall be supported by returning Standard INQUIRY Data to application client
            scsi_defs::InquiryData result = build_standard_inquiry_data();
        }
        return;
    }
};