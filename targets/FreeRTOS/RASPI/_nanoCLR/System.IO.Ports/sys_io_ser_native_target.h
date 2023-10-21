//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef SYS_IO_SER_NATIVE_
#define SYS_IO_SER_NATIVE_

#define UART_INTERRUPT_PRIO 4U

#include "FreeRTOS.h"
#include <nanoHAL.h>
#include <target_system_io_ports_config.h>
#include <sys_io_ser_native.h>
#include "hardware/uart.h"

typedef struct
{
    uint baudrate;
    uint databit;
    uint stopbit;
    uart_parity_t parity;
} uart_config_t;

// struct representing the UART
typedef struct
{
    uint32_t uart_num;
    uart_inst_t *uart;
    uart_config_t config;

    uint8_t *TxBuffer;
    uint16_t TxOngoingCount;

    HAL_RingBuffer<uint8_t> RxRingBuffer;
    uint8_t *RxBuffer;
    uint16_t RxBytesToRead;
    uint8_t WatchChar;
    uint8_t NewLineChar;
    uint32_t ReceivedBytesThreshold;

    TaskHandle_t xRTaskToNotify;
    TaskHandle_t xWTaskToNotify;

} NF_PAL_UART;

#endif //SYS_IO_SER_NATIVE_
