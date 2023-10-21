//
// Copyright (c) .NET Foundation and Contributors
// Copyright (c) Kentaro Sekimoto
// See LICENSE file in the project root for full license information.
//

#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include "debug_printf.h"

//
// Debug functions
//
void DEBUG_PRINTF(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}

void dump_hex(uint8_t *ptr, size_t size) {
    printf("size:%d\n", size);
    for (int i = 0; i < (int)size; i++) {
        printf("%02x ", ptr[i]);
        if ((i % 0x10) == 0xf) {
            printf("\n");
        }
    }
    printf("\n");
}

