#pragma once

#include <stdint.h>

void i2s_begin(uint32_t clk_pin, uint32_t lr_clk_pin, uint32_t data_pin);
int i2s_ready(void);
uint32_t i2s_read(void);
