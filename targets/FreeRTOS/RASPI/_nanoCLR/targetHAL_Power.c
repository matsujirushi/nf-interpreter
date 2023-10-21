//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL_v2.h>
#include "pico/stdlib.h"
#include "hardware/watchdog.h"

#define AIRCR_Register (*((volatile uint32_t*)(PPB_BASE + 0x0ED0C)))

inline void CPU_Reset()
{
    // ToDo: implement
    // NVIC_SystemReset();
    // watchdog_reboot (0, SRAM_END, 10);
    // AIRCR_Register = 0x5FA0004;
    AIRCR_Register = 0x5FA0004;
};

// CPU sleep is not currently implemented in this target
inline void CPU_Sleep(SLEEP_LEVEL_type level, uint64_t wakeEvents)
{
    (void)level;
    (void)wakeEvents;
};

inline bool CPU_IsSoftRebootSupported()
{
    return true;
};

void CPU_SetPowerMode(PowerLevel_type powerLevel)
{
    //TODO power modes support
    (void)powerLevel;
}
