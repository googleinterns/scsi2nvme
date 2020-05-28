#ifndef LIB_SCSI_DEFS_H
#define LIB_SCSI_DEFS_H

#include <cstdint>

#include "absl/base/attributes.h"

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
  struct ControlByte {
    uint8_t obsolete : 2;
    bool naca : 1;
    uint8_t reserved : 3;
    uint8_t vendor_specific : 2;
  };

  // SCSI Reference Manual Table 76 https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
  struct PersistentReserveInCommand {
    OpCode op_code = OpCode::kPersistentReserveIn;
    uint8_t reserved_1 : 3;
    uint8_t service_action : 5;
    uint64_t reserved_2 : 40;
    uint16_t allocation_length : 16;
    ControlByte control_byte;
  } ABSL_ATTRIBUTE_PACKED;
  static_assert(sizeof(PersistentReserveInCommand) == 10);

  // Persistent Reserve In Read Reservation Data No Reservation
  // SCSI Reference Manual Table 79 https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
  struct PriReadReservationDataNoReservation {
    uint32_t prgeneration : 32;
    uint32_t additional_length : 32;
  } ABSL_ATTRIBUTE_PACKED;
  static_assert(sizeof(PriReadReservationDataNoReservation) == 8);

  // Persistent Reserve In Read Reservation Data With Reservation
  // SCSI Reference Manual Table 80 https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
  struct PriReadReservationDataWithReservation {
    PriReadReservationDataNoReservation priDataNoReservation;
    uint64_t reservation_key : 64;
    uint32_t obsolete_1 : 32;
    uint8_t reserved : 8;
    uint8_t scope : 4;
    uint8_t type : 4;
    uint16_t obsolete_2 : 16;
  } ABSL_ATTRIBUTE_PACKED;
  static_assert(sizeof(PriReadReservationDataWithReservation) == 24);

  // SCSI Reference Manual Table 88 https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
  struct PersistentReserveOutCommand {
    OpCode op_code = OpCode::kPersistentReserveOut;
    uint8_t reserved_1 : 3;
    uint8_t service_action : 5;
    uint8_t scope : 4;
    uint8_t type : 4;
    uint16_t reserved_2 : 16;
    uint32_t parameter_list_length : 32;
    ControlByte control_byte;
  } ABSL_ATTRIBUTE_PACKED;
  static_assert(sizeof(PersistentReserveOutCommand) == 10);

  // Persistent Reserve Out Parameter List
  // Used by Persistent Reserve Out command for any service action other than Register and Move
  // SCSI Reference Manual Table 90 https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
  struct ProParamList {
    uint64_t reservation_key : 64;
    uint64_t service_action_reservation_key : 64;
    uint32_t obsolete_1 : 32;
    uint8_t reserved_1 : 4;
    bool spc_i_pt : 1; // Specify Initiator Ports bit
    bool all_tg_pt : 1; // All Target Ports bit
    bool reserved : 1;
    bool aptpl : 1; // Activate Persist Through Power Loss bit
    uint8_t reserved_2 : 8;
    uint16_t obsolete_2 : 16;
    // additional parameter data
  } ABSL_ATTRIBUTE_PACKED;
  static_assert(sizeof(ProParamList) == 24);

} // namepsace scsi_defs
#endif
