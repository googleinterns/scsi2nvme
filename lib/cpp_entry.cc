/*
 * C++ component inside the Linux kernel
 */

#include "cpp_entry.h"

#include <linux/kernel.h>
#include <linux/module.h>

#include <cstring>
#include <fcntl.h>

#include "print.h"
#include "scsi_defs.h"
#include "nvme.h"

/* C++ functions */
void init(void) {
  print("Hello, world!\n");
  scsi_defs::ControlByte cb;
  uint8_t val = 0x04;
  memcpy(&cb, &val, 1);
  print("%u\n", cb.naca);

  int file_descriptor = open("", O_RDWR);
  print("file descriptor %d" , file_descriptor);
  int nvme_code = NVME_IOCTL_IO_CMD;
  uint8_t opcode = 0x02;
  uint8_t flags = 0x0;
  uint16_t rsvd1 = 0x0;
  uint32_t nsid = 0x0;
  uint32_t cdw2 = 0x0;
  uint32_t cdw3 = 0x0; 
  void* metadata = NULL;
  void* addr = NULL;
  uint32_t metadata_len = 0x0;
  uint32_t data_len = 0x0;
  uint32_t cdw10 = 0x0;
  uint32_t cdw11 = 0x0;
  uint32_t cdw12 = 0x0;
  uint32_t cdw13 = 0x0;
  uint32_t cdw14 = 0x0;
  uint32_t cdw15 = 0x0;
  uint32_t timeout_ms = 0x0;
  uint32_t* result = 0;
  int status = nvme::send_passthru(file_descriptor, nvme_code, opcode, flags, rsvd1, nsid, 
  cdw2, cdw3, metadata, addr, metadata_len, data_len, cdw10, cdw11, cdw12, 
  cdw13, cdw14, cdw15, timeout_ms, result);
  print("status is: %d", status);
}

void release(void) { print("Goodbye, world!\n"); }
