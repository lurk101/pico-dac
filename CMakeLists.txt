cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(dac C CXX ASM)

pico_sdk_init()

add_compile_options("-DSYS_CLK_KHZ=127000")
add_compile_options("-DPLL_SYS_VCO_FREQ_KHZ=1524000")
add_compile_options("-DPLL_SYS_POSTDIV1=6")
add_compile_options("-DPLL_SYS_POSTDIV2=2")

add_subdirectory(libpdm)
add_subdirectory(libi2s)

add_executable(dac main.c)

pico_set_program_name(dac "dac")
pico_set_program_version(dac "0.0")

pico_enable_stdio_uart(dac 1)
pico_enable_stdio_usb(dac 0)

target_link_libraries(dac
    libpdm libi2s
    pico_stdlib pico_multicore
    hardware_clocks hardware_pio hardware_gpio)

pico_add_extra_outputs(dac)

