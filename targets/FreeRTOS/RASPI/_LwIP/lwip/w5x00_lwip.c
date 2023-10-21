/**
 * Copyright (c) 2022 WIZnet Co.,Ltd
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * ----------------------------------------------------------------------------------------------------
 * Includes
 * ----------------------------------------------------------------------------------------------------
 */
#include <stdio.h>
#include <string.h>

#include "w5x00_lwip.h"
#include "socket.h"
#include "netif/etharp.h"

#if DEBUG_RP2040
#include "debug_printf.h"
#endif
#if !defined(DEBUG_W5X00_LWIP_STATUS)
#define DEBUG_W5X00_LWIP_STATUS 0
#endif
#if !defined(DEBUG_W5X00_LWIP_SEND)
#define DEBUG_W5X00_LWIP_SEND   0
#endif
#if !defined(DEBUG_W5X00_LWIP_RECV)
#define DEBUG_W5X00_LWIP_RECV   0
#endif
#if !defined(DEBUG_W5X00_LWIP_OUTPUT)
#define DEBUG_W5X00_LWIP_OUTPUT 0
#endif

/**
 * ----------------------------------------------------------------------------------------------------
 * Macros
 * ----------------------------------------------------------------------------------------------------
 */

/**
 * ----------------------------------------------------------------------------------------------------
 * Variables
 * ----------------------------------------------------------------------------------------------------
 */
// uint8_t mac[6] = {0x00, 0x08, 0xDC, 0x12, 0x34, 0x56};
extern uint8_t mac[];

static uint8_t tx_frame[1542];
static const uint32_t ethernet_polynomial_le = 0xedb88320U;

/**
 * ----------------------------------------------------------------------------------------------------
 * Functions
 * ----------------------------------------------------------------------------------------------------
 */
int32_t send_lwip(uint8_t sn, uint8_t *buf, uint16_t len)
{
    uint8_t tmp = 0;
    uint16_t freesize = 0;

    tmp = getSn_SR(sn);

    freesize = getSn_TxMAX(sn);
    if (len > freesize)
        len = freesize; // check size not to exceed MAX size.

    wiz_send_data(sn, buf, len);
    setSn_CR(sn, Sn_CR_SEND);
    while (getSn_CR(sn))
        ;

    while (1)
    {
        uint8_t IRtemp = getSn_IR(sn);
        if (IRtemp & Sn_IR_SENDOK)
        {
            setSn_IR(sn, Sn_IR_SENDOK);
#if DEBUG_RP2040 & DEBUG_W5X00_LWIP_SEND
            DEBUG_PRINTF("PSend OK %4d\n", (int)len);
#endif
            break;
        }
        else if (IRtemp & Sn_IR_TIMEOUT)
        {
            setSn_IR(sn, Sn_IR_TIMEOUT);
            //  There was a timeout
#if DEBUG_RP2040 & DEBUG_W5X00_LWIP_SEND
            DEBUG_PRINTF("PSend NG %4d\n", (int)len);
#endif
            return -1;
        }
    }

    return (int32_t)len;
}

int32_t recv_lwip(uint8_t sn, uint8_t *buf, uint16_t len)
{
    uint8_t head[2];
    uint16_t pack_len = 0;

    pack_len = getSn_RX_RSR(sn);

    if (pack_len > 0)
    {
        wiz_recv_data(sn, head, 2);
        setSn_CR(sn, Sn_CR_RECV);

        // byte size of data packet (2byte)
        pack_len = head[0];
        pack_len = (pack_len << 8) + head[1];
        pack_len -= 2;

        if (pack_len > len)
        {
            // Packet is bigger than buffer - drop the packet
            wiz_recv_ignore(sn, pack_len);
            setSn_CR(sn, Sn_CR_RECV);
#if DEBUG_RP2040 & DEBUG_W5X00_LWIP_RECV
            DEBUG_PRINTF("PRecv NG %4d\n", (int)len);
#endif
            return 0;
        }

#if DEBUG_RP2040 & DEBUG_W5X00_LWIP_RECV
        DEBUG_PRINTF("PRecv OK %4d\n", pack_len);
#endif
        wiz_recv_data(sn, buf, pack_len); // data copy
        setSn_CR(sn, Sn_CR_RECV);
    }

    return (int32_t)pack_len;
}

err_t netif_output(struct netif *netif, struct pbuf *p)
{
    uint32_t send_len = 0;
    uint32_t tot_len = 0;

    memset((void *)tx_frame, 0x00, sizeof(tx_frame));

    for (struct pbuf *q = p; q != NULL; q = q->next)
    {
#if 1
        if ((tot_len + q->len) > sizeof(tx_frame)) {
#if DEBUG_RP2040 & DEBUG_W5X00_LWIP_OUTPUT
            DEBUG_PRINTF("netif_output: NG %04x + %04x > %04x\n", (uint)tot_len, (uint)q->len, (uint)sizeof(tx_frame));
#endif
            break;
        }
#endif
        memcpy((void *)(tx_frame + tot_len), (const void *)q->payload, q->len);

        tot_len += q->len;

        if (q->len == q->tot_len)
        {
            break;
        }
    }

    if (tot_len < 60)
    {
        // pad
        tot_len = 60;
    }

    uint32_t crc = ethernet_frame_crc(tx_frame, tot_len);

    send_len = send_lwip(0, tx_frame, tot_len);

    return ERR_OK;
}

void netif_link_callback(struct netif *netif)
{
#if DEBUG_RP2040 & DEBUG_W5X00_LWIP_STATUS
    DEBUG_PRINTF("netif link status changed %s\n", netif_is_link_up(netif) ? "up" : "down");
#endif
}

void netif_status_callback(struct netif *netif)
{
#if DEBUG_RP2040 & DEBUG_W5X00_LWIP_STATUS
    DEBUG_PRINTF("netif status changed %s\n", ip4addr_ntoa(netif_ip4_addr(netif)));
#endif
}

err_t netif_initialize(struct netif *netif)
{
    netif->linkoutput = netif_output;
    netif->output = etharp_output;
    netif->mtu = ETHERNET_MTU;
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_ETHERNET | NETIF_FLAG_IGMP | NETIF_FLAG_MLD6;
    SMEMCPY(netif->hwaddr, mac, sizeof(netif->hwaddr));
    netif->hwaddr_len = sizeof(netif->hwaddr);
    return ERR_OK;
}

static uint32_t ethernet_frame_crc(const uint8_t *data, int length)
{
    uint32_t crc = 0xffffffff; /* Initial value. */

    while (--length >= 0)
    {
        uint8_t current_octet = *data++;

        for (int bit = 8; --bit >= 0; current_octet >>= 1)
        {
            if ((crc ^ current_octet) & 1)
            {
                crc >>= 1;
                crc ^= ethernet_polynomial_le;
            }
            else
                crc >>= 1;
        }
    }

    return ~crc;
}
