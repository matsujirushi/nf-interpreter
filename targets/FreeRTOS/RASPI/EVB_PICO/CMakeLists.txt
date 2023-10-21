#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

include(binutils.common)
include(binutils.FreeRTOS)

nf_setup_target_build(
    HAS_NANOBOOTER

    BOOTER_LINKER_FILE 
        RP2040

    CLR_LINKER_FILE 
        RP2040

    BOOTER_EXTRA_COMPILE_DEFINITIONS
        -DRP2040
    
    CLR_EXTRA_COMPILE_DEFINITIONS
        -DRP2040

    BOOTER_EXTRA_LINKMAP_PROPERTIES
        ",--library-path=${CMAKE_SOURCE_DIR}/targets/RASPI/common,--defsym=__crt_heap_size__=0x0"

    CLR_EXTRA_LINKMAP_PROPERTIES
        ",--library-path=${CMAKE_SOURCE_DIR}/targets/RASPI/common,--defsym=__crt_heap_size__=0x0"
)

# target_compile_definitions(NF_Network PUBLIC -DRP2040)
target_compile_definitions(NF_NativeAssemblies PUBLIC -DRP2040)

# Set ethernet chip
set(WIZNET_CHIP W5100S)
# set(WIZNET_CHIP W5500)

if(${WIZNET_CHIP} STREQUAL W5100S)
    add_definitions(-D_WIZCHIP_=W5100S)
elseif(${WIZNET_CHIP} STREQUAL W5500)
    add_definitions(-D_WIZCHIP_=W5500)
else()
    message(FATAL_ERROR "WIZNET_CHIP is wrong = ${WIZNET_CHIP}")
endif()

#[[
if(SRECORD_TOOL_AVAILABLE)

    nf_generate_hex_package(
        ${CMAKE_SOURCE_DIR}/build/${NANOBOOTER_PROJECT_NAME}.hex
        ${CMAKE_SOURCE_DIR}/build/${NANOCLR_PROJECT_NAME}.hex
        ${CMAKE_SOURCE_DIR}/build/nanobooter-nanoclr.hex)

endif()
]]
