// Copyright 2020 Google LLC
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// version 2 as published by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.

#include "util.h"

#include <linux/slab.h>

void Print(const char* msg) { printk(msg); }

uint64_t AllocPages(uint32_t page_size, uint16_t count) {
  if (count == 0) return 0;
  printk("Allocating %u bytes", page_size * count);
  void* addr = kzalloc(page_size * count, GFP_ATOMIC | GFP_KERNEL);
  if (addr == NULL) printk("NULLPTR ALLOC PAGES!!!!");
  return (unsigned long long)addr;
}

void DeallocPages(uint64_t addr, uint16_t count) {
  if (addr != 0) kfree((void*)addr);
}
