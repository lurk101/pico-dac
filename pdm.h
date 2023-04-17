#pragma once

#include <stdint.h>
//
#include <hardware/pio.h>

typedef struct {
    int32_t buff[8];
    int32_t w[4];
    uint32_t sm;
    int16_t last;
} pdm_data;

static const PIO pio = pio0;

void pdm_begin(pdm_data* pdm_left, pdm_data* pdm_right, uint32_t pin_left, uint32_t pin_right,
               uint32_t clk_div);
uint32_t pdm_o4_os32_df2(pdm_data* pdm, int16_t signal);
