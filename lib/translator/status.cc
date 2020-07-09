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

#include "common.h"

namespace translator {

namespace {  // anonymous namespace for helper functions

constexpr ScsiStatus kDefaultScsiStatus{
    .status = scsi_defs::Status::kCheckCondition,
    .sense_key = scsi_defs::SenseKey::kNoSense,
    .asc = scsi_defs::AdditionalSenseCode::kNoAdditionalSenseInfo,
    .ascq = scsi_defs::AdditionalSenseCodeQualifier::kNoAdditionalSenseInfo,
};

// Section 7.1
// https://www.nvmexpress.org/wp-content/uploads/NVM-Express-SCSI-Translation-Reference-1_1-Gold.pdf
ScsiStatus StatusToScsi(nvme_defs::GenericCommandStatusCode status_code) {
  ScsiStatus result = kDefaultScsiStatus;

  switch (status_code) {
    case nvme_defs::GenericCommandStatusCode::kSuccess:
      result.status = scsi_defs::Status::kGood;
      result.sense_key = scsi_defs::SenseKey::kNoSense;
      break;
    case nvme_defs::GenericCommandStatusCode::kInvalidOpcode:
      result.status = scsi_defs::Status::kCheckCondition;
      result.sense_key = scsi_defs::SenseKey::kIllegalRequest;
      result.asc = scsi_defs::AdditionalSenseCode::kInvalidCommandOpCode;
      result.ascq =
          scsi_defs::AdditionalSenseCodeQualifier::kInvalidCommandOpCode;
      break;
    case nvme_defs::GenericCommandStatusCode::kInvalidField:
      result.status = scsi_defs::Status::kCheckCondition;
      result.sense_key = scsi_defs::SenseKey::kIllegalRequest;
      result.asc = scsi_defs::AdditionalSenseCode::kInvalidFieldInCdb;
      result.ascq = scsi_defs::AdditionalSenseCodeQualifier::kInvalidFieldInCdb;
      break;
    case nvme_defs::GenericCommandStatusCode::kDataTransferError:
      result.status = scsi_defs::Status::kCheckCondition;
      result.sense_key = scsi_defs::SenseKey::kMediumError;
      break;
    case nvme_defs::GenericCommandStatusCode::kAbortedPowerLoss:
      result.status = scsi_defs::Status::kTaskAborted;
      result.sense_key = scsi_defs::SenseKey::kAbortedCommand;
      result.asc = scsi_defs::AdditionalSenseCode::kWarningPowerLossExpected;
      result.ascq =
          scsi_defs::AdditionalSenseCodeQualifier::kWarningPowerLossExpected;
      break;
    case nvme_defs::GenericCommandStatusCode::kInternalDeviceError:
      result.status = scsi_defs::Status::kCheckCondition;
      result.sense_key = scsi_defs::SenseKey::kHardwareError;
      result.asc = scsi_defs::AdditionalSenseCode::kInternalTargetFailure;
      result.ascq =
          scsi_defs::AdditionalSenseCodeQualifier::kInternalTargetFailure;
      break;
    case nvme_defs::GenericCommandStatusCode::kAbortedByRequest:
      result.status = scsi_defs::Status::kTaskAborted;
      result.sense_key = scsi_defs::SenseKey::kAbortedCommand;
      break;
    case nvme_defs::GenericCommandStatusCode::kAbortedSqDeletion:
      result.status = scsi_defs::Status::kTaskAborted;
      result.sense_key = scsi_defs::SenseKey::kAbortedCommand;
      break;
    case nvme_defs::GenericCommandStatusCode::kAbortedFailedFused:
      result.status = scsi_defs::Status::kTaskAborted;
      result.sense_key = scsi_defs::SenseKey::kAbortedCommand;
      break;
    case nvme_defs::GenericCommandStatusCode::kAbortedMissingFused:
      result.status = scsi_defs::Status::kTaskAborted;
      result.sense_key = scsi_defs::SenseKey::kAbortedCommand;
      break;
    case nvme_defs::GenericCommandStatusCode::kInvalidNamespaceOrFormat:
      result.status = scsi_defs::Status::kCheckCondition;
      result.sense_key = scsi_defs::SenseKey::kIllegalRequest;
      result.asc =
          scsi_defs::AdditionalSenseCode::kAccessDeniedInvalidLuIdentifier;
      result.ascq = scsi_defs::AdditionalSenseCodeQualifier::
          kAccessDeniedInvalidLuIdentifier;
      break;
    case nvme_defs::GenericCommandStatusCode::kLbaOutOfRange:
      result.status = scsi_defs::Status::kCheckCondition;
      result.sense_key = scsi_defs::SenseKey::kIllegalRequest;
      result.asc = scsi_defs::AdditionalSenseCode::kLbaOutOfRange;
      result.ascq = scsi_defs::AdditionalSenseCodeQualifier::kLbaOutOfRange;
      break;
    case nvme_defs::GenericCommandStatusCode::kNamespaceNotReady:
      result.status = scsi_defs::Status::kCheckCondition;
      result.sense_key = scsi_defs::SenseKey::kNotReady;
      result.asc = scsi_defs::AdditionalSenseCode::
          kLogicalUnitNotReadyCauseNotReportable;
      result.ascq = scsi_defs::AdditionalSenseCodeQualifier::
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
ScsiStatus StatusToScsi(nvme_defs::CommandSpecificStatusCode status_code) {
  ScsiStatus result = kDefaultScsiStatus;

  switch (status_code) {
    case nvme_defs::CommandSpecificStatusCode::kCompletionQueueInvalid:
      result.status = scsi_defs::Status::kCheckCondition;
      result.sense_key = scsi_defs::SenseKey::kIllegalRequest;
      break;
    case nvme_defs::CommandSpecificStatusCode::kInvalidFormat:
      result.status = scsi_defs::Status::kCheckCondition;
      result.sense_key = scsi_defs::SenseKey::kIllegalRequest;
      result.asc = scsi_defs::AdditionalSenseCode::kFormatCommandFailed;
      result.ascq =
          scsi_defs::AdditionalSenseCodeQualifier::kFormatCommandFailed;
      break;
    case nvme_defs::CommandSpecificStatusCode::kConflictingAttributes:
      result.status = scsi_defs::Status::kCheckCondition;
      result.sense_key = scsi_defs::SenseKey::kIllegalRequest;
      result.asc = scsi_defs::AdditionalSenseCode::kInvalidFieldInCdb;
      result.ascq = scsi_defs::AdditionalSenseCodeQualifier::kInvalidFieldInCdb;
      break;
    default:
      DebugLog("No SCSI translation for NVMe status with code %#x",
               static_cast<uint8_t>(status_code));
  }
  return result;
}

// Section 7.3
// https://www.nvmexpress.org/wp-content/uploads/NVM-Express-SCSI-Translation-Reference-1_1-Gold.pdf
ScsiStatus StatusToScsi(nvme_defs::MediaErrorStatusCode status_code) {
  ScsiStatus result = kDefaultScsiStatus;

  switch (status_code) {
    case nvme_defs::MediaErrorStatusCode::kWriteFaults:
      result.status = scsi_defs::Status::kCheckCondition;
      result.sense_key = scsi_defs::SenseKey::kMediumError;
      result.asc = scsi_defs::AdditionalSenseCode::kPeripheralDeviceWriteFault;
      result.ascq =
          scsi_defs::AdditionalSenseCodeQualifier::kPeripheralDeviceWriteFault;
      break;
    case nvme_defs::MediaErrorStatusCode::kUnrecoveredReadError:
      result.status = scsi_defs::Status::kCheckCondition;
      result.sense_key = scsi_defs::SenseKey::kMediumError;
      result.asc = scsi_defs::AdditionalSenseCode::kUnrecoveredReadError;
      result.ascq =
          scsi_defs::AdditionalSenseCodeQualifier::kUnrecoveredReadError;
      break;
    case nvme_defs::MediaErrorStatusCode::kGuardCheckError:
      result.status = scsi_defs::Status::kCheckCondition;
      result.sense_key = scsi_defs::SenseKey::kMediumError;
      result.asc =
          scsi_defs::AdditionalSenseCode::kLogicalBlockGuardCheckFailed;
      result.ascq = scsi_defs::AdditionalSenseCodeQualifier::
          kLogicalBlockGuardCheckFailed;
      break;
    case nvme_defs::MediaErrorStatusCode::kApplicationTagCheckError:
      result.status = scsi_defs::Status::kCheckCondition;
      result.sense_key = scsi_defs::SenseKey::kMediumError;
      result.asc = scsi_defs::AdditionalSenseCode::
          kLogicalBlockApplicationTagCheckFailed;
      result.ascq = scsi_defs::AdditionalSenseCodeQualifier::
          kLogicalBlockApplicationTagCheckFailed;
      break;
    case nvme_defs::MediaErrorStatusCode::kReferenceTagCheckError:
      result.status = scsi_defs::Status::kCheckCondition;
      result.sense_key = scsi_defs::SenseKey::kMediumError;
      result.asc =
          scsi_defs::AdditionalSenseCode::kLogicalBlockReferenceTagCheckFailed;
      result.ascq = scsi_defs::AdditionalSenseCodeQualifier::
          kLogicalBlockReferenceTagCheckFailed;
      break;
    case nvme_defs::MediaErrorStatusCode::kCompareFailure:
      result.status = scsi_defs::Status::kCheckCondition;
      result.sense_key = scsi_defs::SenseKey::kMiscompare;
      result.asc = scsi_defs::AdditionalSenseCode::kMiscompareDuringVerifyOp;
      result.ascq =
          scsi_defs::AdditionalSenseCodeQualifier::kMiscompareDuringVerifyOp;
      break;
    case nvme_defs::MediaErrorStatusCode::kAccessDenied:
      result.status = scsi_defs::Status::kCheckCondition;
      result.sense_key = scsi_defs::SenseKey::kIllegalRequest;
      result.asc =
          scsi_defs::AdditionalSenseCode::kAccessDeniedInvalidLuIdentifier;
      result.ascq = scsi_defs::AdditionalSenseCodeQualifier::
          kAccessDeniedInvalidLuIdentifier;
      break;
    default:
      DebugLog("No SCSI translation for NVMe status with code %#x",
               static_cast<uint8_t>(status_code));
  }
  return result;
}

}  // namespace

ScsiStatus StatusToScsi(uint8_t status_code_type, uint8_t status_code) {
  nvme_defs::StatusCodeType type =
      static_cast<nvme_defs::StatusCodeType>(status_code_type);

  switch (type) {
    case nvme_defs::StatusCodeType::kGeneric:
      return StatusToScsi(
          static_cast<nvme_defs::GenericCommandStatusCode>(status_code));
    case nvme_defs::StatusCodeType::kCommandSpecific:
      return StatusToScsi(
          static_cast<nvme_defs::CommandSpecificStatusCode>(status_code));
    case nvme_defs::StatusCodeType::kMediaError:
      return StatusToScsi(
          static_cast<nvme_defs::MediaErrorStatusCode>(status_code));
    case nvme_defs::StatusCodeType::kPath:
    case nvme_defs::StatusCodeType::kVendorSpecific:
    default:
      DebugLog(
          "No SCSI translation for nvme status code type %#x"
          "and status code %#x",
          status_code_type, status_code);
      return kDefaultScsiStatus;
  }
}

}  // namespace translator
