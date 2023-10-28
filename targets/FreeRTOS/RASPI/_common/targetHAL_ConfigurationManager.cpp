//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL.h>
#include <nanoHAL_v2.h>
#include <nanoWeak.h>
#include <nanoHAL_ConfigurationManager.h>
#include "Target_BlockStorage_RP2040_FlashDriver.h"
#include "pico/unique_id.h"

#if DEBUG_RP2040
#include "debug_printf.h"
#endif
#if !defined(DEBUG_HAL_CONFIG_SIZE)
#define DEBUG_HAL_CONFIG_SIZE   0
#endif
#if !defined(DEBUG_HAL_CONFIG_INIT)
#define DEBUG_HAL_CONFIG_INIT   0
#endif
#if !defined(DEBUG_HAL_CONFIG_ENUM)
#define DEBUG_HAL_CONFIG_ENUM   0
#endif
#if !defined(DEBUG_HAL_CONFIG_GET)
#define DEBUG_HAL_CONFIG_GET    0
#endif
#if !defined(DEBUG_HAL_CONFIG_STORE)
#define DEBUG_HAL_CONFIG_STORE  0
#endif
#if !defined(DEBUG_HAL_CONFIG_UPDATE)
#define DEBUG_HAL_CONFIG_UPDATE 0
#endif
#if !defined(DEBUG_HAL_CONFIG_DEF)
#define DEBUG_HAL_CONFIG_DEF    0
#endif
#if !defined(DEBUG_HAL_CONFIG_DETAIL)
#define DEBUG_HAL_CONFIG_DETAIL 0
#endif
#if !defined(DEBUG_ALL_NETWORK_CONFIGS)
#define DEBUG_ALL_NETWORK_CONFIGS    0
#endif
#if !defined(DEBUG_ALL_CERTIFICATES)
#define DEBUG_ALL_CERTIFICATES  0
#endif
#if !defined(DEBUG_ALL_DEVICE_CERTIFICATES)
#define DEBUG_ALL_DEVICE_CERTIFICATES  0
#endif

#if DEBUG_RP2040
static const uint8_t *get_deviceConfigurationName(DeviceConfigurationOption option)
{
    uint8_t *name = (uint8_t *)"";
    switch ((int)option) {
    case (int)DeviceConfigurationOption_Network:
        name = (uint8_t *)"Network";
        break;
    case (int)DeviceConfigurationOption_Wireless80211Network:
        name = (uint8_t *)"Wireless80211Network";
        break;
    case (int)DeviceConfigurationOption_WirelessNetworkAP:
        name = (uint8_t *)"WirelessNetworkAp";
        break;
    case (int) DeviceConfigurationOption_X509CaRootBundle:
        name = (uint8_t *)"X509CaRootBundle";
        break;
    case (int) DeviceConfigurationOption_X509DeviceCertificates:
        name = (uint8_t *)"X509DeviceCertificates";
        break;
    case (int) DeviceConfigurationOption_All:
        name = (uint8_t *)"All";
        break;
    }
    return (const uint8_t *)name;
}

static const uint8_t *get_networkInterfaceName(NetworkInterfaceType type)
{
    uint8_t *name = (uint8_t *)"";
    switch ((int)type) {
    case (int)NetworkInterfaceType_Unknown:
        name = (uint8_t *)"Unknown";
        break;
    case (int)NetworkInterfaceType_Ethernet:
        name = (uint8_t *)"Ethernet";
        break;
    case (int)NetworkInterfaceType_Wireless80211:
        name = (uint8_t *)"Wireless80211";
        break;
    case (int) NetworkInterfaceType_WirelessAP:
        name = (uint8_t *)"WirelessAP";
        break;
    }
    return (const uint8_t *)name;
}
#endif

#if DEBUG_RP2040 & DEBUG_ALL_NETWORK_CONFIGS
static void print_networkConfig(HAL_CONFIGURATION_NETWORK *cfg, int index)
{
    if (cfg != NULL) {
        DEBUG_PRINTF("cfg[%d] type=%s\n", index, get_networkInterfaceName(cfg->Configs[index]->InterfaceType));
    }
}

static void print_allNetworkConfigs(HAL_CONFIGURATION_NETWORK *cfg)
{
   if (cfg != NULL)
   {
       DEBUG_PRINTF("networkInterface Count=%d\n", cfg->Count);
       for (int i = 0; i < cfg->Count; i++) {
           print_networkConfig(cfg, i);
       }
   }
}
#endif

#if DEBUG_RP2040 & DEBUG_ALL_CERTIFICATES
static void print_certificate(HAL_CONFIGURATION_X509_CERTIFICATE *cfg, int index)
{
    if (cfg != NULL) {
        DEBUG_PRINTF("cfg[%d] size=%d\n", index, (int)cfg->Certificates[index]->CertificateSize);
    }
}

