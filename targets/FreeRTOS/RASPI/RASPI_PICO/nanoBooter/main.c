//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <stdio.h>
#include <stdbool.h>
#include "boards/pico.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "FreeRTOS.h"
#include "task.h"

#include <targetHAL.h>
#include <WireProtocol_ReceiverThread.h>
#include <nanoPAL_BlockStorage.h>
#include "nanoHAL_ConfigurationManager.h"
#include "Target_BlockStorage_RP2040_FlashDriver.h"
#include <nanoHAL_v2.h>
#include "tusb.h"

// configure heap memory
uint8_t ucHeap[configTOTAL_HEAP_SIZE];

#define USBD_STACK_SIZE     (3*configMINIMAL_STACK_SIZE)
StaticTask_t usb_device_taskdef;

// USB Device Driver task
// This top level thread process all usb events and invoke callbacks
void usb_device_task(void* param)
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

static void blink_task(void *pvParameters)
{
    (void)pvParameters;

    /* Init output LED GPIO. */
    gpio_init((uint)PICO_DEFAULT_LED_PIN);
    gpio_set_dir((uint)PICO_DEFAULT_LED_PIN, GPIO_OUT);

    for (;;)
    {
        vTaskDelay(100);
        gpio_put((uint)PICO_DEFAULT_LED_PIN, true);
        vTaskDelay(500);
        gpio_put((uint)PICO_DEFAULT_LED_PIN, false);
    }
}

static void boot_nanoCLR(void) {

    uint32_t button = 0;

    /* Define the init structure for the input button pin*/
    // gpio_pin_config_t button_config = {kGPIO_DigitalInput, 0, kGPIO_NoIntmode};

    /* Init input GPIO*/
    // GPIO_PinInit(BOARD_USER_BUTTON_GPIO, BOARD_USER_BUTTON_GPIO_PIN, &button_config);
    // button = GPIO_PinRead(BOARD_USER_BUTTON_GPIO, BOARD_USER_BUTTON_GPIO_PIN);

    /* Button is active low.
       Load nanoCLR program through resetISR or if button is pressed init reciver Task */
    if (button)
    {
        void (*nanoCLR)(void);
        nanoCLR = (void *)*((uint32_t *)0x1008000); // resetISR address
        nanoCLR();
    }
}

int main(void)
// void main_task(void *pvParameters)
{
    stdio_init_all();

    // BOARD_InitBootPins();
    // BOARD_InitBootClocks();
    // BOARD_InitBootPeripherals();

    // init boot clipboard
    InitBootClipboard();

    // check if there is a request to remain on nanoBooter
    if (!IsToRemainInBooter())
    {
        boot_nanoCLR();
    }

    // RP2040FlexSPIDriver_InitializeDevice(NULL);

    // initialize block storage list and devices
    // in CLR this is called in nanoHAL_Initialize()
    // for nanoBooter we have to init it in order to provide the flash map for Monitor_FlashSectorMap command
    BlockStorageList_Initialize();
    BlockStorage_AddDevices();

    // initialize configuration manager
    // in CLR this is called in nanoHAL_Initialize()
    // for nanoBooter we have to init it here to have access to network configuration blocks
    ConfigurationManager_Initialize();

    // report successfull nanoBooter execution
    ReportSuccessfullNanoBooter();

    printf("nanoBooter: start tasks\r\n");
    xTaskCreate(blink_task, "blink_task", configMINIMAL_STACK_SIZE + 10, NULL, configMAX_PRIORITIES - 1, NULL);
    xTaskCreate(usb_device_task, "usbd", USBD_STACK_SIZE, NULL, configMAX_PRIORITIES - 1, NULL);
    xTaskCreate(ReceiverThread, "ReceiverThread", 2048, NULL, configMAX_PRIORITIES - 2, NULL);
    vTaskStartScheduler();

    for (;;)
        ;
    return 0;
}
