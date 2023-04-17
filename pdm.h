#pragma once

#include <stdint.h>
//
#include <hardware/pio.h>

struct pdm_data {
    int32_t buff[8];
    int32_t w[4];
    uint32_t sm;
};

static const PIO pio = pio0;

void pdm_begin(struct pdm_data* pdm_l, struct pdm_data* pdm_r, uint32_t pl, uint32_t pr);
uint32_t pdm_o4_os32_df2(struct pdm_data* pdm, int16_t sig);
