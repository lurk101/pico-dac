cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(pdm C CXX ASM)

pico_sdk_init()

add_executable(pdm main.c)

pico_set_program_name(pdm "pdm")
pico_set_program_version(pdm "0.0")

pico_enable_stdio_uart(pdm 1)
pico_enable_stdio_usb(pdm 0)

target_link_libraries(pdm pico_stdlib hardware_clocks)

pico_add_extra_outputs(pdm)

