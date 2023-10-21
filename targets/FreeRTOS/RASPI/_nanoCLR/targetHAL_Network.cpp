//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// This file includes the platform and board specific Network Intialisation

#include <nanoHAL.h>
#include "nf_lwipthread.h"

#if DEBUG_RP2040
#include "debug_printf.h"
#endif
#if !defined(DEBUG_HAL_NETWORK)
#define DEBUG_HAL_NETWORK   0
#endif

// this is the declaration for the callback implement in nf_sys_arch.c 
extern "C" void set_signal_sock_function( void (*funcPtr)() );

// buffer with host name
char hostName[18] = "nanodevice_";

//
// Callback from lwIP on event
//
void sys_signal_sock_event()
{
     Events_Set(SYSTEM_EVENT_FLAG_SOCKET);
}

static struct lwipthread_opts m_lwipOptions;
static uint8_t m_MacAddress[6];

void nanoHAL_Network_Initialize()
{
#if DEBUG_RP2040 & DEBUG_HAL_NETWORK
    DEBUG_PRINTF("nanoHAL_Network_Initialize: start\n");
#endif
    // Initialise the lwIP CLR signal callback
    set_signal_sock_function( &sys_signal_sock_event );

    // get network configuration, if available
    if(g_TargetConfiguration.NetworkInterfaceConfigs->Count == 0)
    {
        // there is no networking configuration block, can't proceed
#if DEBUG_RP2040 & DEBUG_HAL_NETWORK
        DEBUG_PRINTF("nanoHAL_Network_Initialize: no configuration return\n");
#endif
        return;
    }

    HAL_Configuration_NetworkInterface networkConfig;
    
    if (ConfigurationManager_GetConfigurationBlock((void *)&networkConfig, DeviceConfigurationOption_Network, 0) ==
        true)
    {
        // build lwIP configuration 
        // struct lwipthread_opts lwipOptions;
        
        // init config
        memset(&m_lwipOptions, 0, sizeof(m_lwipOptions));
        m_lwipOptions.macaddress = (uint8_t *)&m_MacAddress;
        // grab MAC address
        memcpy((void *)m_lwipOptions.macaddress, (const void *)networkConfig.MacAddress, sizeof(m_MacAddress));
#if DEBUG_RP2040 & DEBUG_HAL_NETWORK
        DEBUG_PRINTF("nanoHAL_Network_Initialize: mac addr %02x:%02x:%02x:%02x:%02x:%02x\n",
                m_lwipOptions.macaddress[0],
                m_lwipOptions.macaddress[1],
                m_lwipOptions.macaddress[2],
                m_lwipOptions.macaddress[3],
                m_lwipOptions.macaddress[4],
                m_lwipOptions.macaddress[5]
                );
#endif
        // static or dinamic address
        if(networkConfig.StartupAddressMode == AddressMode_Static)
        {
            // IPv4 configs
            m_lwipOptions.address = networkConfig.IPv4Address;
            m_lwipOptions.netmask = networkConfig.IPv4NetMask;
            m_lwipOptions.gateway = networkConfig.IPv4GatewayAddress;
        }
        else if(networkConfig.StartupAddressMode == AddressMode_DHCP)
        {
            // clear  IPv4 configs
            m_lwipOptions.address = 0;
            m_lwipOptions.netmask = 0;
            m_lwipOptions.gateway = 0;
        }
#if DEBUG_RP2040 & DEBUG_HAL_NETWORK
    DEBUG_PRINTF("nanoHAL_Network_Initialize: ip=%08x mask=%08x gw=%08x\n",
            (uint)m_lwipOptions.address,
            (uint)m_lwipOptions.netmask,
            (uint)m_lwipOptions.gateway);
#endif
        // set address mode option
        // our enum follows lwIP defines for address mode
        m_lwipOptions.addrMode = (net_addr_mode_t)networkConfig.StartupAddressMode;

        // compose host name with nanodevice and last 3 bytes of MAC address
        // nanodevice_XXXXXX
        char *macPosition = hostName + 11;

        // copy over last 3 bytes of MAC address
        for (int index = 3; index < 6; index++)
        {
            sprintf(macPosition, "%02X", (int)networkConfig.MacAddress[index]);
            macPosition += 2;
        }

#if LWIP_NETIF_HOSTNAME || defined(__DOXYGEN__)
        // set host name
        m_lwipOptions.ourHostName = hostName;
#endif

        // Start lwIP thread in ChibiOS bindings using the above options
#if DEBUG_RP2040 & DEBUG_HAL_NETWORK
        DEBUG_PRINTF("nanoHAL_Network_Initialize: lwipInit\n");
#endif
        lwipInit(&m_lwipOptions);
    }
#if DEBUG_RP2040 & DEBUG_HAL_NETWORK
    DEBUG_PRINTF("nanoHAL_Network_Initialize: end\n");
#endif
}
