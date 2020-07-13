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
  kServiceActionIn = 0x9e,
  kReportLuns = 0xa0,
  kMaintenanceIn = 0xa3,
  kRead12 = 0xa8,
  kWrite12 = 0xaa,
  kVerify12 = 0xaf,
};

// SCSI Reference Manual Table 10
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
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
  uint8_t reserved : 6;
  bool obsolete : 1;  // formerly CMDDT
  bool evpd : 1;      // Enable Vital Product Data (EVPD)
  uint8_t page_code : 8;
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

// Refer to
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
// , Section 3.6.2 Table 59
struct InquiryData {
  PeripheralQualifier peripheral_qualifier : 3;
  PeripheralDeviceType peripheral_device_type : 5;
  bool rmb : 1;  // Removable Media Bit (RMB)
  bool lu_cong : 1;
  uint8_t reserved_1 : 6;
  Version version : 8;
  uint8_t reserved_2 : 2;
  bool normaca : 1;  // Normal ACA (NORMACA)
  bool hisup : 1;    // Hierarchical Support Bit (HISUP)
  ResponseDataFormat response_data_format : 4;
  uint8_t additional_length : 8;
  bool sccs : 1;              // SCC Supported
  bool acc : 1;               // Access controls Coordinator Bit
  TPGS tpgs : 2;              // Target Port Group Support (TPGS)
  bool third_party_copy : 1;  // referred to as 3PC in the documentation
  uint8_t reserved_3 : 2;
  bool protect : 1;
  bool obsolete_1 : 1;
  bool encserv : 1;  // Enclosure Services Bit
  bool vs_1 : 1;     // vendor specific bit
  bool multip : 1;   // multiple SCSI Port
  bool obsolete_2 : 1;
  uint8_t reserved_4 : 2;
  bool addr_16 : 1;  // SCSI 16-bit address support bit
  bool obsolete_3 : 1;
  bool reserved_5 : 1;
  bool wbus_16 : 1;  // Wide Bus bit
  bool sync : 1;
  bool obsolete_4 : 1;
  bool reserved_6 : 1;
  bool cmdque : 1;  // Command Management Model bit
  bool vs_2 : 1;    // vendor specific bit
  uint64_t vendor_identification : 64;
  uint8_t product_identification[16];
  uint32_t product_revision_level : 32;
  uint8_t vendor_specific_1[20];
  uint8_t reserved_7 : 4;
  uint8_t clocking : 2;
  bool qas : 1;  // Quick Arbitration and Selection Supported bit
  bool ius : 1;  // Information Units Supported bit
  uint8_t reserved_8 : 8;
  uint16_t vendor_descriptors[8];
  uint8_t reserved_9[22];
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(InquiryData) == 96);

// SCSI Reference Manual Table 76
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
struct PersistentReserveInCommand {
  uint8_t reserved_1 : 3;
  uint8_t service_action : 5;
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
  uint8_t scope : 4;
  uint8_t type : 4;
  uint16_t obsolete_2 : 16;
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(PriReadReservationDataWithReservation) == 24);

// SCSI Reference Manual Table 88
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
struct PersistentReserveOutCommand {
  uint8_t reserved_1 : 3;
  uint8_t service_action : 5;
  uint8_t scope : 4;
  uint8_t type : 4;
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
  uint8_t reserved_1 : 4;
  bool spc_i_pt : 1;   // Specify Initiator Ports bit
  bool all_tg_pt : 1;  // All Target Ports bit
  bool reserved : 1;
  bool aptpl : 1;  // Activate Persist Through Power Loss bit
  uint8_t reserved_2 : 8;
  uint16_t obsolete_2 : 16;
  // additional parameter data
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(ProParamList) == 24);

// SCSI Reference Manual Table 95
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
struct Read6Command {
  uint8_t reserved : 3;
  uint32_t logical_block_address : 21;
  uint8_t transfer_length : 8;
  ControlByte control_byte;
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(Read6Command) == 5);

// SCSI Reference Manual Table 97
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
struct Read10Command {
  uint8_t rd_protect : 3;  // read protect bit
  bool dpo : 1;            // disable page output bit
  bool fua : 1;            // Forced Unit access bit
  bool rarc : 1;           // Rebuild Assist Recovery bit
  bool fua_nv : 1;         // Forced Access Unit Non-Volatile
  bool obsolete : 1;
  uint32_t logical_block_address : 32;
  uint8_t reserved : 3;
  uint8_t group_number : 5;
  uint16_t transfer_length : 16;
  ControlByte control_byte;
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(Read10Command) == 9);

