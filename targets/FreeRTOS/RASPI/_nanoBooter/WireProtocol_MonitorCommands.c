//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

/* CMSIS-style register definitions */
// #include "RP2040.h"
#include "pico/stdlib.h"
#include "hardware/watchdog.h"

#include <nanoHAL_v2.h>
#include <WireProtocol.h>
#include <Debugger.h>
#include <WireProtocol_MonitorCommands.h>
#include <target_board.h>
#include <Target_BlockStorage_RP2040_FlashDriver.h>

#if DEBUG_RP2040
#include "debug_printf.h"
#endif

#define FLASH_ERASED_WORD 0xFFFFFFFF

int AccessMemory(uint32_t location, uint32_t lengthInBytes, uint8_t *buffer, int32_t mode, uint32_t *errorCode)
{
#if DEBUG_RP2040
    DEBUG_PRINTF("AccessMemory mode:%d\n", (int)mode);
#endif
    // reset error code
    *errorCode = AccessMemoryErrorCode_NoError;

    switch (mode)
    {
        case AccessMemory_Write:
            // use FLASH driver to perform write operation
            return RP2040_flash_Write(NULL, location, lengthInBytes, buffer, false);

        case AccessMemory_Erase:
            // erase using FLASH driver
            return RP2040_flash_EraseBlock(NULL, location);

        case AccessMemory_Check:
            // compute CRC32 of the memory segment
            *(unsigned int *)buffer = SUPPORT_ComputeCRC((uint32_t *)location, lengthInBytes, 0);
            // done here
            return true;

        case AccessMemory_Read:
        default:
            // default return is FALSE
            return false;
    }
}

////////////////////////////////////////////////////

int Monitor_Reboot(WP_Message *message)
{
#if DEBUG_RP2040
    DEBUG_PRINTF("Monitor_Reboot\n");
#endif
    Monitor_Reboot_Command *cmd = (Monitor_Reboot_Command *)message->m_payload;

    WP_ReplyToCommand(message, true, false, NULL, 0);

    if (cmd != NULL)
    {
        if (Monitor_Reboot_c_EnterProprietaryBooter == (cmd->m_flags & Monitor_Reboot_c_EnterProprietaryBooter))
        {
            // request to load proprietary bootloader
            // OK to call directly as this will launch the bootloader only if the target has support for it
            LaunchProprietaryBootloader();
        }
        else
        {
            // RESET CPU to load nanoCLR
            watchdog_reboot (0, SRAM_END, 10);
        }
    }

    return true;
}
