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

#ifndef LIB_INQUIRY_H
#define LIB_INQUIEY_H

#include <cstdio>
#include <optional>

#include "third_party/spdk_defs/nvme_defs.h"
#include "lib/scsi_defs.h"
#include "absl/types/span.h"

namespace inquiry {
void translate(absl::Span<const uint32_t>);
std::optional<scsi_defs::InquiryCommand> raw_cmd_to_scsi_command(
    absl::Span<const uint32_t>);

nvme_defs::IdentifyControllerData nvme_identify_controller();
nvme_defs::IdentifyNamespace nvme_identify_namespace();

scsi_defs::InquiryData build_standard_inquiry();
scsi_defs::InquiryData translate_standard_inquiry_response(
    nvme_defs::IdentifyControllerData identify_controller_data,
    nvme_defs::IdentifyNamespace identify_namespace_data);

scsi_defs::SupportedVitalProductData build_supported_vpd_pages();

scsi_defs::UnitSerialNumber build_unit_serial_number_vpd();
scsi_defs::UnitSerialNumber translate_unit_serial_number_vpd_response(
    nvme_defs::IdentifyNamespace identify_namespace_data);

};  // namespace inquiry

#endif