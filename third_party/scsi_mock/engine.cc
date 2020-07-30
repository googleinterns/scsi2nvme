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
  translator::Span<const nvme::GenericQueueEntryCmd> nvme_cmds = translation.GetNvmeCmds();
  nvme::GenericQueueEntryCpl cpl_buf[nvme_cmds.size()] = {};
  for (uint32_t i = 0; i < nvme_cmds.size(); ++i) {
    NvmeCommand tmp_cmd;
    memcpy(&tmp_cmd, &nvme_cmds[i], sizeof(tmp_cmd));
    static_assert(sizeof(tmp_cmd) == sizeof(nvme_cmds[i]));
    void* buffer = reinterpret_cast<void*>(nvme_cmds[i].dptr.prp.prp1);
    unsigned bufflen = 4096;
    uint32_t result = 0;
    submit_io_command(&tmp_cmd, buffer, bufflen, &result, 60);
    cpl_buf[i].cdw0 = result;
  }
  
  // Use NVMe completion responses to Complete translation
  translator::Span<nvme::GenericQueueEntryCpl> nvme_cpl;
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
