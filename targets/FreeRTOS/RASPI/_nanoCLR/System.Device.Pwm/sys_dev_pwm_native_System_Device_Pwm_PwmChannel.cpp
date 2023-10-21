//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <targetPAL.h>
#include <sys_dev_pwm_native.h>
#include "board.h"

HRESULT Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::NativeInit___VOID(CLR_RT_StackFrame &stack)
{
    (void)stack;

    NANOCLR_HEADER();

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::NativeSetDesiredFrequency___VOID__I4(
    CLR_RT_StackFrame &stack)
{
    uint32_t timerId;
    int32_t desiredFrequency;
    uint32_t period;

    // PWMConfig pwmConfig;
    // PWMDriver *pwmDriver;

    NANOCLR_HEADER();

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // Retrieves the needed parameters from private class properties
    timerId = pThis[FIELD___pwmTimer].NumericByRef().s4;
    desiredFrequency = stack.Arg1().NumericByRef().s4;

    // parameter check
    if (desiredFrequency < 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    // Gets the PWM driver associated with the requested timer
    // pwmDriver = GetDriverPwm(timerId);

    // Sets the period to something able to be precise enough with low and high frequencies
    // and that allows the clock frequency parameter to fit in an unsigned int
    period = 1000;
    if (desiredFrequency >= 1000)
    {
        period = 100;
    }
    else if (desiredFrequency >= 1000000)
    {
        period = 10;
    }

    // pwmConfig
    // Starts the PWM driver
    // pwmStop(pwmDriver);
    // osDelay(5);
    // pwmStart(pwmDriver, &pwmConfig);

    // store the frequency
    pThis[FIELD___frequency].NumericByRef().s4 = desiredFrequency;

    stack.SetResult_R8((double)desiredFrequency);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::NativeSetActiveDutyCyclePercentage___VOID__R8(
    CLR_RT_StackFrame &stack)
{
    int32_t timerId;
    int32_t channelId;
    uint32_t dutyCycle;

    // PWMDriver *pwmDriver;

    NANOCLR_HEADER();

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // Retrieves the needed parameters from private class properties or method parameters
    timerId = pThis[FIELD___pwmTimer].NumericByRef().s4;
    channelId = pThis[FIELD___channelNumber].NumericByRef().s4;

    // parameter check
    if (stack.Arg1().NumericByRef().r8 < 0 || stack.Arg1().NumericByRef().r8 > 1.0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    dutyCycle = (uint32_t)(stack.Arg1().NumericByRef().r8 * CONST_DutyCycleFactor);

    // Gets the PWM driver associated with the requested timer
    // pwmDriver = GetDriverPwm(timerId);

    // Enables the channel associated with the selected pin on that timer
    // pwmEnableChannel(pwmDriver, channelId, PWM_PERCENTAGE_TO_WIDTH(pwmDriver, dutyCycle));

    // store the new duty cycle
    pThis[FIELD___dutyCycle].NumericByRef().u4 = dutyCycle;

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::NativeStart___VOID(CLR_RT_StackFrame &stack)
{
    int32_t timerId;
    int32_t pinNumber;
    uint32_t dutyCycle;
    int32_t channelId;

    // PWMDriver *pwmDriver;

    NANOCLR_HEADER();

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // Retrieves the needed parameters from private class properties or method parameters
    timerId = pThis[FIELD___pwmTimer].NumericByRef().s4;
    pinNumber = pThis[FIELD___pinNumber].NumericByRef().s4;
    dutyCycle = pThis[FIELD___dutyCycle].NumericByRef().u4;
    channelId = pThis[FIELD___channelNumber].NumericByRef().s4;

    // Gets the PWM driver associated with the requested timer
    // pwmDriver = GetDriverPwm(timerId);

    // Sets the pin to the correct PWM alternate function and...
    // palSetPadMode(GPIO_PORT(pinNumber), pinNumber % 16, PAL_MODE_ALTERNATE(GetAlternateFunctionPwm(timerId)));

    // ...enables the associated channel
    // pwmEnableChannel(pwmDriver, channelId, PWM_PERCENTAGE_TO_WIDTH(pwmDriver, dutyCycle));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::NativeStop___VOID(CLR_RT_StackFrame &stack)
{
    int32_t timerId;
    int32_t channelId;

    NANOCLR_HEADER();

    // get a pointer to the managed object instance and check that it's not NULL
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // Retrieves the needed parameters from private class properties or method parameters
    timerId = pThis[FIELD___pwmTimer].NumericByRef().s4;
    channelId = pThis[FIELD___channelNumber].NumericByRef().s4;

    // Stops PWM output on the channel associated with the selected pin
    // pwmDisableChannel(GetDriverPwm(timerId), channelId);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::DisposeNative___VOID(CLR_RT_StackFrame &stack)
{
    (void)stack;

    NANOCLR_HEADER();

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_sys_dev_pwm_native_System_Device_Pwm_PwmChannel::GetChannel___STATIC__I4__I4__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    // Get pin and potential TIM
    int pin = stack.Arg0().NumericByRef().s4;
    int timerId = stack.Arg1().NumericByRef().s4;

    // Check if the combination is ok and set the result
    // stack.SetResult_I4(GetChannelPwm(pin, timerId));
    // ToDo dummy return
    stack.SetResult_I4(0);

    NANOCLR_NOCLEANUP_NOLABEL();
}
