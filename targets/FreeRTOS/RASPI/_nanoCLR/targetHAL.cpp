//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoPAL.h>
#include <nanoHAL_Time.h>
#include <nanoHAL_Types.h>
#include <nanoPAL_Events.h>
#include <nanoPAL_BlockStorage.h>
#include <nanoHAL_ConfigurationManager.h>
#include "board.h"

#if DEBUG_RP2040
#include "debug_printf.h"
#endif
#if !defined(DEBUG_TARGET_HAL)
#define DEBUG_TARGET_HAL    0
#endif

//
//  Reboot handlers clean up on reboot
//
static ON_SOFT_REBOOT_HANDLER s_rebootHandlers[16] =
    {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

void HAL_AddSoftRebootHandler(ON_SOFT_REBOOT_HANDLER handler)
{
    for (unsigned int i = 0; i < ARRAYSIZE(s_rebootHandlers); i++)
    {
        if (s_rebootHandlers[i] == NULL)
        {
            s_rebootHandlers[i] = handler;
            return;
        }
        else if (s_rebootHandlers[i] == handler)
        {
            return;
        }
    }
}

// because nanoHAL_Initialize/Uninitialize needs to be called in both C and C++ we need a proxy to allow it to be called
// in 'C'
extern "C"
{

    void nanoHAL_Initialize_C()
    {
        nanoHAL_Initialize();
    }

    void nanoHAL_Uninitialize_C(bool isPoweringDown)
    {
        nanoHAL_Uninitialize(isPoweringDown);
    }
}

void nanoHAL_Initialize()
{
#if DEBUG_RP2040 & DEBUG_TARGET_HAL
    DEBUG_PRINTF("nanoHAL_Initialize\n");
#endif
#if DEBUG_RP2040 & DEBUG_TARGET_HAL
    DEBUG_PRINTF("HAL_CONTINUATION::InitializeList\n");
#endif
    HAL_CONTINUATION::InitializeList();
#if DEBUG_RP2040 & DEBUG_TARGET_HAL
    DEBUG_PRINTF("HAL_COMPLETION ::InitializeList\n");
#endif
    HAL_COMPLETION ::InitializeList();

#if DEBUG_RP2040 & DEBUG_TARGET_HAL
    DEBUG_PRINTF("nanoHAL_Initialize: BlockStorageList_Initialize\n");
#endif
    BlockStorageList_Initialize();

#if DEBUG_RP2040 & DEBUG_TARGET_HAL
    DEBUG_PRINTF("nanoHAL_Initialize: BlockStorage_AddDevices\n");
#endif
    // initialize block storage devices
    BlockStorage_AddDevices();

#if DEBUG_RP2040 & DEBUG_TARGET_HAL
    DEBUG_PRINTF("nanoHAL_Initialize: BlockStorageList_InitializeDevices\n");
#endif
    BlockStorageList_InitializeDevices();

    // clear managed heap region
    unsigned char *heapStart = NULL;
    unsigned int heapSize = 0;

    ::HeapLocation(heapStart, heapSize);
    memset(heapStart, 0, heapSize);

#if DEBUG_RP2040 & DEBUG_TARGET_HAL
    DEBUG_PRINTF("nanoHAL_Initialize: ConfigurationManager_Initialize\n");
#endif
    ConfigurationManager_Initialize();

#if DEBUG_RP2040 & DEBUG_TARGET_HAL
    DEBUG_PRINTF("nanoHAL_Initialize: Events_Initialize\n");
#endif
    Events_Initialize();

#if DEBUG_RP2040 & DEBUG_TARGET_HAL
    DEBUG_PRINTF("nanoHAL_Initialize: CPU_GPIO_Initialize\n");
#endif
    CPU_GPIO_Initialize();

#if (HAL_USE_UART == TRUE)

#if defined(NF_SERIAL_COMM_RP2040_UART_USE_UART1) && (NF_SERIAL_COMM_RP2040_UART_USE_UART1 == TRUE)
    Uart1_PAL.UartDriver = NULL;
#endif
#if defined(NF_SERIAL_COMM_RP2040_UART_USE_UART2) && (NF_SERIAL_COMM_RP2040_UART_USE_UART2 == TRUE)
    Uart2_PAL.UartDriver = NULL;
#endif
#if defined(NF_SERIAL_COMM_RP2040_UART_USE_UART3) && (NF_SERIAL_COMM_RP2040_UART_USE_UART3 == TRUE)
    Uart3_PAL.UartDriver = NULL;
#endif
#if defined(NF_SERIAL_COMM_RP2040_UART_USE_UART4) && (NF_SERIAL_COMM_RP2040_UART_USE_UART4 == TRUE)
    Uart4_PAL.UartDriver = NULL;
#endif
#if defined(NF_SERIAL_COMM_RP2040_UART_USE_UART5) && (NF_SERIAL_COMM_RP2040_UART_USE_UART5 == TRUE)
    Uart5_PAL.UartDriver = NULL;
#endif

#endif
    // no PAL events required until now
    // PalEvent_Initialize();

#if DEBUG_RP2040 & DEBUG_TARGET_HAL
    DEBUG_PRINTF("nanoHAL_Initialize: Network_Initialize\n");
#endif
    // Init Networking
    Network_Initialize();

    // Start Network Debugger
    // SOCKETS_DbgInitialize( 0 );
}

void nanoHAL_Uninitialize(bool isPoweringDown)
{
#if DEBUG_RP2040 & DEBUG_TARGET_HAL
    DEBUG_PRINTF("nanoHAL_Uninitialize\n");
#endif
    (void)isPoweringDown;
    
    // check for s_rebootHandlers
    for (unsigned int i = 0; i < ARRAYSIZE(s_rebootHandlers); i++)
    {
        if (s_rebootHandlers[i] != NULL)
        {
            s_rebootHandlers[i]();
        }
        else
        {
            break;
        }
    }

#if DEBUG_RP2040 & DEBUG_TARGET_HAL
    DEBUG_PRINTF("nanoHAL_Uninitialize: SOCKET_CloseConnections\n");
#endif
    SOCKETS_CloseConnections();

#if !defined(HAL_REDUCESIZE)
    // TODO need to call this but it's preventing the debug session from starting
    // Network_Uninitialize();
#endif

#if DEBUG_RP2040 & DEBUG_TARGET_HAL
    DEBUG_PRINTF("nanoHAL_Uninitialize: BlockStorageList_UnInitializeDevices\n");
#endif
    BlockStorageList_UnInitializeDevices();

#if DEBUG_RP2040 & DEBUG_TARGET_HAL
    DEBUG_PRINTF("nanoHAL_Uninitialize: CPU_GPIO_Uninitialize\n");
#endif
    CPU_GPIO_Uninitialize();

    // PalEvent_Uninitialize();

#if DEBUG_RP2040 & DEBUG_TARGET_HAL
    DEBUG_PRINTF("nanoHAL_Uninitialize: Events_Uninitialize\n");
#endif
    Events_Uninitialize();

#if DEBUG_RP2040 & DEBUG_TARGET_HAL
    DEBUG_PRINTF("nanoHAL_Uninitialize: HAL_CONTINUATION::Uninitialize\n");
#endif
    HAL_CONTINUATION::Uninitialize();
#if DEBUG_RP2040 & DEBUG_TARGET_HAL
    DEBUG_PRINTF("nanoHAL_Uninitialize: HAL_COMPLETION ::Uninitialize\n");
#endif
    HAL_COMPLETION ::Uninitialize();
}
