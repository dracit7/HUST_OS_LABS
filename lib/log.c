
#include "log.h"

void _fault(const char* fmt, char* FILE, int LINE, ...) {

  printf("[ERROR] <%s:%d> ", FILE, LINE);

  va_list args;

  va_start(args, LINE);
  vprintf(fmt, args);
  va_end(args);

  exit(1);

}