#ifndef LIB_NVME_DEFS_H
#define LIB_NVME_DEFS_H

namespace nvme_defs {

// NVMe Base Specification Figure 125 https://nvmexpress.org/wp-content/uploads/NVM-Express-1_4-2019.06.10-Ratified.pdf
enum class StatusType : uint8_t {
  kGenericCommandStatus = 0x0,
  kCommandSpecificStatus = 0x1,
  kMediaAndDataIntegrityErrors = 0x2,
  kPathRelatedStatus = 0x3,
  // Reserved = 0x4 to 0x6
  // Vendor Specific = 0x7
};

// NVMe Base Specification Figure 126 and Figure 127 https://nvmexpress.org/wp-content/uploads/NVM-Express-1_4-2019.06.10-Ratified.pdf
enum class GenericCommandStatus : uint8_t {
  kSuccessfulCompletion = 0x0,
  kInvalidCommandOpCode = 0x1,
  kInvalidFieldInCommand = 0x2,
  kCommandIdConflict = 0x3,
  kDataTransferError = 0x4,
  kCommandsAbortedDueToPowerLossNotification = 0x5,
  kInternalError = 0x6,
  kCommandAbortRequested = 0x7,
  kCommandAbortedDueToSqDeletion = 0x8,
  kCommandAbortedDueToFailedFusedCommand = 0x9,
  kCommandAbortedDueToMissingFusedCommand = 0xa,
  kInvalidNamespaceOrFormat = 0xb,
  kCommandSequenceError = 0xc,
  kInvalidSglSegmentDescriptor = 0xd,
  kInvalidNumberOfSglDescriptors = 0xe,
  kDataSglLengthInvalid = 0xf,
  kMetadataSglLengthInvalid = 0x10,
  kSglDescriptorTypeInvalid = 0x11,
  kInvalidUseOfControllerMemoryBuffer = 0x12,
  kPrpOffsetInvalid = 0x13,
  kAtomicWriteUnitExceeded = 0x14,
  kOperationDenied = 0x15,
  kSglOffsetInvalid = 0x16,
  // Reserved = 0x17
  kHostIdentifierInconsistentFormat = 0x18,
  kKeepAliveTimeoutInvalid = 0x1a,
  kCommandAbortedDueToPreemptAndAbort = 0x1b,
  kSanitizeFailed = 0x1c,
  kSanitizeInProgress = 0x1d;
  kSglDataBlockGranularityInvalid = 0x1e,
  kCommandNotSupportedForQueueInCmb = 0x1f,
  kNamespaceIsWriteProtected = 0x20,
  kCommandInterrupted = 0x21,
  kTransientTransportError = 0x22,
  // Reserved = 0x23 to 0x7f
  // I/O Command Set Specific = 0x80 t0 0xbf
  // Vendor Specific = 0xc0 to 0xff

