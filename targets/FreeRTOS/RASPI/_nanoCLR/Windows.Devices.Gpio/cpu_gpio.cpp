//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "win_dev_gpio_native_target.h"
#include "nf_rt_events_native.h"
#include <stdint.h>
#include "board.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"


typedef void (*rp2040_gpio_irq_cb)(void *param);
static rp2040_gpio_irq_cb m_irq_cb[RP2040_GPIO_COUNT];
static void *m_irq_param[RP2040_GPIO_COUNT];
static uint32_t m_debounce_period[RP2040_GPIO_COUNT];
static uint32_t m_debounce_start[RP2040_GPIO_COUNT];
static bool m_debounce_flag[RP2040_GPIO_COUNT];

static void rp2040_gpio_irq_cb_handler(uint gpio, uint32_t events)
{
    if (m_debounce_flag[gpio]) {
        if ((time_us_32() - m_debounce_start[gpio]) > m_debounce_period[gpio]) {
            m_debounce_flag[gpio] = false;
        } else {
            return;
        }
    }
    if (m_irq_cb[gpio] != (rp2040_gpio_irq_cb)NULL)
    {
        if (!m_debounce_flag[gpio]) {
            m_debounce_start[gpio] = time_us_32();
            m_debounce_flag[gpio] = true;
            (*m_irq_cb[gpio])((void *)m_irq_param[gpio]);
        }
    }
}

void rp2040_gpio_irq_init(uint gpio, GPIO_INT_EDGE edge, void *cb, void *param, uint32_t debounce)
{
    if ((int)gpio >= RP2040_GPIO_COUNT) {
        return;
    }
    m_irq_cb[gpio] = (rp2040_gpio_irq_cb)cb;
    m_irq_param[gpio] = param;
    m_debounce_period[gpio] = debounce;

    gpio_set_irq_enabled_with_callback(gpio, edge, true, (gpio_irq_callback_t)rp2040_gpio_irq_cb_handler);
}

void rp2040_gpio_irq_cb_init(void)
{
    for (int i = 0; i < RP2040_GPIO_COUNT; i++)
    {
        m_irq_cb[i] = (rp2040_gpio_irq_cb)NULL;
        m_irq_param[i] = (void *)NULL;
        m_debounce_period[i] = RP2040_GPIO_DEF_BOUNCE_PERIOD;
        m_debounce_start[i] = 0;
        m_debounce_flag[i] = false;
    }
}

bool CPU_GPIO_Initialize()
{
    rp2040_gpio_irq_cb_init();
    return true;
}

bool CPU_GPIO_Uninitialize()
{
    rp2040_gpio_irq_cb_init();
    return true;
}

// Set/reset reserved state of pin
bool CPU_GPIO_ReservePin(GPIO_PIN pinNumber, bool fReserve)
{
    // ToDo: implement
    if ((int)pinNumber >= RP2040_GPIO_COUNT) {
        return false;
    }
    return true;
}

// Return if Pin is reserved
bool CPU_GPIO_PinIsBusy(GPIO_PIN pinNumber)
{
    // ToDo: implement
    if ((int)pinNumber >= RP2040_GPIO_COUNT) {
        return true;
    }
	return false;
}

// Return maximum number of pins
int32_t CPU_GPIO_GetPinCount()
{
    return RP2040_GPIO_COUNT;
}

// Get current state of pin
GpioPinValue CPU_GPIO_GetPinState(GPIO_PIN pinNumber)
{
    if ((int)pinNumber >= RP2040_GPIO_COUNT) {
        return GpioPinValue_Low;
    }
    return (GpioPinValue)gpio_get((uint)pinNumber);
}

// Set Pin state
void CPU_GPIO_SetPinState(GPIO_PIN pinNumber, GpioPinValue PinState)
{
    if ((int)pinNumber >= RP2040_GPIO_COUNT) {
        return;
    }
    gpio_put((uint)pinNumber, (bool)PinState);
}

// Toggle pin state
void CPU_GPIO_TogglePinState(GPIO_PIN pinNumber)
{
    if ((int)pinNumber >= RP2040_GPIO_COUNT) {
        return;
    }
    bool PinState = gpio_get((uint)pinNumber);
    gpio_put((uint)pinNumber, (bool)!PinState);
}

//
// CPU_GPIO_EnableInputPin
// Enable input pin
//
bool CPU_GPIO_EnableInputPin(
    GPIO_PIN pinNumber,
    uint32_t debounceTimeMilliseconds,
    GPIO_INTERRUPT_SERVICE_ROUTINE pinISR,
    void *isrParam,
    GPIO_INT_EDGE intEdge,
    GpioPinDriveMode driveMode)
{
    if ((int)pinNumber >= RP2040_GPIO_COUNT) {
        return false;
    }
    gpio_init((uint)pinNumber);
    gpio_set_dir((uint)pinNumber, GPIO_IN);
    switch((int)driveMode) {
    case (int)GpioPinDriveMode_Input:
        gpio_disable_pulls ((uint)pinNumber);
        break;
    case (int)GpioPinDriveMode_InputPullDown:
        gpio_pull_down((uint)pinNumber);
        break;
    case (int)GpioPinDriveMode_InputPullUp:
        gpio_pull_up((uint)pinNumber);
        break;
    default:
        break;
    }
    rp2040_gpio_irq_init((uint)pinNumber, intEdge, (void *)pinISR, (void *)isrParam, debounceTimeMilliseconds);
    return true;
}

// Enable an output pin
//
// pinNumber    -   Gpio pin number
// InitialState -   Initial state of pin
// driveMode    -   Drive mode and resistors
// return       -   True if succesful, false invalid pin, pin not putput, invalid drive mode for ouptput
//
bool CPU_GPIO_EnableOutputPin(GPIO_PIN pinNumber, GpioPinValue InitialState, GpioPinDriveMode driveMode)
{
    (void)driveMode;

    gpio_init((uint)pinNumber);
    gpio_set_dir((uint)pinNumber, GPIO_OUT);
    gpio_put((uint)pinNumber, (bool)InitialState);
    return true;
}

void CPU_GPIO_DisablePin(GPIO_PIN pinNumber, GpioPinDriveMode driveMode, uint32_t alternateFunction)
{
    // ToDo: implement
}

// Validate pin and set drive mode
// return true if ok
bool CPU_GPIO_SetDriveMode(GPIO_PIN pinNumber, GpioPinDriveMode driveMode)
{
    // ToDo: implement
    return true;
}

bool CPU_GPIO_DriveModeSupported(GPIO_PIN pinNumber, GpioPinDriveMode driveMode)
{
    // ToDo: implement
    return false;
}

uint32_t CPU_GPIO_GetPinDebounce(GPIO_PIN pinNumber)
{
    // ToDo: implement
    return 0;
}

bool CPU_GPIO_SetPinDebounce(GPIO_PIN pinNumber, uint32_t debounceTimeMilliseconds)
{
    // ToDo: implement
    return false;
}
