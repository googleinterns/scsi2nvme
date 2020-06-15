#include "print.h"

#include <linux/kernel.h>

void print(const char* p, ...) {
  va_list args;
  va_start(args, p);
  vprintk(p, args);
  va_end(args);
}