  // NVM Command Set
  kLbaOutOfRange = 0x80,
  kCapacityExceeded = 0x81,
  kNamespaceNotReady = 0x82,
  kReservationConflict = 0x83,
  kFormatInProgress = 0x84,
  // Reserved = 0x85 yo 0xbf
};


// NVMe Base Specification Figure 128 and Figure 129 https://nvmexpress.org/wp-content/uploads/NVM-Express-1_4-2019.06.10-Ratified.pdf
enum class CommandSpecificStatus : uint8_t {
  kCompletionQueueInvalid = 0x0,
  kInvalidQueueIdentifier = 0x1,
  kInvalidQueueSize = 0x2,
  kAbortCommandLimitExceeded = 0x3,
  // Reserved = 0x4
  kAsynchronousEventRequestLimitExceeded = 0x5,
  kInvalidFirmwareSlot = 0x6,
  kInvalidFirmwareImage = 0x7,
  kInvalidInterruptVector = 0x8,
  kInvalidLogPage = 0x9,
  kInvalidFormat = 0xa,
  kFirmwareActivationRequiresConventionalReset = 0xb,
  kInvalidQueueDeletion = 0xc,
  kFeatureIdentifierNotSaveable = 0xd,
  kFeatureNotChangeable = 0xe,
  kFeatureNotNamespaceSpecific = 0xf,
  kFirmwareActivationRequiresNvmSubsystemReset = 0x10,
  kFirmwareActivationRequiresControllerLevelReset = 0x11,
  kFirmwareActivationRequiresMaximumTimeViolation = 0x12,
  kFirmwareActivationProhibited = 0x13,
  kOverlappingRange = 0x14,
  kNamespaceInsufficientCapacity = 0x15,
  kNamespaceIdentifierUnavailable = 0x16,
  // Reserved = 0x17
  kNamespaceAlreadyAttached = 0x18,
  kNamespaceIsPrivate = 0x19,
  kNamespaceNotAttached = 0x1a,
  kThinProvisioningNotSupported = 0x1b,
  kControllerListInvalid = 0x1c,
  kDeviceSelfTestInProgress = 0x1d,
  kBootPartitionWriteProhibited = 0x1e,
  kInvalidControllerIdentifier = 0x1f,
  kInvalidSecondaryControllerState = 0x20,
  kInvalidNumberOfControllerResources = 0x21,
  kInvalidResourceIdentifier = 0x22,
  kSanitizeProhibitedWhilePersistentMemoryRegionIsEnabled = 0x23,
  kAnaGroupIdentifierInvalid = 0x24,
  kAnaAttachFailed = 0x25,
  // Reserved = 0x26 to 0x6f
  // Directive Specific = 0x70 to 0x7f
  // I/O Command Set Specific = 0x80 to 0xbf
  // Vendor Specific = 0xc0 to 0xff

  // NVM Command Set
  kConflictingAttributes = 0x80,
  kInvalidProtectionInformation = 0x81,
  kAttemptedWriteToReadOnlyRange = 0x82,
};

// NVMe Base Specification Figure 130 and Figure 131 https://nvmexpress.org/wp-content/uploads/NVM-Express-1_4-2019.06.10-Ratified.pdf
enum class MediaAndDataIntegrityErrorValues : uint8_t {
  // Reserved = 0x0 to 0x7f
  // I/O Command Set Specific = 0x80 to 0xbf
  // Vendor Specific = 0xc0 to 0xff

  // NVM Command Set
  kWriteFault = 0x80,
  kUnrecoveredReadError = 0x81,
  kEndToEndGuardCheckError = 0x82,
  kEndToEndApplicationTagCheckError = 0x83,
  kEndToEndReferenceTagCheckError = 0x84,
  kCompareFailure = 0x85,
  kAccessDenied = 0x86,
  kDeallocatedOrUnwrittenLogicalBlock = 0x87,
};

// NVMe Base Specification Figure 132 https://nvmexpress.org/wp-content/uploads/NVM-Express-1_4-2019.06.10-Ratified.pdf
enum class PathRelatedStatus : uint8_t {
  kInternalPathError = 0x0,
  kAsymmetricAccessPersistentLoss = 0x1,
  kAsymmetricAccessInaccessible = 0x2,
  kAsymmetricAccessTransition = 0x3,
  // Reserved = 0x4 to 0x5

  // Controller detected Pathing errors
  kControllerPathingError = 0x60,
  // Reserved = 0x61 to 0x6f

  // Host detected Pathing errors
  kHostPathingError = 0x70,
  kCommandAbortedByHost = 0x71,
  // Reserved = 0x72 to 0x7f

  // Other Pathing errors
  // I/O Command Set Specific = 0x80 to 0xbf
  // Vendor Specific = 0xc0 to 0xff
}

// NVMe Base Specification Figure 139 and Figure 140 https://nvmexpress.org/wp-content/uploads/NVM-Express-1_4-2019.06.10-Ratified.pdf
enum class AdminCommandOpCode : uint8_t {
  kDeleteIoSubmissionQueue = 0x0,
  kCreateIoSubmissionQueue = 0x1,
  kGetLogPage = 0x2,
  kDeleteIoCompletionQueue = 0x4,
  kCreateIoCompletionQueue = 0x5,
  kIdentify = 0x6,
  kAbort = 0x8,
  kSetFeatures = 0x9,
  kGetFeatures = 0xa,
  kAsynchronousEventRequests = 0xc,
  kNamespaceManagement = 0xd,
  kFirmwareCommit = 0x10,
  kFirmwareImageDownload = 0x11,
  kDeviceSelfTest = 0x14,
  kNamespaceAttachment = 0x15,
  kKeepAlive = 0x18,
  kDirectiveSend = 0x19,
  kDirectiveReceive = 0x1a,
  kVirtualizationManagement = 0x1c,
  kNvmeMiSend = 0x1d,
  kNvmeMiReceive = 0x1e,
  kDoorbellBufferConfig = 0x7c,
  // I/O Command Set specific = 0x80 to 0xbf
  // Vendor Specific = 0xc0 to 0xff