// SCSI Reference Manual Table 99
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
struct Read12Command {
  uint8_t rd_protect : 3;
  bool dpo : 1;     // disable page output bit
  bool fua : 1;     // Forced Unit access bit
  bool rarc : 1;    // Rebuild Assist Recovery bit
  bool fua_nv : 1;  // Forced Access Unit Non-Volatile
  bool obsolete : 1;
  uint32_t logical_block_address : 32;
  uint32_t transfer_length : 32;
  bool restricted_mmc_6 : 1;
  uint8_t reserved : 2;
  uint8_t group_number : 5;
  ControlByte control_byte;
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(Read12Command) == 11);

// SCSI Reference Manual Table 100
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
struct Read16Command {
  uint8_t rd_protect : 3;
  bool dpo : 1;     // disable page output bit
  bool fua : 1;     // Forced Unit access bit
  bool rarc : 1;    // Rebuild Assist Recovery bit
  bool fua_nv : 1;  // Forced Access Unit Non-Volatile
  bool reserved1 : 1;
  uint64_t logical_block_address : 64;
  uint32_t transfer_length : 32;
  bool restricted_mmc_6 : 1;
  uint8_t reserved2 : 2;
  uint8_t group_number : 5;
  ControlByte control_byte;
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(Read16Command) == 15);

// SCSI Reference Manual Table 102
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
struct Read32Command {
  ControlByte control_byte;
  uint32_t reserved_1 : 32;
  uint8_t reserved_2 : 3;
  uint8_t group_number : 5;
  uint8_t additional_cdb_length : 8;  // 0x18
  uint16_t service_action : 16;       // 0x0009
  uint8_t rd_protect : 3;
  bool dpo : 1;     // disable page output bit
  bool fua : 1;     // Forced Unit access bit
  bool rarc : 1;    // Rebuild Assist Recovery bit
  bool fua_nv : 1;  // Forced Access Unit Non-Volatile
  uint16_t reserved_3 : 9;
  uint64_t logical_block_address : 64;
  uint32_t expected_initial_logical_block_reference_tag : 32;
  uint16_t expected_logical_block_application_tag : 16;
  uint16_t logical_block_application_tag_mask : 16;
  uint32_t transfer_length : 32;
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(Read32Command) == 31);

// SCSI Reference Manual Table 215
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
struct Write6Command {
  uint8_t reserved : 3;
  uint32_t logical_block_address : 21;
  uint8_t transfer_length : 8;
  ControlByte control_byte;
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(Write6Command) == 5);

// SCSI Reference Manual Table 216
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
struct Write10Command {
  uint8_t wr_protect : 3;
  bool dpo : 1;  // disable page output bit
  bool fua : 1;  // Forced Unit access bit
  bool reserved_1 : 1;
  bool fua_nv : 1;  // Forced Access Unit Non-Volatile
  bool obsolete : 1;
  uint32_t logical_block_address : 32;
  uint8_t reserved_2 : 3;
  uint8_t group_number : 5;
  uint16_t transfer_length : 16;
  ControlByte control_byte;
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(Write10Command) == 9);

// SCSI Reference Manual Table 218
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
struct Write12Command {
  uint8_t wr_protect : 3;
  bool dpo : 1;  // disable page output bit
  bool fua : 1;  // Forced Unit access bit
  bool reserved_1 : 1;
  bool fua_nv : 1;  // Forced Access Unit Non-Volatile
  bool obsolete : 1;
  uint32_t logical_block_address : 32;
  uint32_t transfer_length : 32;
  bool restricted_mmc_6 : 1;
  uint8_t reserved_2 : 2;
  uint8_t group_number : 5;
  ControlByte control_byte;
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(Write12Command) == 11);

// SCSI Reference Manual Table 219
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
struct Write16Command {
  uint8_t wr_protect : 3;
  bool dpo : 1;  // disable page output bit
  bool fua : 1;  // Forced Unit access bit
  bool reserved_1 : 1;
  bool fua_nv : 1;  // Forced Access Unit Non-Volatile
  bool reserved_2 : 1;
  uint64_t logical_block_address : 64;
  uint32_t transfer_length : 32;
  bool restricted_mmc_6 : 1;
  uint8_t reserved_3 : 2;
  uint8_t group_number : 5;
  ControlByte control_byte;
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(Write16Command) == 15);

