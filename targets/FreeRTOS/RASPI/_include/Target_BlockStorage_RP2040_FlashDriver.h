//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef TARGET_RP2040_FLASH_DRIVER_H
#define TARGET_RP2040_FLASH_DRIVER_H

#include <nanoPAL_BlockStorage.h>

#ifdef __cplusplus
extern "C" {
#endif

bool RP2040_flash_InitializeDevice(void*);
bool RP2040_flash_UninitializeDevice(void*);
DeviceBlockInfo* RP2040_flash_GetDeviceInfo(void*);
bool RP2040_flash_Read(void*, ByteAddress startAddress, unsigned int numBytes, unsigned char* buffer);
bool RP2040_flash_Write(void*, ByteAddress startAddress, unsigned int numBytes, unsigned char* buffer, bool readModifyWrite);
bool RP2040_flash_IsBlockErased(void*, ByteAddress blockAddress, unsigned int length);
bool RP2040_flash_EraseBlock(void*, ByteAddress address);
void RP2040_flash_SetPowerState(void*, unsigned int state);

#ifdef __cplusplus
}
#endif

#endif // TARGET_RP2040_FLASH_DRIVER_H