  // NVM Command Set Specific
  kFormatNvm 0x80,
  kSecuritySend = 0x81,
  kSecurityReceive = 0x82,
  kSanitize = 0x84,
  kGetLbaStatus = 0x86,
};

// NVMe Base Specification Figure 346 https://nvmexpress.org/wp-content/uploads/NVM-Express-1_4-2019.06.10-Ratified.pdf
enum class NvmCommandOpCode : uint8_t {
  kFlush = 0x0,
  kWrite = 0x1,
  kRead = 0x2,
  kWriteUncorrectable = 0x4,
  kCompare = 0x5,
  kWriteZeroes = 0x8,
  kDatasetManagement = 0x9,
  kVerify = 0xc,
  kReservationRegister = 0xd,
  kReservationReport = 0xe,
  kReservationAcquire = 0x11,
  kReservationRelease = 0x15,
  // Vendor Specific = 0x80 to 0xff
};
enum class StatusCodeType : uint8_t {
  kSctGeneric = 0x0,
  kSctCommandSpecific = 0x1,
  kSctMediaError 0x2,
  kSctPath = 0x3,
  /* 0x4-0x6 - reserved */
  kSctVendorSpecific = 0x7,
};

/**
*  * Generic command status codes
*   */
enum class GenericCommandStatusCode : uint8_t {
  kScSuccess = 0x00,
  kScInvalidOpcode = 0x01,
  kScInvalidField = 0x02,
  kScCommandIdConflict = 0x03,
  kScDataTransferError = 0x04,
  kScAbortedPowerLoss = 0x05,
  kScInternalDeviceError = 0x06,
  kScAbortedByRequest = 0x07,
  kScAbortedSqDeletion = 0x08,
  kScAbortedFailedFused = 0x09,
  kScAbortedMissingFused = 0x0a,
  kScInvalidNamespaceOrFormat = 0x0b,
  kScCommandSequenceError = 0x0c,
  kScInvalidSglSegDescriptor = 0x0d,
  kScInvalidNumSglDescirptors = 0x0e,
  kScDataSglLengthInvalid = 0x0f,
  kScMetadataSglLengthInvalid = 0x10,
  kScSglDescriptorTypeInvalid = 0x11,
  kScInvalidControllerMemBuf = 0x12,
  kScInvalidPrpOffset = 0x13,
  kScAtomicWriteUnitExceeded = 0x14,
  kScOperationDenied = 0x15,
  kScInvalidSglOffset = 0x16,
  /* 0x17 - reserved */
  kScHostidInconsistentFormat = 0x18,
  kScKeepAliveExpired = 0x19,
  kScKeepAliveInvalid = 0x1a,
  kScAbortedPreempt = 0x1b,
  kScSanitizeFailed = 0x1c,
  kScSanitizeInProgress = 0x1d,
  kScSglDataBlockGranularityInvalid = 0x1e,
  kScCommandInvalidInCmb = 0x1f,
  kScLbaOutOfRange = 0x80,
  kScCapacityExceeded = 0x81,
  kScNamespaceNotReady = 0x82,
  kScReservationConflict = 0x83,
  kScFormatInProgress = 0x84,
};

/**
*  * Command specific status codes
*   */
enum class CommandSpecificStatusCode : uint8_t {
  kScCompletionQueueInvalid = 0x00,
  kScInvalidQueueIdentifier = 0x01,
  kScInvalidQueueSize = 0x02,
  kScAbortCommandLimitExceeded = 0x03,
  /* 0x04 - reserved */
  kScAsyncEventRequestLimitExceeded = 0x05,
  kScInvalidFirmwareSlot = 0x06,
  kScInvalidFirmwareImage = 0x07,
  kScInvalidInterruptVector = 0x08,
  kScInvalidLogPage = 0x09,
  kScInvalidFormat = 0x0a,
  kScFirmwareReqConventionalReset = 0x0b,
  kScInvalidQueueDeletion = 0x0c,
  kScFeatureIdNotSaveable = 0x0d,
  kScFeatureNotChangeable = 0x0e,
  kScFeatureNotNamespaceSpecific = 0x0f,
  kScFirmwareReqNvmReset = 0x10,
  kScFirmwareReqReset = 0x11,
  kScFirmwareReqMaxTimeViolation = 0x12,
  kScFirmwareActivationProhibited = 0x13,
  kScOverlappingRange = 0x14,
  kScNamespaceInsufficientCapacity = 0x15,
  kScNamespaceIdUnavailable = 0x16,
  /* 0x17 - reserved */
  kScNamespaceAlreadyAttached = 0x18,
  kScNamespaceIsPrivate = 0x19,
  kScNamespaceNotAttached = 0x1a,
  kScThinprovisioningNotSupported = 0x1b,
  kScControllerListInvalid = 0x1c,
  kScDeviceSelfTestInProgress 0x1d,
  kScBootPartitionWriteProhibited = 0x1e,
  kScInvalidCtrlrId = 0x1f,
  kScInvalidSecondaryCtrlrState = 0x20,
  kScInvalidNumCtrlrResources = 0x21,
  kScInvalidResourceId = 0x22,
  kScConflictingAttributes = 0x80,
  kScInvalidProtectionInfo = 0x81,
  kScAttemptedWriteToRoRange = 0x82,
};

/**
*  * Media error status codes
*   */
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

/**
*  * Path related status codes
*   */
enum class PathStatusCode : uint8_t {
  kInternalPathError = 0x00,
  kControllerPathError = 0x60,
  kHostPathError = 0x70,
  kAbortedByHost = 0x71,
};


/**
*  * Admin opcodes
*   */
enum class AdminOpcode : uint8_t {
  kDeleteIoSq = 0x00,
  kCreateIoSq = 0x01,
  kGetLogPage = 0x02,
  /* 0x03 - reserved */
  kDeleteIoCq = 0x04,
  kCreateIoCq = 0x05,
  kIdentify = 0x06,
  /* 0x07 - reserved */
  kAbort = 0x08,
  kSetFeatures = 0x09,
  kGetFeatures = 0x0a,
  /* 0x0b - reserved */
  kAsyncEventRequest = 0x0c,
  kNsManagement = 0x0d,
  /* 0x0e-0x0f - reserved */
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

/**
*  * Nvm command set opcodes
*   */
enum class NvmOpcode : uint8_t {
  kFlush = 0x00,
  kWrite = 0x01,
  kRead = 0x02,
  /* 0x03 - reserved */
  kWriteUncorrectable = 0x04,
  kCompare = 0x05,
  /* 0x06-0x07 - reserved */
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
  /* 0x4-0x6 - reserved */
  kSctVendorSpecific = 0x7,
};

/**
*  * Generic command status codes
*   */
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
  /* 0x17 - reserved */
  kHostidInconsistentFormat = 0x18,
  kKeepAliveExpired = 0x19,
  kKeepAliveInvalid = 0x1a,
  kAbortedPreempt = 0x1b,
  kSanitizeFailed = 0x1c,
  kSanitizeInProgress = 0x1d,
  kSglDataBlockGranularityInvalid = 0x1e,
  kCommandInvalidInCmb = 0x1f,

