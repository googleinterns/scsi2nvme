#ifndef LIB_TRANSLATOR_COMMON_H
#define LIB_TRANSLATOR_COMMON_H

namespace translator {

const char NVME_VENDOR_IDENTIFICATION[9] = "NVMe    ";

static void (*debug_callback)(const char*);

enum class StatusCode { kSuccess, kInvalidInput, kNoTranslation, kFailure };

static void setDebugCallback(void (*callback)(const char*)) {
  debug_callback = callback;
}  

} // namespace translator
#endif