static void print_allCertificates(HAL_CONFIGURATION_X509_CERTIFICATE *cfg)
{
   if (cfg != NULL)
   {
       DEBUG_PRINTF("Certificate Count=%d\n", cfg->Count);
       for (int i = 0; i < cfg->Count; i++) {
           print_certificate(cfg, i);
       }
   }
}
#endif

#if DEBUG_RP2040 & DEBUG_ALL_DEVICE_CERTIFICATES
static void print_deviceCertificate(HAL_CONFIGURATION_X509_DEVICE_CERTIFICATE *cfg, int index)
{
    if (cfg != NULL) {
        DEBUG_PRINTF("cfg[%d] size=%d\n", index, (int)cfg->Certificates[index]->CertificateSize);
    }
}

static void print_allDeviceCertificates(HAL_CONFIGURATION_X509_DEVICE_CERTIFICATE *cfg)
{
   if (cfg != NULL)
   {
       DEBUG_PRINTF("Device Certificate Count=%d\n", cfg->Count);
       for (int i = 0; i < cfg->Count; i++) {
           print_deviceCertificate(cfg, i);
       }
   }
}
#endif

uint32_t GetExistingConfigSize()
{
    uint32_t currentConfigSize = 0;

    currentConfigSize =
        g_TargetConfiguration.NetworkInterfaceConfigs->Count * sizeof(HAL_Configuration_NetworkInterface);
    currentConfigSize += g_TargetConfiguration.Wireless80211Configs->Count * sizeof(HAL_Configuration_Wireless80211);
#if (DEBUG_RP2040 & DEBUG_HAL_CONFIG_SIZE)
    DEBUG_PRINTF("Config size=%d\n", (int)currentConfigSize);
#endif
    return currentConfigSize;
}

// initialization of configuration manager
// provided as weak so it can be replaced at target level, if required because of the target implementing the storage
// with a mechanism other then saving to flash
__nfweak void ConfigurationManager_Initialize()
{
#if (DEBUG_RP2040 & DEBUG_HAL_CONFIG_INIT)
    DEBUG_PRINTF("ConfigurationManager_Initialize: start\n");
#endif
    // init g_TargetConfiguration
    memset(&g_TargetConfiguration, 0, sizeof(HAL_TARGET_CONFIGURATION));

    // enumerate the blocks
    ConfigurationManager_EnumerateConfigurationBlocks();
#if (DEBUG_RP2040 & DEBUG_HAL_CONFIG_INIT)
    DEBUG_PRINTF("ConfigurationManager_Initialize: end\n");
#endif
};

