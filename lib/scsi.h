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

#ifndef LIB_SCSI_H
#define LIB_SCSI_H

#include <cstdint>

#include "absl/base/attributes.h"

// The fields of structures in this namespace are arranged according to Big
// Endian format.
namespace scsi {

using LunAddress = uint64_t;

// SAM-4 Table 33
enum class Status : uint8_t {
  kGood = 0x0,
  kCheckCondition = 0x2,
  kConditionMet = 0x4,
  kBusy = 0x8,
  kIntermediate = 0x10,              // obsolete
  kIntermediateConditionMet = 0x14,  // obsolete
  kReservationConflict = 0x18,
  kCommandTerminated = 0x22,  // obsolete
  kTaskSetFull = 0x28,
  kAcaActive = 0x30,
  kTaskAborted = 0x40,
};

// SCSI Reference Manual Table 11
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
enum class SenseResponse : uint8_t {
  kCurrentFixedError = 0x70,
  kDeferredFixedError = 0x71,
  kCurrentDescriptorError = 0x72,
  kDeferredDescriptorError = 0x73,
};

// SCSI Reference Manual Table 28
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
enum class SenseKey : uint8_t {
  kNoSense = 0x0,
  kRecoveredError = 0x1,
  kNotReady = 0x2,
  kMediumError = 0x3,
  kHardwareError = 0x4,
  kIllegalRequest = 0x5,
  kUnitAttention = 0x6,
  kDataProtect = 0x7,
  kBlankCheck = 0x8,
  kVendorSpecific = 0x9,
  kCopyAborted = 0xa,
  kAbortedCommand = 0xb,
  kReserved = 0xc,
  kVolumeOverflow = 0xd,
  kMiscompare = 0xe,
  kCompleted = 0xf,
};

// SCSI Reference Manual Table 29
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
enum class AdditionalSenseCode : uint8_t {
  kNoAdditionalSenseInfo = 0x0,
  kPeripheralDeviceWriteFault = 0x03,
  kLogicalUnitNotReadyCauseNotReportable = 0x04,
  kWarningPowerLossExpected = 0x0b,
  kLogicalBlockGuardCheckFailed = 0x10,
  kLogicalBlockApplicationTagCheckFailed = 0x10,
  kLogicalBlockReferenceTagCheckFailed = 0x10,
  kUnrecoveredReadError = 0x11,
  kMiscompareDuringVerifyOp = 0x1d,
  kAccessDeniedInvalidLuIdentifier = 0x20,
  kInvalidCommandOpCode = 0x20,
  kLbaOutOfRange = 0x21,
  kInvalidFieldInCdb = 0x24,
  kFormatCommandFailed = 0x31,
  kInternalTargetFailure = 0x44,
};

// SCSI Reference Manual Table 29
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
// Listed in the same order as the AdditionalSenseCode counterparts
enum class AdditionalSenseCodeQualifier : uint8_t {
  kNoAdditionalSenseInfo = 0x0,
  kPeripheralDeviceWriteFault = 0x0,
  kLogicalUnitNotReadyCauseNotReportable = 0x0,
  kWarningPowerLossExpected = 0x08,
  kLogicalBlockGuardCheckFailed = 0x01,
  kLogicalBlockApplicationTagCheckFailed = 0x02,
  kLogicalBlockReferenceTagCheckFailed = 0x03,
  kUnrecoveredReadError = 0x0,
  kMiscompareDuringVerifyOp = 0x0,
  kAccessDeniedInvalidLuIdentifier = 0x09,
  kInvalidCommandOpCode = 0x0,
  kLbaOutOfRange = 0x0,
  kInvalidFieldInCdb = 0x0,
  kFormatCommandFailed = 0x01,
  kInternalTargetFailure = 0x0,
};

// SCSI Reference Manual Table 61
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
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

// SCSI Reference Manual Table 62
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
// Field code of implemented version of the SPC standard
enum class Version : uint8_t {
  kNoStandard = 0x0,
  kSpc = 0x3,
  kSpc2 = 0x4,
  kSpc3 = 0x5,
  kSpc4 = 0x6,
  kSpc5 = 0x7,
};

// SCSI Reference Manual
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
// Operation codes defined for SCSI commands supported by this project; this
// list may increase
enum class OpCode : uint8_t {
  kTestUnitReady = 0x0,
  kRequestSense = 0x3,
  kRead6 = 0x08,
  kWrite6 = 0x0a,
  kInquiry = 0x12,
  kReserve6 = 0x16,
  kRelease6 = 0x17,
  kModeSense6 = 0x1a,
  kStartStopUnit = 0x1b,
  kDoPreventAllowMediumRemoval = 0x1e,
  kReadCapacity10 = 0x25,
  kRead10 = 0x28,
  kWrite10 = 0x2a,
  kVerify10 = 0x2f,
  kSync10 = 0x35,
  kUnmap = 0x42,
  kReadToc = 0x43,
  kModeSense10 = 0x5a,
  kPersistentReserveIn = 0x5e,
  kPersistentReserveOut = 0x5f,
  kRead32 = 0x7f,
  kWrite32 = 0x7f,
  kVerify32 = 0x7f,
  kRead16 = 0x88,
  kWrite16 = 0x8a,
  kVerify16 = 0x8f,
  kSync16 = 0x91,
  kWriteSame16 = 0x93,
  kServiceActionIn = 0x9e,
  kReportLuns = 0xa0,
  kMaintenanceIn = 0xa3,
  kRead12 = 0xa8,
  kWrite12 = 0xaa,
  kVerify12 = 0xaf,
};

// SCSI Reference Manual Table 359
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
enum class ModePageCode : uint8_t {
  kNull = 0x00,
  kCacheMode = 0x08,
  kControlMode = 0x0a,
  kPowerConditionMode = 0x1a,
  kAllSupportedModes = 0x3f
};

enum class PageCode : uint8_t {
  kSupportedVpd = 0x00,
  kUnitSerialNumber = 0x80,
  kDeviceIdentification = 0x83,
  kExtended = 0x86,
  kBlockLimitsVpd = 0xb0,
  kBlockDeviceCharacteristicsVpd = 0xb1,
  kLogicalBlockProvisioningVpd = 0xb2
};

// SCSI Reference Manual Table 456
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
enum class ActivateMicrocode : uint8_t {
  // The actions of the device server may or may not be as defined for values
  // 01b or 10b.
  kAmbiguous = 0b00,

