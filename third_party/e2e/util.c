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

const int NVME_MIN_PAGE_SIZE = 4096;

void Print(const char* msg) { printk(msg); }

unsigned long long AllocPages(unsigned short count) {
  if (count == 0) return 0;
  printk("Allocating %u bytes", NVME_MIN_PAGE_SIZE * count);
  void* addr = kzalloc(NVME_MIN_PAGE_SIZE * count, GFP_ATOMIC | GFP_KERNEL);
	printk("Address %llu",(unsigned long long) addr);
  if (addr == NULL) printk("NULLPTR ALLOC PAGES!!!!");
  return (unsigned long long)addr;
}

void DeallocPages(unsigned long long addr, unsigned short count) {
  if (addr != 0) kfree((void*)addr);
}
