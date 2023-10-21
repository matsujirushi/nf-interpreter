//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "win_dev_pwm_native_target.h"
#include "board.h"

HRESULT Library_win_dev_pwm_native_Windows_Devices_Pwm_PwmController::get_MaxFrequency___R8( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    stack.SetResult_R8(RP2040_PWM_MAX_FREQ);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_pwm_native_Windows_Devices_Pwm_PwmController::get_MinFrequency___R8( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    stack.SetResult_R8((double)RP2040_PWM_MIN_FREQ);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_win_dev_pwm_native_Windows_Devices_Pwm_PwmController::get_PinCount___I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    stack.SetResult_I4((CLR_INT32)RP2040_PWM_PIN_COUNT);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_win_dev_pwm_native_Windows_Devices_Pwm_PwmController::NativeSetDesiredFrequency___U4__U4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock* pThis = stack.This();
        FAULT_ON_NULL(pThis);
        unsigned int desiredFrequency = (unsigned int)stack.Arg1().NumericByRef().u4;
        unsigned int div = (RP2040_PWM_MAX_FREQ / desiredFrequency);
        stack.SetResult_R8(desiredFrequency);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_pwm_native_Windows_Devices_Pwm_PwmController::GetDeviceSelector___STATIC__STRING( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        char deviceSelectorString[] = "PTM0";
        stack.SetResult_String(deviceSelectorString);
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}