  // The device server:
  // 1) activates the microcode before completion of the final command in the
  // WRITE BUFFER sequence; and
  // 2) establishes a unit attention condition for the initiator port associated
  // with every I_T nexus, except the I_T nexus on which the WRITE BUFFER
  // command was received, with the additional sense code set to MICROCODE HAS
  // BEEN CHANGED.
  kActivateBeforeHardReset = 0b01,

  // The device server:
  // 1) activates the microcode after:
  //    A) a vendor specific event;
  //    B) a power on event; or
  //    C) a hard reset event;
  // and
  // 2) establishes a unit attention condition for the initiator port associated
  //    with every I_T nexus with the additional sense code set to MICROCODE HAS
  //    BEEN CHANGED.
  kActivateAfterHardReset = 0b10,

  kReserved = 0b11,
};

// SCSI Reference Manual Table 461
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
enum class Association : uint8_t {

  // The IDENTIFIER field is associated with the addressed physical or logical
  // device
  kPhysicalDevice = 0x0,

  // The IDENTIFIER field is associated with the port that received the request
  kPort = 0x1,

  // The IDENTIFIER field is associated with the SCSI target device that
  // contains the addressed logical unit.
  kScsiTargetDevice = 0x2,

  // Reserved code
  kReserved = 0x3
};

// SCSI Reference Manual, Section 5.4.11
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
enum class CodeSet : uint8_t {
  kReserved = 0x0,

  // The IDENTIFIER field shall contain binary values
  kBinary = 0x1,

  // 2h The IDENTIFIER field shall contain ASCII graphic codes (i.e., code
  // values 20h through 7Eh)
  kASCII = 0x2,
};

// SCSI Reference Manual, Table 463
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
enum class IdentifierType : uint8_t {
  // No assignment authority was used and consequently there is no guarantee
  // that the identifier is globally unique (i.e., the identifier is vendor
  // specific).
  kVendorSpecific1 = 0x0,

  // The first 8 bytes of the IDENTIFIER field are a Vendor ID. The organization
  // associated with the Vendor ID is responsible for ensuring that the
  // remainder of the identifier field is unique. One recommended method of
  // constructing the remainder of the identifier field is to concatenate the
  // product identification field from the standard INQUIRY data field and the
  // product serial number field from the unit serial number page
  kVendorSpecific2 = 0x1,

  // The IDENTIFIER field contains a Canonical form IEEE Extended Unique
  // Identifier, 64-bit (EUI-64). In this case, the identifier length field
  // shall be set to 8. Note that the IEEE guide-lines for EUI-64 specify a
  // method for unambiguously encapsulating an IEEE 48-bit identifier within an
  // EUI-64.
  kEUI64 = 0x2,

  // The IDENTIFIER field contains an FC-PH, FC-PH3 or FC-FS Name_Identifier.
  // Any FC-PH, FC-PH3 or FC-FS identifier may be used, including one of the
  // four based on a Canonical form IEEE company_id.
  kFibreChannel = 0x3,

  // If the ASSOCIATION field contains 1h, the Identifier value contains a
  // four-byte binary number identifying the port relative to other ports in the
  // device using the values shown Table 462. The CODE SET field shall be set to
  // 1h and the IDENTIFIER LENGTH field shall be set to 4h. If the ASSOCIATION
  // field does not contain 1h, use of this identifier type is reserved.
  kAssociationDependent1 = 0x4,
  kAssociationDependent2 = 0x5,

  // If the ASSOCIATION value is 0h, the IDENTIFIER value contains a four-byte
  // binary number identifying the port relative to other ports in the device
  // using the values shown Table 462. The CODE SET field shall be set to 1h and
  // the IDENTIFIER LENGTH field shall be set to 4h. If the ASSOCIATION field
  // does not contain 0h, use of this identifier type is reserved.
  kAssociationDependent3 = 0x6,

  // The MD5 logical unit identifier shall not be used if a logical unit
  // provides unique identification using identifier types 2h or 3h. A bridge
  // device may return a MD5 logical unit identifier type for that logical unit
  // that does not support the Device Identification VPD page.
  kNoMD5Support = 0x7,
};

// SCSI Reference Manual Table 461
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
enum class ProtocolIdentifier : uint8_t {
  kFibreChannel = 0x0,
  kObsolete = 0x1,
  kSSA = 0x2,
  kIEEE1394 = 0x3,
  kRDMA = 0x4,
  kInternetSCSI = 0x5,
  kSASSerialSCSIProtocol = 0x6,
};

// SCSI Reference Manual Table 440
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
enum class MediumRotationRate : uint16_t {
  kRotationNotReported = 0x0000,
  kNonRotatingMedium = 0x0001,
  kReserved = 0xFFFF,
};

// SCSI Reference Manual Table 441
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
enum class ProductType : uint8_t {
  kNotIndicated = 0x00,
  kCFast = 0x01,
  kCompactFlash = 0x02,
  kMemoryStick = 0x03,
  kMultiMediaCard = 0x04,
  kSecureDigitalCard = 0x05,
  kXQD = 0x06,
  kUniversalFlashStorage = 0x07
};

// SCSI Reference Manual Table 442
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
enum class Wacereq : uint8_t {
  kNotSpecified = 0b00,

  // The device server completes the read command specifying that LBA with GOOD
  // status and any data transferred to the Data-In Buffer is indeterminate.
  kSuccess = 0b01,

  // The device server terminates the read command specifying that LBA with
  // CHECK CONDITION status with sense key set to MEDIUM ERROR and the
  // additional sense code set to an appropriate value other than WRITE AFTER
  // SANITIZE REQUIRED (e.g., ID CRC OR ECC ERROR).
  kFailureWithoutWriteSanitize = 0b10,

