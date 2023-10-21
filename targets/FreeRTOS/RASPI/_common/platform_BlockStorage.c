//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoPAL_BlockStorage.h>
#include <Target_BlockStorage_RP2040_FlashDriver.h>

IBlockStorageDevice RP2040_BlockStorageInterface =
{
    &RP2040_flash_InitializeDevice,
    &RP2040_flash_UninitializeDevice,
    &RP2040_flash_GetDeviceInfo,
    &RP2040_flash_Read,
    &RP2040_flash_Write,
    NULL,
    &RP2040_flash_IsBlockErased,
    &RP2040_flash_EraseBlock,
    NULL,
    NULL
};
