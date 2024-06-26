cmake_minimum_required(VERSION 3.13)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(spiTx)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

add_executable(spiTx
  main.c
)

# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(spiTx pico_stdlib hardware_spi)

# enable usb output, disable uart output
    pico_enable_stdio_usb(spiTx 1)
    pico_enable_stdio_uart(spiTx 0)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(spiTx)

# add url via pico_set_program_url
# example_auto_set_url(spiTx)
