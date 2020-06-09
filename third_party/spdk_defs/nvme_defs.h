#ifndef LIB_NVME_DEFS_H
#define LIB_NVME_DEFS_H

#include <cstdint>

// https://github.com/spdk/spdk/blob/master/include/spdk/nvme_spec.h
namespace nvme_defs {

// NVMe Base Specification Figure 125
// https://nvmexpress.org/wp-content/uploads/NVM-Express-1_4-2019.06.10-Ratified.pdf
enum class StatusCodeType : uint8_t {
  kGeneric = 0x0,
  kCommandSpecific = 0x1,
  kMediaError = 0x2,
  kPath = 0x3,
  // Reserved 0x4-0x6
  kVendorSpecific = 0x7,
};

// NVMe Base Specification Figure 126 and Figure 127
// https://nvmexpress.org/wp-content/uploads/NVM-Express-1_4-2019.06.10-Ratified.pdf
enum class GenericCommandStatusCode : uint8_t {
  kSuccess = 0x00,
  kInvalidOpcode = 0x01,
  kInvalidField = 0x02,
  kCommandIdConflict = 0x03,
  kDataTransferError = 0x04,
  kAbortedPowerLoss = 0x05,
  kInternalDeviceError = 0x06,
  kAbortedByRequest = 0x07,
  kAbortedSqDeletion = 0x08,
  kAbortedFailedFused = 0x09,
  kAbortedMissingFused = 0x0a,
  kInvalidNamespaceOrFormat = 0x0b,
  kCommandSequenceError = 0x0c,
  kInvalidSglSegDescriptor = 0x0d,
  kInvalidNumSglDescirptors = 0x0e,
  kDataSglLengthInvalid = 0x0f,
  kMetadataSglLengthInvalid = 0x10,
  kSglDescriptorTypeInvalid = 0x11,
  kInvalidControllerMemBuf = 0x12,
  kInvalidPrpOffset = 0x13,
  kAtomicWriteUnitExceeded = 0x14,
  kOperationDenied = 0x15,
  kInvalidSglOffset = 0x16,
  // Reserved 0x17
  kHostidInconsistentFormat = 0x18,
  kKeepAliveExpired = 0x19,
  kKeepAliveInvalid = 0x1a,
  kAbortedPreempt = 0x1b,
  kSanitizeFailed = 0x1c,
  kSanitizeInProgress = 0x1d,
  kSglDataBlockGranularityInvalid = 0x1e,
  kCommandInvalidInCmb = 0x1f,

  // NVM command set
  kLbaOutOfRange = 0x80,
  kCapacityExceeded = 0x81,
  kNamespaceNotReady = 0x82,
  kReservationConflict = 0x83,
  kFormatInProgress = 0x84,
};

// NVMe Base Specification Figure 128 and Figure 129
// https://nvmexpress.org/wp-content/uploads/NVM-Express-1_4-2019.06.10-Ratified.pdf
enum class CommandSpecificStatusCode : uint8_t {
  kCompletionQueueInvalid = 0x00,
  kInvalidQueueIdentifier = 0x01,
  kInvalidQueueSize = 0x02,
  kAbortCommandLimitExceeded = 0x03,
  // Reserved 0x04
  kAsyncEventRequestLimitExceeded = 0x05,
  kInvalidFirmwareSlot = 0x06,
  kInvalidFirmwareImage = 0x07,
  kInvalidInterruptVector = 0x08,
  kInvalidLogPage = 0x09,
  kInvalidFormat = 0x0a,
  kFirmwareReqConventionalReset = 0x0b,
  kInvalidQueueDeletion = 0x0c,
  kFeatureIdNotSaveable = 0x0d,
  kFeatureNotChangeable = 0x0e,
  kFeatureNotNamespaceSpecific = 0x0f,
  kFirmwareReqNvmReset = 0x10,
  kFirmwareReqReset = 0x11,
  kFirmwareReqMaxTimeViolation = 0x12,
  kFirmwareActivationProhibited = 0x13,
  kOverlappingRange = 0x14,
  kNamespaceInsufficientCapacity = 0x15,
  kNamespaceIdUnavailable = 0x16,
  // Reserved 0x17
  kNamespaceAlreadyAttached = 0x18,
  kNamespaceIsPrivate = 0x19,
  kNamespaceNotAttached = 0x1a,
  kThinprovisioningNotSupported = 0x1b,
  kControllerListInvalid = 0x1c,
  kDeviceSelfTestInProgress = 0x1,
  kBootPartitionWriteProhibited = 0x1e,
  kInvalidCtrlrId = 0x1f,
  kInvalidSecondaryCtrlrState = 0x20,
  kInvalidNumCtrlrResources = 0x21,
  kInvalidResourceId = 0x22,

