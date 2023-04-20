#pragma once

#include <stdint.h>

extern volatile uint32_t stereo;

void i2s_begin(uint32_t clk_pin, uint32_t lr_clk_pin, uint32_t data_pin);
