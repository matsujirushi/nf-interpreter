//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "FreeRTOS.h"
#include <nanoCLR_Headers.h>

#if DEBUG_RP2040
#include "debug_printf.h"
#endif
#if !defined(DEBUG_PLATFORM_MALLOC)
#define DEBUG_PLATFORM_MALLOC           0
#endif

#if !defined(DEBUG_PLATFORM_MALLOC_ERR_ONLY)
#define DEBUG_PLATFORM_MALLOC_ERR_ONLY  1
#endif

#if !defined(DEBUG_PLATFORM_FREE)
#define DEBUG_PLATFORM_FREE             0
#endif

void *platform_malloc(size_t size)
{
    void *ptr = pvPortMalloc(size);
#if DEBUG_RP2040 & DEBUG_PLATFORM_MALLOC
    DEBUG_PRINTF("pvPortMalloc: %s size=%d p=%08x\n", ptr == (void *)NULL? "NG":"OK", (int)size, (uint)ptr);
#endif
#if DEBUG_RP2040 & DEBUG_PLATFORM_MALLOC_ERR_ONLY
    if (ptr == (void *)NULL) {
        DEBUG_PRINTF("pvPortMalloc: NG size=%d\n", (int)size);
    }
#endif
    memset(ptr, 0, size);
    return ptr;
}

void platform_free(void *ptr)
{
#if DEBUG_RP2040 & DEBUG_PLATFORM_FREE
    DEBUG_PRINTF("pvPortfree: OK p=%08x\n", (uint)ptr);
#endif
    vPortFree(ptr);
}