// Enumerates the configuration blocks from the configuration flash sector
// it's implemented with 'weak' attribute so it can be replaced at target level if a different persistance mechanism is
// used
__nfweak void ConfigurationManager_EnumerateConfigurationBlocks()
{
#if (DEBUG_RP2040 & DEBUG_HAL_CONFIG_ENUM)
    DEBUG_PRINTF("ConfigurationManager_EnumerateConfigurationBlocks: start\n");
#endif
    // start checking if this device has config block
    if (((uint32_t)&__nanoConfig_end__ - (uint32_t)&__nanoConfig_start__) > 0)
    {
        // find network configuration blocks
        HAL_CONFIGURATION_NETWORK *networkConfigs =
            (HAL_CONFIGURATION_NETWORK *)ConfigurationManager_FindNetworkConfigurationBlocks(
                (uint32_t)&__nanoConfig_start__,
                (uint32_t)&__nanoConfig_end__);

        // check network configs count
        if (networkConfigs->Count == 0)
        {
            // there is no network config block available, get a default
            HAL_Configuration_NetworkInterface *networkInterface =
                (HAL_Configuration_NetworkInterface *)platform_malloc(sizeof(HAL_Configuration_NetworkInterface));

            if (InitialiseNetworkDefaultConfig(networkInterface, 0))
            {
                // config block created, store it
                ConfigurationManager_StoreConfigurationBlock(
                    networkInterface,
                    DeviceConfigurationOption_Network,
                    0,
                    sizeof(HAL_Configuration_NetworkInterface),
                    0,
                    false);

                // have to enumerate again to pick it up
                networkConfigs = (HAL_CONFIGURATION_NETWORK *)ConfigurationManager_FindNetworkConfigurationBlocks(
                    (uint32_t)&__nanoConfig_start__,
                    (uint32_t)&__nanoConfig_end__);
            }
            platform_free(networkInterface);
        }

        // find wireless 80211 network configuration blocks
        HAL_CONFIGURATION_NETWORK_WIRELESS80211 *networkWirelessConfigs =
            (HAL_CONFIGURATION_NETWORK_WIRELESS80211 *)ConfigurationManager_FindNetworkWireless80211ConfigurationBlocks(
                (uint32_t)&__nanoConfig_start__,
                (uint32_t)&__nanoConfig_end__);

        // find X509 CA certificate blocks
        HAL_CONFIGURATION_X509_CERTIFICATE *certificateStore =
            (HAL_CONFIGURATION_X509_CERTIFICATE *)ConfigurationManager_FindX509CertificateConfigurationBlocks(
                (uint32_t)&__nanoConfig_start__,
                (uint32_t)&__nanoConfig_end__);

        // find X509 device certificate blocks
        HAL_CONFIGURATION_X509_DEVICE_CERTIFICATE *deviceCertificates = (HAL_CONFIGURATION_X509_DEVICE_CERTIFICATE *)
            ConfigurationManager_FindX509DeviceCertificatesConfigurationBlocks(
                (uint32_t)&__nanoConfig_start__,
                (uint32_t)&__nanoConfig_end__);

        // alloc memory for g_TargetConfiguration
        // because this is a struct of structs that use flexible members the memory has to be allocated from the heap
        // the malloc size for each struct is computed separately
        uint32_t sizeOfNetworkInterfaceConfigs =
            offsetof(HAL_CONFIGURATION_NETWORK, Configs) + networkConfigs->Count * sizeof(networkConfigs->Configs[0]);
        uint32_t sizeOfWireless80211Configs =
            offsetof(HAL_CONFIGURATION_NETWORK_WIRELESS80211, Configs) +
            networkWirelessConfigs->Count * sizeof(networkWirelessConfigs->Configs[0]);
        uint32_t sizeOfX509CertificateStore = offsetof(HAL_CONFIGURATION_X509_CERTIFICATE, Certificates) +
                                              certificateStore->Count * sizeof(certificateStore->Certificates[0]);
        uint32_t sizeOfX509DeviceCertificate = offsetof(HAL_CONFIGURATION_X509_DEVICE_CERTIFICATE, Certificates) +
                                                deviceCertificates->Count * sizeof(deviceCertificates->Certificates[0]);

        g_TargetConfiguration.NetworkInterfaceConfigs =
            (HAL_CONFIGURATION_NETWORK *)platform_malloc(sizeOfNetworkInterfaceConfigs);
        g_TargetConfiguration.Wireless80211Configs =
            (HAL_CONFIGURATION_NETWORK_WIRELESS80211 *)platform_malloc(sizeOfWireless80211Configs);
        g_TargetConfiguration.CertificateStore =
            (HAL_CONFIGURATION_X509_CERTIFICATE *)platform_malloc(sizeOfX509CertificateStore);
        g_TargetConfiguration.DeviceCertificates =
            (HAL_CONFIGURATION_X509_DEVICE_CERTIFICATE *)platform_malloc(sizeOfX509DeviceCertificate);

        // copy structs to g_TargetConfiguration
        memcpy(
            (HAL_CONFIGURATION_NETWORK *)g_TargetConfiguration.NetworkInterfaceConfigs,
            networkConfigs,
            sizeOfNetworkInterfaceConfigs);
        memcpy(
            (HAL_CONFIGURATION_NETWORK_WIRELESS80211 *)g_TargetConfiguration.Wireless80211Configs,
            networkWirelessConfigs,
            sizeOfWireless80211Configs);
        memcpy(
            (HAL_CONFIGURATION_X509_CERTIFICATE *)g_TargetConfiguration.CertificateStore,
            certificateStore,
            sizeOfX509CertificateStore);
        memcpy(
            (HAL_CONFIGURATION_X509_DEVICE_CERTIFICATE *)g_TargetConfiguration.DeviceCertificates,
            deviceCertificates,
            sizeOfX509DeviceCertificate);

        // now free the memory of the original structs
        platform_free(networkConfigs);
        platform_free(networkWirelessConfigs);
        platform_free(certificateStore);
        platform_free(deviceCertificates);
    }
    else
    {
        // no config block
    }
#if (DEBUG_RP2040 & DEBUG_ALL_NETWORK_CONFIGS)
    print_allNetworkConfigs((HAL_CONFIGURATION_NETWORK *)g_TargetConfiguration.NetworkInterfaceConfigs);
#endif
#if DEBUG_RP2040 & DEBUG_ALL_CERTIFICATES
    print_allCertificates((HAL_CONFIGURATION_X509_CERTIFICATE *)g_TargetConfiguration.CertificateStore);
#endif
#if DEBUG_RP2040 & DEBUG_ALL_DEVICE_CERTIFICATES
    print_allDeviceCertificates((HAL_CONFIGURATION_X509_DEVICE_CERTIFICATE *)g_TargetConfiguration.DeviceCertificates);
#endif
#if (DEBUG_RP2040 & DEBUG_HAL_CONFIG_ENUM)
    DEBUG_PRINTF("ConfigurationManager_EnumerateConfigurationBlocks: end\n");
#endif
}

