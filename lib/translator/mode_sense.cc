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

#include "mode_sense.h"

#ifdef __KERNEL__
#include <linux/byteorder/generic.h>
#else
#include <netinet/in.h>
#endif

#include <byteswap.h>

namespace translator {

namespace {

// Section 6.3.3.1
// https://www.nvmexpress.org/wp-content/uploads/NVM-Express-SCSI-Translation-Reference-1_1-Gold.pdf
constexpr scsi::CachingModePage kCachingModePage = {
    .page_code = scsi::ModePageCode::kCacheMode,
    .spf = 0,
    .ps = 0,
    .page_length = 0x12,
    .ic = 1,
};

// Section 6.3.3.2
// https://www.nvmexpress.org/wp-content/uploads/NVM-Express-SCSI-Translation-Reference-1_1-Gold.pdf
constexpr scsi::ControlModePage kControlModePage = {
    .page_code = scsi::ModePageCode::kControlMode,
    .spf = 0,
    .ps = 0,
    .page_length = 0x0a,
    .rlec = 0,
    .gltsd = 1,
    .d_sense = 1,
    .dpicz = 0,
    .tmf_only = 0,
    .tst = 0,
    .qerr = 0x01,
    .nuar = 0,
    .qam = 0,
    .swp = 0,
    .ua_intlck_ctrl = 0,
    .rac = 0,
    .autoload_mode = 0,
    .tas = 0x01,
    .ato = 0,
    .busy_timeout_period = 0xFFFF,
    .estct = 0};

// Section 6.3.3.3
// https://www.nvmexpress.org/wp-content/uploads/NVM-Express-SCSI-Translation-Reference-1_1-Gold.pdf
constexpr scsi::PowerConditionModePage kPowerConditionModePage = {
    .page_code = scsi::ModePageCode::kPowerConditionMode,
    .spf = 0,
    .ps = 0,
    .page_length = 0x26,
    .pm_bg_precedence = 0};

struct CommonCmdAttributes {
  scsi::ModePageCode page_code;
  scsi::PageControl pc;
  bool dbd;    // Disable block descriptors
  bool llbaa;  // Long LBA accepted
};

// Generates an nvme identify command for fetching block descriptor data
StatusCode GenerateBlockDescriptorIdentifyCmd(NvmeCmdWrapper& nvme_wrapper,
                                              Allocation& allocation,
                                              uint32_t nsid) {
  nvme_wrapper.cmd = {.opc = static_cast<uint8_t>(nvme::AdminOpcode::kIdentify),
                      .nsid = nsid};
  if (allocation.SetPages(1, 0) == StatusCode::kFailure) {
    return StatusCode::kFailure;
  }
  DebugLog("allocation data addr %llu", allocation.data_addr);  // TODO investigate: a log needs to exist here otherwise the
                 // engine gets stuck during startup
  nvme_wrapper.cmd.dptr.prp.prp1 = allocation.data_addr;
  DebugLog("allocation data addr %llu", nvme_wrapper.cmd.dptr.prp.prp1);  // TODO investigate: a log needs to exist here otherwise the

  nvme_wrapper.is_admin = true;
	return StatusCode::kSuccess;
}

// Generates an nvme get features command for fetching cache features
StatusCode GenerateCacheGetFeaturesCmd(scsi::PageControl pc, uint32_t nsid,
                                       NvmeCmdWrapper& nvme_wrapper) {
																			 DebugLog("Starting 1");
  nvme::GetFeaturesCmd tmp_get_features = {};
  tmp_get_features.opc = static_cast<uint8_t>(nvme::AdminOpcode::kGetFeatures);
  tmp_get_features.nsid = nsid;
  switch (pc) {
    case scsi::PageControl::kCurrent:
      tmp_get_features.sel = nvme::FeatureSelect::kCurrent;
      break;
    case scsi::PageControl::kChangeable:
      tmp_get_features.sel = nvme::FeatureSelect::kSaved;
      break;
    case scsi::PageControl::kDefault:
      tmp_get_features.sel = nvme::FeatureSelect::kDefault;
      break;
    default:
      DebugLog("Unsupported page control recieved");
      return StatusCode::kFailure;
  }
  tmp_get_features.fid = nvme::FeatureType::kVolatileWriteCache;
  memcpy(&nvme_wrapper.cmd, &tmp_get_features,
         sizeof(nvme::GenericQueueEntryCmd));
  static_assert(sizeof(nvme_wrapper.cmd) == sizeof(tmp_get_features));

  nvme_wrapper.is_admin = true;
																			 DebugLog("Ending1");
  return StatusCode::kSuccess;
}

// Calculates the size of the mode sense 6 response
uint16_t GetModeDataLength(CommonCmdAttributes cmd_attributes,
                           bool is_mode_10) {
																			 DebugLog("Starting 2");
  uint32_t total_size = 0;
  // Subtract size of mode data length field per scsi spec
  // 1 byte for mode 6, 2 bytes for mode 10
  if (is_mode_10)
    total_size += sizeof(scsi::ModeParameter10Header) - 2;
  else
    total_size += sizeof(scsi::ModeParameter6Header) - 1;
  if (!cmd_attributes.dbd) {
    if (cmd_attributes.llbaa)
      total_size += sizeof(scsi::LongLbaBlockDescriptor);
    else
      total_size += sizeof(scsi::ShortLbaBlockDescriptor);
  }
  switch (cmd_attributes.page_code) {
    case scsi::ModePageCode::kCacheMode:
      total_size += sizeof(scsi::CachingModePage);
      break;
    case scsi::ModePageCode::kControlMode:
      total_size += sizeof(scsi::ControlModePage);
      break;
    case scsi::ModePageCode::kPowerConditionMode:
      total_size += sizeof(scsi::PowerConditionModePage);
      break;
    case scsi::ModePageCode::kAllSupportedModes:
      total_size += sizeof(scsi::CachingModePage) +
                    sizeof(scsi::ControlModePage) +
                    sizeof(scsi::PowerConditionModePage);
  }
																			 DebugLog("Ending2");
  return total_size;
}

// Handles common logic between mode sense 6 and 10 to nvme
StatusCode ModeSenseToNvme(CommonCmdAttributes cmd_attributes,
                           Span<NvmeCmdWrapper> nvme_wrappers,
                           Allocation& allocation, uint32_t nsid,
                           uint32_t& cmd_count) {
													 DebugLog("Building mode sense nvme at count %d\n", cmd_count);
		DebugLog("Page code %d", cmd_attributes.page_code);
  // Handle block descriptors
  if (!cmd_attributes.dbd) {
    GenerateBlockDescriptorIdentifyCmd(nvme_wrappers[cmd_count++], allocation,
                                       nsid);
  }

		DebugLog("Page code %d", cmd_attributes.page_code);
  // Validate page code
  if (cmd_attributes.page_code != scsi::ModePageCode::kCacheMode &&
      cmd_attributes.page_code != scsi::ModePageCode::kAllSupportedModes) {
    if (cmd_attributes.page_code == scsi::ModePageCode::kControlMode ||
        cmd_attributes.page_code == scsi::ModePageCode::kPowerConditionMode) {
      return StatusCode::kSuccess;
    }
    DebugLog("Unsuppported mode sense page code recieved");
    return StatusCode::kFailure;
  }

  // Configure NVMe get features cmd
  return GenerateCacheGetFeaturesCmd(cmd_attributes.pc, nsid,
                                     nvme_wrappers[cmd_count++]);
}

// Writes a mode parameter 6 header to buffer
// Table 6-15
// https://www.nvmexpress.org/wp-content/uploads/NVM-Express-SCSI-Translation-Reference-1_1-Gold.pdf
bool WriteMode6Header(CommonCmdAttributes cmd_attributes,
                      Span<uint8_t> buffer) {
  scsi::ModeParameter6Header header = {};
  header.mode_data_length = GetModeDataLength(cmd_attributes, false);
  header.dpofua = 0b01;
  if (!cmd_attributes.dbd)
    header.bdl = sizeof(scsi::ShortLbaBlockDescriptor);
  else
    header.bdl = 0;
  if (!WriteValue(header, buffer)) {
    DebugLog("Insufficient size for mode 6 parameter header");
    return false;
  }
  return true;
}

// Writes a mode parameter 10 header to buffer
bool WriteMode10Header(CommonCmdAttributes cmd_attributes,
                       Span<uint8_t> buffer) {
  scsi::ModeParameter10Header header = {};
  header.mode_data_length = htons(GetModeDataLength(cmd_attributes, true));
  header.dpofua = 0b01;
  header.longlba = cmd_attributes.llbaa;
  if (!cmd_attributes.dbd) {
    if (cmd_attributes.llbaa)
      header.bdl = htons(sizeof(scsi::LongLbaBlockDescriptor));
    else
      header.bdl = htons(sizeof(scsi::ShortLbaBlockDescriptor));
  } else {
    header.bdl = 0;
  }
  if (!WriteValue(header, buffer)) {
    DebugLog("Insufficient size for mode 10 parameter header");
    return false;
  }
  return true;
}

// Writes a device block descriptor to buffer
// Section 6.3.2
// https://www.nvmexpress.org/wp-content/uploads/NVM-Express-SCSI-Translation-Reference-1_1-Gold.pdf
bool WriteBlockDescriptor(const nvme::GenericQueueEntryCmd& identify,
                          Span<uint8_t> buffer, bool llbaa,
                          uint32_t& write_len) {
  uint8_t* ns_dptr = reinterpret_cast<uint8_t*>(identify.dptr.prp.prp1);
  Span<uint8_t> ns_span(ns_dptr, sizeof(nvme::IdentifyNamespace));
  const nvme::IdentifyNamespace* idns;
  idns = SafePointerCastRead<nvme::IdentifyNamespace>(ns_span);
  if (llbaa) {
		DebugLog("llbaa");
    write_len = sizeof(scsi::LongLbaBlockDescriptor);
    scsi::LongLbaBlockDescriptor lbd = {};
    lbd.number_of_blocks = bswap_64(idns->ncap);
    // Figure 246
    // https://nvmexpress.org/wp-content/uploads/NVM-Express-1_4-2019.06.10-Ratified.pdf
    lbd.logical_block_length =
        htonl(1 << (idns->lbaf[idns->flbas.format].lbads));
    return WriteValue(lbd, buffer);
  } else {
		DebugLog("not llbaa");
    write_len = sizeof(scsi::ShortLbaBlockDescriptor);
    scsi::ShortLbaBlockDescriptor sbd = {};
		DebugLog("not llbaa");
    sbd.number_of_blocks = htonl(static_cast<uint32_t>(ltohll(idns->ncap)));
		DebugLog("not llbaa");
    // Figure 246
    // https://nvmexpress.org/wp-content/uploads/NVM-Express-1_4-2019.06.10-Ratified.pdf
    uint32_t lbl_be = htonl(1 << (idns->lbaf[idns->flbas.format].lbads));
    if (IsLittleEndian()) lbl_be >>= 8;
    sbd.logical_block_length = lbl_be;
    return WriteValue(sbd, buffer);
  }
}

// Writes corresponding mode page data to buffer
bool WritePageData(scsi::ModePageCode page_code, uint32_t get_features_result,
                   Span<uint8_t> buffer) {
  switch (page_code) {
    case scsi::ModePageCode::kCacheMode: {
      scsi::CachingModePage tmp_page = kCachingModePage;
      // Figure 281 NVMe Spec
      tmp_page.wce = 0b1 & get_features_result;
      return WriteValue(tmp_page, buffer);
    }
    case scsi::ModePageCode::kControlMode: {
      return WriteValue(kControlModePage, buffer);
    }
    case scsi::ModePageCode::kPowerConditionMode: {
      return WriteValue(kPowerConditionModePage, buffer);
    }
    case scsi::ModePageCode::kAllSupportedModes: {
      scsi::CachingModePage tmp_page = kCachingModePage;
      // Figure 281
      // https://nvmexpress.org/wp-content/uploads/NVM-Express-1_4-2019.06.10-Ratified.pdf
      tmp_page.wce = 0b1 & get_features_result;
      if (!WriteValue(tmp_page, buffer)) return false;
      buffer = buffer.subspan(sizeof(kCachingModePage));
      if (!WriteValue(kControlModePage, buffer)) return false;
      buffer = buffer.subspan(sizeof(kControlModePage));
      return WriteValue(kPowerConditionModePage, buffer);
    }
  }
}

StatusCode ModeSenseToScsi(CommonCmdAttributes cmd_attributes, bool is_mode_10,
                           uint32_t get_features_result,
                           const nvme::GenericQueueEntryCmd& identify,
                           Span<uint8_t> buffer) {
  // Create header
  if (is_mode_10) {
    if (!WriteMode10Header(cmd_attributes, buffer)) return StatusCode::kSuccess;
    buffer = buffer.subspan(sizeof(scsi::ModeParameter10Header));
  } else {
    if (!WriteMode6Header(cmd_attributes, buffer)) return StatusCode::kSuccess;
    buffer = buffer.subspan(sizeof(scsi::ModeParameter6Header));
  }

DebugLog("Gottne here");
  // Populate block descriptor
  if (!cmd_attributes.dbd) {
    uint32_t write_len = 0;
    if (!WriteBlockDescriptor(identify, buffer, cmd_attributes.llbaa,
                              write_len)) {
      DebugLog("Truncating mode sense response at block descriptor");
      return StatusCode::kSuccess;
    }
    buffer = buffer.subspan(write_len);
  }

  // Append page data
  if (!WritePageData(cmd_attributes.page_code, get_features_result, buffer)) {
    DebugLog("Failed to write variable length mode-page data");
  }
  return StatusCode::kSuccess;
}

}  // namespace

// Section 4.4
// https://www.nvmexpress.org/wp-content/uploads/NVM-Express-SCSI-Translation-Reference-1_1-Gold.pdf
StatusCode ModeSense6ToNvme(Span<const uint8_t> scsi_cmd,
                            Span<NvmeCmdWrapper> nvme_wrappers,
                            Allocation& allocation, uint32_t nsid,
                            uint32_t& cmd_count, uint32_t& alloc_len) {
  // cast scsi_cmd to Mode Sense 6 command
  scsi::ModeSense6Command ms6_cmd;
  if (!ReadValue(scsi_cmd, ms6_cmd)) {
    DebugLog("Mode Sense 6 Command Malformed");
    return StatusCode::kFailure;
  }

  alloc_len = ms6_cmd.alloc_length;
  CommonCmdAttributes cmd_attributes = {.page_code = ms6_cmd.page_code,
                                        .pc = ms6_cmd.pc,
                                        .dbd = ms6_cmd.dbd,
                                        .llbaa = false};
  return ModeSenseToNvme(cmd_attributes, nvme_wrappers, allocation, nsid,
                         cmd_count);
}

// Section 4.4
// https://www.nvmexpress.org/wp-content/uploads/NVM-Express-SCSI-Translation-Reference-1_1-Gold.pdf
StatusCode ModeSense10ToNvme(Span<const uint8_t> scsi_cmd,
                             Span<NvmeCmdWrapper> nvme_wrappers,
                             Allocation& allocation, uint32_t nsid,
                             uint32_t& cmd_count, uint32_t& alloc_len) {
  // cast scsi_cmd to Mode Sense 10 command
  scsi::ModeSense10Command ms10_cmd;
  if (!ReadValue(scsi_cmd, ms10_cmd)) {
    DebugLog("Mode Sense 10 Command Malformed");
    return StatusCode::kFailure;
  }

  alloc_len = ntohs(ms10_cmd.alloc_length);
  CommonCmdAttributes cmd_attributes = {.page_code = ms10_cmd.page_code,
                                        .pc = ms10_cmd.pc,
                                        .dbd = ms10_cmd.dbd,
                                        .llbaa = ms10_cmd.llbaa};
  return ModeSenseToNvme(cmd_attributes, nvme_wrappers, allocation, nsid,
                         cmd_count);
}

// Section 6.3
// https://www.nvmexpress.org/wp-content/uploads/NVM-Express-SCSI-Translation-Reference-1_1-Gold.pdf
StatusCode ModeSense6ToScsi(Span<const uint8_t> scsi_cmd,
                            const nvme::GenericQueueEntryCmd& identify,
                            uint32_t get_features_result,
                            Span<uint8_t> buffer) {
  // cast scsi_cmd to Mode Sense 6 command
  scsi::ModeSense6Command ms6_cmd;
  if (!ReadValue(scsi_cmd, ms6_cmd)) {
    DebugLog("Mode Sense 6 Command Malformed");
    return StatusCode::kFailure;
  }

  CommonCmdAttributes cmd_attributes = {.page_code = ms6_cmd.page_code,
                                        .pc = ms6_cmd.pc,
                                        .dbd = ms6_cmd.dbd,
                                        .llbaa = false};
  return ModeSenseToScsi(cmd_attributes, false, get_features_result, identify,
                         buffer);
}

// Section 6.3
// https://www.nvmexpress.org/wp-content/uploads/NVM-Express-SCSI-Translation-Reference-1_1-Gold.pdf
StatusCode ModeSense10ToScsi(Span<const uint8_t> scsi_cmd,
                             const nvme::GenericQueueEntryCmd& identify,
                             uint32_t get_features_result,
                             Span<uint8_t> buffer) {
  // cast scsi_cmd to Mode Sense 10 command
  scsi::ModeSense10Command ms10_cmd;
  if (!ReadValue(scsi_cmd, ms10_cmd)) {
    DebugLog("Mode Sense 10 Command Malformed");
    return StatusCode::kFailure;
  }

  CommonCmdAttributes cmd_attributes = {.page_code = ms10_cmd.page_code,
                                        .pc = ms10_cmd.pc,
                                        .dbd = ms10_cmd.dbd,
                                        .llbaa = ms10_cmd.llbaa};
  return ModeSenseToScsi(cmd_attributes, true, get_features_result, identify,
                         buffer);
}

};  // namespace translator
