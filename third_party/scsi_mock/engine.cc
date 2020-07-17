#include "engine.h"

void RunScsiCommand(const struct scsi_cmnd* cmd) {

}

void ScsiToNvme(unsigned char* cmd_buf, unsigned short cmd_len,
  uint64_t lun, unsigned char* sense_buffer, unsigned short sense_buffer_len,
  unsigned char* data, unsigned short data_len, bool dataIn);