  kLbaOutOfRange = 0x80,
  kCapacityExceeded = 0x81,
  kNamespaceNotReady = 0x82,
  kReservationConflict = 0x83,
  kFormatInProgress = 0x84,
};

/**
*  * Command specific status codes
*   */
enum class CommandSpecificStatusCode : uint8_t {
  kScCompletionQueueInvalid = 0x00,
  kScInvalidQueueIdentifier = 0x01,
  kScInvalidQueueSize = 0x02,
  kScAbortCommandLimitExceeded = 0x03,
  /* 0x04 - reserved */
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
  kFirmwareReqMaxTimeViolation    = 0x12,
  kFirmwareActivationProhibited     = 0x13,
  kOverlappingRange                  = 0x14,
  kNamespaceInsufficientCapacity    = 0x15,
  kNamespaceIdUnavailable           = 0x16,
                            /* 0x17 - reserved */
  kNamespaceAlreadyAttached         = 0x18,
  kNamespaceIsPrivate               = 0x19,
  kNamespaceNotAttached             = 0x1a,
  kThinprovisioningNotSupported     = 0x1b,
  kControllerListInvalid            = 0x1c,
  kDeviceSelfTestInProgress = 0x1d,
  kBootPartitionWriteProhibited  = 0x1e,
  kInvalidCtrlrId     = 0x1f,
  kInvalidSecondaryCtrlrState  = 0x20,
  kInvalidNumCtrlrResources  = 0x21,
  kInvalidResourceId    = 0x22,
  kConflictingAttributes   = 0x80,
  kInvalidProtectionInfo    = 0x81,
  kAttemptedWriteToRoRange  = 0x82,
};

/**
*  * Media error status codes
*   */
enum class MediaErrorStatusCode : uint8_t {
  kWriteFaults     = 0x80,
  kUnrecoveredReadError   = 0x81,
  kGuardCheckError      = 0x82,
  kApplicationTagCheckError  = 0x83,
  kReferenceTagCheckError    = 0x84,
  kCompareFailure      = 0x85,
  kAccessDenied      = 0x86,
  kDeallocatedOrUnwrittenBlock     = 0x87,
};

/**
*  * Path related status codes
*   */
enum class PathStatusCode : uint8_t {
  kInternalPathError = 0x00,

