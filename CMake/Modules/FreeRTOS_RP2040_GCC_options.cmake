#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

#################################################################
# WHEN ADDING A NEW SERIES add the appropriate GCC options bellow
#################################################################

# need to specify this for assembler
set(CMAKE_ASM_FLAGS " -mthumb -mcpu=cortex-m0plus -x assembler-with-cpp" CACHE INTERNAL "asm compiler flags")

# need to specify linker flags here
set(CMAKE_EXE_LINKER_FLAGS " -Wl,--gc-sections -Wl,--no-wchar-size-warning -mthumb -mcpu=cortex-m0plus -mfloat-abi=soft -mabi=aapcs -nostartfiles " CACHE INTERNAL "executable linker flags")


# TARGET parameter to set the target that's setting them for
# optional EXTRA_COMPILE_OPTIONS with compile options to be added
macro(nf_set_compile_options)

    # parse arguments
    cmake_parse_arguments(NFSCO "" "TARGET" "EXTRA_COMPILE_OPTIONS" ${ARGN})
    
    if(NOT NFSCO_TARGET OR "${NFSCO_TARGET}" STREQUAL "")
        message(FATAL_ERROR "Need to set TARGET argument when calling nf_set_compile_options()")
    endif()


    # include any extra options comming from any extra args?
    # TODO: removed -Wundef until fix with FatFS is merged (https://github.com/abbrev/fatfs/pull/8)
    target_compile_options(${NFSCO_TARGET} PUBLIC  ${NFSCO__EXTRA_COMPILE_OPTIONS} -DRP2040 -mthumb -mcpu=cortex-m0 -mfloat-abi=soft -mabi=aapcs -nostdlib -Wall -Wextra -ffunction-sections -fshort-wchar -falign-functions=16 -fdata-sections -fno-builtin -fno-common -fomit-frame-pointer -mlong-calls -fdollars-in-identifiers -fno-exceptions -fno-unroll-loops -frounding-math -fsignaling-nans -ffloat-store -fno-math-errno -ftree-vectorize -fcheck-new -Wno-unused-label -Wno-unused-parameter -Wno-unused-but-set-variable -Wno-uninitialized -Wno-unused-variable -Wno-return-type -Wno-unused-function -Wno-ignored-qualifiers)

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-int-conversion")
    set(CMAKE_CXX_FLAGS "-Wno-class-memaccess" CACHE STRING "C++ Compiler Base Flags")

    # this series has FPU 
    target_compile_definitions(${NFSCO_TARGET} PUBLIC -DPLATFORM_ARM ) 

endmacro()


# TARGET parameter to set the target that's setting them for
# optional EXTRA_LINK_FLAGS with link flags to be added
macro(nf_set_link_options)

    # parse arguments
    cmake_parse_arguments(NFSLO "" "TARGET;EXTRA_LINK_FLAGS" "" ${ARGN})
    
    if(NOT NFSLO_TARGET OR "${NFSLO_TARGET}" STREQUAL "")
        message(FATAL_ERROR "Need to set TARGET argument when calling nf_set_link_options()")
    endif()

    # request specs from newlib nano
    set_property(TARGET ${NFSLO_TARGET} APPEND_STRING PROPERTY LINK_FLAGS " -ffunction-sections --specs=nano.specs --specs=nosys.specs -Wl,--start-group -Xlinker --gc-sections -Xlinker --sort-section=alignment -Xlinker -print-memory-usage -Xlinker --cref -Wl,--end-group ")

    # set optimization linker flags for RELEASE and MinSizeRel
    if(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
        set_property(TARGET ${NFSLO_TARGET} APPEND_STRING PROPERTY LINK_FLAGS " -flto -fuse-linker-plugin -Os ")
    endif()
      
    # include libraries in build
    nf_include_libraries_in_build(${NFSLO_TARGET})

    # set extra linker flags
    set_property(TARGET ${NFSLO_TARGET} APPEND_STRING PROPERTY LINK_FLAGS " ${NFSLO_EXTRA_LINK_FLAGS} ")
      
    # set optimization flags
    nf_set_optimization_options(${NFSLO_TARGET})

endmacro()