// Gets the network configuration block from the configuration flash sector
// it's implemented with 'weak' attribute so it can be replaced at target level if a different persistance mechanism is
// used
__nfweak bool ConfigurationManager_GetConfigurationBlock(
    void *configurationBlock,
    DeviceConfigurationOption configuration,
    uint32_t configurationIndex)
{
#if (DEBUG_RP2040 & DEBUG_HAL_CONFIG_GET)
    DEBUG_PRINTF("ConfigurationManager_GetConfigurationBlock: start\n");
#endif
    int sizeOfBlock = 0;
    uint8_t *blockAddress = NULL;
    bool flag = true;

    // validate if the requested block exists
    // Count has to be non zero
    // requested Index has to exist (array index starts at zero, so need to add one)
    if (configuration == DeviceConfigurationOption_Network)
    {
        if (g_TargetConfiguration.NetworkInterfaceConfigs->Count == 0 ||
            (configurationIndex + 1) > g_TargetConfiguration.NetworkInterfaceConfigs->Count)
            {
            // the requested config block is beyond the available count
            flag = false;
            goto GetConfigurationBlock_exit;
        }

        // set block size
        sizeOfBlock = sizeof(HAL_Configuration_NetworkInterface);

        // get block address
        blockAddress = (uint8_t *)g_TargetConfiguration.NetworkInterfaceConfigs->Configs[configurationIndex];
    }
    else if (configuration == DeviceConfigurationOption_Wireless80211Network)
    {
        if (g_TargetConfiguration.Wireless80211Configs->Count == 0 ||
            (configurationIndex + 1) > g_TargetConfiguration.Wireless80211Configs->Count)
        {
            flag = false;
            goto GetConfigurationBlock_exit;
        }

        // set block size
        sizeOfBlock = sizeof(HAL_Configuration_Wireless80211);

        // get block address
        blockAddress = (uint8_t *)g_TargetConfiguration.Wireless80211Configs->Configs[configurationIndex];
    }
    else if (configuration == DeviceConfigurationOption_X509CaRootBundle)
    {
        if (g_TargetConfiguration.CertificateStore->Count == 0 ||
            (configurationIndex + 1) > g_TargetConfiguration.CertificateStore->Count)
        {
            flag = false;
            goto GetConfigurationBlock_exit;
        }

        // get block address
        blockAddress = (uint8_t *)g_TargetConfiguration.CertificateStore->Certificates[configurationIndex];

        // set block size
        // because X509 certificate has a variable length need to compute the block size in two steps
        sizeOfBlock = offsetof(HAL_Configuration_X509CaRootBundle, Certificate);
        sizeOfBlock += ((HAL_Configuration_X509CaRootBundle *)blockAddress)->CertificateSize;
    }
    else if (configuration == DeviceConfigurationOption_X509CaRootBundle)
    {
        if (g_TargetConfiguration.DeviceCertificates->Count == 0 ||
            (configurationIndex + 1) > g_TargetConfiguration.DeviceCertificates->Count)
        {
            flag = false;
            goto GetConfigurationBlock_exit;
        }

        // get block address
        blockAddress = (uint8_t *)g_TargetConfiguration.DeviceCertificates->Certificates[configurationIndex];

        // set block size
        // because X509 certificate has a variable length need to compute the block size in two steps
        sizeOfBlock = offsetof(HAL_Configuration_X509DeviceCertificate, Certificate);
        sizeOfBlock += ((HAL_Configuration_X509DeviceCertificate *)blockAddress)->CertificateSize;
    }
    // copy the config block content to the pointer in the argument
    memcpy(configurationBlock, blockAddress, sizeOfBlock);

GetConfigurationBlock_exit:
#if (DEBUG_RP2040 & DEBUG_HAL_CONFIG_DETAIL)
    DEBUG_PRINTF("CM_GetConf: %s %s Config[%d]\n", flag? "OK": "NG", get_deviceConfigurationName(configuration), (int)configurationIndex);
#endif
#if (DEBUG_RP2040 & DEBUG_HAL_CONFIG_GET)
    DEBUG_PRINTF("ConfigurationManager_GetConfigurationBlock: end\n");
#endif
    return flag;
}

