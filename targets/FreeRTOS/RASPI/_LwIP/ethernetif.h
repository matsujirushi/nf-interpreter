//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) 2001-2003 Swedish Institute of Computer Science. All rights reserved.
// Portions Copyright (c) 2013-2016, Freescale Semiconductor, Inc. All rights reserved.
// See LICENSE file in the project root for full license information.
//


/*
 * from the original source code file
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */


#ifndef ETHERNETIF_H
#define ETHERNETIF_H

#include "lwip/err.h"

#define ENET_OK             (0U)
#define ENET_ERROR          (0xFFU)
#define ENET_TIMEOUT        (0xFFFU)

/**
 * Helper struct to hold data for configuration of ethernet interface.
 */
typedef struct ethernetif_config
{
    uint32_t phyAddress;
    uint8_t macAddress[NETIF_MAX_HWADDR_LEN];
} ethernetif_config_t;

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

/**
 * This function should be passed as a parameter to netif_add()
 * if you initialize the first ENET interface.
 */
err_t ethernetif0_init(struct netif *netif);

/**
 * This function should be passed as a parameter to netif_add()
 * if you initialize the second ENET interface.
 */
err_t ethernetif1_init(struct netif *netif);


/**
 * This function should be called when a packet is ready to be read
 * from the interface. 
 * It is used by bare-metal applications.
 *
 * @param netif the lwip network interface structure for this ethernetif
 */
void ethernetif_input( struct netif *netif);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif
