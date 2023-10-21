//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <string.h>
#include "win_dev_i2c_native_target.h"
#include "board.h"
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define NANO_I2C_MAX    2

typedef struct
{
    uint8_t slaveAddress;
    uint8_t *txBuffer;
    int txSize;
    uint8_t *rxBuffer;
    int rxSize;
} i2c_structure_t;


static i2c_inst_t *m_i2c[NANO_I2C_MAX] = {
        (i2c_inst_t *)i2c0,    // ch=0 for GP8,GP9
        (i2c_inst_t *)i2c1,    // ch=1 for GP6,GP7
};

i2c_structure_t m_nano_i2c[NANO_I2C_MAX];

typedef Library_win_dev_i2c_native_Windows_Devices_I2c_I2cConnectionSettings I2cConnectionSettings;

i2c_structure_t *I2C_ChoosePeripheralStructure(uint8_t busIndex)
{
    if ((busIndex < 1) | (busIndex > NANO_I2C_MAX))
    {
        return NULL;
    }
    else
    {
        return (i2c_structure_t *)&m_nano_i2c[busIndex - 1];
    }
}

void I2C_InitPins(uint8_t busIndex)
{
    const uint sda0_pin = RP2040_I2C_SDA0;
    const uint scl0_pin = RP2040_I2C_SCL0;
    const uint sda1_pin = RP2040_I2C_SDA1;
    const uint scl1_pin = RP2040_I2C_SCL1;
    // Initialize I2C pins
    switch (busIndex)
    {
        case 1:
            //Initialize I2C port at 400 kHz
            i2c_init(m_i2c[0], 400 * 1000);
            gpio_set_function(sda0_pin, GPIO_FUNC_I2C);
            gpio_set_function(scl0_pin, GPIO_FUNC_I2C);
            break;

        case 2:
            i2c_init(m_i2c[1], 400 * 1000);
            gpio_set_function(sda1_pin, GPIO_FUNC_I2C);
            gpio_set_function(scl1_pin, GPIO_FUNC_I2C);
            break;

    }
}

void I2C_InitClocks(void)
{
    // ToDo: implement
}

int I2C_WriteReadProcedure(uint8_t busIndex, i2c_structure_t *pI2Cx)
{
    int wret = 0;
    int rret = 0;
    if ((busIndex < 1) | (busIndex > NANO_I2C_MAX))
    {
        return I2cTransferStatus_UnknownError;
    }
    m_i2c[busIndex - 1]->restart_on_next = false;
    wret = i2c_write_blocking(
            (i2c_inst *)m_i2c[busIndex - 1],
            (uint8_t)m_nano_i2c[busIndex -1].slaveAddress,
            (const uint8_t *)pI2Cx->txBuffer,
            (size_t)pI2Cx->txSize,
            true);
    if (wret < 0) {
        return wret;
    }
    m_i2c[busIndex - 1]->restart_on_next = true;
    rret = i2c_read_blocking(
            (i2c_inst *)m_i2c[busIndex - 1],
            (uint8_t)m_nano_i2c[busIndex -1].slaveAddress,
            (uint8_t *)pI2Cx->rxBuffer,
            (size_t)pI2Cx->rxSize,
            false);
    if (rret < 0) {
        return rret;
    }
    return wret + rret;
}

int I2C_WriteProcedure(uint8_t busIndex, i2c_structure_t *pI2Cx)
{
    int ret = 0;
    if ((busIndex < 1) | (busIndex > NANO_I2C_MAX))
    {
        return I2cTransferStatus_UnknownError;
    }
    m_i2c[busIndex - 1]->restart_on_next = false;
    ret = i2c_write_blocking(
            (i2c_inst *)m_i2c[busIndex - 1],
            (uint8_t)m_nano_i2c[busIndex -1].slaveAddress,
            (const uint8_t *)pI2Cx->txBuffer,
            (size_t)pI2Cx->txSize,
            false);
    return ret;
}

int I2C_ReadProcedure(uint8_t busIndex, i2c_structure_t *pI2Cx)
{
    int ret = 0;
    if ((busIndex < 1) | (busIndex > NANO_I2C_MAX))
    {
        return I2cTransferStatus_UnknownError;
    }
    m_i2c[busIndex - 1]->restart_on_next = false;
    ret = i2c_read_blocking(
            (i2c_inst *)m_i2c[busIndex - 1],
            (uint8_t)m_nano_i2c[busIndex -1].slaveAddress,
            (uint8_t *)pI2Cx->rxBuffer,
            (size_t)pI2Cx->rxSize,
            false);
    return ret;
}

void I2C_ClearBuffers(i2c_structure_t *pI2Cx)
{
    pI2Cx->txBuffer = 0;
    pI2Cx->txSize = 0;
    pI2Cx->rxBuffer = 0;
    pI2Cx->rxSize = 0;
}

