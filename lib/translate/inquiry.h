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
std::optional<scsi_defs::InquiryCommand> RawToScsiCommand(
    absl::Span<const uint32_t>);

nvme_defs::IdentifyControllerData NvmeIdentifyController();
nvme_defs::IdentifyNamespace NvmeIdentifyNamespace();

scsi_defs::InquiryData BuildStandardInquiry();
scsi_defs::InquiryData TranslateStandardInquiryResponse(
    nvme_defs::IdentifyControllerData identify_controller_data,
    nvme_defs::IdentifyNamespace identify_namespace_data);

scsi_defs::SupportedVitalProductData BuildSupportedVPDPages();

scsi_defs::UnitSerialNumber BuildUnitSerialNumberVPD();
scsi_defs::UnitSerialNumber TranslateUnitSerialNumberVPDResponse(
    nvme_defs::IdentifyNamespace identify_namespace_data);

};  // namespace inquiry

#endif