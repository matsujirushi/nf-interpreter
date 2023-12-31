//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <stdio.h>
#include <stdint.h>

uint32_t lwip_rand(void);

int mbedtls_hardware_poll(void *data, unsigned char *output, size_t len, size_t *olen);

// Get len bytes of entropy from the hardware RNG.
int mbedtls_hardware_poll(void *data, unsigned char *output, size_t len, size_t *olen)
{
    (void)data;

    for (size_t i = 0; i < len; i++)
    {
        // our generator returns 32bits numbers
        *output = lwip_rand();

        output++;
    }

    // callers require this to be set
    *olen = len;

    return 0;
}
