#ifndef LIB_SCSI_DEFS_H
#define LIB_SCSI_DEFS_H

#include <cstdint>

#include "absl/base/attributes.h"

// The fields of structures in this namespace are arranged according to Big Endian format.
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
    kUnmap = 0x42,
    kReadToc = 0x43,
    kModeSense10 = 0x5a,
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

  // Refer to https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf , Section 3.6 Table 58
  struct InquiryCommand {
    OpCode op_code = OpCode::kInquiry;
    uint8_t reserved : 6;
    bool obsolete : 1; // formerly CMDDT
    bool evpd : 1; // Enable Vital Product Data (EVPD)
    uint8_t page_code : 8;
    uint16_t allocation_length : 16;
    ControlByte control_byte;
  } ABSL_ATTRIBUTE_PACKED;
  static_assert(sizeof(InquiryCommand) == 6);

  // Refer to https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf , Section 3.6.2 Table 60
  enum class PeripheralQualifier : uint8_t {
    kPeripheralDeviceConnected = 0b000,
    kPeripheralDeviceNotConnected = 0b001,
    kReserved = 0b010,
    kNotSupported = 0b011
  };

  // Refer to https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf , Section 3.6.2 Table 63
  enum class TPGS : uint8_t {
    kNotSupported = 0b00,
    kImplicitAccess = 0b01,
    kExcplicitAccess = 0b10,
    kFullAccess = 0b11,
  };

  enum class ResponseDataFormat : uint8_t {
    kObsolete0 = 0x0,
    kObsolete1 = 0x1,
    kCompliant = 0x2,
  };

  // Refer to https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf , Section 3.6.2 Table 59
  struct InquiryData {
    PeripheralQualifier peripheral_qualifier : 3;
    PeripheralDeviceType peripheral_device_type : 5;
    bool rmb : 1; // Removable Media Bit (RMB)
    bool lu_cong : 1;
    uint8_t reserved_1 : 6;
    Version version : 8;
    uint8_t reserved_2 : 2;
    bool normaca : 1; //Normal ACA (NORMACA)
    bool hisup : 1; // Hierarchical Support Bit (HISUP)
    ResponseDataFormat response_data_format : 4;
    uint8_t additional_length : 8;
    bool sccs : 1; // SCC Supported
    bool acc : 1; // Access controls Coordinator Bit
    TPGS tpgs : 2; // Target Port Group Support (TPGS)
    bool third_party_copy : 1; // referred to as 3PC in the documentation
    uint8_t reserved_3 : 2;
    bool protect : 1;
    bool obsolete_1 : 1;
    bool encserv : 1; // Enclosure Services Bit
    bool vs_1 : 1; // vendor specific bit
    bool multip : 1; // multiple SCSI Port
    bool obsolete_2 : 1;
    uint8_t reserved_4 : 2;
    bool addr_16 : 1; // SCSI 16-bit address support bit
    bool obsolete_3 : 1;
    bool reserved_5 : 1;
    bool wbus_16 : 1; // Wide Bus bit 
    bool sync : 1;
    bool obsolete_4 : 1;
    bool reserved_6 : 1;
    bool cmdque : 1; // Command Management Model bit
    bool vs_2 : 1; // vendor specific bit
    uint64_t vendor_identification : 64;
    uint8_t product_identification[16];
    uint32_t product_revision_level : 32;
    uint8_t vendor_specific_1[20];
    uint8_t reserved_7 : 4;
    uint8_t clocking : 2;
    bool qas : 1; // Quick Arbitration and Selection Supported bit
    bool ius : 1; // Information Units Supported bit
    uint8_t reserved_8 : 8;
    uint16_t vendor_descriptors[8];
    uint8_t reserved_9[22];
  } ABSL_ATTRIBUTE_PACKED; 
  static_assert(sizeof(InquiryData) == 96);

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

  // SCSI Reference Manual Table 73 https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
  struct ModeSense6Command {
    OpCode opCode = OpCode::kModeSense6;
    uint8_t reserved_1 : 4;
    bool dbd : 1; // Disable block descriptors
    uint8_t reserved_2 : 3;
    uint8_t pc : 2; // Page control
    uint8_t page_code : 6;
    uint8_t sub_page_code : 8;
    uint8_t alloc_length : 8;
    ControlByte control_byte;
  } ABSL_ATTRIBUTE_PACKED;
  static_assert(sizeof(ModeSense6Command) == 6);

  // SCSI Reference Manual Table 75 https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
  struct ModeSense10Command {
    OpCode opCode = OpCode::kModeSense10;
    uint8_t reserved_1 : 3;
    bool llbaa : 1; // Long LBA accepted
    bool dbd : 1; // Disable block descriptors
    uint8_t reserved_2 : 3;
    uint8_t pc : 2; // Page control
    uint8_t page_code : 6;
    uint8_t sub_page_code : 8;
    uint32_t reserved_3 : 24;
    uint16_t alloc_length : 16;
    ControlByte control_byte;
  } ABSL_ATTRIBUTE_PACKED;
  static_assert(sizeof(ModeSense10Command) == 10);

  // SCSI Reference Manual Table 204 https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
  struct UnmapCommand {
    OpCode op_code = OpCode::kUnmap;
    uint8_t reserved_1 : 7;
    bool anchor : 1;
    uint32_t reserved_2 : 32;
    uint8_t reserved_3 : 3;
    uint8_t group_number : 5;
    uint16_t param_list_length : 16;
    ControlByte control_byte;
  } ABSL_ATTRIBUTE_PACKED;
  static_assert(sizeof(UnmapCommand) == 10);

  // SCSI Reference Manual Table 205 https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
  // This struct is a header for variable length data
  struct UnmapParamList {
    uint16_t data_length : 16;
    uint16_t block_desc_data_length : 16;
    uint32_t reserved_1 : 32;
  } ABSL_ATTRIBUTE_PACKED;
  static_assert(sizeof(UnmapParamList) == 8);

    // SCSI Reference Manual Table 206 https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
  struct UnmapBlockDescriptor {
    uint64_t logical_block_addr : 64;
    uint32_t logical_block_count : 32;
    uint32_t reserved_1 : 32;
  } ABSL_ATTRIBUTE_PACKED;
  static_assert(sizeof(UnmapBlockDescriptor) == 16);

} // namepsace scsi_defs
#endif