// Stores the configuration block to the configuration flash sector
// NOTE: because inserting or removing a configuration block it's very 'RAM expensive' we choose not to support those
// operations the host debugger will have to be used to manage these operations on the device configuration collection
// it's implemented with 'weak' attribute so it can be replaced at target level if a different persistance mechanism is
// used
__nfweak bool ConfigurationManager_StoreConfigurationBlock(
    void *configurationBlock,
    DeviceConfigurationOption configuration,
    uint32_t configurationIndex,
    uint32_t blockSize,
    uint32_t offset,
    bool done)
{
#if (DEBUG_RP2040 & DEBUG_HAL_CONFIG_STORE)
    DEBUG_PRINTF("ConfigurationManager_StoreConfigurationBlock: start\n");
#endif
    ByteAddress storageAddress = 0;
    bool requiresEnumeration = FALSE;
    bool flag = false;

    if (configuration == DeviceConfigurationOption_Network)
    {
        if (g_TargetConfiguration.NetworkInterfaceConfigs == NULL ||
            (g_TargetConfiguration.NetworkInterfaceConfigs->Count == 0 && configurationIndex == 0))
        {
            // there is no network config block, we are storing the default one
            // THIS IS THE ONLY CONFIG BLOCK THAT'S AUTO-CREATED
            // OK to continue
            // set storage address as the start of the flash configuration sector
            storageAddress = (ByteAddress)&__nanoConfig_start__;
        }
        else
        {
            // the requested config block is beyond the available count
            if ((configurationIndex + 1) > g_TargetConfiguration.NetworkInterfaceConfigs->Count)
        {
            // there is no room for this block, or there are no blocks stored at all
            // failing the operation
            flag = false;
            goto StoreConfigurationBlock_exit;
        }

        // set storage address from block address, plus the requested offset
        storageAddress =
            (ByteAddress)g_TargetConfiguration.NetworkInterfaceConfigs->Configs[configurationIndex] + offset;
        }

        // set block size, in case it's not already set
        blockSize = sizeof(HAL_Configuration_NetworkInterface);

        // make sure the config block marker is set
        memcpy(configurationBlock, c_MARKER_CONFIGURATION_NETWORK_V1, sizeof(c_MARKER_CONFIGURATION_NETWORK_V1));

        _ASSERTE(((HAL_Configuration_NetworkInterface *)configurationBlock)->StartupAddressMode > 0);
    }
    else if (configuration == DeviceConfigurationOption_Wireless80211Network)
    {
        if (g_TargetConfiguration.Wireless80211Configs == NULL ||
            (g_TargetConfiguration.Wireless80211Configs->Count == 0 ||
             (configurationIndex + 1) > g_TargetConfiguration.Wireless80211Configs->Count))
        {
            // there is no room for this block, or there are no blocks stored at all
            // failing the operation
            flag = false;
            goto StoreConfigurationBlock_exit;
        }

        // set storage address from block address, plus the requested offset
        storageAddress = (ByteAddress)g_TargetConfiguration.Wireless80211Configs->Configs[configurationIndex] + offset;

        // set block size, in case it's not already set
        blockSize = sizeof(HAL_Configuration_Wireless80211);

        // make sure the config block marker is set
        memcpy(
            configurationBlock,
            c_MARKER_CONFIGURATION_WIRELESS80211_V1,
            sizeof(c_MARKER_CONFIGURATION_WIRELESS80211_V1));
    }
    else if (configuration == DeviceConfigurationOption_X509CaRootBundle)
    {
        // compute block size
        // because X509 certificate has a variable length need to compute the block size in two steps
        blockSize = offsetof(HAL_Configuration_X509CaRootBundle, Certificate);
        blockSize += ((HAL_Configuration_X509CaRootBundle *)configurationBlock)->CertificateSize;

        //
        if (g_TargetConfiguration.CertificateStore->Count == 0)
        {
            // there is nothing at the certificate store
            // find size of existing config blocks
            storageAddress = (uint32_t)&__nanoConfig_start__ + GetExistingConfigSize();
        }
        else
        {
            // set storage address from block address, plus the requested offset
            storageAddress =
                (ByteAddress)g_TargetConfiguration.CertificateStore->Certificates[configurationIndex] + offset;
        }

        if (g_TargetConfiguration.CertificateStore == NULL ||
            (g_TargetConfiguration.CertificateStore->Count == 0 ||
             (configurationIndex + 1) > g_TargetConfiguration.CertificateStore->Count))
        {
            // there is no block stored
            // check if there is room for it
            if (((uint32_t)&__nanoConfig_end__ - storageAddress) < blockSize)
            {
                // not enough room
                flag = false;
                goto StoreConfigurationBlock_exit;
            }

            // now check if memory is erase, so the block can be stored
            if (!RP2040_flash_IsBlockErased(NULL, storageAddress, blockSize))
            {
                // memory not erased, can't store
                flag = false;
                goto StoreConfigurationBlock_exit;
            }
        }

        // make sure the config block marker is set, ONLY required on the 1st chunk
        if (offset == 0)
        {
            memcpy(
                configurationBlock,
                c_MARKER_CONFIGURATION_X509CAROOTBUNDLE_V1,
                sizeof(c_MARKER_CONFIGURATION_X509CAROOTBUNDLE_V1));
        }
    }
    else if (configuration == DeviceConfigurationOption_X509DeviceCertificates)
    {
        // compute block size
        // because X509 certificate has a variable length need to compute the block size in two steps
        blockSize = offsetof(HAL_Configuration_X509DeviceCertificate, Certificate);
        blockSize += ((HAL_Configuration_X509DeviceCertificate *)configurationBlock)->CertificateSize;

        //
        if (g_TargetConfiguration.DeviceCertificates->Count == 0)
        {
            // there is nothing at the certificate store
            // find size of existing config blocks
            storageAddress = (uint32_t)&__nanoConfig_start__ + GetExistingConfigSize();
        }
        else
        {
        // set storage address from block address, plus the requested offset
            storageAddress =
                (ByteAddress)g_TargetConfiguration.DeviceCertificates->Certificates[configurationIndex] + offset;
        }

        if (g_TargetConfiguration.DeviceCertificates == NULL ||
            (g_TargetConfiguration.DeviceCertificates->Count == 0 ||
             (configurationIndex + 1) > g_TargetConfiguration.DeviceCertificates->Count))
        {
            // there is no block stored
            // check if there is room for it
            if (((uint32_t)&__nanoConfig_end__ - storageAddress) < blockSize)
            {
                // not enough room
                flag = false;
                goto StoreConfigurationBlock_exit;
            }

            // now check if memory is erase, so the block can be stored
            if (!RP2040_flash_IsBlockErased(NULL, storageAddress, blockSize))
            {
                // memory not erased, can't store
                flag = false;
                goto StoreConfigurationBlock_exit;
            }
        }

        // make sure the config block marker is set, ONLY required on the 1st chunk
        if (offset == 0)
        {
        memcpy(
            configurationBlock,
                c_MARKER_CONFIGURATION_X509DEVICECERTIFICATE_V1,
                sizeof(c_MARKER_CONFIGURATION_X509DEVICECERTIFICATE_V1));
        }
    }
    else if (configuration == DeviceConfigurationOption_All)
    {
        // particular situation where we are receiving the full configuration block

        // set storage address as the start of the flash configuration sector, plus the requested offset
        storageAddress = (ByteAddress)&__nanoConfig_start__ + offset;

        // for save all the block size has to be provided, check that
        if (blockSize == 0)
        {
            flag = false;
            goto StoreConfigurationBlock_exit;
        }
    }

    // copy the config block content to the config block storage
    flag = RP2040_flash_Write(NULL, storageAddress, blockSize, (unsigned char *)configurationBlock, true);

    // enumeration is required after we are DONE with SUCCESSFULLY storing all the config chunks
    requiresEnumeration = (flag && done);

    if (requiresEnumeration)
    {
        // free the current allocation(s)
        platform_free(g_TargetConfiguration.NetworkInterfaceConfigs);
        platform_free(g_TargetConfiguration.Wireless80211Configs);
        platform_free(g_TargetConfiguration.CertificateStore);
        platform_free(g_TargetConfiguration.DeviceCertificates);

        // perform enumeration of configuration blocks
        ConfigurationManager_EnumerateConfigurationBlocks();
    }
StoreConfigurationBlock_exit:
#if (DEBUG_RP2040 & DEBUG_HAL_CONFIG_STORE)
    DEBUG_PRINTF("ConfigurationManager_StoreConfigurationBlock: end - %s\n", flag? "OK" : "NG");
#endif
    return flag;
}

