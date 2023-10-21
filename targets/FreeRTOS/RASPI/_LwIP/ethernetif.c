//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) 2001-2004 Swedish Institute of Computer Science. All rights reserved.
// Portions Copyright (c) 2013-2016, Freescale Semiconductor, Inc. All rights reserved.
// Portions Copyright 2022 Kentaro Sekimoto
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
#include <string.h>
#include "nanoHAL_v2.h"
#include "nf_lwipthread.h"

#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/stats.h"
#include "lwip/snmp.h"
#include "lwip/ethip6.h"
#include "netif/etharp.h"
#include "netif/ppp/pppoe.h"
#include "lwip/igmp.h"
#include "lwip/mld6.h"
#include "lwip/init.h"
#include "lwip/netif.h"
#include "lwip/timeouts.h"
#include "lwip/apps/lwiperf.h"
#include "lwip/dhcp.h"
#include "lwip/dns.h"
#include "lwip/tcpip.h"
#include <apps/sntp.h>

#include "board.h"
#include "FreeRTOS.h"
#include "event_groups.h"

#include "pico/time.h"
#include "wizchip_conf.h"
#include "w5x00_lwip.h"
#include "w5x00_spi.h"
#if (_WIZCHIP_ == W5500)
#include "w5500/w5500.h"
#elif (_WIZCHIP_ == W5100)
#include "w5100/w5100.h"
#elif (_WIZCHIP_ == W5100)
#include "w5100S/w5100s.h"
#elif (_WIZCHIP_ == W5200)
#include "w5200/w5200.h"
#elif (_WIZCHIP_ == W5300)
#include "w5300/w5300.h"
#endif
#include "socket.h"

#include "ethernetif.h"

#if DEBUG_RP2040
#include "debug_printf.h"
#endif
#if !defined(DEBUG_ETHERNETIF)
#define DEBUG_ETHERNETIF    1
#endif

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* Define those to better describe your network interface. */
#define IFNAME0 'e'
#define IFNAME1 'n'

#define SOCKET_MACRAW 0
#define PORT_LWIPERF 5001
#define TMP_BUF_SIZE    1516
#if !defined(LWIP_NETIF_MTU)
#define LWIP_NETIF_MTU  1500
#endif

extern uint8_t mac[6];
struct netif g_netif;
static ip4_addr_t m_ip;
static ip4_addr_t m_mask;
static ip4_addr_t m_gw;

static void low_level_init(struct netif *netif)
{
    /* set MAC hardware address length */
    netif->hwaddr_len = ETHARP_HWADDR_LEN;
    SMEMCPY(netif->hwaddr, mac, sizeof(netif->hwaddr));

    /* maximum transfer unit */
    netif->mtu = LWIP_NETIF_MTU;

    /* device capabilities */
    /* don't set NETIF_FLAG_ETHARP if this device is not an Ethernet one */
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP | NETIF_FLAG_IGMP;

    /* Do whatever else is needed to initialize interface. */
}

/*
 * This function does the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
 * @return ERR_OK if the packet could be sent
 *         an err_t value if the packet couldn't be sent
 *
 * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
 *       strange results. You might consider waiting for space in the DMA queue
 *       to become available since the stack doesn't retry to send a packet
 *       dropped because of memory failure (except for the TCP timers).
 */

static err_t low_level_output(struct netif *netif, struct pbuf *p)
{
    return netif_output(netif, p);
}

/*
 * Receives a frame.
 * Allocates a pbuf and transfers the bytes of the incoming
 * packet from the interface into the pbuf.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return a pbuf filled with the received packet (including MAC header)
 *         NULL on memory error
 */
static struct pbuf *low_level_input(struct netif *netif)
{
    struct pbuf *p = NULL;
    struct pbuf *q;
    uint16_t buf_size = (uint16_t)LWIP_NETIF_MTU;
    uint8_t *buf;
    uint16_t remain = 0;
    uint16_t readed = 0;

