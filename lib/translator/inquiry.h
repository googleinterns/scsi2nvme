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

#ifndef LIB_TRANSLATOR_INQUIRY_H
#define LIB_TRANSLATOR_INQUIRY_H

#include <cstdio>

#include "absl/types/span.h"
#include "common.h"
#include "lib/scsi_defs.h"
#include "third_party/spdk_defs/nvme_defs.h"

namespace translator {

void translate(absl::Span<const uint8_t>);
StatusCode RawToScsiCommand(absl::Span<const uint8_t>,
                            scsi_defs::InquiryCommand &);

scsi_defs::InquiryData TranslateStandardInquiry(
    const nvme_defs::IdentifyControllerData &identify_controller_data,
    const nvme_defs::IdentifyNamespace &identify_namespace_data);

scsi_defs::UnitSerialNumber TranslateUnitSerialNumberVpd(
    const nvme_defs::IdentifyNamespace &identify_namespace_data);

scsi_defs::SupportedVitalProductData TranslateSupportedVpdPages();

};  // namespace translator
#endif