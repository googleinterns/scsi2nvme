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

#include "status.h"

#include "third_party/spdk/nvme.h"

namespace translator {

namespace {  // anonymous namespace for helper functions

constexpr ScsiStatus kDefaultScsiStatus{
    .status = scsi::Status::kCheckCondition,
    .sense_key = scsi::SenseKey::kNoSense,
    .asc = scsi::AdditionalSenseCode::kNoAdditionalSenseInfo,
    .ascq = scsi::AdditionalSenseCodeQualifier::kNoAdditionalSenseInfo,
};

// Section 7.1
// https://www.nvmexpress.org/wp-content/uploads/NVM-Express-SCSI-Translation-Reference-1_1-Gold.pdf
ScsiStatus StatusToScsi(nvme::GenericCommandStatusCode status_code) {
  ScsiStatus result = kDefaultScsiStatus;

  switch (status_code) {
    case nvme::GenericCommandStatusCode::kSuccess:
      result.status = scsi::Status::kGood;
      result.sense_key = scsi::SenseKey::kNoSense;
      break;
    case nvme::GenericCommandStatusCode::kInvalidOpcode:
      result.status = scsi::Status::kCheckCondition;
      result.sense_key = scsi::SenseKey::kIllegalRequest;
      result.asc = scsi::AdditionalSenseCode::kInvalidCommandOpCode;
      result.ascq = scsi::AdditionalSenseCodeQualifier::kInvalidCommandOpCode;
      break;
    case nvme::GenericCommandStatusCode::kInvalidField:
      result.status = scsi::Status::kCheckCondition;
      result.sense_key = scsi::SenseKey::kIllegalRequest;
      result.asc = scsi::AdditionalSenseCode::kInvalidFieldInCdb;
      result.ascq = scsi::AdditionalSenseCodeQualifier::kInvalidFieldInCdb;
      break;
    case nvme::GenericCommandStatusCode::kDataTransferError:
      result.status = scsi::Status::kCheckCondition;
      result.sense_key = scsi::SenseKey::kMediumError;
      break;
    case nvme::GenericCommandStatusCode::kAbortedPowerLoss:
      result.status = scsi::Status::kTaskAborted;
      result.sense_key = scsi::SenseKey::kAbortedCommand;
      result.asc = scsi::AdditionalSenseCode::kWarningPowerLossExpected;
      result.ascq =
          scsi::AdditionalSenseCodeQualifier::kWarningPowerLossExpected;
      break;
    case nvme::GenericCommandStatusCode::kInternalDeviceError:
      result.status = scsi::Status::kCheckCondition;
      result.sense_key = scsi::SenseKey::kHardwareError;
      result.asc = scsi::AdditionalSenseCode::kInternalTargetFailure;
      result.ascq = scsi::AdditionalSenseCodeQualifier::kInternalTargetFailure;
      break;
    case nvme::GenericCommandStatusCode::kAbortedByRequest:
      result.status = scsi::Status::kTaskAborted;
      result.sense_key = scsi::SenseKey::kAbortedCommand;
      break;
    case nvme::GenericCommandStatusCode::kAbortedSqDeletion:
      result.status = scsi::Status::kTaskAborted;
      result.sense_key = scsi::SenseKey::kAbortedCommand;
      break;
    case nvme::GenericCommandStatusCode::kAbortedFailedFused:
      result.status = scsi::Status::kTaskAborted;
      result.sense_key = scsi::SenseKey::kAbortedCommand;
      break;
    case nvme::GenericCommandStatusCode::kAbortedMissingFused:
      result.status = scsi::Status::kTaskAborted;
      result.sense_key = scsi::SenseKey::kAbortedCommand;
      break;
    case nvme::GenericCommandStatusCode::kInvalidNamespaceOrFormat:
      result.status = scsi::Status::kCheckCondition;
      result.sense_key = scsi::SenseKey::kIllegalRequest;
      result.asc = scsi::AdditionalSenseCode::kAccessDeniedInvalidLuIdentifier;
      result.ascq =
          scsi::AdditionalSenseCodeQualifier::kAccessDeniedInvalidLuIdentifier;
      break;
    case nvme::GenericCommandStatusCode::kLbaOutOfRange:
      result.status = scsi::Status::kCheckCondition;
      result.sense_key = scsi::SenseKey::kIllegalRequest;
      result.asc = scsi::AdditionalSenseCode::kLbaOutOfRange;
      result.ascq = scsi::AdditionalSenseCodeQualifier::kLbaOutOfRange;
      break;
    case nvme::GenericCommandStatusCode::kNamespaceNotReady:
      result.status = scsi::Status::kCheckCondition;
      result.sense_key = scsi::SenseKey::kNotReady;
      result.asc =
          scsi::AdditionalSenseCode::kLogicalUnitNotReadyCauseNotReportable;
      result.ascq = scsi::AdditionalSenseCodeQualifier::
          kLogicalUnitNotReadyCauseNotReportable;
      break;
    default:
      DebugLog("No SCSI translation for NVMe status with code %#x",
               static_cast<uint8_t>(status_code));
  }
  return result;
}

// Section 7.2
// https://www.nvmexpress.org/wp-content/uploads/NVM-Express-SCSI-Translation-Reference-1_1-Gold.pdf
ScsiStatus StatusToScsi(nvme::CommandSpecificStatusCode status_code) {
  ScsiStatus result = kDefaultScsiStatus;

  switch (status_code) {
    case nvme::CommandSpecificStatusCode::kCompletionQueueInvalid:
      result.status = scsi::Status::kCheckCondition;
      result.sense_key = scsi::SenseKey::kIllegalRequest;
      break;
    case nvme::CommandSpecificStatusCode::kInvalidFormat:
      result.status = scsi::Status::kCheckCondition;
      result.sense_key = scsi::SenseKey::kIllegalRequest;
      result.asc = scsi::AdditionalSenseCode::kFormatCommandFailed;
      result.ascq = scsi::AdditionalSenseCodeQualifier::kFormatCommandFailed;
      break;
    case nvme::CommandSpecificStatusCode::kConflictingAttributes:
      result.status = scsi::Status::kCheckCondition;
      result.sense_key = scsi::SenseKey::kIllegalRequest;
      result.asc = scsi::AdditionalSenseCode::kInvalidFieldInCdb;
      result.ascq = scsi::AdditionalSenseCodeQualifier::kInvalidFieldInCdb;
      break;
    default:
      DebugLog("No SCSI translation for NVMe status with code %#x",
               static_cast<uint8_t>(status_code));
  }
  return result;
}

// Section 7.3
// https://www.nvmexpress.org/wp-content/uploads/NVM-Express-SCSI-Translation-Reference-1_1-Gold.pdf
ScsiStatus StatusToScsi(nvme::MediaErrorStatusCode status_code) {
  ScsiStatus result = kDefaultScsiStatus;

  switch (status_code) {
    case nvme::MediaErrorStatusCode::kWriteFaults:
      result.status = scsi::Status::kCheckCondition;
      result.sense_key = scsi::SenseKey::kMediumError;
      result.asc = scsi::AdditionalSenseCode::kPeripheralDeviceWriteFault;
      result.ascq =
          scsi::AdditionalSenseCodeQualifier::kPeripheralDeviceWriteFault;
      break;
    case nvme::MediaErrorStatusCode::kUnrecoveredReadError:
      result.status = scsi::Status::kCheckCondition;
      result.sense_key = scsi::SenseKey::kMediumError;
      result.asc = scsi::AdditionalSenseCode::kUnrecoveredReadError;
      result.ascq = scsi::AdditionalSenseCodeQualifier::kUnrecoveredReadError;
      break;
    case nvme::MediaErrorStatusCode::kGuardCheckError:
      result.status = scsi::Status::kCheckCondition;
      result.sense_key = scsi::SenseKey::kMediumError;
      result.asc = scsi::AdditionalSenseCode::kLogicalBlockGuardCheckFailed;
      result.ascq =
          scsi::AdditionalSenseCodeQualifier::kLogicalBlockGuardCheckFailed;
      break;
    case nvme::MediaErrorStatusCode::kApplicationTagCheckError:
      result.status = scsi::Status::kCheckCondition;
      result.sense_key = scsi::SenseKey::kMediumError;
      result.asc =
          scsi::AdditionalSenseCode::kLogicalBlockApplicationTagCheckFailed;
      result.ascq = scsi::AdditionalSenseCodeQualifier::
          kLogicalBlockApplicationTagCheckFailed;
      break;
    case nvme::MediaErrorStatusCode::kReferenceTagCheckError:
      result.status = scsi::Status::kCheckCondition;
      result.sense_key = scsi::SenseKey::kMediumError;
      result.asc =
          scsi::AdditionalSenseCode::kLogicalBlockReferenceTagCheckFailed;
      result.ascq = scsi::AdditionalSenseCodeQualifier::
          kLogicalBlockReferenceTagCheckFailed;
      break;
    case nvme::MediaErrorStatusCode::kCompareFailure:
      result.status = scsi::Status::kCheckCondition;
      result.sense_key = scsi::SenseKey::kMiscompare;
      result.asc = scsi::AdditionalSenseCode::kMiscompareDuringVerifyOp;
      result.ascq =
          scsi::AdditionalSenseCodeQualifier::kMiscompareDuringVerifyOp;
      break;
    case nvme::MediaErrorStatusCode::kAccessDenied:
      result.status = scsi::Status::kCheckCondition;
      result.sense_key = scsi::SenseKey::kIllegalRequest;
      result.asc = scsi::AdditionalSenseCode::kAccessDeniedInvalidLuIdentifier;
      result.ascq =
          scsi::AdditionalSenseCodeQualifier::kAccessDeniedInvalidLuIdentifier;
      break;
    default:
      DebugLog("No SCSI translation for NVMe status with code %#x",
               static_cast<uint8_t>(status_code));
  }
  return result;
}

}  // namespace

ScsiStatus StatusToScsi(uint8_t status_code_type, uint8_t status_code) {
  nvme::StatusCodeType type =
      static_cast<nvme::StatusCodeType>(status_code_type);

  switch (type) {
    case nvme::StatusCodeType::kGeneric:
      return StatusToScsi(
          static_cast<nvme::GenericCommandStatusCode>(status_code));
    case nvme::StatusCodeType::kCommandSpecific:
      return StatusToScsi(
          static_cast<nvme::CommandSpecificStatusCode>(status_code));
    case nvme::StatusCodeType::kMediaError:
      return StatusToScsi(static_cast<nvme::MediaErrorStatusCode>(status_code));
    case nvme::StatusCodeType::kPath:
    case nvme::StatusCodeType::kVendorSpecific:
    default:
      DebugLog(
          "No SCSI translation for nvme status code type %#x"
          "and status code %#x",
          status_code_type, status_code);
      return kDefaultScsiStatus;
  }
}

}  // namespace translator
