cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(pdm C CXX ASM)

pico_sdk_init()

add_compile_options("-DSYS_CLK_KHZ=96000")
add_compile_options("-DPLL_SYS_VCO_FREQ_KHZ=768000")
add_compile_options("-DPLL_SYS_POSTDIV1=4")
add_compile_options("-DPLL_SYS_POSTDIV2=2")

add_executable(pdm main.c pdm.c pdm.h pdm.pio)

pico_generate_pio_header(pdm ${CMAKE_CURRENT_LIST_DIR}/pdm.pio)

pico_set_program_name(pdm "pdm")
pico_set_program_version(pdm "0.0")

pico_enable_stdio_uart(pdm 1)
pico_enable_stdio_usb(pdm 0)

target_link_libraries(pdm
    pico_stdlib pico_multicore
    hardware_clocks hardware_pio hardware_gpio)

pico_add_extra_outputs(pdm)

