//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <stdio.h>
#include <stdbool.h>
#include "boards/pico.h"
#include "hardware/clocks.h"
#include "hardware/rtc.h"
#include "pico/stdlib.h"
#include "pico/util/datetime.h"

#include "FreeRTOS.h"
#include "task.h"

#include <nanoHAL_v2.h>

#include <WireProtocol_ReceiverThread.h>
#include <nanoCLR_Application.h>
#include "Target_BlockStorage_RP2040_FlashDriver.h"
#include "CLR_Startup_Thread.h"
#include "tusb.h"
#include "mbedtls_config.h"

#if !defined(DEBUG_MAIN)
#define DEBUG_MAIN  0
#endif
#if !defined(DEBUG_CALLOC_MBEDTLS_FAIL)
#define DEBUG_CALLOC_MBEDTLS_FAIL   1
#endif
#if (DEBUG_RP2040 & DEBUG_MAIN) | (DEBUG_RP2040 & DEBUG_CALLOC_MBEDTLS_FAIL)
#include "debug_printf.h"
#endif

// configure heap memory
uint8_t ucHeap[configTOTAL_HEAP_SIZE];

//
// memory functions for mbedtls from MicroPython
//
void *m_calloc_mbedtls(size_t nmemb, size_t size) {
    void **ptr = platform_malloc(nmemb * size + 2 * sizeof(uintptr_t));
#if DEBUG_RP2040 & DEBUG_CALLOC_MBEDTLS_FAIL
    if (ptr == (void **)NULL) {
        DEBUG_PRINTF("m_calloc_mbedtls: %s size=%d p=%08x\n", ptr == (void **)NULL? "NG":"OK", (int)size, (uint)ptr);
    }
#endif
    return &ptr[2];
}

void m_free_mbedtls(void *ptr_in) {
    if (ptr_in == NULL) {
        return;
    }
    void **ptr = &((void**)ptr_in)[-2];
    platform_free(ptr);
}

//
// thread for tinyusb
//
#define USBD_STACK_SIZE     (3*configMINIMAL_STACK_SIZE)
StaticTask_t usb_device_taskdef;

// USB Device Driver task
// This top level thread process all usb events and invoke callbacks
void USBDeviceThread(void* param)
{
  (void) param;

  // This should be called after scheduler/kernel is started.
  // Otherwise it could cause kernel issue since USB IRQ handler does use RTOS queue API.
  tusb_init();

  // RTOS forever loop
  while (1)
  {
    // tinyusb device task
    tud_task();
    vTaskDelay(1);
  }
}

#if PICO_CONFIG_CLOCK
/**
 * ----------------------------------------------------------------------------------------------------
 * Macros
 * ----------------------------------------------------------------------------------------------------
 */
/* Clock */
#define PLL_SYS_KHZ (133 * 1000)

/**
 * ----------------------------------------------------------------------------------------------------
 * Functions
 * ----------------------------------------------------------------------------------------------------
 */
/* Clock */
static void set_clock_khz(void)
{
    // set a system clock frequency in khz
    set_sys_clock_khz(PLL_SYS_KHZ, true);

    // configure the specified clock
    clock_configure(
        clk_peri,
        0,                                                // No glitchless mux
        CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLKSRC_PLL_SYS, // System PLL on AUX mux
        PLL_SYS_KHZ * 1000,                               // Input frequency
        PLL_SYS_KHZ * 1000                                // Output (must be same as no divider)
    );
}
#endif

void set_hardfault_handler(void);

int main(void)
// void main_task(void *pvParameters)
{
    set_hardfault_handler();
#if PICO_CONFIG_CLOCK
    set_clock_khz();
#endif
#if DEBUG_RP2040
    stdio_init_all();
#endif
#if DEBUG_RP2040 & DEBUG_MAIN
    printf("nanoCLR: start tasks\r\n");
#endif
    rtc_init();
    datetime_t t = {
            .year  = 2022,
            .month = 04,
            .day   = 11,
            .dotw  = 1, // 0 is Sunday, so 5 is Friday
            .hour  = 00,
            .min   = 00,
            .sec   = 00
    };
    rtc_set_datetime(&t);
    xTaskCreate(USBDeviceThread, "USBDeviceThread", USBD_STACK_SIZE, NULL, configMAX_PRIORITIES - 1, NULL);
    xTaskCreate(ReceiverThread, "ReceiverThread", 1024, NULL, configMAX_PRIORITIES - 2, NULL);
    xTaskCreate(CLRStartupThread, "CLRStartupThread", 4096, NULL, configMAX_PRIORITIES - 2, NULL);
#if PICO_CONFIG_SDCARD
    xTaskCreate(SdCardThread, "SDCardThread", configMINIMAL_STACK_SIZE + 100, NULL, configMAX_PRIORITIES - 2, NULL);
#endif
    vTaskStartScheduler();

    for (;;)
        ;
    return 0;
}
