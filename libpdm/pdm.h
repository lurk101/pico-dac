#pragma once

#include <hardware/pio.h>

typedef struct {
    int32_t w[4];
    uint32_t sm;
    uint32_t last;
    uint16_t signal;
    uint16_t idles;
} pdm_data;

static const PIO pio = pio0;

void pdm_begin(pdm_data* pdm_left, pdm_data* pdm_right, uint32_t pin_left, uint32_t pin_right,
               uint32_t clk_div);
uint32_t pdm_o4_os32_df2(pdm_data* pdm, int16_t signal);
void pdm_pause(pdm_data* pdm_l, pdm_data* pdm_r);
