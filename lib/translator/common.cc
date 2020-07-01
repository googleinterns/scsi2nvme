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
// limitations under the License.i// Copyright 2020 Google LLC

#include "common.h"

#include <stdarg.h>
#include <stdio.h>

namespace translator {

static void (*debug_callback)(const char*);
static void* (*alloc_pages_callback)(uint16_t);
static void* (*dealloc_pages_callback)(void*, uint16_t);

void DebugLog(const char* format, ...) {
  if (debug_callback == nullptr) return;
  char buffer[1024];
  va_list args;
  va_start(args, format);
  vsnprintf(buffer, 1024, format, args);
  debug_callback(buffer);
  va_end(args);
}

void SetDebugCallback(void (*callback)(const char*)) {
  debug_callback = callback;
}

void* AllocPages(uint16_t count) {
  if (alloc_pages_callback == nullptr) return nullptr;
  return alloc_pages_callback(count);
}

void* DeallocPages(void* pages, uint16_t count) {
  if (dealloc_pages_callback == nullptr) return nullptr;
  return dealloc_pages_callback(pages, count);
}

void SetAllocPageCallbacks(void* (*alloc_callback)(uint16_t),
                           void* (*dealloc_callback)(void*, uint16_t)) {
  alloc_pages_callback = alloc_callback;
  dealloc_pages_callback = dealloc_callback;
}

}  // namespace translator
