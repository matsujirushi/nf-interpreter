//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoPAL_BlockStorage.h>
#include <targetHAL.h>
#include <Target_BlockStorage_RP2040_FlashDriver.h>
#include "pico/stdlib.h"
#include "hardware/flash.h"
#include "pico/stdlib.h"
#include "FreeRTOS.h"

#if DEBUG_RP2040
#include "debug_printf.h"
#endif
#if !defined(DEBUG_RP2040_FLASH_INIT)
#define DEBUG_RP2040_FLASH_INIT 0
#endif
#if !defined(DEBUG_RP2040_FLASH_INFO)
#define DEBUG_RP2040_FLASH_INFO 0
#endif
#if !defined(DEBUG_RP2040_FLASH_READ)
#define DEBUG_RP2040_FLASH_READ 0
#endif
#if !defined(DEBUG_RP2040_FLASH_WRITE)
#define DEBUG_RP2040_FLASH_WRITE 0
#endif
#if !defined(DEBUG_RP2040_FLASH_IS_ERASED)
#define DEBUG_RP2040_FLASH_IS_ERASED 0
#endif
#if !defined(DEBUG_RP2040_FLASH_ERASE)
#define DEBUG_RP2040_FLASH_ERASE 0
#endif

#define FLASH_TOP   0x10000000

bool RP2040_flash_InitializeDevice(void *context)
{
    (void)context;
    // ToDo: implement initialize
#if DEBUG_RP2040 & DEBUG_RP2040_FLASH_INIT
    DEBUG_PRINTF("flash_InitializeDevice: OK\n");
#endif
    return true;
}

bool RP2040_flash_UninitializeDevice(void *context)
{
    (void)context;
    // ToDo: implement uninitialize
    return true;
}

DeviceBlockInfo *RP2040_flash_GetDeviceInfo(void *context)
{
    MEMORY_MAPPED_NOR_BLOCK_CONFIG *config = context;
    // ToDo: implement
#if DEBUG_RP2040 & DEBUG_RP2040_FLASH_INFO
    // DEBUG_PRINTF("flash_GetDeviceInfo\n");
#endif
    return config->BlockConfig.BlockDeviceInformation;
}

bool RP2040_flash_Read(void *context, ByteAddress startAddress, unsigned int numBytes, unsigned char *buffer)
{
    (void)context;

#if DEBUG_RP2040 & DEBUG_RP2040_FLASH_READ
    DEBUG_PRINTF("FR: OK %08x %d bytes\n", (uint)startAddress, (uint)numBytes);
#endif
    memcpy(buffer, (void *)startAddress, numBytes);

    return true;
}

uint8_t ibuffer[FLASH_SECTOR_SIZE];

bool RP2040_flash_Write(
    void *context,
    ByteAddress startAddress,
    unsigned int numBytes,
    unsigned char *buffer,
    bool readModifyWrite)
{
    (void)context;
    bool flag = true;
    uint32_t start = (uint32_t)startAddress;
    uint32_t end = start + (uint32_t)numBytes;
    uint32_t size = numBytes;
    // Read-modify-write is used for FAT filesystems only
    // if (readModifyWrite)
    // {
    //     return false;
    // }
    while (start < end) {
        uint32_t base = start & ~(FLASH_SECTOR_SIZE - 1);
        uint32_t ofs = start % (FLASH_SECTOR_SIZE);
        uint32_t count;
        count = size;
        if (count > (FLASH_SECTOR_SIZE - ofs)) {
            count = (FLASH_SECTOR_SIZE - ofs);
        }
#if DEBUG_RP2040 & DEBUG_RP2040_FLASH_WRITE
        // DEBUG_PRINTF("FW %08x %08x\n", (int)(base - FLASH_TOP), (int)count);
#endif
        memcpy((void *)&ibuffer[0], (const void *)base, (size_t)FLASH_SECTOR_SIZE);
        memcpy((void *)&ibuffer[ofs], (const void *)buffer, (size_t)count);
        portENTER_CRITICAL();
        flash_range_program((uint32_t)(base - FLASH_TOP), (const uint8_t *)ibuffer, (size_t)FLASH_SECTOR_SIZE);
        portEXIT_CRITICAL();
        if (memcmp((const void *)start, (const void *)buffer, (size_t)count) != 0) {
            flag = false;
            break;
        }
        start += count;
        buffer += count;
        size -= count;
    }
#if DEBUG_RP2040 & DEBUG_RP2040_FLASH_WRITE
    DEBUG_PRINTF("FW: %s %08x %d bytes\n", flag? "OK":"NG", (uint)startAddress, (int)numBytes);
#endif
    return flag;
}

#define FLASH_ERASED_WORD 0xFFFFFFFF

bool RP2040_flash_IsBlockErased(void *context, ByteAddress blockAddress, unsigned int length)
{
    (void)context;
    bool flag = true;
    uint32_t *cursor = (uint32_t *)blockAddress;
    uint32_t *endAddress = (uint32_t *)(blockAddress + length);
    while (cursor < endAddress)
    {
        if (*cursor++ != FLASH_ERASED_WORD)
        {
            flag = false;
        }
    }
#if DEBUG_RP2040 & DEBUG_RP2040_FLASH_IS_ERASED
    DEBUG_PRINTF("FI: %s %08x %d bytes\n", flag? "OK":"NG", (uint)blockAddress, (int)length);
#endif
    return flag;
}

bool RP2040_flash_EraseBlock(void *context, ByteAddress address)
{
    (void)context;
    bool flag = true;
    uint32_t ofs = ((uint32_t)address - FLASH_TOP) & ~(FLASH_SECTOR_SIZE - 1);
    portENTER_CRITICAL();
    flash_range_erase((uint32_t)ofs, (size_t)FLASH_SECTOR_SIZE);
    portEXIT_CRITICAL();
    // sleep_ms(1);
    flag = RP2040_flash_IsBlockErased(context, (ByteAddress)((uint32_t)address & ~(FLASH_SECTOR_SIZE - 1)), FLASH_SECTOR_SIZE);
#if DEBUG_RP2040 & DEBUG_RP2040_FLASH_ERASE
    DEBUG_PRINTF("FE: %s %08x %d bytes\n", flag? "OK":"NG", (uint)address, (int)FLASH_SECTOR_SIZE);
#endif
    return flag;
}
