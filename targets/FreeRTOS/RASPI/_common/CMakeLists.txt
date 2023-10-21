#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# append include directory for target RP2040
list(APPEND TARGET_RASPI_COMMON_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR})

list(APPEND TARGET_RASPI_COMMON_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/FreeRTOS/RASPI/lib/pico-sdk/rp2_common/hardware_adc/include)
list(APPEND TARGET_RASPI_COMMON_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/FreeRTOS/RASPI/lib/pico-sdk/rp2_common/hardware_exception/include)
list(APPEND TARGET_RASPI_COMMON_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/FreeRTOS/RASPI/lib/pico-sdk/rp2_common/hardware_gpio/include)
list(APPEND TARGET_RASPI_COMMON_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/FreeRTOS/RASPI/lib/pico-sdk/rp2_common/hardware_resets/include)
list(APPEND TARGET_RASPI_COMMON_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/FreeRTOS/RASPI/lib/pico-sdk/rp2_common/hardware_pwm/include)
list(APPEND TARGET_RASPI_COMMON_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/FreeRTOS/RASPI/lib/pico-sdk/rp2_common/hardware_base/include)
list(APPEND TARGET_RASPI_COMMON_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/FreeRTOS/RASPI/lib/pico-sdk/rp2_common/hardware_uart/include)
list(APPEND TARGET_RASPI_COMMON_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/FreeRTOS/RASPI/lib/pico-sdk/rp2_common/hardware_spi/include)
list(APPEND TARGET_RASPI_COMMON_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/FreeRTOS/RASPI/lib/pico-sdk/rp2_common/hardware_i2c/include)
list(APPEND TARGET_RASPI_COMMON_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/FreeRTOS/RASPI/lib/pico-sdk/rp2_common/hardware_irq/include)
list(APPEND TARGET_RASPI_COMMON_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/FreeRTOS/RASPI/lib/pico-sdk/rp2_common/hardware_flash/include)
list(APPEND TARGET_RASPI_COMMON_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/FreeRTOS/RASPI/lib/pico-sdk/rp2_common/hardware_clocks/include)
list(APPEND TARGET_RASPI_COMMON_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/FreeRTOS/RASPI/lib/pico-sdk/rp2_common/hardware_rtc/include)
list(APPEND TARGET_RASPI_COMMON_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/FreeRTOS/RASPI/lib/pico-sdk/rp2_common/hardware_watchdog/include)
list(APPEND TARGET_RASPI_COMMON_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/FreeRTOS/RASPI/lib/pico-sdk/rp2_common/hardware_timer/include)
list(APPEND TARGET_RASPI_COMMON_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/FreeRTOS/RASPI/lib/pico-sdk/rp2_common/hardware_pll/include)
list(APPEND TARGET_RASPI_COMMON_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/FreeRTOS/RASPI/lib/pico-sdk/rp2_common/hardware_sync/include)
list(APPEND TARGET_RASPI_COMMON_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/FreeRTOS/RASPI/lib/pico-sdk/rp2_common/hardware_xosc/include)
list(APPEND TARGET_RASPI_COMMON_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/FreeRTOS/RASPI/lib/pico-sdk/rp2_common/pico_platform/include)
list(APPEND TARGET_RASPI_COMMON_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/FreeRTOS/RASPI/lib/pico-sdk/rp2_common/pico_fix/rp2040_usb_device_enumeration/include/pico/fix)
list(APPEND TARGET_RASPI_COMMON_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/FreeRTOS/RASPI/lib/pico-sdk/rp2_common/pico_bootrom/include)
list(APPEND TARGET_RASPI_COMMON_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/FreeRTOS/RASPI/lib/pico-sdk/rp2_common/pico_stdio/include)
list(APPEND TARGET_RASPI_COMMON_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/FreeRTOS/RASPI/lib/pico-sdk/rp2_common/pico_stdio_usb/include)
list(APPEND TARGET_RASPI_COMMON_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/FreeRTOS/RASPI/lib/pico-sdk/rp2_common/pico_stdio_uart/include)
list(APPEND TARGET_RASPI_COMMON_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/FreeRTOS/RASPI/lib/pico-sdk/rp2_common/pico_unique_id/include)
list(APPEND TARGET_RASPI_COMMON_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/FreeRTOS/RASPI/lib/pico-sdk/rp2_common/hardware_claim/include)
list(APPEND TARGET_RASPI_COMMON_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/FreeRTOS/RASPI/lib/pico-sdk/rp2040/hardware_structs/include)
list(APPEND TARGET_RASPI_COMMON_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/FreeRTOS/RASPI/lib/pico-sdk/rp2040/hardware_regs/include)
list(APPEND TARGET_RASPI_COMMON_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/FreeRTOS/RASPI/lib/pico-sdk/common/pico_sync/include)
list(APPEND TARGET_RASPI_COMMON_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/FreeRTOS/RASPI/lib/pico-sdk/common/pico_time/include)
list(APPEND TARGET_RASPI_COMMON_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/FreeRTOS/RASPI/lib/pico-sdk/common/pico_base/include)
list(APPEND TARGET_RASPI_COMMON_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/FreeRTOS/RASPI/lib/pico-sdk/common/pico_binary_info/include)
list(APPEND TARGET_RASPI_COMMON_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/FreeRTOS/RASPI/lib/pico-sdk/common/pico_util/include)
list(APPEND TARGET_RASPI_COMMON_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/FreeRTOS/RASPI/lib/pico-sdk/common/pico_stdlib/include)
list(APPEND TARGET_RASPI_COMMON_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/FreeRTOS/RASPI/lib/pico-sdk/common/pico_usb_reset_interface/include)
list(APPEND TARGET_RASPI_COMMON_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/FreeRTOS/RASPI/lib/pico-sdk/boards/include)
list(APPEND TARGET_RASPI_COMMON_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/targets/FreeRTOS/RASPI/lib/pico-sdk/generated)

list(APPEND TARGET_RASPI_COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/WireProtocol_HAL_Interface.c)
list(APPEND TARGET_RASPI_COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/debug_printf.c)
list(APPEND TARGET_RASPI_COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/port_utils.c)

# append Target files
list(APPEND TARGET_RASPI_COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Device_BlockStorage.c)
list(APPEND TARGET_RASPI_COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/Target_BlockStorage_RP2040_FlashDriver.c)
list(APPEND TARGET_RASPI_COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/platform_BlockStorage.c)

# include configuration manager file, if feature is enabled
if(NF_FEATURE_HAS_CONFIG_BLOCK)
    list(APPEND TARGET_RASPI_COMMON_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/targetHAL_ConfigurationManager.cpp)
endif()

# make var global
set(TARGET_RASPI_COMMON_SOURCES ${TARGET_RASPI_COMMON_SOURCES} CACHE INTERNAL "make global")
set(TARGET_RASPI_COMMON_INCLUDE_DIRS ${TARGET_RASPI_COMMON_INCLUDE_DIRS} CACHE INTERNAL "make global")
