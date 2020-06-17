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

    scsi_defs::InquiryData* translate(uint32_t* raw_cmd, int buffer_length) {
        if (buffer_length == 0 || raw_cmd == nullptr) {
            printf("buffer is empty or nullptr\n");
            return nullptr;
        }

        uint8_t opcode = raw_cmd[0];
        if (static_cast<scsi_defs::OpCode>(opcode) != scsi_defs::OpCode::kInquiry) {
            printf("invalid opcode. expected %ux. got %ux.", static_cast<uint8_t>(scsi_defs::OpCode::kInquiry), opcode);
            return nullptr;
        }

        scsi_defs::InquiryCommand* scsi_cmd = reinterpret_cast<scsi_defs::InquiryCommand*>(raw_cmd[0]);
 
        // TODO: check invalid parameters
        if (scsi_cmd->evpd) {

        }
        else {
            // Shall be supported by returning Standard INQUIRY Data to application client
            
            /*
            TODO: actually execute nvme commands
            The Identify command uses the Data Pointer, Command Dword 10, Command Dword 11, and Command
            Dword 14 fields. All other command specific fields are reserved.
            */

            // Identify controller results
            nvme_defs::IdentifyControllerData *identify_controller_data = new nvme_defs::IdentifyControllerData();
            // Identify namespace results
            nvme_defs::IdentifyNamespace *identify_namespace_data = new nvme_defs::IdentifyNamespace();

            // SCSI Inquiry Standard Result
            scsi_defs::InquiryData *scsi_result = new scsi_defs::InquiryData();
            scsi_result->peripheral_qualifier = static_cast<scsi_defs::PeripheralQualifier>(0);
            scsi_result->peripheral_device_type = static_cast<scsi_defs::PeripheralDeviceType>(0);
            scsi_result->rmb = 0;
            scsi_result->version = static_cast<scsi_defs::Version>(0x6);
            scsi_result->normaca = 0;
            scsi_result->hisup = 0;
            scsi_result->response_data_format = static_cast<scsi_defs::ResponseDataFormat>(0b10);
            scsi_result->additional_length = 0x1f;
            scsi_result->sccs = 0;
            scsi_result->acc = 0;
            scsi_result->tpgs = static_cast<scsi_defs::TPGS>(0);
            scsi_result->third_party_copy = 0;
            scsi_result->protect = (identify_namespace_data->dps.pit == 0 and identify_namespace_data->dps.md_start == 0) ? 0 : 1;
            scsi_result->encserv = 0;
            scsi_result->multip = 0;
            scsi_result->addr_16 = 0;
            scsi_result->wbus_16 = 0;
            scsi_result->sync = 0;
            scsi_result->cmdque = 1;

            // Shall be set to “NVMe” followed by 4 spaces: “NVMe “
            char x[9] = "NVMe    ";
            scsi_result->vendor_identification = *(reinterpret_cast<uint64_t*>(&x));
            printf("%lux\n", scsi_result->vendor_identification);

            // Shall be set to the first 16 bytes of the Model Number (MN) field within the Identify Controller Data Structure
            for(int i = 0; i < 16; i++) {
                scsi_result->product_identification[i] = identify_controller_data->mn[i];
            }

            // Shall be set to the first 4 bytes of the Firmware Revision (FR) field within the Identify Controller Data Structure
            for(int i = 0; i < 4; i++) {
                scsi_result->product_revision_level <<= 8;
                scsi_result->product_revision_level |= identify_controller_data->fr[i];
            }
            return scsi_result;
        }

        return nullptr;
    }
};