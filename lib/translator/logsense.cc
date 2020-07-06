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
// limitations under the License.i// Copyright 2020 Google LLC

#include "logsense.h"

namespace translator {

void TranslateSupportedLogPages(absl::Span<uint8_t> buffer) {
    scsi_defs::SupportedLogPages result = {
        .page_len = 0x4
    };
    scsi_defs::PageCode supported_pages_list[4] = {
        scsi_defs::PageCode::kSupportedLogPages,
        scsi_defs::PageCode::kTemperature,
        scsi_defs::PageCode::kSolidStateMedia,
        scsi_defs::PageCode::kInformationalExceptions
    };
    memcpy(buffer.data(), &result, sizeof(scsi_defs::SupportedLogPages));
    memcpy(&buffer[sizeof(scsi_defs::SupportedLogPages)], supported_pages_list, sizeof(supported_pages_list));

}

// Main logic engine for the Inquiry command
void translate(const scsi_defs::LogSenseCommand &cmd, absl::Span<uint8_t> buffer) {

    if (cmd.sp == 1 || cmd.pc == 1 || cmd.control_byte.naca == 1) {
        /*
        Command may be terminated with CHECK CONDITION
        status, ILLEGAL REQUEST sense key, and ILLEGAL FIELD IN
        CDB additional sense code.
        */
    }

    switch (cmd.page_code) {
        case scsi_defs::PageCode::kSupportedLogPages:
            TranslateSupportedLogPages(buffer);
            break;
        case scsi_defs::PageCode::kTemperature:
        // TranslateTemperature(buffer);
            break;
        case scsi_defs::PageCode::kSolidStateMedia:
        // TranslateSolidStateMedia(buffer);
            break;
        case scsi_defs::PageCode::kInformationalExceptions:
        // TranslateInformationalExceptions(buffer);
            break;
    }
}

}  // namespace translator
