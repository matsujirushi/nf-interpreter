//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// #include "board.h"
#include "pico/stdio_usb.h"
#include "pico/stdio/driver.h"
#include "hardware/timer.h"

#include <nanoHAL_v2.h>
#include <WireProtocol.h>
#include <WireProtocol_Message.h>
#include <WireProtocol_HAL_Interface.h>
#include "tusb.h"

#if DEBUG_RP2040
#include "debug_printf.h"
#endif
#if !defined(SLOW_WIREPROTOCOL)
#define SLOW_WIREPROTOCOL   0
#endif
#if !defined(DEBUG_WIREPROTOCOL)
#define DEBUG_WIREPROTOCOL_CDC  0
#endif
#if !defined(DEBUG_WIREPROTOCOL_CMD)
#define DEBUG_WIREPROTOCOL_CMD  0
#endif

#define TIMEOUT         1000
#define CHUNCK_SIZE     CFG_TUD_CDC_EP_BUFSIZE

void vTaskSuspend(void *);

#if DEBUG_RP2040 & DEBUG_WIREPROTOCOL_CMD
typedef struct wp_cmd_string {
    const uint32_t cmd;
    const uint8_t *string;
} wp_cmd_string_t;

static const wp_cmd_string_t wp_strings[] = {
    {0x00000000, (const uint8_t *)"Ping"},
    {0x00000001, (const uint8_t *)"Message"},
    {0x00000002, (const uint8_t *)"ReadMemory"},
    {0x00000003, (const uint8_t *)"WriteMemory"},
    {0x00000004, (const uint8_t *)"CheckMemory"},
    {0x00000005, (const uint8_t *)"EraseMemory"},
    {0x00000006, (const uint8_t *)"Execute"},
    {0x00000007, (const uint8_t *)"Reboot"},
    {0x00000008, (const uint8_t *)"MemoryMap"},
    {0x00000009, (const uint8_t *)"ProgramExit"},
    {0x0000000a, (const uint8_t *)"CheckSignature"},
    {0x0000000b, (const uint8_t *)"DeploymentMap"},
    {0x0000000c, (const uint8_t *)"FlashSectorMap"},
    {0x0000000e, (const uint8_t *)"OemInfo"},
    {0x0000000f, (const uint8_t *)"QueryConfiguration"},
    {0x00000010, (const uint8_t *)"UpdateConfiguraiton"},
    {0x00000020, (const uint8_t *)"TargetInfo"},
};
#define NUM_OF_WP_STRINGS   (sizeof(wp_strings)/sizeof(wp_cmd_string_t))
static const uint8_t *WP_CmdString(uint32_t cmd)
{
    for (int i = 0; i < (int)NUM_OF_WP_STRINGS; i++) {
        if ((cmd & 0xff) == wp_strings[i].cmd) {
            return (const uint8_t *)wp_strings[i].string;
        }
    }
    return (const uint8_t *)"Unknown";
}
#endif

void WP_ReceiveBytes(uint8_t **ptr, uint32_t *size)
{
    uint32_t requestedSize = *size;
    uint32_t read = 0;
    if (*size) {
       if (tud_cdc_available()) {
           read = (uint32_t)tud_cdc_read((char *)*ptr, (int)(requestedSize));
#if DEBUG_RP2040 & DEBUG_WIREPROTOCOL_CDC
           printf("Recv: %d(%d)\n", (int)requestedSize, (int)read);
           // dump_hex((uint8_t *)ptr, (size_t)read);
#endif
           *ptr += read;
           *size -= read;
       }
    }
}

#if !SLOW_WIREPROTOCOL
// Probably works fine.
// if not, use slow version
uint32_t wrap_tud_cdc_write(const char *ptr, int size) {
    int written = 0;
    int count = 0;
    while (size > 0) {
        count = size;
        if (count > CHUNCK_SIZE) {
            count = CHUNCK_SIZE;
        }
#if DEBUG_RP2040 & DEBUG_WIREPROTOCOL_CDC
        printf("CDCW %d\n", count);
#endif
        int timeout = 0;
        while (CHUNCK_SIZE > tud_cdc_write_available()) {
            busy_wait_us_32(1000);
            timeout ++;
            if (timeout >= TIMEOUT) {
                break;
            }
        }
        int c = tud_cdc_write((const char *)&ptr[written], count);
        tud_cdc_write_flush();
        written += c;
        size -= c;
    }
    return (uint32_t)written;
}
#else
uint32_t wrap_tud_cdc_write(const char *ptr, int size) {
    int count = size;
    while (count-- > 0) {
        tud_cdc_write((const char *)ptr, 1);
        tud_cdc_write_flush();
        ptr++;
    }
    return (uint32_t)size;
}
#endif

uint8_t WP_TransmitMessage(WP_Message *message)
{
    wrap_tud_cdc_write((const char *)&message->m_header, (int)sizeof(message->m_header));
    // if there is anything on the payload send it to the output stream
    if (message->m_header.m_size && message->m_payload)
    {
        wrap_tud_cdc_write((const char *)message->m_payload, (int)message->m_header.m_size);
#if DEBUG_RP2040 & DEBUG_WIREPROTOCOL_CMD
        printf("WP Send: 8-%4d cmd=%08x %s\n",
                (int)message->m_header.m_size,
                (uint)message->m_header.m_cmd,
                WP_CmdString(message->m_header.m_cmd));
        // dump_hex((uint8_t *)&message->m_header, (size_t)sizeof(message->m_header));
        // dump_hex((uint8_t *)message->m_payload, (size_t)message->m_header.m_size);
#endif
    }
#if DEBUG_RP2040 & DEBUG_WIREPROTOCOL_CMD
    else {
        printf("Send: 8-0 cmd=%08x\n", (uint)message->m_header.m_cmd);
        // dump_hex((uint8_t *)&message->m_header, (size_t)sizeof(message->m_header));
    }
#endif
     return 1;
}