    getsockopt(SOCKET_MACRAW, SO_RECVBUF, &remain);
    if (remain != 0)
    {
// #if DEBUG_RP2040 & DEBUG_ETHERNETIF
//         DEBUG_PRINTF("low_level_input: remain=%d\n", (int)remain);
// #endif
        buf = (uint8_t *)platform_malloc(TMP_BUF_SIZE);
        if (buf == NULL)
        {
#if DEBUG_RP2040 & DEBUG_ETHERNETIF
            DEBUG_PRINTF("low_level_input: NG - platform_malloc\n");
#endif
        } else {
            readed = (uint16_t)recv_lwip(SOCKET_MACRAW, (uint8_t *)buf, (uint16_t)TMP_BUF_SIZE);
            p = pbuf_alloc(PBUF_RAW, (uint16_t)readed, PBUF_POOL);
            if (p != NULL)
            {
                if (p->next == NULL)
                {
                    memcpy(p->payload, (const void *)buf, (size_t)readed);
                }
                else
                {
                    uint32_t ofs = 0;
                    for (q = p;  q != NULL ; q = q->next) {
                        memcpy(q->payload,  (const void *)&buf[ofs], q->len);
                        ofs += (uint32_t)q->len;
                    }
                }
            } else {
#if DEBUG_RP2040 & DEBUG_ETHERNETIF
                DEBUG_PRINTF("low_level_input: NG - pbuf_alloc\n");
#endif
            }
            platform_free((void *)buf);
        }
    }
    return p;
}

/*
 * Called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 * This function should be passed as a parameter to netifapi_netif_add().
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if the loopif is initialised
 *         ERR_MEM if private data couldn't be allocated
 *         any other err_t on error
 */
static err_t ethernetif_init(struct netif *netif)
{
#if 0
    netif->linkoutput = netif_output;
    netif->output = etharp_output;
    netif->mtu = ETHERNET_MTU;
#endif
    /*
     * Initialize the snmp variables and counters inside the struct netif.
     * The last argument should be replaced with your link speed, in units
     * of bits per second.
     */
    netif->state = NULL;
    netif->name[0] = IFNAME0;
    netif->name[1] = IFNAME1;
    /* We directly use etharp_output() here to save a function call.
     * You can instead declare your own function an call etharp_output()
     * from it if you have to do some checks before sending (e.g. if link
     * is available...) */
    netif->output = etharp_output;
    netif->linkoutput = low_level_output;

    /* initialize the hardware */
    low_level_init(netif);

    return ERR_OK;
}

#if 1
void ethernetif_input(struct netif *netif)
{
    struct pbuf *p;

    LWIP_ASSERT("netif != NULL", (netif != NULL));

    /* move received packet into a new pbuf */
    while ((p = low_level_input(netif)) != NULL)
    {
        /* pass all packets to ethernet_input, which decides what packets it supports */
        if (netif->input(p, netif) != ERR_OK)
        {
            LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error\n"));
            pbuf_free(p);
            p = NULL;
        }
    }
}
#else
void ethernetif_input(struct netif *netif)
{
    struct pbuf *p = NULL;
    uint8_t *pack = platform_malloc(LWIP_NETIF_MTU);
    uint16_t pack_len = 0;

    getsockopt(SOCKET_MACRAW, SO_RECVBUF, &pack_len);
    if (pack_len > 0)
    {
        pack_len = recv_lwip(SOCKET_MACRAW, (uint8_t *)pack, pack_len);
        if (pack_len)
        {
            p = pbuf_alloc(PBUF_RAW, pack_len, PBUF_POOL);
            pbuf_take(p, pack, pack_len);
            platform_free(pack);
            pack = platform_malloc(LWIP_NETIF_MTU);
        }
        else
        {
            DEBUG_PRINTF(" No packet received\n");
        }
        if (pack_len && p != NULL)
        {
            if (netif->input(p, &g_netif) != ERR_OK)
            {
                pbuf_free(p);
            }
        }
    }
    if (pack)
    {
        platform_free(pack);
    }
}
#endif

