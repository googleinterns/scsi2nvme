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

#ifndef LIB_TRANSLATOR_MAINTENANCE_IN_H
#define LIB_TRANSLATOR_MAINTENANCE_IN_H

#include "common.h"

namespace translator {

// Translation library only supports ReportSupportedOpCodes for WriteSame16
// We always report that we do not support WriteSame16
// This command does not require call to NVMe so it doesn't require translation

// Validates command to ensure it is a well-formatted ReportSupportedOpCodes cmd
// and that the requested OpCode is scsi::OpCode::WriteSame16
StatusCode ValidateReportSupportedOpCodes(Span<const uint8_t> scsi_cmd);

// Writes to the buffer that we do not support the command
void WriteReportSupportedOpCodesResult(Span<uint8_t> buffer);

}  // namespace translator

#endif
