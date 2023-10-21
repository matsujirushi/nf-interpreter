//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "sys_io_ser_native.h"
#include "sys_io_ser_native_target.h"
#include "board.h"
#include "hardware/uart.h"
#include "hardware/irq.h"
#include "hardware/gpio.h"

extern "C"
{
    static void rp2040_uart_int_rx(void);
}

static uart_inst_t *uarts[2] = {
    uart0,
    uart1,
};

static const uint uart_num_to_id[5] {
    0,
    1,
    1,
    0,
    0,
};

// if uart instance is not assigned, value is 0
static uint uart_inst_to_num[2] = {
    0,
    0,
};

static uint rp2040_uart_irg_no(NF_PAL_UART *pal_uart)
{
    if (pal_uart->uart == uart0){
        return UART0_IRQ;
    }
    else
    {
        return UART1_IRQ;
    }
}

static bool rp2040_uart_init(NF_PAL_UART *pal_uart)
{
    uint uart_inst_id = uart_num_to_id[pal_uart->uart_num - 1];
    // if (uart_inst_to_num[uart_inst_id] != 0) {
    //     return false;
    // } else {
        uart_inst_to_num[uart_inst_id] = pal_uart->uart_num;
    // }
    uart_init(pal_uart->uart, pal_uart->config.baudrate);
    switch (pal_uart->uart_num) {
    case 1:
#if NF_SERIAL_COMM_RP2040_UART_USE_UART1
        gpio_set_function(RP2040_UART_1_TX, GPIO_FUNC_UART);
        gpio_set_function(RP2040_UART_1_RX, GPIO_FUNC_UART);
#endif
        break;
    case 2:
#if NF_SERIAL_COMM_RP2040_UART_USE_UART2
        gpio_set_function(RP2040_UART_2_TX, GPIO_FUNC_UART);
        gpio_set_function(RP2040_UART_2_RX, GPIO_FUNC_UART);
#endif
        break;
    case 3:
#if NF_SERIAL_COMM_RP2040_UART_USE_UART3
        gpio_set_function(RP2040_UART_3_TX, GPIO_FUNC_UART);
        gpio_set_function(RP2040_UART_3_RX, GPIO_FUNC_UART);
#endif
        break;
    case 4:
#if NF_SERIAL_COMM_RP2040_UART_USE_UART4
        gpio_set_function(RP2040_UART_4_TX, GPIO_FUNC_UART);
        gpio_set_function(RP2040_UART_4_RX, GPIO_FUNC_UART);
#endif
        break;
    case 5:
#if NF_SERIAL_COMM_RP2040_UART_USE_UART5
        gpio_set_function(RP2040_UART_5_TX, GPIO_FUNC_UART);
        gpio_set_function(RP2040_UART_5_RX, GPIO_FUNC_UART);
#endif
        break;
    }
    return true;
}

static void rp2040_uart_deinit(NF_PAL_UART *pal_uart)
{
    uart_deinit(pal_uart->uart);
    switch (pal_uart->uart_num) {
    case 1:
#if NF_SERIAL_COMM_RP2040_UART_USE_UART1
        gpio_set_function(RP2040_UART_1_TX, GPIO_FUNC_NULL);
        gpio_set_function(RP2040_UART_1_RX, GPIO_FUNC_NULL);
#endif
        break;
    case 2:
#if NF_SERIAL_COMM_RP2040_UART_USE_UART2
        gpio_set_function(RP2040_UART_2_TX, GPIO_FUNC_NULL);
        gpio_set_function(RP2040_UART_2_RX, GPIO_FUNC_NULL);
#endif
        break;
    case 3:
#if NF_SERIAL_COMM_RP2040_UART_USE_UART3
        gpio_set_function(RP2040_UART_3_TX, GPIO_FUNC_NULL);
        gpio_set_function(RP2040_UART_3_RX, GPIO_FUNC_NULL);
#endif
        break;
    case 4:
#if NF_SERIAL_COMM_RP2040_UART_USE_UART4
        gpio_set_function(RP2040_UART_4_TX, GPIO_FUNC_NULL);
        gpio_set_function(RP2040_UART_4_RX, GPIO_FUNC_NULL);
#endif
        break;
    case 5:
#if NF_SERIAL_COMM_RP2040_UART_USE_UART5
        gpio_set_function(RP2040_UART_5_TX, GPIO_FUNC_NULL);
        gpio_set_function(RP2040_UART_5_RX, GPIO_FUNC_NULL);
#endif
        break;
    }
    uint uart_inst_id = uart_num_to_id[pal_uart->uart_num - 1];
    uart_inst_to_num[uart_inst_id] = 0;
}

