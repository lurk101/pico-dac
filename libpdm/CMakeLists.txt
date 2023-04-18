add_library(libpdm INTERFACE)
target_include_directories(libpdm INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(libpdm INTERFACE
    pico_multicore
    hardware_clocks hardware_pio hardware_gpio)
target_sources(libpdm INTERFACE
    pdm.c pdm.h pdm.pio)
pico_generate_pio_header(libpdm ${CMAKE_CURRENT_LIST_DIR}/pdm.pio)
