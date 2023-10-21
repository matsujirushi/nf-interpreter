//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <stdint.h>

// GPIO

#define RP2040_GPIO_COUNT   30
#define RP2040_GPIO_DEF_BOUNCE_PERIOD   200000  // default 200ms
#define RP2040_GPIO_0_RESERVED  0
#define RP2040_GPIO_1_RESERVED  0
#define RP2040_GPIO_2_RESERVED  0
#define RP2040_GPIO_3_RESERVED  0
#define RP2040_GPIO_4_RESERVED  0
#define RP2040_GPIO_5_RESERVED  0
#define RP2040_GPIO_6_RESERVED  1	// I2C2 SDA
#define RP2040_GPIO_7_RESERVED  1	// I2C2 SCL
#define RP2040_GPIO_8_RESERVED  0
#define RP2040_GPIO_9_RESERVED  0
#define RP2040_GPIO_10_RESERVED  0
#define RP2040_GPIO_11_RESERVED  0
#define RP2040_GPIO_12_RESERVED  0
#define RP2040_GPIO_13_RESERVED  0
#define RP2040_GPIO_14_RESERVED  0
#define RP2040_GPIO_15_RESERVED  0
#define RP2040_GPIO_16_RESERVED  1	// W5100S MISO
#define RP2040_GPIO_17_RESERVED  1	// W5100S CS
#define RP2040_GPIO_18_RESERVED  1	// W5100S CLK
#define RP2040_GPIO_19_RESERVED  1	// W5100S MOSI
#define RP2040_GPIO_20_RESERVED  1	// W5100S RST
#define RP2040_GPIO_21_RESERVED  1	// W5100S INT
#define RP2040_GPIO_22_RESERVED  0
#define RP2040_GPIO_23_RESERVED  0
#define RP2040_GPIO_24_RESERVED  0
#define RP2040_GPIO_25_RESERVED  0
#define RP2040_GPIO_26_RESERVED  0
#define RP2040_GPIO_27_RESERVED  0
#define RP2040_GPIO_28_RESERVED  0
#define RP2040_GPIO_29_RESERVED  0

// ADC
#define RP2040_ADC_MAX_COUNT  3
#define RP2040_ADC_MAX_VALUE  4095
#define RP2040_ADC_MIN_VALUE  0
#define RP2040_ADC_RESOLUTION 12
#define RP2040_ADC_0    26
#define RP2040_ADC_1    27
#define RP2040_ADC_2    28
#define RP2040_ADC_3    29

// I2C
#define RP2040_I2C_SDA0   8
#define RP2040_I2C_SCL0   9
#define RP2040_I2C_SDA1   6
#define RP2040_I2C_SCL1   7

// SPI
#define RP2040_SPI_MAX_FREQ     80000000
#define RP2040_SPI_MIN_FREQ     10000
#define RP2040_SPI_DEF_FREQ     20000000
#define RP2040_SPI_DEF_PORT_MAP 0x3     // busid: 0 and 1
#define RP2040_SPI_DUMMY_BYTE   0xff

#define RP2040_SPI_COUNT        2
#define RP2040_SPI1_ASSIGNED    1
#define RP2040_SPI1_CLK         18
#define RP2040_SPI1_MISO        16
#define RP2040_SPI1_MOSI        19
#define RP2040_SPI2_ASSIGNED    0
#define RP2040_SPI2_CLK         10
#define RP2040_SPI2_MISO        12
#define RP2040_SPI2_MOSI        11

// PWM
#define RP2040_PWM_MAX_FREQ     125000000
#define RP2040_PWM_MIN_FREQ     1
#define RP2040_PWM_PIN_COUNT    15
#define RP2040_PWM_DEF_FREQ     1000
#define RP2040_PWM_DEF_DUTY     50
#define RP2040_PWM_DEF_TID      1
#define RP2040_PWM_1      0
#define RP2040_PWM_2      1
#define RP2040_PWM_3      2
#define RP2040_PWM_4      3
#define RP2040_PWM_5      4
#define RP2040_PWM_6      5
#define RP2040_PWM_7      6
#define RP2040_PWM_8      7
#define RP2040_PWM_9      8
#define RP2040_PWM_10     9
#define RP2040_PWM_11     10
#define RP2040_PWM_12     11
#define RP2040_PWM_13     12
#define RP2040_PWM_14     13
#define RP2040_PWM_15     14
#define RP2040_PWM_16     15

// UART
#define RP2040_UART_1_TX 0      // uart0
#define RP2040_UART_1_RX 1      // uart0
#define RP2040_UART_2_TX 4      // uart1
#define RP2040_UART_2_RX 5      // uart1
#define RP2040_UART_3_TX 8      // uart1
#define RP2040_UART_3_RX 9      // uart1
// #define RP2040_UART_4_TX 12     // uart0
// #define RP2040_UART_4_RX 13     // uart0
// #define RP2040_UART_5_TX 16     // uart0
// #define RP2040_UART_5_RX 17     // uart0
#define NF_SERIAL_COMM_RP2040_UART_USE_UART1 1
#define NF_SERIAL_COMM_RP2040_UART_USE_UART2 1
#define NF_SERIAL_COMM_RP2040_UART_USE_UART3 1
#define NF_SERIAL_COMM_RP2040_UART_USE_UART4 0
#define NF_SERIAL_COMM_RP2040_UART_USE_UART5 0

// NETWORK
#undef _WIZCHIP_
#define _WIZCHIP_   W5100S
#define W5X00_SPI_PORT  spi0
#define W5X00_SPI_SCK   18
#define W5X00_SPI_MOSI  19
#define W5X00_SPI_MISO  16
#define W5X00_SPI_CS    17
#define W5X00_SPI_RST   20
#define W5X00_SPI_INT   21

uint32_t rp2040_adc_count(void);
extern const uint8_t m_ch_to_pin[];
