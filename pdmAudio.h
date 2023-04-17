#pragma once

#include <stdint.h>

struct pdmAudio {
    int32_t buff[8];
    int32_t w[4];
};

void pdmAudio_begin(struct pdmAudio* pdm_l, struct pdmAudio* pdm_r, uint32_t pl, uint32_t pr);
uint32_t o4_os32_df2(struct pdmAudio* pdm, int16_t sig);
