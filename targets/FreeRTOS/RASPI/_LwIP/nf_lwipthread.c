//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <string.h>
#include "lwip/opt.h"

#include "nf_lwipthread.h"

#include "lwip/netif.h"

#include "FreeRTOS.h"
#include "task.h"

extern struct netif g_netif;

struct netif * nf_getNetif()
{
    return &g_netif;
}

#if !USE_THREAD
void stack_init(const lwipthread_opts_t *opts);
#else
void lwipThread(void *param);
#endif

void lwipInit(const lwipthread_opts_t *opts)
{
#if !USE_THREAD
    stack_init(opts);
#else
    xTaskCreate(lwipThread, "lwipThread", 4096, (void *)opts, configMAX_PRIORITIES - 2, NULL);
#endif
}
