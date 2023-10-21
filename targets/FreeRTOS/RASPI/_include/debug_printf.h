//
// Copyright (c) Kentaro Sekimoto
// See LICENSE file in the project root for full license information.
//

#ifndef DEBUG_PRINTF_H
#define DEBUG_PRINTF_H

#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void DEBUG_PRINTF(const char *fmt, ...);
void dump_hex(uint8_t *ptr, size_t size);

#ifdef __cplusplus
}
#endif

#endif //DEBUG_PRINTF_H