#if NF_SERIAL_COMM_RP2040_UART_USE_UART1
NF_PAL_UART Uart1_PAL;
#endif
#if NF_SERIAL_COMM_RP2040_UART_USE_UART2
NF_PAL_UART Uart2_PAL;
#endif
#if NF_SERIAL_COMM_RP2040_UART_USE_UART3
NF_PAL_UART Uart3_PAL;
#endif
#if NF_SERIAL_COMM_RP2040_UART_USE_UART4
NF_PAL_UART Uart4_PAL;
#endif
#if NF_SERIAL_COMM_RP2040_UART_USE_UART5
NF_PAL_UART Uart5_PAL;
#endif

// NF_PAL_UART *const Uart_PAL[] = {
NF_PAL_UART *Uart_PAL[] = {
#if NF_SERIAL_COMM_RP2040_UART_USE_UART1
    &Uart1_PAL,
#else
    (NF_PAL_UART *)NULL,
#endif
#if NF_SERIAL_COMM_RP2040_UART_USE_UART2
    &Uart2_PAL,
#else
    (NF_PAL_UART *)NULL,
#endif
#if NF_SERIAL_COMM_RP2040_UART_USE_UART3
    &Uart3_PAL,
#else
    (NF_PAL_UART *)NULL,
#endif
#if NF_SERIAL_COMM_RP2040_UART_USE_UART4
    &Uart4_PAL,
#else
    (NF_PAL_UART *)NULL,
#endif
#if NF_SERIAL_COMM_RP2040_UART_USE_UART5
    &Uart5_PAL,
#else
    (NF_PAL_UART *)NULL,
#endif
};

// Task firing up event after receiving wanted amount of bytes
static void vREvent(void *pvParameters)
{
    // This is main UART struct, holding config, handlers, and the rest of variables.d
    (void)pvParameters;
    uint32_t ulNotifiedValue;

    while (1)
    {
        xTaskNotifyWait(
            pdFALSE,          // Don't clear bits on entry.
            0xffffffff,       // Clear all bits on exit.
            &ulNotifiedValue, // Stores the notified value.
            portMAX_DELAY);

        // Check is there event from task reading bytes.
        if ((ulNotifiedValue & 0x02) != 0)
        {
            Events_Set(SYSTEM_EVENT_FLAG_COM_IN);
        }
    }
}

// Task for transmitting data through blocking uart
static void vWEvent(void *pvParameters)
{
    uint32_t ulNotifiedValue;
    uint8_t *uartNum_p = (uint8_t *)pvParameters;
    uint8_t uartNum = *uartNum_p;
    NF_PAL_UART *palUart = Uart_PAL[uartNum - 1];

    while (1)
    {
        xTaskNotifyWait(
            pdFALSE,          // Don't clear bits on entry.
            0xffffffff,       // Clear all bits on exit.
            &ulNotifiedValue, // Stores the notified value.
            portMAX_DELAY);

        if ((ulNotifiedValue & 0x01) != 0)
        {
            uart_write_blocking(palUart->uart, (const uint8_t *)palUart->TxBuffer, (size_t)palUart->TxOngoingCount);
            Events_Set(SYSTEM_EVENT_FLAG_COM_OUT);
        }
    }
}

