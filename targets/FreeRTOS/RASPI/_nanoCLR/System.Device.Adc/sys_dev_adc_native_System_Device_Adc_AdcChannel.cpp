//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <sys_dev_adc_native_target.h>
#include "board.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"

const uint8_t m_ch_to_pin[] = {
   RP2040_ADC_0,
   RP2040_ADC_1,
   RP2040_ADC_2,
};
#define RP2040_ADC_COUNT    (sizeof(m_ch_to_pin)/sizeof(uint8_t))

uint32_t rp2040_adc_count(void)
{
    return RP2040_ADC_COUNT;
}

HRESULT Library_sys_dev_adc_native_System_Device_Adc_AdcChannel::NativeReadValue___I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        // get a pointer to the managed object instance and check that it's not NULL
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);
        int channelNumber = pThis[FIELD___channelNumber].NumericByRef().s4;
        if (channelNumber >= (int)rp2040_adc_count()) {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
        adc_gpio_init((uint)m_ch_to_pin[channelNumber]);
        // const float conversion_factor = 3.3f / (1 << 12);
        uint16_t raw = adc_read();
        // float value = (float)raw * conversion_factor;
        stack.SetResult_I4((int)raw);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_adc_native_System_Device_Adc_AdcChannel::NativeDisposeChannel___VOID(CLR_RT_StackFrame &stack)
{
    (void)stack;

    NANOCLR_HEADER();

    NANOCLR_NOCLEANUP_NOLABEL();
}
