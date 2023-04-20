add_library(libi2s INTERFACE)
target_include_directories(libi2s INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(libi2s INTERFACE
    hardware_clocks hardware_pio hardware_dma)
target_sources(libi2s INTERFACE
    i2s.c i2s.h i2s.pio)
pico_generate_pio_header(libi2s ${CMAKE_CURRENT_LIST_DIR}/i2s.pio)
