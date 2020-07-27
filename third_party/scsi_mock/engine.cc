#include "engine.h"

#include <cstdint>

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

void ScsiToNvme(unsigned char* cmd_buf, unsigned short cmd_len,
  unsigned long long lun, unsigned char* sense_buf, unsigned short sense_len,
  unsigned char* data_buf, unsigned short data_len, bool is_data_in) {
  // Create translation object
  translator::Translation translation;
  
  // Package parameters and run translation begin
  translator::Span<uint8_t> scsi_cmd(cmd_buf, cmd_len);
  translator::BeginResponse begin_resp = translation.Begin(scsi_cmd, lun);
  
  // Allocate data-in buffer space
  uint8_t data_in[begin_resp.alloc_len];
  
  // Grab NVMe cmds and call NVMe interface
  
  
  // Use NVMe completion responses to Complete translation
  translator::Span<nvme::GenericQueueEntryCpl> nvme_cpl;
  translator::Span<uint8_t> buffer_in;
  if (is_data_in)
    buffer_in = translator::Span(data_in, begin_resp.alloc_len);
  translator::Span<uint8_t> sense_buffer(sense_buf, sense_len);
  translator::CompleteResponse cpl_resp = translation.Complete(nvme_cpl, buffer_in, sense_buffer);
  
  
}
