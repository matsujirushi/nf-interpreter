set(CMAKE_VERBOSE_MAKEFILE on)
set(CMAKE_BUILD_TYPE Debug)

add_library(rp2040 INTERFACE)

target_include_directories(rp2040
    INTERFACE
        TARGET_RASPBERRY_PI_PICO
        pico-sdk/common/pico_base/include
        pico-sdk/common/pico_binary_info/include
        pico-sdk/common/pico_stdlib/include
        pico-sdk/common/pico_sync/include
        pico-sdk/common/pico_time/include
        pico-sdk/common/pico_usb_reset_interface/include
        pico-sdk/common/pico_util/include
        pico-sdk/rp2040/hardware_structs/include
        pico-sdk/rp2040/hardware_regs/include
        pico-sdk/rp2_common/hardware_adc/include
        pico-sdk/rp2_common/hardware_dma/include
        pico-sdk/rp2_common/hardware_exception/include
        pico-sdk/rp2_common/hardware_gpio/include
        pico-sdk/rp2_common/hardware_resets/include
        pico-sdk/rp2_common/hardware_pwm/include
        pico-sdk/rp2_common/hardware_base/include
        pico-sdk/rp2_common/hardware_uart/include
        pico-sdk/rp2_common/hardware_spi/include
        pico-sdk/rp2_common/hardware_i2c/include
        pico-sdk/rp2_common/hardware_irq/include
        pico-sdk/rp2_common/hardware_flash/include
        pico-sdk/rp2_common/hardware_clocks/include
        pico-sdk/rp2_common/hardware_rtc/include
        pico-sdk/rp2_common/hardware_watchdog/include
        pico-sdk/rp2_common/hardware_timer/include
        pico-sdk/rp2_common/hardware_pll/include
        pico-sdk/rp2_common/hardware_pwm/include
        pico-sdk/rp2_common/hardware_rtc/include
        pico-sdk/rp2_common/hardware_sync/include
        pico-sdk/rp2_common/hardware_xosc/include
        pico-sdk/rp2_common/pico_bootrom/include
        pico-sdk/rp2_common/pico_fix/rp2040_usb_device_enumeration/include/pico/fix
        pico-sdk/rp2_common/pico_platform/include
        pico-sdk/rp2_common/pico_stdio/include
        pico-sdk/rp2_common/pico_stdio_uart/include
        pico-sdk/rp2_common/pico_stdio_usb/include
        pico-sdk/rp2_common/pico_unique_id/include
        pico-sdk/rp2_common/hardware_claim/include
        pico-sdk/boards/include
        pico-sdk/generated
        .
)

target_sources(rp2040
    INTERFACE
        TARGET_RASPBERRY_PI_PICO/board.c
        TARGET_RASPBERRY_PI_PICO/bs2_default_padded_checksummed.S
        pico-sdk/common/pico_sync/critical_section.c
        pico-sdk/common/pico_sync/mutex.c
        pico-sdk/common/pico_sync/lock_core.c
        pico-sdk/common/pico_time/time.c
        pico-sdk/common/pico_util/pheap.c
        pico-sdk/rp2_common/pico_standard_link/crt0.S
        pico-sdk/rp2_common/hardware_adc/adc.c
        pico-sdk/rp2_common/hardware_dma/dma.c
        pico-sdk/rp2_common/hardware_claim/claim.c
        pico-sdk/rp2_common/hardware_clocks/clocks.c
        pico-sdk/rp2_common/hardware_exception/exception.c
        pico-sdk/rp2_common/hardware_flash/flash.c
        pico-sdk/rp2_common/hardware_gpio/gpio.c
        pico-sdk/rp2_common/hardware_i2c/i2c.c
        pico-sdk/rp2_common/hardware_irq/irq_handler_chain.S
        pico-sdk/rp2_common/hardware_irq/irq.c
        pico-sdk/rp2_common/hardware_pll/pll.c
        pico-sdk/rp2_common/hardware_rtc/rtc.c
        pico-sdk/rp2_common/hardware_spi/spi.c
        pico-sdk/rp2_common/hardware_sync/sync.c
        pico-sdk/rp2_common/hardware_timer/timer.c
        pico-sdk/rp2_common/hardware_uart/uart.c
        pico-sdk/rp2_common/hardware_watchdog/watchdog.c
        pico-sdk/rp2_common/hardware_xosc/xosc.c
        pico-sdk/rp2_common/pico_bootrom/bootrom.c
        pico-sdk/rp2_common/pico_runtime/runtime.c
        pico-sdk/rp2_common/pico_platform/platform.c
        pico-sdk/rp2_common/pico_stdio/stdio.c
        pico-sdk/rp2_common/pico_stdlib/stdlib.c
        pico-sdk/rp2_common/pico_stdio_uart/stdio_uart.c
        pico-sdk/rp2_common/pico_stdio_usb/stdio_usb.c
        pico-sdk/rp2_common/pico_stdio_usb/reset_interface.c
        pico-sdk/rp2_common/pico_unique_id/unique_id.c
        usb_descriptor/nano_usb_descriptors.c
    )

add_subdirectory(tinyusb)

target_link_libraries(rp2040 INTERFACE
    tinyusb
)

target_compile_definitions(rp2040 INTERFACE
    -DLIB_PICO_STDIO_USB=1
    -DLIB_PICO_STDIO_UART=0
    -DPICO_NO_BINARY_INFO=0
    -DPICO_DEFAULT_UART_INSTANCE=uart0
    -DPICO_DEFAULT_UART_TX_PIN=0
    -DPICO_DEFAULT_UART_RX_PIN=1
    -DPICO_DEFAULT_UART_BAUD_RATE=115200
    -DPICO_STDIO_ENABLE_CRLF_SUPPORT=0
    -DPICO_DEFAULT_SPI_INSTANCE
    )

#[[
add_compile_options(-Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
        -Wno-maybe-uninitialized
        -mthumb
        -mcpu=cortex-m0plus
        -nostdlib
        -Wall
        -Wextra
        -ffunction-sections
        -fdata-sections
        -Wno-unused-label
        -Wno-unused-parameter
        -Wno-unused-but-set-variable
        -Wno-uninitialized
        -Wno-unused-variable
        -Wno-return-type
        -Wno-unused-function
        )
# target_link_libraries(rp2040 INTERFACE)
]]
