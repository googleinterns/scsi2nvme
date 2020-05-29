#ifndef LIB_NVME_DEFS_H
#define LIB_NVME_DEFS_H

namespace nvme_defs {

  // NVMe Base Specification Figure 139 and Figure 140 https://nvmexpress.org/wp-content/uploads/NVM-Express-1_4-2019.06.10-Ratified.pdf
  enum class AdminCommandOpCode : uint8_t {
    kDeleteIOSubmissionQueue = 0x0,
    kCreateIOSubmissionQueue = 0x1,
    kGetLogPage = 0x2,
    kDeleteIOCompletionQueue = 0x4,
    kCreateIOCompletionQueue = 0x5,
    kIdentify = 0x6,
    kAbort = 0x8,
    kSetFeatures = 0x9,
    kGetFeatures = 0xa,
    kAsynchronousEvenRequests = 0xc,
    kNamespaceManagement = 0xd,
    kFirwareCommit = 0x10,
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
    // IO Command Set specific 0x80 to 0xbf
    // Vendor specific = 0xc0 to 0xff
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
    // Vendor specific = 0x80 to 0xff
  };

} // namespace nvme_defs

#endif
