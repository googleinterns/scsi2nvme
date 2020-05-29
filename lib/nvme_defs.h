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
  enum class GenericCommandStatus {
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
    kFirmwareActiationRequiresMaximumTimeViolation = 0x12,
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

} // namespace nvme_defs

#endif