static void rp2040_uart_int_rx(void)
{
    uint32_t status;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    for (int i = 0; i < 2; i++)
    {
        while (uart_is_readable(uarts[i]))
        {
            NF_PAL_UART *palUart = Uart_PAL[uart_inst_to_num[i] - 1];
            uint8_t ch = uart_getc(uarts[i]);
            palUart->RxRingBuffer.Push((uint8_t)ch);
            if (palUart->RxBytesToRead > 0)
            {
                // check if the requested bytes are available in the buffer
                if (palUart->RxRingBuffer.Length() >= palUart->RxBytesToRead)
                {
                    // reset Rx bytes to read count
                    palUart->RxBytesToRead = 0;
                    // Notify task that we want to receive data.
                    xTaskNotifyFromISR(palUart->xRTaskToNotify, 0x02, eSetBits, &xHigherPriorityTaskWoken);
                }
            }
            else if (palUart->NewLineChar > 0 && ch == palUart->NewLineChar)
            {
                // fire event for new line char found
                Events_Set(SYSTEM_EVENT_FLAG_COM_IN);
            }
        }
    }
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

static NF_PAL_UART *GetUartPAL(int index)
{
    // Choose the driver for this SerialDevice
    switch (index)
    {
#if NF_SERIAL_COMM_RP2040_UART_USE_UART1
        case 1:
            return &Uart1_PAL;
#endif

#if NF_SERIAL_COMM_RP2040_UART_USE_UART2
        case 2:
            return &Uart2_PAL;
#endif

#if NF_SERIAL_COMM_RP2040_UART_USE_UART3
        case 3:
            return &Uart3_PAL;
#endif

#if NF_SERIAL_COMM_RP2040_UART_USE_UART4
        case 4:
            return &Uart4_PAL;
#endif

#if NF_SERIAL_COMM_RP2040_UART_USE_UART5
        case 5:
            return &Uart5_PAL;
#endif
        default:
            // this COM port is not valid;
            return (NF_PAL_UART *)NULL;
    }
}

// public int BytesToRead { get; }
// Gets the number of bytes of data in the receive buffer.
HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::get_BytesToRead___I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    NF_PAL_UART *palUart;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // Choose the driver for this SerialDevice
    palUart = GetUartPAL((int)pThis[FIELD___portIndex].NumericByRef().s4);
    if (palUart == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    // get length of Rx ring buffer
    stack.SetResult_U4(palUart->RxRingBuffer.Length());
    NANOCLR_NOCLEANUP();
}

// public bool InvertSignalLevels { get; set; }
HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::get_InvertSignalLevels___BOOLEAN(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    (void)stack;

    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);

    NANOCLR_NOCLEANUP();
}

// public bool InvertSignalLevels { get; set; }
HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::set_InvertSignalLevels___VOID__BOOLEAN(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    (void)stack;

    NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);

    NANOCLR_NOCLEANUP();
}

// public int Read (byte[] buffer, int offset, int count);
// Reads from the SerialPort input buffer.
// Reads a number of bytes from the SerialPort input buffer and writes those bytes into a byte array at the specified offset.
HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::Read___I4__SZARRAY_U1__I4__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock hbTimeout;
    CLR_RT_HeapBlock_Array *dataBuffer = NULL;
    NF_PAL_UART *palUart = NULL;

    uint8_t *data = NULL;

    uint32_t length;
    uint32_t count = 0;
    uint32_t bytesRead = 0;
    uint32_t offset = 0;
    uint32_t bytesToRead = 0;

    int64_t *timeoutTicks;
    bool eventResult = true;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    if (pThis[FIELD___disposed].NumericByRef().u1 != 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
    }

    // setup timeout
    hbTimeout.SetInteger((CLR_INT64)pThis[FIELD___readTimeout].NumericByRef().s4 * TIME_CONVERSION__TO_MILLISECONDS);
    NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks(hbTimeout, timeoutTicks));

    // dereference the data buffer from the argument
    dataBuffer = stack.Arg1().DereferenceArray();
    offset = stack.Arg2().NumericByRef().s4;
    count = stack.Arg3().NumericByRef().s4;

    // perform parameter validation (only on initial call)
    if (stack.m_customState == 1)
    {
        // get the size of the buffer
        length = dataBuffer->m_numOfElements;

        // check parameters
        FAULT_ON_NULL_ARG(dataBuffer);

        if ((offset > length) || (count > length))
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
        }

        if (offset + count > length)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
    }

    // get a the pointer to the array by using the first element of the array
    data = dataBuffer->GetElement(offset);

    // Choose the driver for this SerialDevice
    palUart = GetUartPAL((int)pThis[FIELD___portIndex].NumericByRef().s4);
    if (palUart == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    // figure out what's available in the Rx ring buffer
    if (palUart->RxRingBuffer.Length() >= count)
    {
        // read from Rx ring buffer
        bytesToRead = count;

        // we have enough bytes, skip wait for event
        eventResult = false;

        // clear event by getting it
        Events_Get(SYSTEM_EVENT_FLAG_COM_IN);
    }
    else
    {
        if (stack.m_customState == 1)
        {
            // not enough bytes available, have to read from UART
            palUart->RxBytesToRead = count;

            // clear event by getting it
            Events_Get(SYSTEM_EVENT_FLAG_COM_IN);

            // don't read anything from the buffer yet
            bytesToRead = 0;
        }
    }

    while (eventResult)
    {
        if (stack.m_customState == 1)
        {
            if (bytesToRead > 0)
            {
                // enough bytes available
                eventResult = false;
            }
            else
            {
                // need to read from the UART
                // update custom state
                stack.m_customState = 2;
            }
        }
        else
        {
            // wait for event
            NANOCLR_CHECK_HRESULT(
                g_CLR_RT_ExecutionEngine
                    .WaitEvents(stack.m_owningThread, *timeoutTicks, Event_SerialPortIn, eventResult));

            if (!eventResult)
            {
                // event timeout
                NANOCLR_SET_AND_LEAVE(CLR_E_TIMEOUT);
            }
        }
    }

    if (bytesToRead > 0)
    {
        // pop the requested bytes from the ring buffer
        bytesRead = palUart->RxRingBuffer.Pop(data, bytesToRead);
    }

    // pop "hbTimeout" heap block from stack
    stack.PopValue();

    // return how many bytes were read
    stack.SetResult_U4(bytesRead);

    NANOCLR_NOCLEANUP();
}

