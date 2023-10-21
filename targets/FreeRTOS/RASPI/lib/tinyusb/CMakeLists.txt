add_library(tinyusb INTERFACE)

target_include_directories(tinyusb
    INTERFACE
        ${CMAKE_SOURCE_DIR}/libs/tinyusb/src
        ${CMAKE_SOURCE_DIR}/libs/tinyusb/src/common
        ${CMAKE_SOURCE_DIR}/libs/tinyusb/src/device
        ${CMAKE_SOURCE_DIR}/libs/tinyusb/src/host
        ${CMAKE_SOURCE_DIR}/libs/tinyusb/src/osal
        ${CMAKE_SOURCE_DIR}/libs/tinyusb/src/portable/raspberrypi/rp2040
#        ${CMAKE_SOURCE_DIR}/libs/tinyusb/src/class/audio
#        ${CMAKE_SOURCE_DIR}/libs/tinyusb/src/class/bth
        ${CMAKE_SOURCE_DIR}/libs/tinyusb/src/class/cdc
#        ${CMAKE_SOURCE_DIR}/libs/tinyusb/src/class/dfu
#        ${CMAKE_SOURCE_DIR}/libs/tinyusb/src/class/hid
#        ${CMAKE_SOURCE_DIR}/libs/tinyusb/src/class/midi
#        ${CMAKE_SOURCE_DIR}/libs/tinyusb/src/class/msc
#        ${CMAKE_SOURCE_DIR}/libs/tinyusb/src/class/net
#        ${CMAKE_SOURCE_DIR}/libs/tinyusb/src/class/usbtmc
#        ${CMAKE_SOURCE_DIR}/libs/tinyusb/src/class/vendor
#        ${CMAKE_SOURCE_DIR}/libs/tinyusb/src/class/video
        .
        TARGET_RASPBERRY_PI_PICO
        ../pico-sdk/rp2_common/hardware_adc/include
        ../pico-sdk/rp2_common/hardware_gpio/include
        ../pico-sdk/rp2_common/hardware_resets/include
        ../pico-sdk/rp2_common/hardware_pwm/include
        ../pico-sdk/rp2_common/hardware_base/include
        ../pico-sdk/rp2_common/hardware_uart/include
        ../pico-sdk/rp2_common/hardware_spi/include
        ../pico-sdk/rp2_common/hardware_i2c/include
        ../pico-sdk/rp2_common/hardware_irq/include
        ../pico-sdk/rp2_common/hardware_flash/include
        ../pico-sdk/rp2_common/hardware_clocks/include
        ../pico-sdk/rp2_common/hardware_rtc/include
        ../pico-sdk/rp2_common/hardware_watchdog/include
        ../pico-sdk/rp2_common/hardware_timer/include
        ../pico-sdk/rp2_common/hardware_pll/include
        ../pico-sdk/rp2_common/hardware_sync/include
        ../pico-sdk/rp2_common/hardware_xosc/include
        ../pico-sdk/rp2_common/pico_platform/include
        ../pico-sdk/rp2_common/pico_fix/rp2040_usb_device_enumeration/include/pico/fix
        ../pico-sdk/rp2_common/pico_bootrom/include
        ../pico-sdk/rp2_common/pico_stdio/include
        ../pico-sdk/rp2_common/hardware_claim/include
        ../pico-sdk/rp2040/hardware_structs/include
        ../pico-sdk/rp2040/hardware_regs/include
        ../pico-sdk/common/pico_sync/include
        ../pico-sdk/common/pico_time/include
        ../pico-sdk/common/pico_base/include
        ../pico-sdk/common/pico_binary_info/include
        ../pico-sdk/common/pico_util/include
        ../pico-sdk/common/pico_stdlib/include
        ../pico-sdk/boards/include
        ../pico-sdk/generated
        ../pico-sdk/rp2_common/pico_stdio_usb/include
)

target_sources(tinyusb
    INTERFACE
        ${CMAKE_SOURCE_DIR}/libs/tinyusb/src/tusb.c
        ${CMAKE_SOURCE_DIR}/libs/tinyusb/src/common/tusb_fifo.c
        ${CMAKE_SOURCE_DIR}/libs/tinyusb/src/device/usbd_control.c
        ${CMAKE_SOURCE_DIR}/libs/tinyusb/src/device/usbd.c
#        ${CMAKE_SOURCE_DIR}/libs/tinyusb/src/host/hub.c
#        ${CMAKE_SOURCE_DIR}/libs/tinyusb/src/host/usbh.c
        ${CMAKE_SOURCE_DIR}/libs/tinyusb/src/portable/raspberrypi/rp2040/dcd_rp2040.c
#        ${CMAKE_SOURCE_DIR}/libs/tinyusb/src/portable/raspberrypi/rp2040/hcd_rp2040.c
        ${CMAKE_SOURCE_DIR}/libs/tinyusb/src/portable/raspberrypi/rp2040/rp2040_usb.c
#        ${CMAKE_SOURCE_DIR}/libs/tinyusb/src/class/audio/audio_device.c
#        ${CMAKE_SOURCE_DIR}/libs/tinyusb/src/class/bth/bth_device.c
        ${CMAKE_SOURCE_DIR}/libs/tinyusb/src/class/cdc/cdc_device.c
#        ${CMAKE_SOURCE_DIR}/libs/tinyusb/src/class/cdc/cdc_host.c
#        ${CMAKE_SOURCE_DIR}/libs/tinyusb/src/class/cdc/cdc_mdis_host.c
#        ${CMAKE_SOURCE_DIR}/libs/tinyusb/src/class/dfu/dfu_device.c
#        ${CMAKE_SOURCE_DIR}/libs/tinyusb/src/class/dfu/dfu_rt_device.c
#        ${CMAKE_SOURCE_DIR}/libs/tinyusb/src/class/hid/hid_device.c
#        ${CMAKE_SOURCE_DIR}/libs/tinyusb/src/class/hid/hid_host.c
#        ${CMAKE_SOURCE_DIR}/libs/tinyusb/src/class/midi/midi_device.c
#        ${CMAKE_SOURCE_DIR}/libs/tinyusb/src/class/msc/msc_device.c
#        ${CMAKE_SOURCE_DIR}/libs/tinyusb/src/class/msc/msc_host.c
#        ${CMAKE_SOURCE_DIR}/libs/tinyusb/src/class/net/ecm_mdis_device.c
#        ${CMAKE_SOURCE_DIR}/libs/tinyusb/src/class/net/ncm_device.c
#        ${CMAKE_SOURCE_DIR}/libs/tinyusb/src/class/usbtmc/usbtmc_device.c
#        ${CMAKE_SOURCE_DIR}/libs/tinyusb/src/class/vendor/vendor_device.c
#        ${CMAKE_SOURCE_DIR}/libs/tinyusb/src/class/vendor/vendor_host.c
#        ${CMAKE_SOURCE_DIR}/libs/tinyusb/src/class/video/video_device.c
    )

target_compile_definitions(tinyusb INTERFACE
    -DCFG_TUSB_MCU=OPT_MCU_RP2040
    -DTUP_DCD_ENDPOINT_MAX=16
    )
