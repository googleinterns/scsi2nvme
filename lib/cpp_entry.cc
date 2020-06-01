/*
 * C++ component inside the Linux kernel
 */
 
#include <linux/kernel.h>
#include <linux/module.h>

#include <cstring>

#include "cpp_entry.h"
#include "print.h"
#include "scsi_defs.h"

/* C++ functions */
void init(void) {
  print("Hello, world!\n");
  scsi_defs::ControlByte cb;
  uint8_t val = 0x04;
  memcpy(&cb, &val, 1);
  print("%u\n", cb.naca);
}

void release(void) {
  print("Goodbye, world!\n");
}
