//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "win_dev_pwm_native_target.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "board.h"

typedef struct rp2040_pwn_ch {
    uint8_t tid;    // timer id
    uint8_t pin;    // pin number
    float freq;
    int duty;
} rp2040_pwm_t;

// channel index starts from 1
static rp2040_pwm_t rp2040_pwm_pins[] = {
    {
        (uint8_t)RP2040_PWM_DEF_TID,
        (uint8_t)RP2040_PWM_1,
        (float)RP2040_PWM_DEF_FREQ,
        (int)RP2040_PWM_DEF_DUTY,
    },
    {
        (uint8_t)RP2040_PWM_DEF_TID,
        (uint8_t)RP2040_PWM_2,
        (float)RP2040_PWM_DEF_FREQ,
        (int)RP2040_PWM_DEF_DUTY,
    },
    {
        (uint8_t)RP2040_PWM_DEF_TID,
        (uint8_t)RP2040_PWM_3,
        (float)RP2040_PWM_DEF_FREQ,
        (int)RP2040_PWM_DEF_DUTY,
    },
    {
        (uint8_t)RP2040_PWM_DEF_TID,
        (uint8_t)RP2040_PWM_4,
        (float)RP2040_PWM_DEF_FREQ,
        (int)RP2040_PWM_DEF_DUTY,
    },
    {
        (uint8_t)RP2040_PWM_DEF_TID,
        (uint8_t)RP2040_PWM_5,
        (float)RP2040_PWM_DEF_FREQ,
        (int)RP2040_PWM_DEF_DUTY,
    },
    {
        (uint8_t)RP2040_PWM_DEF_TID,
        (uint8_t)RP2040_PWM_6,
        (float)RP2040_PWM_DEF_FREQ,
        (int)RP2040_PWM_DEF_DUTY,
    },
    {
        (uint8_t)RP2040_PWM_DEF_TID,
        (uint8_t)RP2040_PWM_7,
        (float)RP2040_PWM_DEF_FREQ,
        (int)RP2040_PWM_DEF_DUTY,
    },
    {
        (uint8_t)RP2040_PWM_DEF_TID,
        (uint8_t)RP2040_PWM_8,
        (float)RP2040_PWM_DEF_FREQ,
        (int)RP2040_PWM_DEF_DUTY,
    },
    {
        (uint8_t)RP2040_PWM_DEF_TID,
        (uint8_t)RP2040_PWM_9,
        (float)RP2040_PWM_DEF_FREQ,
        (int)RP2040_PWM_DEF_DUTY,
    },
    {
        (uint8_t)RP2040_PWM_DEF_TID,
        (uint8_t)RP2040_PWM_10,
        (float)RP2040_PWM_DEF_FREQ,
        (int)RP2040_PWM_DEF_DUTY,
    },
    {
        (uint8_t)RP2040_PWM_DEF_TID,
        (uint8_t)RP2040_PWM_11,
        (float)RP2040_PWM_DEF_FREQ,
        (int)RP2040_PWM_DEF_DUTY,
    },
    {
        (uint8_t)RP2040_PWM_DEF_TID,
        (uint8_t)RP2040_PWM_12,
        (float)RP2040_PWM_DEF_FREQ,
        (int)RP2040_PWM_DEF_DUTY,
    },
    {
        (uint8_t)RP2040_PWM_DEF_TID,
        (uint8_t)RP2040_PWM_13,
        (float)RP2040_PWM_DEF_FREQ,
        (int)RP2040_PWM_DEF_DUTY,
    },
    {
        (uint8_t)RP2040_PWM_DEF_TID,
        (uint8_t)RP2040_PWM_14,
        (float)RP2040_PWM_DEF_FREQ,
        (int)RP2040_PWM_DEF_DUTY,
    },
    {
        (uint8_t)RP2040_PWM_DEF_TID,
        (uint8_t)RP2040_PWM_15,
        (float)RP2040_PWM_DEF_FREQ,
        (int)RP2040_PWM_DEF_DUTY,
    },
    {
        (uint8_t)RP2040_PWM_DEF_TID,
        (uint8_t)RP2040_PWM_16,
        (float)RP2040_PWM_DEF_FREQ,
        (int)RP2040_PWM_DEF_DUTY,
    },
    {
        (uint8_t)RP2040_PWM_DEF_TID,
        (uint8_t)RP2040_PWM_1,
        (float)RP2040_PWM_DEF_FREQ,
        (int)RP2040_PWM_DEF_DUTY,
    },
};
#define RP2040_PWM_CH_SIZE  (sizeof(rp2040_pwm_pins)/sizeof(rp2040_pwm_t))