  // The device server terminates the read command specifying that LBA with
  // CHECK CONDITION status with sense key set to MEDIUM ERROR and the
  // additional sense code set to WRITE AFTER SANITIZE REQUIRED.
  kFailureWithWriteSanitize = 0b11
};

// SCSI Reference Manual Table 443
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
enum class Wabereq : uint8_t {
  kNotSpecified = 0b00,

  // The device server completes the read command specifying that LBA with GOOD
  // status and any data transferred to the Data-In Buffer is indeterminate.
  kSuccess = 0b01,

  // The device server terminates the read command specifying that LBA with
  // CHECK CONDITION status with sense key set to MEDIUM ERROR and the
  // additional sense code set to an appropriate value other than WRITE AFTER
  // SANITIZE REQUIRED (e.g., ID CRC OR ECC ERROR).
  kFailureWithoutWriteSanitize = 0b10,

  // The device server terminates the read command specifying that LBA with
  // CHECK CONDITION status with sense key set to MEDIUM ERROR and the
  // additional sense code set to WRITE AFTER SANITIZE REQUIRED.
  kFailureWithWriteSanitize = 0b11
};

// SCSI Reference Manual Table 444
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
enum class NominalFormFactor : uint8_t {
  kNotReported = 0x0,
  kExtraLarge = 0x1,  // 5.25 inch
  kLarge = 0x2,       // 3.5 inch
  kMedium = 0x3,      // 2.5 inch
  kSmall = 0x4,       // 1.8 inch
  kExtraSmall = 0x5,  // less than 1.8 inch
};

// SCSI Reference Manual Table 445
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
enum class Zoned : uint8_t {
  kNotReported = 0b00,

  // Device server implements the host aware zoned block device capabilities
  // defined in ZBC
  kHostAware = 0b01,

