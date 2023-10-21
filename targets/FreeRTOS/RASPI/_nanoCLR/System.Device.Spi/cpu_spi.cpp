//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
//
// CPU_SPI_xxxx
//
// Read / Write physical SPI bus
//

#include <targetHAL.h>
#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoCLR_Checks.h>

#include <sys_dev_spi_native.h>
#include "board.h"
#include <CPU_GPIO_decl.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/spi.h"

typedef struct rp2040_spi {
    spi_inst *spi;
    uint clk;
    uint mosi;
    uint miso;
    bool assigned;
} rp2040_spi_t;

static rp2040_spi_t m_spi[] = {
    {
        .spi = (spi_inst *)spi0,
        .clk = RP2040_SPI1_CLK,
        .mosi = RP2040_SPI1_MOSI,
        .miso = RP2040_SPI1_MISO,
        .assigned = (bool)RP2040_SPI1_ASSIGNED,
    },
    {
        .spi = (spi_inst *)spi1,
        .clk = RP2040_SPI2_CLK,
        .mosi = RP2040_SPI2_MOSI,
        .miso = RP2040_SPI2_MISO,
        .assigned = (bool)RP2040_SPI2_ASSIGNED,
    }
};

bool CPU_SPI_Initialize(uint8_t bus, SpiBusConfiguration spiConfiguration)
{
    (void)spiConfiguration;

    if (bus >= RP2040_SPI_COUNT)
    {
        return false;
    }
    m_spi[bus].assigned = true;
    spi_init(m_spi[bus].spi, (uint)RP2040_SPI_DEF_FREQ);
    spi_set_baudrate(m_spi[bus].spi, (uint)RP2040_SPI_DEF_FREQ);
    gpio_set_function(m_spi[bus].clk, GPIO_FUNC_SPI);
    gpio_set_function(m_spi[bus].miso, GPIO_FUNC_SPI);
    gpio_set_function(m_spi[bus].mosi, GPIO_FUNC_SPI);
    return true;
}

bool CPU_SPI_Uninitialize(uint8_t bus)
{
    if (bus >= RP2040_SPI_COUNT)
    {
        return false;
    }
    m_spi[bus].assigned = false;
    spi_deinit(m_spi[bus].spi);
    return true;
}

// Performs a read/write operation on 8-bit word data.
//
// Parameters
//  deviceHandle
//      Device handle from add_device
//  sdev
//		reference to SPI_DEVICE_CONFIGURATION
//  wrc
//		reference to SPI_WRITE_READ_SETTINGS
//  writeData
//      A pointer to the buffer from which the data is to be written to the device.
//  writeSize
//      The number of elements(8 or 16) to be written.
//  readData
//      A pointer to the buffer into which the data is to be read from the device.
//  readSize
//      The number of elements(8 or 16) to be read.
//
// return S_OK=Successful, Async started=CLR_BUSY, Error=CLR_E_OUT_OF_MEMORY, CLR_E_INVALID_PARAMETER, CLR_E_FAIL
//

