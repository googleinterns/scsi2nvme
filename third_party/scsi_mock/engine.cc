#include "engine.h"

#include <cstdint>

#include "lib/translator/translation.h"

void SetEngineCallbacks(void) {
  translator::SetDebugCallback(print);
}

void ScsiToNvme(unsigned char* cmd_buf, unsigned short cmd_len,
  unsigned long long lun, unsigned char* sense_buf, unsigned short sense_len,
  unsigned char* data_buf, unsigned short data_len, bool isDataIn) {
  translator::Translation t;
  
}
