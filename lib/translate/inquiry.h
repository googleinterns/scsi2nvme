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
#define LIB_INQUIRY_H

#include <cstdio>

#include "common.h"
#include "absl/types/span.h"
#include "lib/scsi_defs.h"
#include "third_party/spdk_defs/nvme_defs.h"

namespace translator {
void translate(absl::Span<const uint32_t>);
StatusCode RawToScsiCommand(
    absl::Span<const uint32_t>, scsi_defs::InquiryCommand&);

nvme_defs::IdentifyControllerData NvmeIdentifyController();
nvme_defs::IdentifyNamespace NvmeIdentifyNamespace();

scsi_defs::InquiryData BuildStandardInquiry();
scsi_defs::InquiryData TranslateStandardInquiryResponse(
    const nvme_defs::IdentifyControllerData &identify_controller_data,
    const nvme_defs::IdentifyNamespace &identify_namespace_data);

scsi_defs::SupportedVitalProductData BuildSupportedVpdPages();

scsi_defs::UnitSerialNumber BuildUnitSerialNumberVpd();
scsi_defs::UnitSerialNumber TranslateUnitSerialNumberVpdResponse(
    const nvme_defs::IdentifyNamespace &identify_namespace_data);

};  // namespace inquiry
#endif