HRESULT CPU_SPI_nWrite_nRead(
    uint32_t deviceHandle,
    SPI_DEVICE_CONFIGURATION &sdev,
    SPI_WRITE_READ_SETTINGS &wrc,
    uint8_t *writeData,
    int32_t writeSize,
    uint8_t *readData,
    int32_t readSize)
{
    (void)deviceHandle;
    uint32_t bus;
    int ret = 0;
    int cs = -1;
    uint bits;
    spi_cpol_t polarity;
    spi_cpha_t phase;
    spi_order_t order;

    NANOCLR_HEADER();
    {
        bus = sdev.Spi_Bus - 1;
        cs = (int)sdev.DeviceChipSelect;
        CPU_GPIO_EnableOutputPin((GPIO_PIN)cs, GpioPinValue_High, PinMode_Output);
        switch (sdev.Spi_Mode) {
        case SpiMode_Mode0:
            polarity = SPI_CPOL_0;
            phase = SPI_CPHA_0;
            break;
        case SpiMode_Mode1:
            polarity = SPI_CPOL_0;
            phase = SPI_CPHA_1;
            break;
        case SpiMode_Mode2:
            polarity = SPI_CPOL_1;
            phase = SPI_CPHA_0;
            break;
        case SpiMode_Mode3:
            polarity = SPI_CPOL_1;
            phase = SPI_CPHA_1;
            break;
        }
        bits = wrc.Bits16ReadWrite ? 16 : 8;
        if (sdev.DataOrder16 == DataBitOrder_MSB)
        {
            order = SPI_MSB_FIRST;
        }
        else
        {
            order = SPI_LSB_FIRST;
        }
        spi_set_baudrate(m_spi[bus].spi, (uint)sdev.Clock_RateHz);
        spi_set_format(m_spi[bus].spi, (uint)bits, polarity, phase, order);
        CPU_GPIO_SetPinState((GPIO_PIN)cs, GpioPinValue_Low);
        if (bits == 8) {
            if (wrc.fullDuplex) {
                if ((writeSize != 0) && (readSize != 0)) {
                    if (writeSize > readSize) {
                        ret = spi_write_read_blocking(m_spi[bus].spi, (const uint8_t *)writeData, (uint8_t *)readData, (size_t)readSize);
                        ret += spi_write_blocking(m_spi[bus].spi, (const uint8_t *)(writeData+readSize), (size_t)(writeSize - readSize));
                    } else {
                        ret = spi_write_read_blocking(m_spi[bus].spi, (const uint8_t *)writeData, (uint8_t *)readData, (size_t)writeSize);
                        ret += spi_read_blocking(m_spi[bus].spi, RP2040_SPI_DUMMY_BYTE, (uint8_t *)(readData+writeSize), (size_t)(readSize - writeSize));
                    }
                    ret = spi_write_read_blocking(m_spi[bus].spi, (const uint8_t *)writeData, (uint8_t *)readData, (size_t)writeSize);
                } else if (writeSize != 0) {
                    ret = spi_write_blocking(m_spi[bus].spi, (const uint8_t *)writeData, (size_t)writeSize);
                } else if (readSize != 0) {
                    ret = spi_read_blocking(m_spi[bus].spi, RP2040_SPI_DUMMY_BYTE, (uint8_t *)readData, (size_t)readSize);
                }
            } else {
                if (writeSize != 0) {
                    ret = spi_write_blocking(m_spi[bus].spi, (const uint8_t *)writeData, (size_t)writeSize);
                }
                if (readSize != 0) {
                    ret += spi_read_blocking(m_spi[bus].spi, RP2040_SPI_DUMMY_BYTE, (uint8_t *)readData, (size_t)readSize);
                }
            }
        } else if (bits == 16) {
            if (wrc.fullDuplex) {
                if ((writeSize != 0) && (readSize != 0)) {
                    if (writeSize > readSize) {
                        ret = spi_write16_read16_blocking((spi_inst_t *)m_spi[bus].spi, (const uint16_t *)writeData, (uint16_t *)readData, (size_t)readSize);
                        ret += spi_write16_blocking(m_spi[bus].spi, (const uint16_t *)(writeData+readSize), (size_t)(writeSize - readSize));
                    } else {
                        ret = spi_write16_read16_blocking((spi_inst_t *)m_spi[bus].spi, (const uint16_t *)writeData, (uint16_t *)readData, (size_t)writeSize);
                        ret += spi_read16_blocking(m_spi[bus].spi, RP2040_SPI_DUMMY_BYTE, (uint16_t *)(readData+writeSize), (size_t)(readSize - writeSize));
                    }
                    ret = spi_write16_read16_blocking(m_spi[bus].spi, (const uint16_t *)writeData, (uint16_t *)readData, (size_t)writeSize);
                } else if (writeSize != 0) {
                    ret = spi_write16_blocking(m_spi[bus].spi, (const uint16_t *)writeData, (size_t)writeSize);
                } else if (readSize != 0) {
                    ret = spi_read16_blocking(m_spi[bus].spi, RP2040_SPI_DUMMY_BYTE, (uint16_t *)readData, (size_t)readSize);
                }
            } else {
                if (writeSize != 0) {
                    ret = spi_write16_blocking(m_spi[bus].spi, (const uint16_t *)writeData, (size_t)writeSize);
                }
                if (readSize != 0) {
                    ret += spi_read16_blocking(m_spi[bus].spi, RP2040_SPI_DUMMY_BYTE, (uint16_t *)readData, (size_t)readSize);
                }
            }
        }
        CPU_GPIO_SetPinState((GPIO_PIN)cs, GpioPinValue_High);
    }
    NANOCLR_NOCLEANUP();
}

// Performs a read/write operation on 16-bit word data.
HRESULT CPU_SPI_nWrite16_nRead16(
    uint32_t deviceHandle,
    SPI_DEVICE_CONFIGURATION &sdev,
    SPI_WRITE_READ_SETTINGS &swrs,
    uint16_t *writePtr,
    int32_t writeSize,
    uint16_t *readPtr,
    int32_t readSize)
{
    swrs.Bits16ReadWrite = true;
    return CPU_SPI_nWrite_nRead(deviceHandle, sdev, swrs, (uint8_t *)writePtr, writeSize, (uint8_t *)readPtr, readSize);
}

// Return status of current SPI operation
// Used to find status of an Async SPI call ( Not supported ), just return complete
SPI_OP_STATUS CPU_SPI_OP_STATUS(uint8_t spi_bus, uint32_t deviceHandle)
{
    (void)spi_bus;
    (void)deviceHandle;
    // ToDo: implement

    return SPI_OP_COMPLETE;
}

// Return map of available SPI buses as a bit map
uint32_t CPU_SPI_PortsMap()
{
    // ToDo: implement
    return RP2040_SPI_DEF_PORT_MAP;
}

// Return pins used for SPI bus
void CPU_SPI_GetPins(uint32_t spi_bus, GPIO_PIN &clk, GPIO_PIN &miso, GPIO_PIN &mosi)
{
    if (spi_bus >> RP2040_SPI_COUNT)
    {
        return;
    }
    clk = (GPIO_PIN)m_spi[spi_bus].clk;
    mosi = (GPIO_PIN)m_spi[spi_bus].mosi;
    miso = (GPIO_PIN)m_spi[spi_bus].miso;
}

// Return SPI minimum clock frequency
HRESULT CPU_SPI_MinClockFrequency(uint32_t spiBus, int32_t *frequency)
{
    if (spiBus >= NUM_SPI_BUSES)
    {
        return CLR_E_INVALID_PARAMETER;
    }
    // TODO check what is minimum ( min clock that can be configured on chip, master only)
    *frequency = RP2040_SPI_MIN_FREQ;
    return S_OK;
}

// Return SPI maximum clock frequency
HRESULT CPU_SPI_MaxClockFrequency(uint32_t spiBus, int32_t *frequency)
{
    if (spiBus >= NUM_SPI_BUSES)
    {
        return CLR_E_INVALID_PARAMETER;
    }
    *frequency = RP2040_SPI_MAX_FREQ;
    return S_OK;
}

//
// Return the number of chip select lines available on the bus.
// TODO this still needs to be sorted as no CS handling
uint32_t CPU_SPI_ChipSelectLineCount(uint32_t spi_bus)
{
    // ToDo: implement
    (void)spi_bus;
    return (MAX_SPI_DEVICES);
}