// public string ReadExisting ();
// Reads all immediately available bytes, based on the encoding, in both the stream and the input buffer of the SerialPort object.
HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::ReadExisting___STRING(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NF_PAL_UART *palUart = NULL;

    uint8_t *buffer = NULL;
    uint32_t bufferLength;

    CLR_RT_HeapBlock &top = stack.PushValue();

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    if (pThis[FIELD___disposed].NumericByRef().u1 != 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
    }

    // Choose the driver for this SerialDevice
    palUart = GetUartPAL((int)pThis[FIELD___portIndex].NumericByRef().s4);
    if (palUart == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    bufferLength = palUart->RxRingBuffer.Length();

    if (bufferLength)
    {
        // there are bytes available in the Rx buffer
        // setup read buffer
        buffer = (uint8_t *)platform_malloc(bufferLength);

        // sanity check
        if (buffer == NULL)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
        }

        // fill data buffer from Rx buffer
        palUart->RxRingBuffer.Pop(buffer, bufferLength);

        NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance(top, (const char *)buffer, bufferLength));
    }
    else
    {
        // create an empty <string>
        NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance(top, (const char *)NULL));
    }

    NANOCLR_CLEANUP();

    if (buffer != NULL)
    {
        platform_free(buffer);
    }

    NANOCLR_CLEANUP_END();
}

// public string ReadLine ();
// Reads up to the NewLine value in the input buffer.
HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::ReadLine___STRING(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock hbTimeout;
    NF_PAL_UART *palUart = NULL;

    uint8_t *line = NULL;
    const char *newLine;
    uint32_t newLineLength;

    int64_t *timeoutTicks;
    bool eventResult = true;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    if (pThis[FIELD___disposed].NumericByRef().u1 != 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
    }

    // setup timeout
    hbTimeout.SetInteger((CLR_INT64)pThis[FIELD___readTimeout].NumericByRef().s4 * TIME_CONVERSION__TO_MILLISECONDS);
    NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks(hbTimeout, timeoutTicks));

    // Choose the driver for this SerialDevice
    palUart = GetUartPAL((int)pThis[FIELD___portIndex].NumericByRef().s4);
    if (palUart == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    if (stack.m_customState == 1)
    {
        // check if there is a full line available to read
        if (GetLineFromRxBuffer(pThis, &(palUart->RxRingBuffer), line))
        {
            // got one!
            eventResult = false;
        }
        else
        {
            // get new line from field
            newLine = pThis[FIELD___newLine].RecoverString();
            newLineLength = hal_strlen_s(newLine);
            // need to subtract one because we are 0 indexed
            newLineLength--;

            // set new line char as the last one in the string
            // only if this one is found it will have a chance of the others being there
            palUart->NewLineChar = newLine[newLineLength];

            stack.m_customState = 2;
        }
    }

    while (eventResult)
    {
        // wait for event
        NANOCLR_CHECK_HRESULT(
            g_CLR_RT_ExecutionEngine.WaitEvents(stack.m_owningThread, *timeoutTicks, Event_SerialPortIn, eventResult));

        // clear the new line watch char
        palUart->NewLineChar = 0;

        if (eventResult)
        {
            GetLineFromRxBuffer(pThis, &(palUart->RxRingBuffer), line);

            // done here
            break;
        }
        else
        {
            // event timeout
            NANOCLR_SET_AND_LEAVE(CLR_E_TIMEOUT);
        }
    }

    // pop "hbTimeout" heap block from stack
    stack.PopValue();

    // return how many bytes were read
    stack.SetResult_String((const char *)line);

    // free memory, if needed
    if (line != NULL)
    {
        platform_free(line);
    }

    NANOCLR_NOCLEANUP();
}

