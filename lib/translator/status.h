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

#ifndef LIB_TRANSLATOR_STATUS_H
#define LIB_TRANSLATOR_STATUS_H

#include "lib/scsi.h"

namespace translator {

struct ScsiStatus {
  scsi::Status status;
  scsi::SenseKey sense_key;
  scsi::AdditionalSenseCode asc;
  scsi::AdditionalSenseCodeQualifier ascq;
};

/**
 * Takes in a raw NVMe status code type and status code
 *
 * Parses them into nvme::StatusCodeType
 * and nvme::[GenericCommand/CommandSpecific/MediaError]StatusCode
 *
 * Translates to corresponding SCSI status, sense key, additional sense code,
 * and additional sense qualifier code
 */
ScsiStatus StatusToScsi(uint8_t status_code_type, uint8_t status_code);

}  // namespace translator
#endif