// Updates a configuration block in the configuration flash sector
// The flash sector has to be erased before writing the updated block
// it's implemented with 'weak' attribute so it can be replaced at target level if a different persistance mechanism is
// used
__nfweak bool ConfigurationManager_UpdateConfigurationBlock(
    void *configurationBlock,
    DeviceConfigurationOption configuration,
    uint32_t configurationIndex)
{
#if (DEBUG_RP2040 & DEBUG_HAL_CONFIG_UPDATE)
    DEBUG_PRINTF("ConfigurationManager_UpdateConfigurationBlock: start\n");
#endif
#if (DEBUG_RP2040 & DEBUG_HAL_CONFIG_DETAIL)
    DEBUG_PRINTF("CM_UpdateConf: %s Config[%d] Block=%08x\n",
            get_deviceConfigurationName(configuration), (int)configurationIndex, (uint)configurationBlock);
#endif

    ByteAddress storageAddress;
    uint32_t blockOffset;
    uint8_t *blockAddressInCopy;
    uint32_t blockSize;
    bool flag = false;

    // config sector size
    int sizeOfConfigSector = (uint32_t)&__nanoConfig_end__ - (uint32_t)&__nanoConfig_start__;

    // allocate memory from CRT heap
    uint8_t *configSectorCopy = (uint8_t *)platform_malloc(sizeOfConfigSector);

    if (configSectorCopy != NULL)
    {
        // copy config sector from flash to RAM
        memcpy(configSectorCopy, &__nanoConfig_start__, sizeOfConfigSector);

        // find out the address for the config block to update in the configSectorCopy
        // because we are copying back the config block to flash and just replacing the config block content
        // the addresses in g_TargetConfiguration will remain the same
        // plus we can calculate the offset of the config block from g_TargetConfiguration
        if (configuration == DeviceConfigurationOption_Network)
        {
            // make sure the config block marker is set
            memcpy(configurationBlock, c_MARKER_CONFIGURATION_NETWORK_V1, sizeof(c_MARKER_CONFIGURATION_NETWORK_V1));

            // check if the configuration block is the same
            if (ConfigurationManager_CheckExistingConfigurationBlock(
                    g_TargetConfiguration.NetworkInterfaceConfigs->Configs[configurationIndex],
                    configurationBlock,
                    sizeof(HAL_Configuration_NetworkInterface),
                    sizeof(HAL_Configuration_NetworkInterface)))
            {
                // block is the same
                // free memory
                platform_free(configSectorCopy);

                // operation is successfull (nothing to update)
                flag = true;
                goto UpdateConfigurationBlock_exit;
            }

            // get storage address from block address
            storageAddress = (ByteAddress)g_TargetConfiguration.NetworkInterfaceConfigs->Configs[configurationIndex];

            // set block size, in case it's not already set
            blockSize = sizeof(HAL_Configuration_NetworkInterface);

            _ASSERTE(((HAL_Configuration_NetworkInterface *)configurationBlock)->StartupAddressMode > 0);

        }
        else if (configuration == DeviceConfigurationOption_Wireless80211Network)
        {
            // make sure the config block marker is set
            memcpy(
                configurationBlock,
                c_MARKER_CONFIGURATION_WIRELESS80211_V1,
                sizeof(c_MARKER_CONFIGURATION_WIRELESS80211_V1));

            // check if the configuration block is the same
            if (ConfigurationManager_CheckExistingConfigurationBlock(
                    g_TargetConfiguration.Wireless80211Configs->Configs[configurationIndex],
                    configurationBlock,
                    sizeof(HAL_Configuration_Wireless80211),
                    sizeof(HAL_Configuration_Wireless80211)))
            {
                // block is the same
                // free memory
                platform_free(configSectorCopy);

                // operation is successfull (nothing to update)
                flag = true;
                goto UpdateConfigurationBlock_exit;
            }

            // storage address from block address
            storageAddress = (ByteAddress)g_TargetConfiguration.Wireless80211Configs->Configs[configurationIndex];

            // set block size, in case it's not already set
            blockSize = sizeof(HAL_Configuration_Wireless80211);
        }
        else if (configuration == DeviceConfigurationOption_X509CaRootBundle)
        {
            // make sure the config block marker is set
            memcpy(
                configurationBlock,
                c_MARKER_CONFIGURATION_X509CAROOTBUNDLE_V1,
                sizeof(c_MARKER_CONFIGURATION_X509CAROOTBUNDLE_V1));

            // check if certificate is the same
            if (ConfigurationManager_CheckExistingConfigurationBlock(
                    g_TargetConfiguration.CertificateStore->Certificates[configurationIndex]->Certificate,
                    ((HAL_Configuration_X509CaRootBundle *)configurationBlock)->Certificate,
                    g_TargetConfiguration.CertificateStore->Certificates[configurationIndex]->CertificateSize,
                    ((HAL_Configuration_X509CaRootBundle *)configurationBlock)->CertificateSize))
            {
                // block is the same
                // free memory
                platform_free(configSectorCopy);

                // operation is successfull (nothing to update)
                flag = true;
                goto UpdateConfigurationBlock_exit;
        }

            // storage address from block address
            storageAddress = (ByteAddress)g_TargetConfiguration.CertificateStore->Certificates[configurationIndex];

            // set block size, in case it's not already set
            // because X509 certificate has a variable length need to compute the block size in two steps
            blockSize = offsetof(HAL_Configuration_X509CaRootBundle, Certificate);
            blockSize += ((HAL_Configuration_X509CaRootBundle *)configurationBlock)->CertificateSize;
        }
        else if (configuration == DeviceConfigurationOption_X509DeviceCertificates)
        {
            // make sure the config block marker is set
            memcpy(
                configurationBlock,
                c_MARKER_CONFIGURATION_X509DEVICECERTIFICATE_V1,
                sizeof(c_MARKER_CONFIGURATION_X509DEVICECERTIFICATE_V1));

            // check if certificate is the same
            if (ConfigurationManager_CheckExistingConfigurationBlock(
                    g_TargetConfiguration.DeviceCertificates->Certificates[configurationIndex]->Certificate,
                    ((HAL_Configuration_X509DeviceCertificate *)configurationBlock)->Certificate,
                    g_TargetConfiguration.DeviceCertificates->Certificates[configurationIndex]->CertificateSize,
                    ((HAL_Configuration_X509DeviceCertificate *)configurationBlock)->CertificateSize))
            {
                // block is the same
                // free memory
                platform_free(configSectorCopy);

                // operation is successfull (nothing to update)
                flag = true;
                goto UpdateConfigurationBlock_exit;
            }

            // storage address from block address
            storageAddress = (ByteAddress)g_TargetConfiguration.DeviceCertificates->Certificates[configurationIndex];

            // set block size, in case it's not already set
            // because X509 certificate has a variable length need to compute the block size in two steps
            blockSize = offsetof(HAL_Configuration_X509DeviceCertificate, Certificate);
            blockSize += ((HAL_Configuration_X509DeviceCertificate *)configurationBlock)->CertificateSize;
        }
        else
        {
            // this not a valid configuration option to update, quit
            // free memory first
            platform_free(configSectorCopy);
            flag = false;
            goto UpdateConfigurationBlock_exit;
        }

        // erase config sector
        if (RP2040_flash_EraseBlock(NULL, (uint32_t)&__nanoConfig_start__) == TRUE)
        {
            // flash block is erased

            // subtract the start address of config sector to get the offset
            blockOffset = storageAddress - (uint32_t)&__nanoConfig_start__;

            // set pointer to block to udpate
            blockAddressInCopy = configSectorCopy + blockOffset;

            // replace config block with new content by replacing memory
            memcpy(blockAddressInCopy, configurationBlock, blockSize);

            // copy the config block copy back to the config block storage
            flag = RP2040_flash_Write(
                NULL,
                (uint32_t)&__nanoConfig_start__,
                sizeOfConfigSector,
                (unsigned char *)configSectorCopy,
                true);
        }

        // free memory
        platform_free(configSectorCopy);
    }
UpdateConfigurationBlock_exit:
#if (DEBUG_RP2040 & DEBUG_HAL_CONFIG_UPDATE)
    DEBUG_PRINTF("ConfigurationManager_UpdateConfigurationBlock: end - %s\n", flag? "OK" : "NG");
#endif
    return flag;
}