// SCSI Reference Manual Table 220
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
struct Write32Command {
  ControlByte control_byte;
  uint32_t reserved_1 : 32;
  uint8_t reserved_2 : 3;
  uint8_t group_number : 5;
  uint8_t additional_cdb_length : 8;  // 0x18
  uint16_t service_action : 16;       // 0x000B
  uint8_t wr_protect : 3;
  bool dpo : 1;  // disable page output bit
  bool fua : 1;  // Forced Unit access bit
  bool reserved_3 : 1;
  bool fua_nv : 1;  // Forced Access Unit Non-Volatile
  uint16_t reserved_4 : 9;
  uint64_t logical_block_address : 64;
  uint32_t expected_initial_logical_block_reference_tag : 32;
  uint16_t expected_logical_block_application_tag : 16;
  uint16_t logical_block_application_tag_mask : 16;
  uint32_t transfer_length : 32;
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(Write32Command) == 31);

// SCSI Reference Manual Table 207
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
struct Verify10Command {
  uint8_t vr_protect : 3;
  bool dpo : 1;  // disable page output bit
  bool reserved_1 : 1;
  uint8_t bytchk : 2;
  bool obsolete : 1;
  uint32_t logical_block_address : 32;
  bool restricted_mmc_5 : 1;
  uint8_t reserved_2 : 2;
  uint8_t group_number : 5;
  uint16_t verification_length : 16;
  ControlByte control_byte;
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(Verify10Command) == 9);

// SCSI Reference Manual Table 218
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
struct Verify12Command {
  uint8_t vr_protect : 3;
  bool dpo : 1;  // disable page output bit
  bool reserved_1 : 1;
  uint8_t bytchk : 2;
  bool obsolete : 1;
  uint32_t logical_block_address : 32;
  uint32_t verification_length : 32;
  bool restricted_mmc_6 : 1;
  uint8_t reserved_2 : 2;
  uint8_t group_number : 5;
  ControlByte control_byte;
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(Verify12Command) == 11);

// SCSI Reference Manual Table 219
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
struct Verify16Command {
  uint8_t vr_proetect : 3;
  bool dpo : 1;  // disable page output bit
  bool reserved_1 : 1;
  bool bytchk : 2;
  bool reserved_2 : 1;
  uint64_t logical_block_address : 64;
  uint32_t verification_length : 32;
  bool restricted_mmc_6 : 1;
  uint8_t reserved_3 : 2;
  uint8_t group_number : 5;
  ControlByte control_byte;
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(Verify16Command) == 15);

// SCSI Reference Manual Table 220
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
struct Verify32Command {
  ControlByte control_byte;
  uint32_t reserved_1 : 32;
  uint8_t reserved_2 : 3;
  uint8_t group_number : 5;
  uint8_t additional_cdb_length : 8;  // 0x18
  uint16_t service_action : 16;       // 0x000A
  uint8_t vr_protect : 3;
  bool dpo : 1;  // disable page output bit
  bool reserved_3 : 1;
  uint8_t bytchk : 2;
  uint16_t reserved_4 : 9;
  uint64_t logical_block_address : 64;
  uint32_t expected_initial_logical_block_reference_tag : 32;
  uint16_t expected_logical_block_application_tag : 16;
  uint16_t logical_block_application_tag_mask : 16;
  uint32_t transfer_length : 32;
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(Verify32Command) == 31);

// SCSI Reference Manual Table 199
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
struct SynchronizeCache10Command {
  uint8_t reserved_1 : 5;
  bool sync_nv : 1;  // Sync volatile or non-volatile caches
  bool immed : 1;    // Immediate bit
  bool obsolete_2 : 1;
  uint32_t logical_block_address : 32;
  uint8_t reserved_2 : 3;
  uint8_t group_number : 5;
  uint16_t number_of_blocks : 16;
  ControlByte control_byte;
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(SynchronizeCache10Command) == 9);