  // Device server implements device managed zoned block device capabilities
  kDeviceManaged = 0b10,
  kReserved = 0b11,
};

// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
enum class PageLength : uint16_t {
  kExtendedInquiryCommand = 0x3c,
  kBlockDeviceCharacteristicsVpd = 0x3c,
};

struct ControlByte {
  uint8_t obsolete : 2;
  bool naca : 1;
  uint8_t reserved : 3;
  uint8_t vendor_specific : 2;
};

// SCSI Reference Manual Table 202
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
struct TestUnitReadyCommand {
  uint32_t reserved : 32;
  ControlByte control_byte;
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(TestUnitReadyCommand) == 5);

// SCSI Reference Manual Table 119
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
struct ReadCapacity10Command {
  uint8_t reserved_1 : 8;               // obsolete
  uint32_t logical_block_address : 32;  // obsolete
  uint16_t reserved_2 : 16;
  uint8_t reserved_3 : 8;  // obsolete PMI bit
  ControlByte control_byte;
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(ReadCapacity10Command) == 9);

// SCSI Reference Manual Table 120
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
struct ReadCapacity10Data {
  uint32_t returned_logical_block_address : 32;
  uint32_t block_length : 32;
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(ReadCapacity10Data) == 8);

// Refer to
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
// , Section 3.6 Table 58
struct InquiryCommand {
  bool evpd : 1;      // Enable Vital Product Data (EVPD)
  bool obsolete : 1;  // formerly CMDDT
  uint8_t reserved : 6;
  PageCode page_code : 8;
  uint16_t allocation_length : 16;
  ControlByte control_byte;
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(InquiryCommand) == 5);

// Refer to
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
// , Section 3.6.2 Table 60
enum class PeripheralQualifier : uint8_t {
  kPeripheralDeviceConnected = 0b000,
  kPeripheralDeviceNotConnected = 0b001,
  kReserved = 0b010,
  kNotSupported = 0b011
};

// Refer to
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
// , Section 3.6.2 Table 63
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

// Table 74
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
enum class PageControl : uint8_t {
  kCurrent = 0b00,
  kChangeable = 0b01,
  kDefault = 0b10,
  kSaved = 0b11
};

// Refer to
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
// , Section 3.6.2 Table 59
// Based on internal documentation SPC-4 Revision 37 Table 176
struct InquiryData {
  PeripheralDeviceType peripheral_device_type : 5;
  PeripheralQualifier peripheral_qualifier : 3;
  uint8_t reserved_1 : 6;
  bool lu_cong : 1;
  bool rmb : 1;  // Removable Media Bit (RMB)
  Version version : 8;
  ResponseDataFormat response_data_format : 4;
  bool hisup : 1;    // Hierarchical Support Bit (HISUP)
  bool normaca : 1;  // Normal ACA (NORMACA)
  uint8_t reserved_2 : 2;
  uint8_t additional_length : 8;
  bool protect : 1;
  uint8_t reserved_3 : 2;
  bool third_party_copy : 1;  // referred to as 3PC in the documentation
  TPGS tpgs : 2;              // Target Port Group Support (TPGS)
  bool acc : 1;               // Access controls Coordinator Bit
  bool sccs : 1;              // SCC Supported
  bool addr_16 : 1;           // SCSI 16-bit address support bit
  uint8_t reserved_4 : 2;
  bool obsolete_1 : 1;
  bool multip : 1;   // multiple SCSI Port
  bool vs_1 : 1;     // vendor specific bit
  bool encserv : 1;  // Enclosure Services Bit
  bool obsolete_2 : 1;
  bool vs_2 : 1;    // vendor specific bit
  bool cmdque : 1;  // Command Management Model bit
  bool reserved_5 : 1;
  bool obsolete_3 : 1;
  bool sync : 1;
  bool wbus_16 : 1;  // Wide Bus bit
  bool reserved_6 : 1;
  bool obsolete_4 : 1;
  char vendor_identification[8];
  uint8_t product_identification[16];
  uint8_t product_revision_level[4];
  uint8_t vendor_specific_1[20];
  bool ius : 1;  // Information Units Supported bit
  bool qas : 1;  // Quick Arbitration and Selection Supported bit
  uint8_t clocking : 2;
  uint8_t reserved_7 : 4;
  uint8_t reserved_8 : 8;
  uint16_t vendor_descriptors[8];
  uint8_t reserved_9[22];
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(InquiryData) == 96);

// SCSI Reference Manual Table 76
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
struct PersistentReserveInCommand {
  uint8_t service_action : 5;
  uint8_t reserved_1 : 3;
  uint64_t reserved_2 : 40;
  uint16_t allocation_length : 16;
  ControlByte control_byte;
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(PersistentReserveInCommand) == 9);

// Persistent Reserve In Read Reservation Data No Reservation
// SCSI Reference Manual Table 79
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
struct PriReadReservationDataNoReservation {
  uint32_t prgeneration : 32;
  uint32_t additional_length : 32;
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(PriReadReservationDataNoReservation) == 8);

// Persistent Reserve In Read Reservation Data With Reservation
// SCSI Reference Manual Table 80
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
struct PriReadReservationDataWithReservation {
  PriReadReservationDataNoReservation priDataNoReservation;
  uint64_t reservation_key : 64;
  uint32_t obsolete_1 : 32;
  uint8_t reserved : 8;
  uint8_t type : 4;
  uint8_t scope : 4;
  uint16_t obsolete_2 : 16;
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(PriReadReservationDataWithReservation) == 24);

// SCSI Reference Manual Table 88
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
struct PersistentReserveOutCommand {
  uint8_t service_action : 5;
  uint8_t reserved_1 : 3;
  uint8_t type : 4;
  uint8_t scope : 4;
  uint16_t reserved_2 : 16;
  uint32_t parameter_list_length : 32;
  ControlByte control_byte;
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(PersistentReserveOutCommand) == 9);

// Persistent Reserve Out Parameter List
// Used by Persistent Reserve Out command for any service action other than
// Register and Move SCSI Reference Manual Table 90
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
struct ProParamList {
  uint64_t reservation_key : 64;
  uint64_t service_action_reservation_key : 64;
  uint32_t obsolete_1 : 32;
  bool aptpl : 1;  // Activate Persist Through Power Loss bit
  bool reserved : 1;
  bool all_tg_pt : 1;  // All Target Ports bit
  bool spc_i_pt : 1;   // Specify Initiator Ports bit
  uint8_t reserved_1 : 4;
  uint8_t reserved_2 : 8;
  uint16_t obsolete_2 : 16;
  // additional parameter data
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(ProParamList) == 24);

// SCSI Reference Manual Table 95
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
struct Read6Command {
  uint8_t logical_block_address_1 : 5;
  uint8_t reserved : 3;
  uint16_t logical_block_address_2 : 16;
  uint8_t transfer_length : 8;
  ControlByte control_byte;
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(Read6Command) == 5);

// SCSI Reference Manual Table 97
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
struct Read10Command {
  uint8_t obsolete : 2;
  bool rarc : 1;           // Rebuild Assist Recovery bit
  bool fua : 1;            // Forced Unit access bit
  bool dpo : 1;            // disable page output bit
  uint8_t rd_protect : 3;  // read protect bit
  uint32_t logical_block_address : 32;
  uint8_t group_number : 5;
  uint8_t reserved : 3;
  uint16_t transfer_length : 16;
  ControlByte control_byte;
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(Read10Command) == 9);

// SCSI Reference Manual Table 99
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
struct Read12Command {
  uint8_t obsolete : 2;
  bool rarc : 1;  // Rebuild Assist Recovery bit
  bool fua : 1;   // Forced Unit access bit
  bool dpo : 1;   // disable page output bit
  uint8_t rd_protect : 3;
  uint32_t logical_block_address : 32;
  uint32_t transfer_length : 32;
  uint8_t group_number : 5;
  uint8_t reserved : 2;
  bool restricted_mmc_6 : 1;
  ControlByte control_byte;
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(Read12Command) == 11);

// SCSI Reference Manual Table 100
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
struct Read16Command {
  bool dld_2 : 1;
  bool obsolete : 1;
  bool rarc : 1;  // Rebuild Assist Recovery bit
  bool fua : 1;   // Forced Unit access bit
  bool dpo : 1;   // disable page output bit
  uint8_t rd_protect : 3;
  uint64_t logical_block_address : 64;
  uint32_t transfer_length : 32;
  uint8_t group_number : 6;
  bool dld_0 : 1;
  bool dld_1 : 1;
  ControlByte control_byte;
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(Read16Command) == 15);

// SCSI Reference Manual Table 215
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
struct Write6Command {
  uint8_t logical_block_address_1 : 5;
  uint8_t reserved : 3;
  uint16_t logical_block_address_2 : 16;
  uint8_t transfer_length : 8;
  ControlByte control_byte;
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(Write6Command) == 5);

// SCSI Reference Manual Table 216
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
struct Write10Command {
  uint8_t obsolete : 2;
  bool reserved_1 : 1;
  bool fua : 1;  // Forced Unit access bit
  bool dpo : 1;  // disable page output bit
  uint8_t wr_protect : 3;
  uint32_t logical_block_address : 32;
  uint8_t group_number : 5;
  uint8_t reserved_2 : 3;
  uint16_t transfer_length : 16;
  ControlByte control_byte;
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(Write10Command) == 9);

// SCSI Reference Manual Table 218
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
struct Write12Command {
  uint8_t obsolete : 2;
  bool reserved_1 : 1;
  bool fua : 1;  // Forced Unit access bit
  bool dpo : 1;  // disable page output bit
  uint8_t wr_protect : 3;
  uint32_t logical_block_address : 32;
  uint32_t transfer_length : 32;
  uint8_t group_number : 5;
  uint8_t reserved_2 : 2;
  bool restricted_mmc_5 : 1;
  ControlByte control_byte;
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(Write12Command) == 11);

// SCSI Reference Manual Table 219
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
struct Write16Command {
  bool dld_2 : 1;
  bool obsolete : 1;
  bool reserved : 1;
  bool fua : 1;  // Forced Unit access bit
  bool dpo : 1;  // disable page output bit
  uint8_t wr_protect : 3;
  uint64_t logical_block_address : 64;
  uint32_t transfer_length : 32;
  uint8_t group_number : 6;
  bool dld_0 : 1;
  bool dld_1 : 1;
  ControlByte control_byte;
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(Write16Command) == 15);

// SCSI Reference Manual Table 207
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
struct Verify10Command {
  bool obsolete : 1;
  uint8_t bytchk : 2;
  bool reserved_1 : 1;
  bool dpo : 1;  // disable page output bit
  uint8_t vr_protect : 3;
  uint32_t logical_block_address : 32;
  uint8_t group_number : 5;
  uint8_t reserved_2 : 2;
  bool restricted_mmc_5 : 1;
  uint16_t verification_length : 16;
  ControlByte control_byte;
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(Verify10Command) == 9);

// SCSI Reference Manual Table 218
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
struct Verify12Command {
  bool obsolete : 1;
  uint8_t bytchk : 2;
  bool reserved_1 : 1;
  bool dpo : 1;  // disable page output bit
  uint8_t vr_protect : 3;
  uint32_t logical_block_address : 32;
  uint32_t verification_length : 32;
  uint8_t group_number : 5;
  uint8_t reserved_2 : 2;
  bool restricted_mmc_5 : 1;
  ControlByte control_byte;
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(Verify12Command) == 11);

// SCSI Reference Manual Table 219
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
struct Verify16Command {
  bool reserved_1 : 1;
  uint8_t bytchk : 2;
  bool reserved_2 : 1;
  bool dpo : 1;  // disable page output bit
  uint8_t vr_protect : 3;
  uint64_t logical_block_address : 64;
  uint32_t verification_length : 32;
  uint8_t group_number : 5;
  uint8_t reserved_3 : 2;
  bool restricted_mmc_5 : 1;
  ControlByte control_byte;
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(Verify16Command) == 15);

// SCSI Reference Manual Table 199
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
struct SynchronizeCache10Command {
  bool obsolete_1 : 1;
  bool immed : 1;  // Immediate bit
  bool obsolete_2 : 1;
  uint8_t reserved_1 : 5;
  uint32_t logical_block_address : 32;
  uint8_t group_number : 5;
  uint8_t reserved_2 : 3;
  uint16_t number_of_blocks : 16;
  ControlByte control_byte;
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(SynchronizeCache10Command) == 9);

// SCSI Reference Manual Table 201
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
struct SynchronizeCache16Command {
  bool reserved_1 : 1;
  bool immed : 1;  // Immediate bit
  bool obsolete : 1;
  uint8_t reserved_2 : 5;
  uint64_t logical_block_address : 64;
  uint32_t number_of_blocks : 32;
  uint8_t group_number : 5;
  uint8_t reserved_3 : 3;
  ControlByte control_byte;
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(SynchronizeCache16Command) == 15);

// SCSI Reference Manual Table 73
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
struct ModeSense6Command {
  uint8_t reserved_1 : 3;
  bool dbd : 1;  // Disable block descriptors
  uint8_t reserved_2 : 4;
  ModePageCode page_code : 6;
  PageControl pc : 2;  // Page control
  uint8_t sub_page_code : 8;
  uint8_t alloc_length : 8;
  ControlByte control_byte;
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(ModeSense6Command) == 5);

// SCSI Reference Manual Table 75
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
struct ModeSense10Command {
  uint8_t reserved_1 : 3;
  bool dbd : 1;    // Disable block descriptors
  bool llbaa : 1;  // Long LBA accepted
  uint8_t reserved_2 : 3;
  ModePageCode page_code : 6;
  PageControl pc : 2;  // Page control
  uint8_t sub_page_code : 8;
  uint32_t reserved_3 : 24;
  uint16_t alloc_length : 16;
  ControlByte control_byte;
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(ModeSense10Command) == 9);

// SCSI Reference Manual Table 148
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
enum class SelectReport : uint8_t {
  kRestrictedMethods = 0x0,
  kWellKnown = 0x1,
  kAllLogical = 0x2
};

// SCSI Reference Manual Table 147
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
struct ReportLunsCommand {
  uint8_t reserved_1 : 8;
  SelectReport select_report : 8;
  uint32_t reserved_2 : 24;
  uint32_t alloc_length : 32;
  uint8_t reserved_3 : 8;
  ControlByte control_byte;
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(ReportLunsCommand) == 11);

// SCSI Reference Manual Table 149
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
// This struct is a header for variable sized data
struct ReportLunsParamData {
  uint32_t list_byte_length : 32;
  uint32_t reserved_1 : 32;
};
static_assert(sizeof(ReportLunsParamData) == 8);

// SCSI Reference Manual Table 150, 159, 162
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
struct MaintenanceInHeader {
  uint8_t service_action : 5;
  uint8_t reserved_1 : 3;
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(MaintenanceInHeader) == 1);

// SCSI Reference Manual Table 150
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
struct ReportOpCodesCommand {
  MaintenanceInHeader maintenance_in_header;
  uint8_t reporting_options : 3;
  uint8_t reserved_1 : 4;
  bool rctd : 1;  // Return commands timeout descriptor
  uint8_t requested_op_code : 8;
  uint16_t requested_service_action : 16;
  uint32_t alloc_length : 32;
  uint8_t reserved_2 : 8;
  ControlByte control_byte;
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(ReportOpCodesCommand) == 11);

// SCSI Reference Manual Table 157
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
struct CommandTimeoutsDescriptor {
  uint16_t descriptor_length : 16;
  uint8_t reserved_1 : 8;
  uint8_t cmd_specific : 8;
  uint32_t nominal_cmd_timeout : 32;
  uint32_t reccomended_cmd_timeout : 32;
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(CommandTimeoutsDescriptor) == 12);

// SCSI Reference Manual Table 153
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
struct CommandDescriptor {
  uint8_t reserved_1 : 8;
  uint16_t service_action : 16;
  uint8_t reserved_2 : 8;
  bool servactv : 1;  // Service action valid
  bool ctdp : 1;      // Command timeouts descriptor present
  uint8_t reserved_3 : 6;
  uint16_t cdb_length : 16;  // Command descriptor block length
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(CommandDescriptor) == 7);

struct CommandDescriptorTimeoutIncluded {
  uint8_t reserved_1 : 8;
  uint16_t service_action : 16;
  uint8_t reserved_2 : 8;
  bool servactv : 1;  // Service action valid
  bool ctdp : 1;      // Command timeouts descriptor present
  uint8_t reserved_3 : 6;
  uint16_t cdb_length : 16;  // Command descriptor block length
  CommandTimeoutsDescriptor
      cmd_timeouts_desc;  // This field's validity is specified by ctdp.
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(CommandDescriptorTimeoutIncluded) == 19);

// SCSI Reference Manual Table 152
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
// This struct is a header for variable sized data
struct AllCommandsParamData {
  uint32_t listByteSize : 32;
};

// SCSI Reference Manual Table 155
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
// This struct is a header for variable sized data
struct OneCommandParamData {
  uint8_t reserved_1 : 8;
  uint8_t support : 3;
  uint8_t reserved_2 : 4;
  bool ctdp : 1;  // Command timeouts descriptor present
  uint16_t cdb_size : 16;
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(OneCommandParamData) == 4);

// SCSI Reference Manual Table 159
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
// Report supported task managment functions command
struct ReportTmfCommand {
  MaintenanceInHeader maintenance_in_header;
  uint8_t reserved_1 : 7;
  bool repd : 1;  // Return extended parameter data
  uint32_t reserved_2 : 24;
  uint32_t alloc_length : 32;
  uint8_t reserved_3 : 8;
  ControlByte control_byte;
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(ReportTmfCommand) == 11);

// SCSI Reference Manual Table 160
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
// Report supported task managment functions parameter data
struct ReportTmfParamData {
  uint8_t obsolete_1 : 2;
  bool qts : 1;    // Query task supported
  bool lurs : 1;   // Logical unit reset supported
  bool ctss : 1;   // Clear task set supported
  bool cacas : 1;  // Clear ACA supported
  bool atss : 1;   // Abort task set supported
  bool ats : 1;    // Abort task supported
  bool itnrs : 1;  // I-T Nexus reset supported
  bool qtss : 1;   // Query task set supported
  bool qaes : 1;   // Query async event supported
  uint8_t reserved_1 : 5;
  uint8_t reserved_2 : 8;
  uint8_t additional_data_length : 8;
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(ReportTmfParamData) == 4);

// SCSI Reference Manual Table 162
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
struct ReportTimestampCommand {
  uint32_t reserved_1 : 32;
  uint32_t alloc_length : 32;
  uint8_t reserved_2 : 8;
  ControlByte control_byte;
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(ReportTimestampCommand) == 10);

// SCSI Reference Manual Table 163
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
struct ReportTimestampParamData {
  uint16_t data_length : 16;
  uint8_t ts_origin : 3;  // Timestamp origin
  uint8_t reserved_1 : 5;
  uint8_t reserved : 8;
  uint64_t timestamp : 48;
  uint8_t reserved_2 : 8;
  uint8_t reserved_3 : 8;
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(ReportTimestampParamData) == 12);

// SCSI Reference Manual Table 204
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
struct UnmapCommand {
  bool anchor : 1;
  uint8_t reserved_1 : 7;
  uint32_t reserved_2 : 32;
  uint8_t group_number : 5;
  uint8_t reserved_3 : 3;
  uint16_t param_list_length : 16;
  ControlByte control_byte;
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(UnmapCommand) == 9);

// SCSI Reference Manual Table 205
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
// This struct is a header for variable length data
struct UnmapParamList {
  uint16_t data_length : 16;
  uint16_t block_desc_data_length : 16;
  uint32_t reserved_1 : 32;
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(UnmapParamList) == 8);

// SCSI Reference Manual Table 206
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
struct UnmapBlockDescriptor {
  uint64_t logical_block_addr : 64;
  uint32_t logical_block_count : 32;
  uint32_t reserved_1 : 32;
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(UnmapBlockDescriptor) == 16);

// SCSI Reference Manual Table 372
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
struct CachingModePage {
  ModePageCode page_code : 6;
  bool spf : 1;  // Sub-page format
  bool ps : 1;   // Parameter saveable
  uint8_t page_length : 8;
  bool rcd : 1;        // Read cache disable
  bool mf : 1;         // Multiplication factor
  bool wce : 1;        // Write cache enabled
  bool size : 1;       // Size enable
  bool disc : 1;       // Disconinuity
  bool cap : 1;        // Caching analysis permitted
  bool abpf : 1;       // Abort prefetch
  bool ic : 1;         // Initiator control
  uint8_t wrp : 4;     // Write retention priority
  uint8_t drrp : 4;    // Demand read retention priority
  uint16_t dptl : 16;  // Disable prefetch transfer length
  uint16_t min_prefetch : 16;
  uint16_t max_prefetch : 16;
  uint16_t max_prefetch_ceil : 16;
  bool nv_dis : 1;  // Non-volatile cache disabled
  uint8_t sync_prog : 2;
  uint8_t vs : 2;    // Vendor specific
  bool dra : 1;      // Disable read ahead
  bool lbcss : 1;    //  Logical block cache segment size bit
  bool fsw : 1;      // Force sequential write
  uint8_t nocs : 8;  // Number of cache segments
  uint16_t cache_segment_time : 16;
  uint8_t reserved : 8;
  uint32_t obsolete : 24;
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(CachingModePage) == 20);

// SCSI Reference Manual Table 377
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
struct ControlModePage {
  ModePageCode page_code : 6;
  bool spf : 1;  // Sub-page format
  bool ps : 1;   // Parameter saveable
  uint8_t page_length : 8;
  bool rlec : 1;     // Report log exception condition
  bool gltsd : 1;    // Global logging target solve disable
  bool d_sense : 1;  // Descriptor format sense data bit
  bool dpicz : 1;  // Disable protection information check if protect field zero
  bool tmf_only : 1;  // Allow task managment functions only
  uint8_t tst : 3;    // Task set type
  bool dque : 1;      // Disable queueing
  uint8_t qerr : 2;   // Queue error mangment
  bool nuar : 1;      // No unit attention on release
  uint8_t qam : 4;    // Queue algorithm modifier
  uint8_t obsolete_1 : 3;
  bool swp : 1;                // Software write protect
  uint8_t ua_intlck_ctrl : 2;  // Unit attention interlocks control
  bool rac : 1;                // Report a check
  bool vs : 1;                 // Vendor specific
  uint8_t autoload_mode : 3;
  bool reserved : 1;
  bool rwwp : 1;   // Reject write without protection
  bool atmpe : 1;  // Application tag mode page enabled
  bool tas : 1;    // Task aborted status
  bool ato : 1;    // Application tag owner
  uint16_t obsolete_2 : 16;
  uint16_t busy_timeout_period : 16;
  uint16_t estct : 16;  // Extended self-test completion time
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(ControlModePage) == 12);

// SCSI Reference Manual Table 397
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
struct PowerConditionModePage {
  ModePageCode page_code : 6;
  bool spf : 1;  // Sub-page format
  bool ps : 1;   // Parameter saveable
  uint8_t page_length : 8;
  bool standby_y : 1;
  uint8_t reserved_1 : 5;
  uint8_t pm_bg_precedence : 2;
  bool standby_z : 1;
  bool idle_a : 1;
  bool idle_b : 1;
  bool idle_c : 1;
  uint8_t reserved_2 : 4;
  uint32_t idle_a_ct : 32;     // Idle A condition timer
  uint32_t standby_z_ct : 32;  // Standby Z condition timer
  uint32_t idle_b_ct : 32;     // Idle B condition timer
  uint32_t idle_c_ct : 32;     // Idle C condition timer
  uint32_t standby_y_ct : 32;  // Standby Y condition timer
  uint8_t reserved_3[15];
  uint8_t reserved_4 : 2;
  int8_t ccf_stopped : 2;   // Check condition from stopped
  uint8_t ccf_standby : 2;  // Check condition from standby
  uint8_t ccf_idle : 2;     // Check condition from idle
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(PowerConditionModePage) == 40);

// SCSI Reference Manual Table 361
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
struct ModeParameter6Header {
  uint8_t mode_data_length : 8;
  uint8_t medium_type : 8;
  uint8_t reserved_2 : 4;
  bool dpofua : 1;  // DPO and FUA support
  uint8_t reserved_1 : 2;
  bool wp : 1;      // Write protect
  uint8_t bdl : 8;  // Block descriptor length
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(ModeParameter6Header) == 4);

// SCSI Reference Manual Table 362
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
struct ModeParameter10Header {
  uint16_t mode_data_length : 16;
  uint8_t medium_type : 8;
  uint8_t reserved_1 : 4;
  bool dpofua : 1;  // DPO and FUA support
  uint16_t reserved_2 : 2;
  bool wp : 1;  // Write protect
  bool longlba : 1;
  uint16_t reserved_3 : 15;
  uint16_t bdl : 16;  // Block descriptor length
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(ModeParameter10Header) == 8);

struct ShortLbaBlockDescriptor {
  uint32_t number_of_blocks : 32;
  uint8_t reserved : 8;
  uint32_t logical_block_length : 24;
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(ShortLbaBlockDescriptor) == 8);

struct LongLbaBlockDescriptor {
  uint64_t number_of_blocks : 64;
  uint32_t reserved : 32;
  uint32_t logical_block_length : 32;
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(LongLbaBlockDescriptor) == 16);

// SCSI Reference Manual Table 164
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
struct RequestSenseCommand {
  bool desc : 1;
  uint8_t reserved_1 : 7;
  uint16_t reserved_2 : 16;
  uint8_t allocation_length : 8;
  ControlByte control_byte;
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(RequestSenseCommand) == 5);

// SCSI Reference Manual Table 27
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
struct FixedFormatSenseData {
  SenseResponse response_code : 7;
  bool valid : 1;
  uint8_t _obsolete : 8;
  SenseKey sense_key : 4;
  bool reserved_1 : 1;
  bool ili : 1;  // Incorrect length indicator
  bool eom : 1;  // End-of-Medium
  bool filemark : 1;
  uint32_t info : 32;
  uint8_t additional_sense_length : 8;
  uint32_t command_specific_info : 32;
  AdditionalSenseCode additional_sense_code : 8;
  AdditionalSenseCodeQualifier additional_sense_code_qualifier : 8;
  uint8_t field_replaceable_unit_code : 8;
  uint8_t sense_key_specific_1 : 7;
  bool sksv : 1;
  uint16_t sense_key_specific_2 : 16;
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(FixedFormatSenseData) == 18);

// SCSI Reference Manual Table 12
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
struct DescriptorFormatSenseData {
  SenseResponse response_code : 7;
  bool reserved_1 : 1;
  SenseKey sense_key : 4;
  uint8_t reserved_2 : 4;
  AdditionalSenseCode additional_sense_code : 8;
  AdditionalSenseCodeQualifier additional_sense_code_qualifier : 8;
  uint32_t reserved_3 : 24;
  uint8_t additional_sense_length : 8;
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(DescriptorFormatSenseData) == 8);

// SCSI Reference Manual Table 483
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
struct SupportedVitalProductData {
  PeripheralDeviceType peripheral_device_type : 5;
  PeripheralQualifier peripheral_qualifier : 3;
  PageCode page_code : 8;
  uint8_t _reserved : 8;
  uint8_t page_length : 8;
  // PageCode supported_page_list[256];
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(SupportedVitalProductData) == 4);

// SCSI Reference Manual Table 484
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
struct UnitSerialNumber {
  PeripheralDeviceType peripheral_device_type : 5;
  PeripheralQualifier peripheral_qualifier : 3;
  PageCode page_code : 8;
  uint8_t _reserved : 8;
  uint8_t page_length : 8;
  // uint8_t product_serial_number[256];
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(UnitSerialNumber) == 4);

// SCSI Reference Manual Table 460
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
struct IdentificationDescriptor {
  CodeSet code_set : 4;
  ProtocolIdentifier protocol_identifier : 4;
  IdentifierType identifier_type : 4;
  Association association : 2;
  bool _reserved1 : 1;
  bool protocol_identifier_valid : 1;
  uint8_t _reserved2 : 8;
  uint8_t identifier_length : 8;
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(IdentificationDescriptor) == 4);

// SCSI Reference Manual Table 459
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
struct DeviceIdentificationVpd {
  PeripheralDeviceType peripheral_device_type : 5;
  PeripheralQualifier peripheral_qualifier : 3;
  PageCode page_code : 8;
  uint8_t page_length : 8;
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(DeviceIdentificationVpd) == 3);

// SCSI reference Manual Table 455
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
struct ExtendedInquiryDataVpd {
  PeripheralDeviceType peripheral_device_type : 5;
  PeripheralQualifier peripheral_qualifer : 3;
  PageCode page_code : 8;
  PageLength page_length : 16;
  bool ref_chk : 1;  // reference tag check bit
  bool app_chk : 1;  // application tag check bit
  bool grd_chk : 1;  // guard check bit
  uint8_t spt : 3;   // supported protection type
  ActivateMicrocode activate_microcode : 2;
  bool simpsup : 1;    // Simple Supported bit
  bool ordsup : 1;     // Ordered Supported bit
  bool headsup : 1;    // Head of Queue Supported bit
  bool prior_sup : 1;  // Priority Supported bit
  bool group_sup : 1;  // Grouping Function Supported bit
  bool uask_sup : 1;   // Unit Attention Sense Key Supported bit
  uint8_t _reserved1 : 2;
  bool v_sup : 1;    // Volatile Cache Supported Bit
  bool nv_sup : 1;   // Non-Volatile Cache Supported bit
  bool crd_sup : 1;  // Correction Disable Supported bit
  bool wu_sup : 1;   // Write Uncorrectable Supported bit
  uint8_t _reserved2 : 4;
  bool luiclr : 1;  // Logical Unit I_T Nexus Clear bit
  uint8_t _reserved3 : 3;
  bool p_i_i_sup : 1;  // Protection Information Interval Supported bit
  bool no_pi_chk : 1;  // No Protection Information Checking bit
  uint8_t _reserved4 : 2;
  bool obsolete : 1;
  bool hssrelef : 1;  // History Snapshots Release Effects bit
  bool rtd_sup : 1;   // Resistance Temperature Detection bit
  bool _reserved5 : 1;
  bool r_sup : 1;  // Referrals Supported bit
  uint8_t _reserved6 : 3;
  uint8_t multi_t_nexus_microcode_download : 4;
  uint8_t _reserved7 : 4;
  uint16_t extended_self_test_completion_minutes : 16;
  uint8_t _reserved8 : 5;
  bool vsa_sup : 1;  // Vendor Specific Activation Supported bit
  bool hra_sup : 1;  // Hard Reset Activation Supported bit
  bool poa_sup : 1;  // Power on Activation Supported bit
  uint8_t maximum_supported_sense_data_length : 8;
  // uint8_t _reserved9[50];
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(ExtendedInquiryDataVpd) == 14);

// SCSI Reference Manual Table 459
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
struct BlockDeviceCharacteristicsVpd {
  PeripheralDeviceType peripheral_device_type : 5;
  PeripheralQualifier peripheral_qualifier : 3;
  PageCode page_code : 8;
  PageLength page_length : 16;
  MediumRotationRate medium_rotation_rate : 16;
  ProductType product_type : 8;
  NominalFormFactor nominal_form_factor : 4;
  Wacereq wacereq : 2;
  Wabereq wabereq : 2;
  bool vbuls : 1;
  bool fuab : 1;
  bool bocs : 1;
  bool _reserved1 : 1;
  Zoned zoned : 2;  //
  uint8_t _reserved2 : 2;
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(BlockDeviceCharacteristicsVpd) == 9);

// namespace scsi_defs
// SCSI Reference Manual Table 467
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
struct LogicalBlockProvisioningVpd {
  PeripheralDeviceType peripheral_device_type : 5;
  PeripheralQualifier peripheral_qualifier : 3;
  PageCode page_code : 8;
  uint16_t page_length : 16;
  uint8_t threshold_exponent : 8;
  bool dp : 1;
  bool anc_sup : 1;
  uint8_t lbprz : 3;
  bool lbpws10 : 1;
  bool lbpws : 1;
  bool lbpu : 1;
  uint8_t provisioning_type : 3;
  uint8_t min_percentage : 5;
  uint8_t threshold_percentage : 8;
  uint8_t provisioning_group_descriptor[56];
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(LogicalBlockProvisioningVpd) == 64);

// SCSI Reference Manual Table 450
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
struct BlockLimitsVpd {
  PeripheralDeviceType peripheral_device_type : 5;
  PeripheralQualifier peripheral_qualifier : 3;
  PageCode page_code : 8;
  uint16_t page_length : 16;
  bool wsnz : 1;
  uint8_t _reserved : 7;
  uint8_t max_compare_write_length : 8;
  uint16_t optimal_translater_length_granularity : 16;
  uint32_t max_transfer_length : 32;
  uint32_t optimal_transfer_length : 32;
  uint32_t max_prefetch_length : 32;
  uint32_t max_unmap_lba_count : 32;
  uint32_t max_unmap_block_descriptor_count : 32;
  uint32_t optimal_unmap_granularity : 32;
  uint8_t unmap_granularity_alignment_1 : 7;
  bool ugavalid : 1;
  uint32_t unmap_graularity_alignment_2 : 24;
  uint64_t max_write_same_length : 64;
  uint32_t max_atomic_transfer_length : 32;
  uint32_t atomic_alignment : 32;
  uint32_t atomic_transfer_length_granularity : 32;
  uint32_t max_atomic_transfer_length_with_atomic_boundary : 32;
  uint32_t max_atomic_boundary_size : 32;
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(BlockLimitsVpd) == 64);

}  // namespace scsi

#endif
