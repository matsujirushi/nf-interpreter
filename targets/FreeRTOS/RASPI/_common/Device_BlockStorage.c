//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL_Types.h>
#include <nanoPAL_BlockStorage.h>

#ifndef RP2040_NANOBOOTER
#define RP2040_NANOBOOTER   0
#endif

const BlockRange BlockRange0[] = // 32KB blocks
{
#if RP2040_NANOBOOTER
    { BlockRange_BLOCKTYPE_BOOTSTRAP,   0x000,  0x00F },     // 0x10000000-0x10010000 nanoBooter
    { BlockRange_BLOCKTYPE_CODE,        0x010,  0x0FF },     // 0x10010000-0x100C0000 nanoCLR
    { BlockRange_BLOCKTYPE_CONFIG,      0x100,  0x101 },     // 0x100C0000-0x100C2000 configuration block
    { BlockRange_BLOCKTYPE_DEPLOYMENT,  0x108,  0x17F },     // 0x100D0000-0x10100000 deployment
#else
    { BlockRange_BLOCKTYPE_CODE,        0x000,  0x0FF },     // 0x10000000-0x10100000 nanoCLR
    { BlockRange_BLOCKTYPE_CONFIG,      0x100,  0x101 },     // 0x10100000-0x10102000 configuration block
    { BlockRange_BLOCKTYPE_DEPLOYMENT,  0x108,  0x17F },     // 0x10108000-0x10180000 deployment
#endif
};

const BlockRegionInfo BlockRegions[] = 
{
    {
        (0),                            // no attributes for this region
		0x10000000,                     // start address for block region
        0x0200,                         // total number of blocks in this region
        0x1000,                         // total number of bytes per block
        ARRAYSIZE_CONST_EXPR(BlockRange0),
        BlockRange0,
    }
};

const DeviceBlockInfo Device_BlockInfo =
{
    (MediaAttribute_SupportsXIP),           // RP2040 flash memory is XIP
    2,                                      // UINT32 BytesPerSector
    ARRAYSIZE_CONST_EXPR(BlockRegions),     // UINT32 NumRegions;
    (BlockRegionInfo*)BlockRegions,         // const BlockRegionInfo* pRegions;
};

MEMORY_MAPPED_NOR_BLOCK_CONFIG Device_BlockStorageConfig =
{
    { // BLOCK_CONFIG
        {
            0,          // GPIO_PIN             Pin;
            false,      // BOOL                 ActiveState;
        },

        (DeviceBlockInfo*)&Device_BlockInfo              // BlockDeviceinfo
    },

    { // CPU_MEMORY_CONFIG
        0,                          // UINT8  CPU_MEMORY_CONFIG::ChipSelect;
        true,                       // UINT8  CPU_MEMORY_CONFIG::ReadOnly;
        0,                          // UINT32 CPU_MEMORY_CONFIG::WaitStates;
        0,                          // UINT32 CPU_MEMORY_CONFIG::ReleaseCounts;
        32,                         // UINT32 CPU_MEMORY_CONFIG::BitWidth;
        0x10000000,                 // UINT32 CPU_MEMORY_CONFIG::BaseAddress;
        0x00200000,                 // UINT32 CPU_MEMORY_CONFIG::SizeInBytes;
        0,                          // UINT8  CPU_MEMORY_CONFIG::XREADYEnable 
        0,                          // UINT8  CPU_MEMORY_CONFIG::ByteSignalsForRead 
        0,                          // UINT8  CPU_MEMORY_CONFIG::ExternalBufferEnable
    },

    0,                              // UINT32 ChipProtection;
    0,                              // UINT32 ManufacturerCode;
    0,                              // UINT32 DeviceCode;
};

BlockStorageDevice    Device_BlockStorage;
