#include "engine.h"

#include <cstdint>

#include "nvme_driver.h"
#include "lib/translator/translation.h"

namespace {

uint64_t AllocPagesCallback(uint16_t count) {
  return AllocPages(count);
}

void DeallocPagesCallback(uint64_t addr, uint16_t count) {
  DeallocPages(addr, count);
}

}  // namespace

void SetEngineCallbacks(void) {
  translator::SetDebugCallback(Print);
  translator::SetAllocPageCallbacks(AllocPagesCallback, DeallocPagesCallback);
}

ScsiToNvmeResponse ScsiToNvme(unsigned char* cmd_buf, unsigned short cmd_len,
  unsigned long long lun, unsigned char* sense_buf, unsigned short sense_len,
  unsigned char* data_buf, unsigned short data_len, bool is_data_in) {
  // Create translation object
  translator::Translation translation;
  
  // Package parameters and run translation begin
  translator::Span<uint8_t> scsi_cmd(cmd_buf, cmd_len);
  translator::BeginResponse begin_resp = translation.Begin(scsi_cmd, lun);
  
  if (begin_resp.alloc_len > data_len) {
    Print("Specified allocation length exceeds buffer size. Possible malicious request?");
    ScsiToNvmeResponse resp = {
      .return_code = 0,
      .alloc_len = 0
    };
    return resp;
  }
  
  // Grab NVMe cmds and call NVMe interface
  translator::Span<const translator::NvmeCmdWrapper> nvme_wrappers = translation.GetNvmeWrappers();
  nvme::GenericQueueEntryCpl cpl_buf[nvme_wrappers.size()] = {};
  for (uint32_t i = 0; i < nvme_wrappers.size(); ++i) {
    NvmeCommand tmp_cmd;
    NvmeCompletion tmp_cpl = {};
    memcpy(&tmp_cmd, &nvme_wrappers[i].cmd, sizeof(tmp_cmd));
    static_assert(sizeof(tmp_cmd) == sizeof(nvme_wrappers[i].cmd));
    void* buffer = reinterpret_cast<void*>(nvme_wrappers[i].cmd.dptr.prp.prp1);
    unsigned bufflen = 4096;
    if (nvme_wrappers[i].is_admin) {
      submit_admin_command(&tmp_cmd, buffer, bufflen, &tmp_cpl, 60);
    } else {
      Print("Submitting IO Command");
      submit_io_command(&tmp_cmd, buffer, bufflen, &tmp_cpl, 60);
    }
    memcpy(&cpl_buf[i], &tmp_cpl, sizeof(cpl_buf[i]));
    static_assert(sizeof(cpl_buf[i]) == sizeof(tmp_cpl));
  }
  
  // Use NVMe completion responses to Complete translation
  translator::Span<nvme::GenericQueueEntryCpl> nvme_cpl(cpl_buf, nvme_wrappers.size());
  translator::Span<uint8_t> buffer_in;
  if (is_data_in)
    buffer_in = translator::Span(data_buf, begin_resp.alloc_len);
  translator::Span<uint8_t> sense_buffer(sense_buf, sense_len);
  translator::CompleteResponse cpl_resp = translation.Complete(nvme_cpl, buffer_in, sense_buffer);
  
  ScsiToNvmeResponse resp = {
    .return_code = static_cast<uint8_t>(cpl_resp.scsi_status),
    .alloc_len = begin_resp.alloc_len
  };
  
  return resp;
}