  // NVM command set
  kConflictingAttributes = 0x80,
  kInvalidProtectionInfo = 0x81,
  kAttemptedWriteToRoRange = 0x82,
};

// NVMe Base Specification Figure 130 and Figure 131
// https://nvmexpress.org/wp-content/uploads/NVM-Express-1_4-2019.06.10-Ratified.pdf
enum class MediaErrorStatusCode : uint8_t {
  kWriteFaults = 0x80,
  kUnrecoveredReadError = 0x81,
  kGuardCheckError = 0x82,
  kApplicationTagCheckError = 0x83,
  kReferenceTagCheckError = 0x84,
  kCompareFailure = 0x85,
  kAccessDenied = 0x86,
  kDeallocatedOrUnwrittenBlock = 0x87,
};

// NVMe Base Specification Figure 132
// https://nvmexpress.org/wp-content/uploads/NVM-Express-1_4-2019.06.10-Ratified.pdf
enum class PathStatusCode : uint8_t {
  kInternalPathError = 0x00,
  kControllerPathError = 0x60,
  kHostPathError = 0x70,
  kAbortedByHost = 0x71,
};

// NVMe Base Specification Figure 139 and Figure 140
// https://nvmexpress.org/wp-content/uploads/NVM-Express-1_4-2019.06.10-Ratified.pdf
enum class AdminOpcode : uint8_t {
  kDeleteIoSq = 0x00,
  kCreateIoSq = 0x01,
  kGetLogPage = 0x02,
  // Reserved 0x03
  kDeleteIoCq = 0x04,
  kCreateIoCq = 0x05,
  kIdentify = 0x06,
  // Reserved 0x07
  kAbort = 0x08,
  kSetFeatures = 0x09,
  kGetFeatures = 0x0a,
  // Reserved 0x0b
  kAsyncEventRequest = 0x0c,
  kNsManagement = 0x0d,
  // Reserved 0x0e-0x0f
  kFirmwareCommit = 0x10,
  kFirmwareImageDownload = 0x11,
  kDeviceSelfTest = 0x14,
  kNsAttachment = 0x15,
  kKeepAlive = 0x18,
  kDirectiveSend = 0x19,
  kDirectiveReceive = 0x1a,
  kVirtualizationManagement = 0x1c,
  kNvmeMiSend = 0x1d,
  kNvmeMiReceive = 0x1e,
  kDoorbellBufferConfig = 0x7c,
  kFormatNvm = 0x80,
  kSecuritySend = 0x81,
  kSecurityReceive = 0x82,
  kSanitize = 0x84,
  kGetLbaStatus = 0x86,
};

// NVMe Base Specification Figure 346
// https://nvmexpress.org/wp-content/uploads/NVM-Express-1_4-2019.06.10-Ratified.pdf
enum class NvmOpcode : uint8_t {
  kFlush = 0x00,
  kWrite = 0x01,
  kRead = 0x02,
  // Reserved 0x3
  kWriteUncorrectable = 0x04,
  kCompare = 0x05,
  // Reserved 0x06-0x07
  kWriteZeroes = 0x08,
  kDatasetManagement = 0x09,
  kReservationRegister = 0x0d,
  kReservationReport = 0x0e,
  kReservationAcquire = 0x11,
  kReservationRelease = 0x15,
  kPdkNvmeSctGeneric = 0x0,
  kSctCommandSpecific = 0x1,
  kSctMediaError = 0x2,
  kSctPath = 0x3,
  // Reserved 0x4-0x6
  kSctVendorSpecific = 0x7,
};

}  // namespace nvme_defs

#endif