// It seems to be different in I2cTransferStatus between sys_dev_i2c and win_dev_i2c
static I2cTransferStatus I2cTransferStatusCheck(int ret, int size)
{
    I2cTransferStatus status;
    if (ret == size)
    {
        status = I2cTransferStatus_FullTransfer;
    }
    else if ((ret < size) && (ret >= 0))
    {
        status = I2cTransferStatus_PartialTransfer;
    }
    else if (ret == (int)PICO_ERROR_TIMEOUT)
    {
        status = I2cTransferStatus_ClockStretchTimeout;
    }
    else if (ret == (int)PICO_ERROR_GENERIC)
    {
        status =  I2cTransferStatus_UnknownError;
    }
    return status;
}

HRESULT Library_win_dev_i2c_native_Windows_Devices_I2c_I2cDevice::NativeInit___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        i2c_structure_t *pI2Cx;
        uint8_t busIndex;

        // Get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        // Get a pointer to the managed I2C connectionSettings object instance
        CLR_RT_HeapBlock *pConfig = pThis[FIELD___connectionSettings].Dereference();

        // Get bus index
        // this is coded with a multiplication, need to perform and int division to get the number
        // see the comments in the I2cDevice() constructor in managed code for details
        busIndex = (uint8_t)(pThis[FIELD___deviceId].NumericByRef().s4 / 1000);
        pI2Cx = I2C_ChoosePeripheralStructure(busIndex);
        I2C_InitPins(busIndex);
        I2C_InitClocks();

        pI2Cx->slaveAddress = pConfig[I2cConnectionSettings::FIELD___slaveAddress].NumericByRef().s4;
        // ToDo: implement i2c_init

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_i2c_native_Windows_Devices_I2c_I2cDevice::NativeDispose___VOID__BOOLEAN(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        uint8_t busIndex;
        bool disposeController = false;

        // Get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        disposeController = (bool)stack.Arg0().NumericByRef().u1;

        if (disposeController)
        {
            busIndex = (uint8_t)(pThis[FIELD___deviceId].NumericByRef().s4 / 1000);
            // ToDo: implement i2c_deinit
        }
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_i2c_native_Windows_Devices_I2c_I2cDevice::
    NativeTransmit___WindowsDevicesI2cI2cTransferResult__SZARRAY_U1__SZARRAY_U1(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        i2c_structure_t *pI2Cx;
        int ret = 0;
        I2cTransferStatus status = I2cTransferStatus_UnknownError;

        CLR_RT_HeapBlock_Array *writeBuffer;
        CLR_RT_HeapBlock_Array *readBuffer;
        CLR_RT_HeapBlock *result;

        // Get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);
        CLR_RT_HeapBlock &top = stack.PushValueAndClear();

        // Get bus index
        // This is coded with a multiplication, need to perform and int division to get the number
        // See the comments in the I2cDevice() constructor in managed code for details
        uint8_t busIndex = (uint8_t)(pThis[FIELD___deviceId].NumericByRef().s4 / 1000);
        pI2Cx = I2C_ChoosePeripheralStructure(busIndex);

        I2C_ClearBuffers(pI2Cx);
        // Dereference the write buffer from the arguments
        writeBuffer = stack.Arg1().DereferenceArray();
        if (writeBuffer != NULL)
        {
            pI2Cx->txBuffer = writeBuffer->GetFirstElement();
            pI2Cx->txSize = writeBuffer->m_numOfElements;
        }

        readBuffer = stack.Arg2().DereferenceArray();
        if (readBuffer != NULL)
        {
            pI2Cx->rxBuffer = readBuffer->GetFirstElement();
            pI2Cx->rxSize = readBuffer->m_numOfElements;
        }

        if ((pI2Cx->txSize != 0) && (pI2Cx->rxSize != 0))
        {
            ret = I2C_WriteReadProcedure(busIndex, pI2Cx);
            status = I2cTransferStatusCheck(ret, pI2Cx->txSize + pI2Cx->rxSize);
        }
        else if (pI2Cx->rxSize != 0)
        {
            ret = I2C_ReadProcedure(busIndex, pI2Cx);
            status = I2cTransferStatusCheck(ret, pI2Cx->rxSize);
        }
        else if (pI2Cx->txSize != 0)
        {
            ret = I2C_WriteProcedure(busIndex, pI2Cx);
            status = I2cTransferStatusCheck(ret, pI2Cx->txSize);
        }

        // Create return object
        NANOCLR_CHECK_HRESULT(
            g_CLR_RT_ExecutionEngine.NewObjectFromIndex(top, g_CLR_RT_WellKnownTypes.m_I2cTransferResult_old));
        result = top.Dereference();
        FAULT_ON_NULL(result);

        // ToDo: error check
        uint32_t transferResult = (uint32_t)status;
        result[Library_win_dev_i2c_native_Windows_Devices_I2c_I2cTransferResult::FIELD___status].SetInteger(
            (CLR_UINT32)transferResult);
        result[Library_win_dev_i2c_native_Windows_Devices_I2c_I2cTransferResult::FIELD___bytesTransferred].SetInteger(
            (CLR_UINT32)(pI2Cx->txSize + pI2Cx->rxSize));
    }
    NANOCLR_NOCLEANUP();
}
