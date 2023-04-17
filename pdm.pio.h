// -------------------------------------------------- //
// This file is autogenerated by pioasm; do not edit! //
// -------------------------------------------------- //

#if !PICO_NO_HARDWARE
#include "hardware/pio.h"
#endif

// --- //
// pdm //
// --- //

#define pdm_wrap_target 0
#define pdm_wrap 0

static const uint16_t pdm_program_instructions[] = {
             //     .wrap_target
    0x6001,  //  0: out    pins, 1
             //     .wrap
};

#if !PICO_NO_HARDWARE
static const struct pio_program pdm_program = {
    .instructions = pdm_program_instructions,
    .length = 1,
    .origin = -1,
};

static inline pio_sm_config pdm_program_get_default_config(uint offset) {
    pio_sm_config c = pio_get_default_sm_config();
    sm_config_set_wrap(&c, offset + pdm_wrap_target, offset + pdm_wrap);
    return c;
}

#endif