//  Default initialisation for wireless config block
// it's implemented with 'weak' attribute so it can be replaced at target level if different configurations are intended
__nfweak void InitialiseWirelessDefaultConfig(HAL_Configuration_Wireless80211 *pconfig, uint32_t configurationIndex)
{
    (void)pconfig;
    (void)configurationIndex;

    // currently empty
}

#if 0
//  Default initialisation for Network interface config blocks
// it's implemented with 'weak' attribute so it can be replaced at target level if different configurations are intended
__nfweak bool InitialiseNetworkDefaultConfig(HAL_Configuration_NetworkInterface *pconfig, uint32_t configurationIndex)
{
    (void)pconfig;
    (void)configurationIndex;

    // can't create a "default" network config because we are lacking definition of a MAC address
    return FALSE;
}
#else

// extern uint8_t mac[];
uint8_t mac[6] = {0x00, 0x08, 0xDC, 0x12, 0x34, 0x56};

// Default initialisation for Network interface config blocks
// strong implementation replacing 'weak' one
bool InitialiseNetworkDefaultConfig(HAL_Configuration_NetworkInterface *pconfig, uint32_t configurationIndex)
{
    (void)configurationIndex;
    uint8_t unique_id[8];

#if (DEBUG_RP2040 & DEBUG_HAL_CONFIG_DEF)
    DEBUG_PRINTF("InitialiseNetworkDefaultConfig: Config[%d]=%08x\n", (int)configurationIndex, (uint)pconfig);
#endif
    // zero memory
    memset(pconfig, 0, sizeof(HAL_Configuration_NetworkInterface));

    // make sure the config block marker is set
    memcpy(pconfig->Marker, c_MARKER_CONFIGURATION_NETWORK_V1, sizeof(c_MARKER_CONFIGURATION_NETWORK_V1));

    pconfig->InterfaceType = NetworkInterfaceType_Ethernet;
    pconfig->StartupAddressMode = AddressMode_DHCP;
    pconfig->AutomaticDNS = 1;
    pconfig->SpecificConfigId = UINT32_MAX;

    pico_get_unique_board_id((pico_unique_board_id_t *)unique_id);
    pconfig->MacAddress[0] = mac[0];
    pconfig->MacAddress[1] = mac[1];
    pconfig->MacAddress[2] = mac[2];
    pconfig->MacAddress[3] = unique_id[5];
    pconfig->MacAddress[4] = unique_id[6];
    pconfig->MacAddress[5] = unique_id[7];

    return true;
}
#endif