static int GetChannel (int pin, int tid)
{
    (void)tid;
    for (int i = 0; i < (int)RP2040_PWM_CH_SIZE; i++) {
        if (pin == (int)rp2040_pwm_pins[i].pin) {
            return (int)(i + 1);
        }
    }
    return -1;
}

static void pwm_start(int pin, int tid) {
    (void)tid;
    static pwm_config pwm0_slice_config;
    uint pwm0_slice_num;
    int ch = GetChannel(pin, tid);
    if (ch != -1) {
        gpio_set_function(pin, GPIO_FUNC_PWM);
        pwm0_slice_num = pwm_gpio_to_slice_num(pin);
        pwm0_slice_config = pwm_get_default_config();
        pwm_set_gpio_level(pin, (pwm0_slice_config.top * rp2040_pwm_pins[ch].duty / 100));
        pwm_init(pwm0_slice_num, &pwm0_slice_config, true);
    }
}

static void pwm_stop(int pin, int tid) {
    (void)tid;
    static pwm_config pwm0_slice_config;
    uint pwm0_slice_num;
    int ch = GetChannel(pin, tid);
    if (ch != -1) {
        pwm0_slice_num = pwm_gpio_to_slice_num(pin);
        pwm_set_enabled(pwm0_slice_num, false);
    }
}

HRESULT Library_win_dev_pwm_native_Windows_Devices_Pwm_PwmPin::NativeSetActiveDutyCyclePercentage___VOID__U4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);
        int pin = (int)(pThis[ FIELD___pinNumber ].NumericByRef().u4);
        unsigned int duty = (unsigned int)stack.Arg1().NumericByRef().u4;
        int ch = GetChannel(pin, 0);
        if (ch != -1) {
            rp2040_pwm_pins[ch].duty = duty;
        } else {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_pwm_native_Windows_Devices_Pwm_PwmPin::NativeSetPolarity___VOID__U1( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_pwm_native_Windows_Devices_Pwm_PwmPin::NativeStart___VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);
        int pin = (int)(pThis[ FIELD___pinNumber ].NumericByRef().u4);
        CLR_RT_HeapBlock* controller = pThis[ FIELD___pwmController ].Dereference();
        double freq= controller[ Library_win_dev_pwm_native_Windows_Devices_Pwm_PwmController::FIELD___actualFrequency ].NumericByRef().r8;
        int ch = GetChannel(pin, RP2040_PWM_DEF_TID);
        if (ch != -1) {
            pwm_start(pin, RP2040_PWM_DEF_TID);
        } else {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_pwm_native_Windows_Devices_Pwm_PwmPin::NativeStop___VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);
        int pin = (int)(pThis[ FIELD___pinNumber ].NumericByRef().u4);
        int ch = GetChannel(pin, RP2040_PWM_DEF_TID);
        if (ch != -1) {
            pwm_stop(pin, RP2040_PWM_DEF_TID);
        } else {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_pwm_native_Windows_Devices_Pwm_PwmPin::NativeInit___VOID( CLR_RT_StackFrame& stack )
{
    (void)stack;

    NANOCLR_HEADER();

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_win_dev_pwm_native_Windows_Devices_Pwm_PwmPin::DisposeNative___VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        // dummy implementation
        // ToDo: implement
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);
        int pin = (int)(pThis[ FIELD___pinNumber ].NumericByRef().u4);
        int ch = GetChannel(pin, RP2040_PWM_DEF_TID);
        if (ch != -1) {
            pwm_stop(pin, RP2040_PWM_DEF_TID);
        } else {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
    }
    NANOCLR_NOCLEANUP();
}