// public void Write(byte[] buffer, int offset, int count);
// Writes a specified number of bytes to the serial port using data from a buffer.
HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::Write___VOID__SZARRAY_U1__I4__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NF_PAL_UART *palUart = NULL;

    CLR_RT_HeapBlock_Array *dataBuffer;
    CLR_RT_HeapBlock hbTimeout;
    int64_t *timeoutTicks;
    bool eventResult = true;

    uint8_t *data;
    int32_t length = 0;
    int32_t count = 0;
    int32_t offset = 0;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    if (pThis[FIELD___disposed].NumericByRef().u1 != 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
    }

    // Choose the driver for this SerialDevice
    palUart = GetUartPAL((int)pThis[FIELD___portIndex].NumericByRef().s4);
    if (palUart == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    // setup timeout
    hbTimeout.SetInteger((CLR_INT64)pThis[FIELD___writeTimeout].NumericByRef().s4 * TIME_CONVERSION__TO_MILLISECONDS);
    NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks(hbTimeout, timeoutTicks));

    // perform parameter validation and setup TX operation
    if (stack.m_customState == 1)
    {
        // dereference the data buffer from the argument
        dataBuffer = stack.Arg1().DereferenceArray();
        offset = stack.Arg2().NumericByRef().s4;
        count = stack.Arg3().NumericByRef().s4;

        // get the size of the buffer
        length = dataBuffer->m_numOfElements;

        // check parameters
        FAULT_ON_NULL_ARG(dataBuffer);

        if ((offset > length) || (count > length))
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
        }

        if (offset + count > length)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }

        // get a the pointer to the array by using the first element of the array
        data = dataBuffer->GetElement(offset);

        // push onto the eval stack how many bytes are being pushed to the UART
        stack.PushValueI4(count);

        // store pointer
        palUart->TxBuffer = data;
        // set TX ongoing count
        palUart->TxOngoingCount = count;

        // Notify task that we want to transmit data.
        xTaskNotify(palUart->xWTaskToNotify, 0x01, eSetBits);

        // bump custom state
        stack.m_customState = 2;
    }

    while (eventResult)
    {
        // non-blocking wait allowing other threads to run while we wait for the TX operation to complete
        NANOCLR_CHECK_HRESULT(
            g_CLR_RT_ExecutionEngine.WaitEvents(stack.m_owningThread, *timeoutTicks, Event_SerialPortOut, eventResult));

        if (eventResult)
        {
            // event occurred
            // get from the eval stack how many bytes were buffered to TX
            count = stack.m_evalStack[1].NumericByRef().s4;

            // reset TX ongoing count
            palUart->TxOngoingCount = 0;

            // done here
            break;
        }
        else
        {
            palUart->TxOngoingCount = 0;
            NANOCLR_SET_AND_LEAVE(CLR_E_TIMEOUT);
        }
    }

    // pop "count" heap block from stack
    stack.PopValue();

    // pop "hbTimeout" heap block from stack
    stack.PopValue();

    stack.SetResult_U4(count);

    // null pointers and vars
    pThis = NULL;

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::NativeDispose___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NF_PAL_UART *palUart;

    uint8_t uartNum = 0;

    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    uartNum = pThis[FIELD___portIndex].NumericByRef().s4;

    // Quit if parameters or device is invalid or out of range
    if (uartNum >= (sizeof(Uart_PAL) / sizeof(Uart_PAL[0])))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    palUart = Uart_PAL[uartNum - 1];

    // Free ring buffer memory
    platform_free(palUart->RxBuffer);

    // null all pointers
    palUart->RxBuffer = NULL;
    palUart->TxBuffer = NULL;

    // Deinitialize device and delete FreeRTOS idle tasks
    rp2040_uart_deinit(palUart);
    vTaskDelete(palUart->xRTaskToNotify);
    vTaskDelete(palUart->xWTaskToNotify);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::NativeInit___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    BaseType_t xReturned;
    NF_PAL_UART *palUart;
    int32_t bufferSize;
    uint8_t watchChar;
    uint irq_no;
    bool flag;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // Choose the driver for this SerialDevice
    switch ((int)pThis[FIELD___portIndex].NumericByRef().s4)
    {
#if defined(NF_SERIAL_COMM_RP2040_UART_USE_UART1) && (NF_SERIAL_COMM_RP2040_UART_USE_UART1 == TRUE)
        case 1:
            // Init_UART1();
            Uart1_PAL.uart = uart0;
            Uart1_PAL.uart_num = 1;
            palUart = &Uart1_PAL;
            break;
#endif
#if defined(NF_SERIAL_COMM_RP2040_UART_USE_UART2) && (NF_SERIAL_COMM_RP2040_UART_USE_UART2 == TRUE)
        case 2:
            // Init_UART2();
            Uart2_PAL.uart = uart1;
            Uart2_PAL.uart_num = 2;
            palUart = &Uart2_PAL;
            break;
#endif
#if defined(NF_SERIAL_COMM_RP2040_UART_USE_UART3) && (NF_SERIAL_COMM_RP2040_UART_USE_UART3 == TRUE)
        case 3:
            // Init_UART3();
            Uart3_PAL.uart = uart1;
            Uart3_PAL.uart_num = 3;
            palUart = &Uart3_PAL;
            break;
#endif
#if defined(NF_SERIAL_COMM_RP2040_UART_USE_UART4) && (NF_SERIAL_COMM_RP2040_UART_USE_UART4 == TRUE)
        case 4:
            // Init_UART4();
            Uart4_PAL.uart = uart0;
            Uart4_PAL.uart_num = 4;
            palUart = &Uart4_PAL;
            break;
#endif
#if defined(NF_SERIAL_COMM_RP2040_UART_USE_UART5) && (NF_SERIAL_COMM_RP2040_UART_USE_UART5 == TRUE)
        case 5:
            // Init_UART5();
            Uart5_PAL.uart = uart0;
            Uart5_PAL.uart_num = 5;
            palUart = &Uart5_PAL;
            break;
#endif
        default:
            // this COM port is not valid
            // NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
            break;
    }

    // alloc buffer memory
    bufferSize = pThis[FIELD___bufferSize].NumericByRef().s4;
    palUart->RxBuffer = (uint8_t *)platform_malloc(bufferSize * sizeof(uint8_t));

    // sanity check
    if (palUart->RxBuffer == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
    }

    // Initialize RX buffer
    palUart->RxRingBuffer.Initialize(palUart->RxBuffer, bufferSize);
    palUart->RxBytesToRead = 0;

    // get watch character
    watchChar = pThis[FIELD___watchChar].NumericByRef().u1;

    // set watch char, if set
    if (watchChar != 0)
    {
        palUart->WatchChar = watchChar;
    }

    // now all the rest
    palUart->TxOngoingCount = 0;
    palUart->NewLineChar = 0;
    palUart->config.baudrate = 115200;
    flag = rp2040_uart_init(palUart);
    if (!flag) {
        NANOCLR_SET_AND_LEAVE(CLR_E_BUSY);
    }
    // Get default config structure for initializing given UART peripheral and enable TX, RX
    // Initialize UART peripheral with default config
    // And set up and enable the interrupt handlers
    irq_no = rp2040_uart_irg_no(palUart);
    irq_set_exclusive_handler(irq_no, rp2040_uart_int_rx);
    irq_set_enabled(irq_no, true);
    // Now enable the UART to send interrupts - RX only
    uart_set_irq_enables(palUart->uart, true, false);
    // Create idle task waiting for read/write.
    xReturned = xTaskCreate(
        vREvent,
        "UART Read Event",
        configMINIMAL_STACK_SIZE,
        (void *)palUart,
        configMAX_PRIORITIES - 1,
        &palUart->xRTaskToNotify);
    if (xReturned == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
    }

    xReturned = xTaskCreate(
        vWEvent,
        "UART Write Event",
        configMINIMAL_STACK_SIZE,
        (void *)palUart,
        configMAX_PRIORITIES,
        &palUart->xWTaskToNotify);

    if (xReturned == errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::NativeConfig___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    bool flag;
    NF_PAL_UART *palUart = NULL;


    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // Choose the driver for this SerialDevice
    palUart = GetUartPAL((int)pThis[FIELD___portIndex].NumericByRef().s4);
    if (palUart == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }
    // Check RS485 mode is not selected as currently not supported
    if ((SerialMode)pThis[FIELD___mode].NumericByRef().s4 != SerialMode_Normal)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_NOTIMPL);
    }
    // baudrate
    palUart->config.baudrate = (uint32_t)pThis[FIELD___baudRate].NumericByRef().s4;
    uart_set_baudrate (palUart->uart, palUart->config.baudrate);
    // databit
    palUart->config.databit = (uint32_t)pThis[FIELD___dataBits].NumericByRef().s4;
    // stopbit
    switch ((StopBits)pThis[FIELD___stopBits].NumericByRef().s4)
    {
        case StopBits_One:
            palUart->config.stopbit = 1;
            break;
        case StopBits_OnePointFive:
            palUart->config.stopbit = 1;
            break;
        case StopBits_Two:
            palUart->config.stopbit = 2;
            break;
    }
    // parity
    switch ((Parity)pThis[FIELD___parity].NumericByRef().s4)
    {
        case Parity_Even:
            palUart->config.parity = UART_PARITY_EVEN;
            break;
        case Parity_None:
            palUart->config.parity = UART_PARITY_NONE;
            break;
        case Parity_Odd:
            palUart->config.parity = UART_PARITY_ODD;
            break;
    }
    uart_set_format(palUart->uart, palUart->config.databit, palUart->config.stopbit, palUart->config.parity);
    flag = rp2040_uart_init(palUart);
    if (!flag) {
        NANOCLR_SET_AND_LEAVE(CLR_E_BUSY);
    }
    // null pointers and vars
    pThis = NULL;

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::NativeSetWatchChar___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NF_PAL_UART *palUart;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // Choose the driver for this SerialDevice
    palUart = GetUartPAL((int)pThis[FIELD___portIndex].NumericByRef().s4);
    if (palUart == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    // set watch char
    palUart->WatchChar = (uint8_t)pThis[FIELD___watchChar].NumericByRef().u1;

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::NativeWriteString___VOID__STRING__BOOLEAN(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NF_PAL_UART *palUart = NULL;

    CLR_RT_HeapBlock hbTimeout;
    int64_t *timeoutTicks;
    bool eventResult = true;

    bool isNewAllocation = false;
    char *buffer = NULL;
    uint32_t bufferLength;
    int32_t length = 0;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    if (pThis[FIELD___disposed].NumericByRef().u1 != 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
    }

    if (stack.Arg1().RecoverString() == NULL)
    {
        // text string it's empty so there is noting to do here
        stack.SetResult_U4(0);
        NANOCLR_SET_AND_LEAVE(S_OK);
    }

    // Choose the driver for this SerialDevice
    palUart = GetUartPAL((int)pThis[FIELD___portIndex].NumericByRef().s4);
    if (palUart == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    // setup timeout
    hbTimeout.SetInteger((CLR_INT64)pThis[FIELD___writeTimeout].NumericByRef().s4 * TIME_CONVERSION__TO_MILLISECONDS);
    NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks(hbTimeout, timeoutTicks));

    // perform parameter validation and setup TX operation
    if (stack.m_customState == 1)
    {
        // get buffer to output
        NANOCLR_CHECK_HRESULT(SetupWriteLine(stack, &buffer, &bufferLength, &isNewAllocation));

        // push onto the eval stack how many bytes are being pushed to the UART
        stack.PushValueI4(bufferLength);

        // store pointer
        palUart->TxBuffer = (uint8_t *)buffer;
        // set TX ongoing count
        palUart->TxOngoingCount = bufferLength;

        // Notify task that we want to transmit data.
        xTaskNotify(palUart->xWTaskToNotify, 0x01, eSetBits);

        // bump custom state
        stack.m_customState = 2;
    }

    while (eventResult)
    {
        // non-blocking wait allowing other threads to run while we wait for the Tx operation to complete
        NANOCLR_CHECK_HRESULT(
            g_CLR_RT_ExecutionEngine.WaitEvents(stack.m_owningThread, *timeoutTicks, Event_SerialPortOut, eventResult));

        if (eventResult)
        {
            // event occurred
            // get from the eval stack how many bytes were buffered to Tx
            length = stack.m_evalStack[1].NumericByRef().s4;

            // reset Tx ongoing count
            palUart->TxOngoingCount = 0;

            // done here
            break;
        }
        else
        {
            palUart->TxOngoingCount = 0;

            NANOCLR_SET_AND_LEAVE(CLR_E_TIMEOUT);
        }
    }

    // pop "length" heap block from stack
    stack.PopValue();

    // pop "hbTimeout" heap block from stack
    stack.PopValue();

    stack.SetResult_U4(length);

    // free memory, if it was allocated
    if (isNewAllocation && buffer)
    {
        platform_free(buffer);
    }

    // null pointers and vars
    pThis = NULL;

    NANOCLR_NOCLEANUP();
}

// public int ReceivedBytesThreshold { get; set; }
// Gets or sets the number of bytes in the internal input buffer before a DataReceived event occurs.
HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::NativeReceivedBytesThreshold___VOID__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NF_PAL_UART *palUart;
    int32_t threshold;
    uint8_t uartNum;

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // check if threshold is valid
    threshold = (int32_t)stack.Arg1().NumericByRef().s4;

    if (threshold <= 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_RANGE);
    }

    uartNum = (int)pThis[FIELD___portIndex].NumericByRef().s4;

    // Choose the driver for this SerialDevice
    palUart = Uart_PAL[uartNum - 1];

    // update field
    pThis[FIELD___receivedBytesThreshold].NumericByRef().s4 = threshold;

    // update threshold value
    palUart->ReceivedBytesThreshold = threshold;

    // fake call to event handler in case port is open and the new threshold was set
    // to a value lower than the bytes that are already available
    if (pThis[FIELD___opened].NumericByRef().u1 && (uint32_t)threshold <= palUart->RxRingBuffer.Length())
    {
        PostManagedEvent(EVENT_SERIAL, 0, uartNum, SerialData_Chars);
    }

    NANOCLR_NOCLEANUP();
}

