//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL.h>
#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoCLR_Checks.h>
#include <nf_rt_native.h>
#include <target_platform.h>

#if !defined(RP2040_USE_RTC) 
    #error "Need the RTC to be enabled. Please set CMake option NF_FEATURE_RTC to ON."
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// these functions are the real implementation of the 'weak' ones declared at src\CLR\Runtime.Native\nf_rt_native_nanoFramework_Runtime_Native_Rtc_stubs.cpp //
// the stubs for this class are generated by nanoFramework.Runtime.Native                                                                                    //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 0
HRESULT Library_nf_rt_native_nanoFramework_Runtime_Native_Rtc::Native_RTC_SetSystemTime___STATIC__BOOLEAN__I4__U1__U1__U1__U1__U1__U1( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        // ToDo: implement
        // snvs_lp_srtc_datetime_t srtcDate;

        // srtcDate.year = (uint16_t) stack.Arg0().NumericByRef().s4;
        // srtcDate.month = (uint8_t) stack.Arg1().NumericByRef().u1;
        // srtcDate.day = (uint8_t) stack.Arg2().NumericByRef().u1;
        // srtcDate.hour = (uint8_t) stack.Arg4().NumericByRef().u1;
        // srtcDate.minute = (uint8_t )stack.Arg5().NumericByRef().u1;
        // srtcDate.second = (uint8_t) stack.Arg6().NumericByRef().u1;

        // Set new date and start RTC
        // ToDo: implement
        // SNVS_LP_SRTC_SetDatetime(SNVS, &srtcDate);

        // Return value to the managed application
        stack.SetResult_Boolean(true);
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}
#endif