  kControllerPathError = 0x60,

  kHostPathError = 0x70,
  kAbortedByHost = 0x71,
}
//Persistent Reserve In Read Reservation Data No Reservation;

#define MaxOpc 0xff

/**
*  * Admin opcodes
*   */
//Persistent Reserve In Read Reservation Data With Reservation;
enum class AdminOpcode : uint8_t {
  kDeleteIoSq = 0x00,
  kCreateIoSq = 0x01,
  kGetLogPage = 0x02,
  /* 0x03 - reserved */
  kDeleteIoCq = 0x04,
  kCreateIoCq = 0x05,
  kIdentify = 0x06,
  /* 0x07 - reserved */
  kAbort = 0x08,
  kSetFeatures = 0x09,
  kGetFeatures = 0x0a,
  /* 0x0b - reserved */
  kAsyncEventRequest = 0x0c,
  kNsManagement = 0x0d,
  /* 0x0e-0x0f - reserved */
  kFirmwareCommit = 0x10,
  kFirmwareImageDownload = 0x11
  kDeviceSelfTest = 0x14,
  kNsAttachment = 0x15,

  kKeepAlive = 0x18,
  kDirectiveSend =
      //Persistent Reserve Out Parameter List0x19,
  kDirectiveReceive = 0x1a,
  kVirtualization Management = 0x1c,
  kNvmeMiSend = 0x1d,
  kNvmeMiReceive = 0x1e,

  kDoorbellBufferConfig = 0x7c,

  kFormatNvm = 0x80,
  kSecuritySend = 0x81,
  kSecurityReceive = 0x82,

  kSanitize = 0x84,
  kGetLbaStatus = 0x86,
};

/**
*  * Nvm command set opcodes
*   */
enum class NvmOpcode : uint8_t {
  kFlush = 0x00,
  kWrite = 0x01,
  kRead = 0x02,
  /* 0x03 - reserved */
  kWriteUncorrectable = 0x04,
  kCompare = 0x05,
  /* 0x06-0x07 - reserved */
  kWriteZeroes = 0x08,
  kDatasetManagement = 0x09,

  kReservationRegister = 0x0d,
  kReservationReport = 0x0e,

  kReservationAcquire = 0x11,
  kReservationRelease = 0x15,
};

} // namespace NvmeDefs