// COM1: TX:GP0, RX:GP1
// COM2: TX:GP4, RX:GP5
// COM3: TX:GP8, RX:GP9
// COM4: TX:GP12, RX:GP13
// COM5: TX:GP16, RX:GP17

// public static string GetDeviceSelector(string portName);
HRESULT Library_sys_io_ser_native_System_IO_Ports_SerialPort::GetDeviceSelector___STATIC__STRING(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    // declare the device selector string whose max size is "COM1,COM2,COM3,COM4,COM5," + terminator
    // and init with the terminator
    char deviceSelectorString[25 + 1] = {0};

    #if defined(NF_SERIAL_COMM_RP2040_UART_USE_UART1) && (NF_SERIAL_COMM_RP2040_UART_USE_UART1 == TRUE)
        strcat(deviceSelectorString, "COM1,");
    #endif
    #if defined(NF_SERIAL_COMM_RP2040_UART_USE_UART2) && (NF_SERIAL_COMM_RP2040_UART_USE_UART2 == TRUE)
        strcat(deviceSelectorString, "COM2,");
    #endif
    #if defined(NF_SERIAL_COMM_RP2040_UART_USE_UART3) && (NF_SERIAL_COMM_RP2040_UART_USE_UART3 == TRUE)
        strcat(deviceSelectorString, "COM3,");
    #endif
    #if defined(NF_SERIAL_COMM_RP2040_UART_USE_UART4) && (NF_SERIAL_COMM_RP2040_UART_USE_UART4 == TRUE)
        strcat(deviceSelectorString, "COM4,");
    #endif
    #if defined(NF_SERIAL_COMM_RP2040_UART_USE_UART5) && (NF_SERIAL_COMM_RP2040_UART_USE_UART5 == TRUE)
        strcat(deviceSelectorString, "COM5,");
    #endif

    // replace the last comma with a terminator
    deviceSelectorString[hal_strlen_s(deviceSelectorString) - 1] = '\0';

    // because the caller is expecting a result to be returned
    // we need set a return result in the stack argument using the appropriate SetResult according to the variable
    // type (a string here)
    stack.SetResult_String(deviceSelectorString);
    NANOCLR_NOCLEANUP_NOLABEL();
}