#if !USE_THREAD
void W5x00_RecvThread(void *param) {
    (void)param;
#if DEBUG_RP2040 & DEBUG_ETHERNETIF
    DEBUG_PRINTF("W5x00_RecvThread: start\n");
#endif
    while (true)
    {
        ethernetif_input((struct netif *)&g_netif);
        taskYIELD();
    }
}
#endif

#if !USE_THREAD
void stack_init(const lwipthread_opts_t *opts)
#else
void lwipThread(void *param)
#endif
{
#if DEBUG_RP2040 & DEBUG_ETHERNETIF
    // DEBUG_PRINTF("stack_init: start: ip=%08x mask=%08x gw=%08x\n", (uint)ip, (uint)mask, (uint)gw);
    DEBUG_PRINTF("lwipThread: start\n");
#endif
#if !USE_THREAD
    m_ip.addr = opts->address;
    m_mask.addr = opts->netmask;
    m_gw.addr = opts->gateway;
#else
    lwipthread_opts_t *opts = (lwipthread_opts_t *)param;
    m_ip.addr = opts->address;
    m_mask.addr = opts->netmask;
    m_gw.addr = opts->gateway;
#endif

#if DEBUG_RP2040 & DEBUG_ETHERNETIF
    DEBUG_PRINTF("lwipThread: ip=%08x, mask=%08x, gw=%08x\n",
            (uint)opts->address, (uint)opts->netmask, (uint)opts->gateway);
#endif

    int8_t retval = 0;

    wizchip_spi_initialize();
    wizchip_cris_initialize();

    wizchip_reset();
    wizchip_initialize();
    wizchip_check();

    // Set ethernet chip MAC address
    setSHAR(mac);
    ctlwizchip(CW_RESET_PHY, 0);

#if NO_SYS
    // Initialize LWIP in NO_SYS mode
    lwip_init();
    netif_add(&g_netif, (const ip4_addr_t *)&m_ip, (const ip4_addr_t *)&m_mask, (const ip4_addr_t *)&m_gw, NULL, ethernetif_init,  netif_input);
#else
    // Initialize LWIP in OS mode
    tcpip_init(NULL, NULL);
    netif_add(&g_netif, (const ip4_addr_t *)&m_ip, (const ip4_addr_t *)&m_mask, (const ip4_addr_t *)&m_gw, NULL, ethernetif_init,  netif_input);
#endif
    g_netif.name[0] = IFNAME0;
    g_netif.name[1] = IFNAME1;

    // Assign callbacks for link and status
    netif_set_link_callback(&g_netif, netif_link_callback);
    netif_set_status_callback(&g_netif, netif_status_callback);

    // MACRAW socket open
    retval = socket(SOCKET_MACRAW, Sn_MR_MACRAW, PORT_LWIPERF, 0x00);

    if (retval < 0)
    {
#if DEBUG_RP2040 & DEBUG_ETHERNETIF
        DEBUG_PRINTF(" stack_init: MACRAW socket open failed\n");
#endif
    }

    // Set the default interface and bring it up
    netif_set_default(&g_netif);
    netif_set_link_up(&g_netif);
    netif_set_up(&g_netif);

#if !USE_THREAD
    xTaskCreate(W5x00_RecvThread, "W5x00_RecvThread", 2048, NULL, configMAX_PRIORITIES - 2, NULL);

    // setup SNTP
#if SNTP_SERVER_DNS
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, SNTP_SERVER0_DEFAULT_ADDRESS);
    sntp_setservername(1, SNTP_SERVER1_DEFAULT_ADDRESS);
    sntp_init();
#endif

    sleep_ms(2000);

#else
    while (true)
    {
        ethernetif_input((struct netif *)&g_netif);
    }
#endif

// #if DEBUG_RP2040 & DEBUG_ETHERNETIF
//     DEBUG_PRINTF("stack_init: end\n");
// #endif
}
