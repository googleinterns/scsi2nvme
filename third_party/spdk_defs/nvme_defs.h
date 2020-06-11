#ifndef LIB_NVME_DEFS_H
#define LIB_NVME_DEFS_H

#include <cstdint>

#include "absl/base/attributes.h"

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

// NVMe Base Specification Figure 184
// https://nvmexpress.org/wp-content/uploads/NVM-Express-1_4-2019.06.10-Ratified.pdf
enum class FeatureType : uint8_t {
  // Reserved 0x00
  kArbitration = 0x01,
  kPowerManagement = 0x02,
  kLbaRangeType = 0x03,
  kTemperatureThreshold = 0x04,
  kErrorRecovery = 0x05,
  kVolatileWriteCache = 0x06,
  kNumberOfQueues = 0x07,
  kInterruptCoalescing = 0x08,
  kInterruptVectorConfiguration = 0x09,
  kWriteAtomicity = 0x0a,
  kAsyncEventConfiguration = 0x0b,
  kAutonomousPowerStateTransition = 0x0c,
  kHostMemBuffer = 0x0d,
  kTimestamp = 0x0e,
  kKeepAliveTimer = 0x0f,
  kHostControlledThermalManagement = 0x10,
  kNonOperationalPowerStateConfig = 0x11,
  // Reserved 0x12-0x77
  // Nvme-MI features 0x78-0x7f
  kSoftwareProgressMarker = 0x80,
  kHostIdentifier = 0x81,
  kHostReserveMask = 0x82,
  kHostReservePersist = 0x83,
  // command set specific (reserved) 0x84-0xbf
  // vendor specific 0xc0-0xff
};

// NVMe Base Specification Figure 112
// https://nvmexpress.org/wp-content/uploads/NVM-Express-1_4-2019.06.10-Ratified.pdf
enum class SglDescriptorType : uint8_t {
  kDataBlock = 0x0,
  kBitBucket = 0x1,
  kSegment = 0x2,
  kLastSegment = 0x3,
  kKeyedDataBlock = 0x4,
  kTransportDataBlock = 0x5,
  // Reserved 0x6-0xe
  kVendorSpecific = 0xf,
};

// NVMe Base Specification Figure 113
// https://nvmexpress.org/wp-content/uploads/NVM-Express-1_4-2019.06.10-Ratified.pdf
enum class SglDescriptorSubtype : uint8_t {
  kAddress = 0x0,
  kOffset = 0x1,
  kTransport = 0xa,
};

// NVMe Base Specification Figure 114 to Figure 119
// https://nvmexpress.org/wp-content/uploads/NVM-Express-1_4-2019.06.10-Ratified.pdf
struct SglDescriptor {
  uint64_t address : 64;
  union {
    struct {
      uint32_t length : 32;
      uint32_t reserved : 24;
      uint8_t subtype : 4;
      uint8_t type : 4;
    } unkeyed;

    struct {
      uint32_t length : 24;
      uint64_t key : 32;
      uint8_t subtype : 4;
      uint8_t type : 4;
    } keyed;

    struct {
      uint64_t reserved : 56;
      uint8_t subtype : 4;
      uint8_t type : 4;
    } generic;
  };
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(SglDescriptor) == 16);

// NVMe Base Specification Figure 105
// https://nvmexpress.org/wp-content/uploads/NVM-Express-1_4-2019.06.10-Ratified.pdf
struct GenericQueueEntryCmd {
  // dword 0
  uint16_t opc : 8;   // opcode
  uint16_t fuse : 2;  // fused operation
  uint16_t rsvd1 : 4;
  uint16_t psdt : 2;
  uint16_t cid : 16;  // command identifier
  // dword 1
  uint32_t nsid : 32;  // namespace identifier
  // dword 2-3
  uint32_t rsvd2 : 32;
  uint32_t rsvd3 : 32;
  // dword 4-5
  uint64_t mptr : 64;  // metadata pointer
  // dword 6-9: data pointer
  union {
    struct {
      uint64_t prp1 : 64;  // prp entry 1
      uint64_t prp2 : 64;  // prp entry 2
    } prp;
    SglDescriptor sgl_descriptor;
  } dptr;
  // dword 10-15
  uint32_t cdw[6];  // command-specific
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(GenericQueueEntryCmd) == 64);

// NVMe Base Specification Figure 245
// https://nvmexpress.org/wp-content/uploads/NVM-Express-1_4-2019.06.10-Ratified.pdf
struct IdentifyNamespace {
  uint64_t nsze : 64;  // namespace size
  uint64_t ncap : 64;  // namespace capacity
  uint64_t nuse : 64;  // namespace utilization

  struct {
    uint8_t thin_prov : 1;  // thin provisioning
    // NAWUN, NAWUPF, and NACWU are defined for this namespace
    uint8_t ns_atomic_write_unit : 1;
    // Supports Deallocated or Unwritten LBA error for this namespace
    uint8_t dealloc_or_unwritten_err : 1;
    // Non-zero NGUID and EUI64 for namespace are never reused
    uint8_t guid_never_reused : 1;
    uint8_t reserved1 : 4;
  } nsfeat;  // namespace features

  uint8_t nlbaf : 8;  // number of lba formats

  struct {
    uint8_t format : 4;
    uint8_t extended : 1;
    uint8_t reserved2 : 3;
  } flbas;  // formatted lba size

  struct {
    // metadata can be transferred using prp list
    uint8_t extended : 1;
    // metadata can be transferred w/ separate metadata ptr
    uint8_t pointer : 1;
    uint8_t reserved3 : 6;
  } mc;  // metadata capabilities

  struct {
    uint8_t pit1 : 1;      // protection information type 1
    uint8_t pit2 : 1;      // protection information type 2
    uint8_t pit3 : 1;      // protection information type 3
    uint8_t md_start : 1;  // first eight bytes of metadata
    uint8_t md_end : 1;    // last eight bytes of metadata
  } dpc;                   // end-to-end data protection capabilities

  struct {
    uint8_t pit : 3;  // protection information type
    // 1 == protection info transferred at start of metadata
    // 0 == protection info transferred at end of metadata
    uint8_t md_start : 1;
    uint8_t reserved4 : 4;
  } dps;  // end-to-end data protection type settings

  struct {
    uint8_t can_share : 1;
    uint8_t reserved : 7;
  } nmic;  // namespace multi-path I/O and namespace sharing capabilities

  union {
    struct {
      uint8_t persist : 1;           // supports persist through power loss
      uint8_t write_exclusive : 1;   // supports write exclusive
      uint8_t exclusive_access : 1;  // supports exclusive access
      // supports write exclusive - registrants only
      uint8_t write_exclusive_reg_only : 1;
      // supports exclusive access registrants only
      uint8_t exclusive_access_reg_only : 1;
      // supports write exclusive - all registrants
      uint8_t write_exclusive_all_reg : 1;
      // supports exclusive access - all registrants
      uint8_t exclusive_access_all_reg : 1;
      // supports ignore existing key
      uint8_t ignore_existing_key : 1;
    } rescap;
    uint8_t raw : 8;
  } nsrescap;  // reservation capabilities

  struct {
    uint8_t percentage_remaining : 7;
    uint8_t fpi_supported : 1;
  } fpi;  // format progress indicator

  union {
    uint8_t raw : 8;
    struct {
      // Value read from deallocated blocks
      // 000b = not reported
      // 001b = all bytes 0x00
      // 010b = all bytes 0xFF
      // \ref spdk_nvme_dealloc_logical_block_read_value
      uint8_t read_value : 3;

      // Supports Deallocate bit in Write
      // Zeroes Guard field behavior for
      // deallocated logical blocks 0:
      // contains 0xFFFF 1: contains CRC for read value
      uint8_t write_zero_deallocate : 1;

      uint8_t guard_value : 1;
      uint8_t reserved : 3;
    } bits;
  } dlfeat;  // deallocate logical features

  uint16_t nawun : 16;     // namespace atomic write unit normal
  uint16_t nawupf : 16;    // namespace atomic write unit power fail
  uint16_t nacwu : 16;     // namespace atomic compare & write unit
  uint16_t nabsn : 16;     // namespace atomic boundary size normal
  uint16_t nabo : 16;      // namespace atomic boundary offset
  uint16_t nabspf : 16;    // namespace atomic boundary size power fail
  uint16_t noiob : 16;     // namespace optimal I/O boundary in logical blocks
  uint64_t nvmcap[2];      // NVM capacity
  uint8_t reserved64[40];  // includes fields added in NVMe Revision 1.4
  uint8_t nguid[16];       // namespace globally unique identifier
  uint64_t eui64 : 64;     // IEEE extended unique identifier

  struct {
    uint32_t ms : 16;    // metadata size
    uint32_t lbads : 8;  // lba data size
    uint32_t rp : 2;     // relative performance
    uint32_t reserved6 : 6;
  } lbaf[16];  // lba format support

  uint8_t reserved6[192];
  uint8_t vendor_specific[3712];
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(IdentifyNamespace) == 4096);

}  // namespace nvme_defs

#endif