// SCSI Reference Manual Table 201
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
struct SynchronizeCache16Command {
  uint8_t reserved_1 : 5;
  bool sync_nv : 1;  // Sync volatile or non-volatile caches
  bool immed : 1;    // Immediate bit
  bool reserved_2 : 1;
  uint64_t logical_block_address : 64;
  uint32_t number_of_blocks : 32;
  uint8_t reserved_3 : 3;
  uint8_t group_number : 5;
  ControlByte control_byte;
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(SynchronizeCache16Command) == 15);

// SCSI Reference Manual Table 73
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
struct ModeSense6Command {
  uint8_t reserved_1 : 4;
  bool dbd : 1;  // Disable block descriptors
  uint8_t reserved_2 : 3;
  uint8_t pc : 2;  // Page control
  uint8_t page_code : 6;
  uint8_t sub_page_code : 8;
  uint8_t alloc_length : 8;
  ControlByte control_byte;
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(ModeSense6Command) == 5);

// SCSI Reference Manual Table 75
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
struct ModeSense10Command {
  uint8_t reserved_1 : 3;
  bool llbaa : 1;  // Long LBA accepted
  bool dbd : 1;    // Disable block descriptors
  uint8_t reserved_2 : 3;
  uint8_t pc : 2;  // Page control
  uint8_t page_code : 6;
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
  uint8_t reserved_1 : 3;
  uint8_t service_action : 5;
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(MaintenanceInHeader) == 1);

// SCSI Reference Manual Table 150
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
struct ReportOpCodesCommand {
  bool rctd : 1;  // Return commands timeout descriptor
  uint8_t reserved_1 : 4;
  uint8_t reporting_options : 3;
  uint8_t requested_op_code : 8;
  uint16_t requested_service_action : 16;
  uint32_t alloc_length : 32;
  uint8_t reserved_2 : 8;
  ControlByte control_byte;
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(ReportOpCodesCommand) == 10);

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
  uint8_t reserved_3 : 6;
  bool ctdp : 1;             // Command timeouts descriptor present
  bool servactv : 1;         // Service action valid
  uint16_t cdb_length : 16;  // Command descriptor block length
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(CommandDescriptor) == 7);

struct CommandDescriptorTimeoutIncluded {
  uint8_t reserved_1 : 8;
  uint16_t service_action : 16;
  uint8_t reserved_2 : 8;
  uint8_t reserved_3 : 6;
  bool ctdp : 1;             // Command timeouts descriptor present
  bool servactv : 1;         // Service action valid
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
  bool ctdp : 1;  // Command timeouts descriptor present
  uint8_t reserved_2 : 4;
  uint8_t support : 3;
  uint16_t cdb_size : 16;
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(OneCommandParamData) == 4);

// SCSI Reference Manual Table 159
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
// Report supported task managment functions command
struct ReportTmfCommand {
  bool repd : 1;  // Return extended parameter data
  uint32_t reserved_1 : 31;
  uint32_t alloc_length : 32;
  uint8_t reserved_2 : 8;
  ControlByte control_byte;
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(ReportTmfCommand) == 10);

// SCSI Reference Manual Table 161
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
// Report supported task managment functions parameter data
struct ReportTmfParamData {
  bool ats : 1;            // Abort task supported
  bool atss : 1;           // Abort task set supported
  bool cacas : 1;          // Clear ACA supported
  bool ctss : 1;           // Clear task set supported
  bool lurs : 1;           // Logical unit reset supported
  bool qts : 1;            // Query task supported
  uint8_t reserved_1 : 7;  // Obsolete
  bool qaes : 1;           // Query async event supported
  bool qtss : 1;           // Query task set supported
  bool itnrs : 1;          // I-T Nexus reset supported
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
  uint8_t reserved_1 : 5;
  uint8_t ts_origin : 3;
  uint8_t reserved : 8;
  uint64_t timestamp : 48;
  uint8_t reserved_2 : 8;
  uint8_t reserved_3 : 8;
} ABSL_ATTRIBUTE_PACKED;
static_assert(sizeof(ReportTimestampParamData) == 12);

// SCSI Reference Manual Table 204
// https://www.seagate.com/files/staticfiles/support/docs/manual/Interface%20manuals/100293068j.pdf
struct UnmapCommand {
  uint8_t reserved_1 : 7;
  bool anchor : 1;
  uint32_t reserved_2 : 32;
  uint8_t reserved_3 : 3;
  uint8_t group_number : 5;
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

}  // namespace scsi

#endif
