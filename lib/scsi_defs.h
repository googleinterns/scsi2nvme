#ifndef LIB_SCSI_DEFS_H
#define LIB_SCSI_DEFS_H

#include <cstdint>

namespace scsi_defs {

  // SAM-4 Table 33 
  enum class Status : uint8_t {
    kGood = 0x0,
    kCheckCondition = 0x2,
    kConditionMet = 0x4,
    kBusy = 0x8,
    kIntermediate = 0x10,              // obsolete
    kIntermediateConditionMet = 0x14,  // obsolete
    kReservationConflict = 0x18,
    kCommandTerminated = 0x22,         // obsolete
    kTaskSetFull = 0x28,
    kAcaActive = 0x30,
    kTaskAborted = 0x40,
  };

  // SCSI Reference Manual Table 61 https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
  enum class PeripheralDeviceType : uint8_t {
    kDirectAccessBlock = 0x0,
    kSequentialAccess = 0x1,
    kPrinter = 0x2,
    kProcessor = 0x3,
    kWriteOnce = 0x4,
    kCdDvd = 0x5,
    kOpticalMemory = 0x7,
    kMediumChanger = 0x8,
    kStorageArrayController = 0xc,
    kEnclosureServices = 0xd,
    kSimplifiedDirectAccess = 0xe,
    kOpticalCardReaderWriter = 0xf,
    kBridgeControllerCommands = 0x10,
    kObjectBasedStorage = 0x11,
    kAutomationDriveInterface = 0x12,
    kWellKnownLogicalUnit = 0x1e,
    kUnknown = 0x1f,
  };

  // SCSI Reference Manual Table 62 https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
  // Field code of implemented version of the SPC standard 
  enum class Version : uint8_t {
    kNoStandard = 0x0,
    kSpc = 0x3,
    kSpc2 = 0x4,
    kSpc3 = 0x5,
    kSpc4 = 0x6,
    kSpc5 = 0x7,
  };

  // SCSI Reference Manual https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
  // Operation codes defined for SCSI commands supported by this project; this list may increase
  enum class OpCode : uint8_t {
    kTestUnitReady = 0x0,
    kRequestSense = 0x3,
    kInquiry = 0x12,
    kReserve6 = 0x16,
    kRelease6 = 0x17,
    kModeSense6 = 0x1a,
    kStartStopUnit = 0x1b,
    kDoPreventAllowMediumRemoval = 0x1e,
    kReadCapacity10 = 0x25,
    kReadToc = 0x43,
    kPersistentReserveIn = 0x5e,
    kPersistentReserveOut = 0x5f,
    kServiceActionIn = 0x9e,
    kReportLuns = 0xa0,
    kMaintenanceIn = 0xa3,
  };

  // SCSI Reference Manual Table 10 https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
  typedef struct ControlByte {
    uint8_t obsolete : 2;
    bool naca : 1;
    uint8_t reserved : 3;
    uint8_t vendorSpecific : 2;
  } ControlByte;

} // namepsace scsi_defs
#endif
