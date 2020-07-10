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

#include "test_unit_ready.h"
namespace translator {
namespace {
// TODO: mocked. this fn should be on the NVMe side.
StatusCode NvmeReady(bool& result) {
  result = true;
  return StatusCode::kSuccess;
}

}  // namespace

StatusCode TestUnitReadyToNvme(absl::Span<const uint8_t> scsi_cmd) {
  scsi::TestUnitReadyCommand test_unit_ready_cmd{};
  if (!ReadValue(scsi_cmd, test_unit_ready_cmd)) {
    DebugLog("Malformed TestUnitReady Command");
    return StatusCode::kInvalidInput;
  }

  if (test_unit_ready_cmd.control_byte.naca == 1) {
    return StatusCode::kInvalidInput;
  }

  return StatusCode::kSuccess;
}

StatusCode TestUnitReadyToScsi(bool& result) { return NvmeReady(result); }
}  // namespace translator