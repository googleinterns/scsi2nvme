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

#include "inquiry.h"

#include <cstring>
#ifdef __KERNEL__
#include <linux/byteorder/generic.h>
#else
#include <netinet/in.h>
#endif

namespace translator {

// command specific helpers
namespace {

constexpr uint8_t kIdentifierLengthNguid = 0x10;
constexpr uint8_t kIdentifierLengthEui64 = 0x8;

// The SPT field of the ExtendedInquiryData depends on the DPC field of the
// IdentifyNamespaceData
StatusCode GetSptValueFromDPC(
    const nvme::IdentifyNamespace& identify_namespace_data, uint8_t& spt) {
  // no need to convert from network to host endian as all three fields are 1
  // byte long
  uint8_t dpc = identify_namespace_data.dpc.pit1 << 2 |
                identify_namespace_data.dpc.pit2 << 1 |
                identify_namespace_data.dpc.pit3;

  // SPT shall be translated using the DPC field of the Identify
  // Namespace Data Structure. Refer to:
  // https://nvmexpress.org/wp-content/uploads/NVM_Express_-_SCSI_Translation_Reference-1_5_20150624_Gold.pdf
  // Section 6.1.5
  switch (dpc) {
    case 0b001:
      spt = 0b000;
      break;
    case 0b010:
      spt = 0b010;
      break;
    case 0b011:
      spt = 0b001;
      break;
    case 0b100:
      spt = 0b100;
      break;
    case 0b101:
      spt = 0b011;
      break;
    case 0b110:
      spt = 0b101;
      break;
    case 0b111:
      spt = 0b111;
      break;
    default:
      DebugLog("DPC value not recoginized while translating ExtendedInquiry\n");
      return StatusCode::kFailure;
  }
  return StatusCode::kSuccess;
}

// Builds the IdentificationDescriptor according to 6.1.4.5 of the translation
// spec:
// https://nvmexpress.org/wp-content/uploads/NVM_Express_-_SCSI_Translation_Reference-1_5_20150624_Gold.pdf
// This only builds the constant part of the Identification Descriptor. The
// caller is responsible for building the identifier.
StatusCode BuildIdentificationDescriptor(
    const nvme::IdentifyNamespace& identify_namespace_data,
    Span<uint8_t> buffer, uint8_t& identifier_length) {
  // check if nonzero
  bool nguid_nz =
      identify_namespace_data.nguid[0] || identify_namespace_data.nguid[1];
  bool eui64_nz = identify_namespace_data.eui64;

  // Writes the value of the identification at the end of the buffer. This data
  // comes after the DeviceIdentificationVPD and IdentificationDescriptor
  // struct. Hence, the data is written at index +
  // sizeof(IdentificationDescriptor), where index is the size of the
  // DeviceIdentificationVPD struct.
  if (nguid_nz) {
    // Shall be set to 08h if the EUI-64 field is populated from the NVMe EUI64
    // field.
    identifier_length = kIdentifierLengthNguid;
    if (!WriteValue(identify_namespace_data.nguid,
                    buffer.subspan(sizeof(scsi::IdentificationDescriptor)))) {
      DebugLog("Failed to write IdentificationDescriptor to the buffer\n");
      return StatusCode::kFailure;
    }
  } else if (eui64_nz) {
    // Shall be set to 10h if the EUI-64 field is populated from the NVMe NGUID
    // field.
    identifier_length = kIdentifierLengthEui64;
    if (!WriteValue(identify_namespace_data.eui64,
                    buffer.subspan(sizeof(scsi::IdentificationDescriptor)))) {
      DebugLog("Failed to write IdentificationDescriptor to the buffer\n");
      return StatusCode::kFailure;
    }

  } else {
    // function not supported as both NGUID and EUI64 fields are zero
    DebugLog("Both NGUID and EUI-64 fields are zero in IdentityNamespaceData");
    return StatusCode::kFailure;
  }

  scsi::IdentificationDescriptor identification_descriptor = {

      // Shall be set to 1h indicating associated fields are in binary format.
      .code_set = scsi::CodeSet::kBinary,

      // Shall be set to 0h. PIV field shall indicate this field is reserved as
      // no
      // specific protocol to be identified. This field will be ignored!
      .protocol_identifier = scsi::ProtocolIdentifier::kFibreChannel,

      // Shall be set to 2h indicating EUI-64 based identifier.
      .identifier_type = scsi::IdentifierType::kEUI64,

      // Shall be set to 00b indicating DESIGNATOR field is associated with
      // logical unit.
      .association = scsi::Association::kPhysicalDevice,

      // Shall be set to 0b indicating PROTOCOL IDENTIFIER field is reserved.
      .protocol_identifier_valid = 0,

      .identifier_length = identifier_length};
  if (!WriteValue(identification_descriptor, buffer)) {
    DebugLog("Failed to write IdentificationDescriptor to the buffer\n");
    return StatusCode::kFailure;
  };
  return StatusCode::kSuccess;
}

StatusCode TranslateStandardInquiry(
    const nvme::IdentifyControllerData& identify_ctrl,
    const nvme::IdentifyNamespace& identify_ns, Span<uint8_t> buffer) {
  scsi::InquiryData result = {
      .version = scsi::Version::kSpc4,
      .response_data_format = scsi::ResponseDataFormat::kCompliant,
      .additional_length = 0x1f,
      .protect =
          (identify_ns.dps.pit == 0 && identify_ns.dps.md_start == 0) ? 0 : 1,
      .tpgs = scsi::TPGS::kNotSupported,
      .cmdque = 1};

  // Shall be set to “NVMe" followed by 4 spaces: “NVMe    “
  // Vendor Identification is not null terminated.

  memcpy(result.vendor_identification, kNvmeVendorIdentification,
         strlen(kNvmeVendorIdentification));

  // Shall be set to the first 16 bytes of the Model Number (MN) field within
  // the Identify Controller Data Structure
  memcpy(result.product_identification, identify_ctrl.mn,
         sizeof(result.product_identification));

  // Shall be set to the last 4 ASCII graphic characters in the range of 21h-7Eh
  // (i.e. last 4 non-space characters) of the Firmware Revision (FR) field
  // within the Identify Controller Data Structure.
  int idx = 3;
  for (int i = 7; i >= 0; --i) {
    if (identify_ctrl.fr[i] != ' ' && identify_ctrl.fr[i] >= 0x21 &&
        identify_ctrl.fr[i] <= 0x7e) {
      result.product_revision_level[idx] = identify_ctrl.fr[i];
      if (idx-- == 0) break;
    }
  }
  // SCSI specs only require first 36 bytes to be written to the buffer
  if (!WriteValue(result, buffer, 36)) {
    DebugLog("Error writing 36 bytes of Inquiry Data to buffer");
    return StatusCode::kFailure;
  }
  return StatusCode::kSuccess;
}

StatusCode TranslateSupportedVpdPages(Span<uint8_t> buffer) {
  scsi::PageCode supported_page_list[7] = {
      scsi::PageCode::kSupportedVpd,
      scsi::PageCode::kUnitSerialNumber,
      scsi::PageCode::kDeviceIdentification,
      scsi::PageCode::kExtended,
      scsi::PageCode::kBlockLimitsVpd,
      scsi::PageCode::kBlockDeviceCharacteristicsVpd,
      scsi::PageCode::kLogicalBlockProvisioningVpd};

  scsi::SupportedVitalProductData result = {
      .page_length = sizeof(supported_page_list),
  };

  if (!WriteValue(result, buffer) ||
      !WriteValue(supported_page_list, buffer.subspan(sizeof(result)))) {
    DebugLog("Error writing Supported VPD pages or Page List to buffer");
    return StatusCode::kFailure;
  }
  return StatusCode::kSuccess;
}

StatusCode TranslateUnitSerialNumberVpd(
    const nvme::IdentifyControllerData& identify_ctrl,
    const nvme::IdentifyNamespace& identify_ns, uint32_t nsid,
    Span<uint8_t> buffer) {
  scsi::UnitSerialNumber result = {.page_code =
                                       scsi::PageCode::kUnitSerialNumber};

  // converts the nguid or eui64 into a formatter hex string
  // 0x0123456789ABCDEF would be converted to “0123_4567_89AB_CDEF."
  char product_serial_number[40] = {0}, hex_string[33];

  // TOOD: Create shared constants to be used in tests as well when merging all
  // Inquiry paths
  const size_t kNguidLen = 40, kEui64Len = 20, kV1SerialLen = 30;

  // check if nonzero
  bool nguid_nz = identify_ns.nguid[0] || identify_ns.nguid[1];
  bool eui64_nz = identify_ns.eui64;

  if (nguid_nz || eui64_nz) {
    if (nguid_nz) {
      // 6.1.3.1.1
      result.page_length = kNguidLen;

      // convert 128-bit number into hex string, 64-bits at a time
      sprintf(hex_string, "%08lx", identify_ns.nguid[0]);
      sprintf(&hex_string[16], "%08lx", identify_ns.nguid[1]);

    } else if (eui64_nz) {
      // 6.1.3.1.2
      result.page_length = kEui64Len;

      // convert 64-bit number into hex string
      sprintf(hex_string, "%08lx", ltohll(identify_ns.eui64));
    }

    // insert _ and . in the correct positions
    for (int i = 4; i < result.page_length - 1; i += 5) {
      product_serial_number[i] = '_';
    }
    product_serial_number[result.page_length - 1] = '.';

    // insert numbers
    int pos = 0;
    for (int i = 0; i < result.page_length - 1; ++i) {
      if (product_serial_number[i] != '_') {
        product_serial_number[i] = hex_string[pos++];
      }
    }
  } else {
    // 6.1.3.1.3
    // valid for NVMe 1.0 devices only

    result.page_length = kV1SerialLen;

    // PRODUCT SERIAL NUMBER should be formed as follows:
    // Bits 239:80 20 bytes of Serial Number (bytes 23:04 of Identify
    // Controller data structure)
    static_assert(sizeof(identify_ctrl.sn) == 20);
    static_assert(sizeof(product_serial_number) >= 20);
    memcpy(&product_serial_number, identify_ctrl.sn, sizeof(identify_ctrl.sn));

    // Bits 79:72 ASCII representation of “_"
    product_serial_number[sizeof(identify_ctrl.sn)] = '_';

    // Bits 71:08 ASCII representation of 32 bit Namespace Identifier (NSID)
    sprintf(&product_serial_number[sizeof(identify_ctrl.sn) + 1], "%08x", nsid);

    // Bits 07:00 ASCII representation of “."
    product_serial_number[kV1SerialLen - 1] = '.';
  }

  if (!WriteValue(result, buffer) ||
      !WriteValue(product_serial_number, buffer.subspan(sizeof(result)))) {
    DebugLog(
        "Error writing Unit Serial Number or Product Serial Number to buffer");
    return StatusCode::kFailure;
  }
  return StatusCode::kSuccess;
}

// Refer to section 6.1.2 on
// https://nvmexpress.org/wp-content/uploads/NVM_Express_-_SCSI_Translation_Reference-1_5_20150624_Gold.pdf
StatusCode TranslateDeviceIdentificationVpd(
    const nvme::IdentifyNamespace& identify_namespace, Span<uint8_t> buffer) {
  uint8_t identification_descriptor_length = 0;
  StatusCode status = translator::BuildIdentificationDescriptor(
      identify_namespace, buffer.subspan(sizeof(scsi::DeviceIdentificationVpd)),
      identification_descriptor_length);

  if (status != StatusCode::kSuccess) {
    return status;
  }

  uint8_t page_length =
      sizeof(scsi::DeviceIdentificationVpd) + identification_descriptor_length;

  scsi::DeviceIdentificationVpd result = {

      // Shall be set to 00h indicating direct access block device
      .peripheral_device_type = scsi::PeripheralDeviceType::kDirectAccessBlock,

      // Shall be set to 000b indicating device for PERIPHERAL DEVICE TYPE
      // connected to logical unit
      .peripheral_qualifier =
          scsi::PeripheralQualifier::kPeripheralDeviceConnected,

      // Shall be set to 83h indicating Device Identification VPD Page, refer
      // to 6.1.4
      .page_code = scsi::PageCode::kDeviceIdentification,

      // Shall be set to the size of the remaining bytes of Device
      // Identification
      // VPD Page.
      .page_length = page_length,
  };
  if (!WriteValue(result, buffer)) {
    DebugLog("Error! Cannot write DeviceIdentificationVPD to buffer\n");
    return StatusCode::kFailure;
  }
  return status;
}

// Returns Extended Inquiry Data Page. Refer to section 6.1.5
// https://nvmexpress.org/wp-content/uploads/NVM_Express_-_SCSI_Translation_Reference-1_5_20150624_Gold.pdf
StatusCode TranslateExtendedInquiryDataVpd(
    const nvme::IdentifyNamespace& identify_namespace_data,
    const nvme::IdentifyControllerData& identify_controller_data,
    Span<uint8_t> buffer) {
  uint8_t dps_result =
      identify_namespace_data.dps.md_start || identify_namespace_data.dps.pit;
  uint8_t spt = 0;
  StatusCode status = GetSptValueFromDPC(identify_namespace_data, spt);

  if (status != StatusCode::kSuccess) {
    return status;
  }

  scsi::ExtendedInquiryDataVpd extended_inquiry_data = {

      // Shall be set to 00h indicating direct access block device
      .peripheral_device_type = scsi::PeripheralDeviceType::kDirectAccessBlock,

      // Shall be set to 000b indicating device for PERIPHERAL DEVICE TYPE
      // connected to logical unit
      .peripheral_qualifer =
          scsi::PeripheralQualifier::kPeripheralDeviceConnected,

      // Shall be set to 86h indicating Extending INQUIRY Data VPD
      .page_code = scsi::PageCode::kExtended,

      // Shall be set to 3ch
      .page_length = scsi::PageLength::kExtendedInquiryCommand,

      // If DPS field of Identify Namespace Data Structure is 000b, these fields
      // shall be set to 0b, otherwise set to 1b.
      .ref_chk = dps_result,
      .app_chk = dps_result,
      .grd_chk = dps_result,

      // SPT shall be translated using the DPC field of the Identify Namespace
      // Data
      // Structure
      .spt = spt,

      // Shall be set to 10b indicating microcode will be activated after a hard
      // reset.
      .activate_microcode = scsi::ActivateMicrocode::kActivateAfterHardReset,

      // Shall be set to 1b indicating sense key specific data is returned for
      // UNIT
      // ATTENTION sense key.
      .uask_sup = 0b1,

      // Shall be set using the Volatile Write Cache (VMC) field of the Identify
      // Controller Data Structure.
      .v_sup = identify_controller_data.vwc.present,

      // Shall be set to 1b indicating unit attentions are cleared according to
      // SPC-4.
      .luiclr = 0b1,

      // Rest of the fields of this structure shall be set to 0. The Desginator
      // Lists initialize all fields to zero unless they are explicitly declared
      // to ll some other value.
  };
  if (!WriteValue(extended_inquiry_data, buffer)) {
    DebugLog("Couldn't write ExtendedInquiry to buffer\n");
    return StatusCode::kFailure;
  }
  return status;
}

StatusCode TranslateBlockDeviceCharacteristicsVpd(Span<uint8_t> buffer) {
  scsi::BlockDeviceCharacteristicsVpd block_device_characteristics_vpd = {
      // Shall be set to B1h indicating Device Identification VPD Page
      .page_code = scsi::PageCode::kDeviceIdentification,

      // Shall be set to 3Ch.
      .page_length = scsi::PageLength::kBlockDeviceCharacteristicsVpd,

      // Shall be set to 0001h indicating a non-rotating device(SSD)
      .medium_rotation_rate = scsi::MediumRotationRate::kNonRotatingMedium,

      // Shall be set to 0h indicating form factor not reported
      .nominal_form_factor = scsi::NominalFormFactor::kNotReported};

  if (!WriteValue(block_device_characteristics_vpd, buffer)) {
    DebugLog("Couldn't write BlockDeviceCharacteristicsVpd to buffer\n");
    return StatusCode::kFailure;
  }
  return StatusCode::kSuccess;
}

StatusCode TranslateBlockLimitsVpd(
    const nvme::IdentifyControllerData& identify_ctrl, Span<uint8_t> buffer) {
  // The value is in units of the minimum memory
  // page size (CAP.MPSMIN) and is reported as a power of two (2^n).
  // A value of 0h indicates that there is no maximum data transfer size
  uint32_t max_transfer_length;
  if (identify_ctrl.mdts > 16) {
    // Max transfer length should be 2^16 according to section 3.13 of
    // https://www.nvmexpress.org/wp-content/uploads/NVM-Express-SCSI-Translation-Reference-1_1-Gold.pdf
    DebugLog("max transfer length is > 2^16");
    max_transfer_length = 1 << 16;
  } else {
    max_transfer_length = identify_ctrl.mdts ? 1 << identify_ctrl.mdts : 0;
  }

  // TODO: put in common?
  const uint8_t kMaxCompareWriteLen = 255;
  // compare and write length is 8 bits. we ensure the size fits.
  uint8_t compare_and_write_len = (max_transfer_length > kMaxCompareWriteLen)
                                      ? kMaxCompareWriteLen
                                      : max_transfer_length;

  // TODO: named var for page len
  scsi::BlockLimitsVpd result = {
      .page_code = scsi::PageCode::kBlockLimitsVpd,
      .page_length = 0x003c,

      // Shall be set to 00h if Fused Operation is not supported;
      // May be set to a non-zero value that is less than or equal
      // to the value in MAXIMUM TRANSFER LENGTH field if
      // Fused Operation is supported.
      .max_compare_write_length =
          identify_ctrl.fuses.compare_and_write ? compare_and_write_len : 0,

      // Shall be set to value calculated according to method
      // described in NVMe v1.1 Identify Controller Data
      // Structure: Maximum Data Transfer Size (MDTS)

      // The maximum transfer data size is reported as 2^(scsi MDTS)
      // 0 means no max limit
      .max_transfer_length = htonl(max_transfer_length),

      // Shall be set to 0000_0000h if Dataset Management
      // command – Deallocate (AD) attribute is not supported.
      // Shall be set to non-zero value if Dataset Management
      // command – Deallocate (AD) attribute is supported.
      .max_unmap_lba_count =
          htonl(static_cast<uint32_t>(identify_ctrl.oncs.dsm)),

      // Shall be set to 0000_0000h if Dataset Management
      // command – Deallocate (AD) attribute is not supported.
      // Shall be set to 0000_0100h if Dataset Management
      // command – Deallocate (AD) attribute is supported.

      // TODO: add named var for 0x0100
      .max_unmap_block_descriptor_count =
          htonl(identify_ctrl.oncs.dsm ? 0x0100 : 0)};

  if (!WriteValue(result, buffer)) {
    DebugLog("Error writing Block Limits VPD to the buffer");
    return StatusCode::kFailure;
  }
  return StatusCode::kSuccess;
}

StatusCode TranslateLogicalBlockProvisioningVpd(
    const nvme::IdentifyControllerData& identify_ctrl,
    const nvme::IdentifyNamespace& identify_ns, Span<uint8_t> buffer) {
  bool ad = identify_ctrl.oncs.dsm;

  scsi::LogicalBlockProvisioningVpd result = {
      .page_code = scsi::PageCode::kLogicalBlockProvisioningVpd,
      .page_length = htons(0x04),  // TODO: constant val in common.h

      // THRESHOLD_EXPONENT
      // Shall be set to 00h to indicate that there are no thin provisioning
      // thresholds This would require modification if thin-provisioning is
      // supported

      // TODO:
      // Right now we assume this field is 0.
      // If thin provisioning is enabled, we can calculate a valid threshold
      // exponnet from the READ CAPACITY (16) parameter data.

      // .threshold_exponent= identify_ns.nsfeat.thin_prov ? read_capacity : 0,

      // LBPRZ
      // Shall be set to 1 if Dataset Management command – Deallocate (AD)
      // attribute is supported and the device returns all zeros for reads of
      // deallocated LBAs, otherwise set to 0.

      // TODO: the AD attribute itself may not guarantee the device returns all
      // zeros for reads of deallocated LBAs. May have to double check this one
      // if there are any errors.
      .lbprz = ad,

      // ANC_SUP
      // Shall be set to 0, to indicate that setting the ANCHOR bit in UNMAP is
      // not supported if the namespace is not resource or thin provisioned.

  };

  // PROVISIONING TYPE
  // Shall be set to 0 (Full) if Dataset Management command – Deallocate (AD)
  // attribute is not supported and Identify Namespace Data = NSFEAT bit 0 is
  // reported “0” indicating that the namespace is not thin-provisioned.

  // Shall be set to 1(Resource) if Dataset Management command – Deallocate (AD)
  // attribute is supported and Identify Namespace Data – NSFEAT bit 0 is
  // reported “0”,, indicating that the namespace is resource-provisioned.

  // Shall be set to 2(Thin) if Dataset Management command – Deallocate (AD)
  // attribute is supported and Identify Namespace Data – NSFEAT bit 0 is
  // reported “1”, indicating that the namespace is thin-provisioned.
  if (!ad && !identify_ns.nsfeat.thin_prov) {
    result.provisioning_type = 0;
  } else if (ad && !identify_ns.nsfeat.thin_prov) {
    result.provisioning_type = 1;
  } else if (ad && identify_ns.nsfeat.thin_prov) {
    result.provisioning_type = 2;
  }

  // LBPU
  // Shall be set to 0 if Dataset Management command – Deallocate (AD) attribute
  // is not supported. Shall be set to 1 if Dataset Management command –
  // Deallocate (AD) attribute is supported. Shall be set to 1 if PROVISIONING
  // TYPE is set to 1 or 2. This is reporting whether use of UNMAP to unmap LBAs
  // is supported.
  if (ad || result.provisioning_type == 1 || result.provisioning_type == 2) {
    result.lbpu = 1;
  } else {
    result.lbpu = 0;
  }

  if (!WriteValue(result, buffer)) {
    DebugLog("Error writing Logical Block Provisioning VPD to buffer");
    return StatusCode::kFailure;
  }
  return StatusCode::kSuccess;
}

}  // namespace

StatusCode InquiryToNvme(Span<const uint8_t> raw_scsi,
                         NvmeCmdWrapper& identify_ns_wrapper,
                         NvmeCmdWrapper& identify_ctrl_wrapper,
                         uint32_t page_size, uint32_t nsid,
                         Span<Allocation> allocations, uint32_t& alloc_len) {
  scsi::InquiryCommand cmd = {};
  if (!ReadValue(raw_scsi, cmd)) {
    DebugLog("Malformed Inquiry Command");
    return StatusCode::kInvalidInput;
  };

  alloc_len = static_cast<uint32_t>(ntohs(cmd.allocation_length));

  uint16_t num_pages = 1;
  StatusCode status_alloc1 = allocations[0].SetPages(page_size, num_pages, 0);
  if (status_alloc1 != StatusCode::kSuccess) {
    return status_alloc1;
  }

  identify_ns_wrapper.cmd = nvme::GenericQueueEntryCmd{
      .opc = static_cast<uint8_t>(nvme::AdminOpcode::kIdentify), .nsid = nsid};
  identify_ns_wrapper.cmd.dptr.prp.prp1 = allocations[0].data_addr;
  identify_ns_wrapper.cmd.cdw[0] =
      0x0;  // Controller or Namespace Structure (CNS): This field specifies the
            // information to be returned to the host.
  identify_ns_wrapper.buffer_len = page_size * num_pages;

  StatusCode status_alloc2 = allocations[1].SetPages(page_size, num_pages, 0);
  if (status_alloc2 != StatusCode::kSuccess) {
    return status_alloc2;
  }

  identify_ctrl_wrapper.cmd = nvme::GenericQueueEntryCmd{
      .opc = static_cast<uint8_t>(nvme::AdminOpcode::kIdentify),
  };
  identify_ctrl_wrapper.cmd.dptr.prp.prp1 = allocations[1].data_addr;
  identify_ctrl_wrapper.cmd.cdw[0] =
      htoll(0x1);  // Controller or Namespace Structure (CNS): This field
                   // specifies the information to be returned to the host.
  identify_ctrl_wrapper.buffer_len = page_size * num_pages;

  identify_ns_wrapper.is_admin = true;
  identify_ctrl_wrapper.is_admin = true;
  return StatusCode::kSuccess;
}

// Main logic engine for the Inquiry command
StatusCode InquiryToScsi(Span<const uint8_t> raw_scsi, Span<uint8_t> buffer,
                         const nvme::GenericQueueEntryCmd& identify_ns,
                         const nvme::GenericQueueEntryCmd& identify_ctrl) {
  scsi::InquiryCommand inquiry_cmd = {};

  if (!ReadValue(raw_scsi, inquiry_cmd)) {
    DebugLog("Malformed Inquiry Command");
    return StatusCode::kInvalidInput;
  };

  uint8_t* ns_dptr = reinterpret_cast<uint8_t*>(identify_ns.dptr.prp.prp1);
  Span<uint8_t> ns_span(ns_dptr, sizeof(nvme::IdentifyNamespace));

  uint8_t* ctrl_dptr = reinterpret_cast<uint8_t*>(identify_ctrl.dptr.prp.prp1);
  Span<uint8_t> ctrl_span(ctrl_dptr, sizeof(nvme::IdentifyControllerData));

  const nvme::IdentifyNamespace* identify_ns_data =
      SafePointerCastRead<nvme::IdentifyNamespace>(ns_span);
  if (identify_ns_data == nullptr) {
    DebugLog("Identify namespace structure failed to cast");
    return StatusCode::kFailure;
  }

  const nvme::IdentifyControllerData* identify_ctrl_data =
      SafePointerCastRead<nvme::IdentifyControllerData>(ctrl_span);
  if (identify_ctrl_data == nullptr) {
    DebugLog("Identify controller structure failed to cast");
    return StatusCode::kFailure;
  }

  // nsid should come from Namespace
  uint32_t nsid = identify_ns.nsid;

  if (inquiry_cmd.evpd) {
    switch (inquiry_cmd.page_code) {
      case scsi::PageCode::kSupportedVpd:
        // Return Supported Vpd Pages data page to application client, refer
        // to 6.1.2.
        return TranslateSupportedVpdPages(buffer);
      case scsi::PageCode::kUnitSerialNumber:
        // Return Unit Serial Number data page toapplication client.
        // Referto 6.1.3.
        return TranslateUnitSerialNumberVpd(*identify_ctrl_data,
                                            *identify_ns_data, nsid, buffer);
      case scsi::PageCode::kDeviceIdentification:
        return TranslateDeviceIdentificationVpd(*identify_ns_data, buffer);
      case scsi::PageCode::kExtended:
        return TranslateExtendedInquiryDataVpd(*identify_ns_data,
                                               *identify_ctrl_data, buffer);
      case scsi::PageCode::kBlockLimitsVpd:
        // May be supported by returning Block Limits VPD data page to
        // application client, refer to 6.1.6.
        return TranslateBlockLimitsVpd(*identify_ctrl_data, buffer);
      case scsi::PageCode::kBlockDeviceCharacteristicsVpd:
        // Return Block Device Characteristics Vpd Page to application
        // client, refer to 6.1.7.
        return TranslateBlockDeviceCharacteristicsVpd(buffer);
      case scsi::PageCode::kLogicalBlockProvisioningVpd:
        return TranslateLogicalBlockProvisioningVpd(*identify_ctrl_data,
                                                    *identify_ns_data, buffer);
      // May be supported by returning Logical Block Provisioning VPD Page to
      // application client, refer to 6.1.8.
      default:
        // Command may be terminated with CHECK CONDITION status, ILLEGAL
        // REQUEST dense key, and ILLEGAL FIELD IN CDB additional sense code
        DebugLog("Inquiry Command parameters do not map to any action.");
        return StatusCode::kInvalidInput;
    }
  } else {
    // Return Standard INQUIRY Data to application client
    return TranslateStandardInquiry(*identify_ctrl_data, *identify_ns_data,
                                    buffer);
  }
  return StatusCode::kSuccess;
}

};  // namespace translator
