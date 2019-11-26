
#ifndef _LOG_H
#define _LOG_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

// Give our logger the ability to print filename and line serial.
#define fault(FMT, ...) _fault(FMT, __FILE__, __LINE__, __VA_ARGS__)
void _fault(const char* fmt, char* FILE, int LINE, ...);

#endif