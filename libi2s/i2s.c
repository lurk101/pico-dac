
#include "i2s.h"

#include "i2s.pio.h"
#include "pdm.h"  // just need the pio instance

static uint32_t sm;

void i2s_begin(uint32_t clk_pin, uint32_t lr_clk_pin, uint32_t data_pin) {
    uint32_t offset = pio_add_program(pio, &i2s_program);
    sm = pio_claim_unused_sm(pio, true);
    // PIO configuration
    pio_sm_config c = i2s_program_get_default_config(offset);
    sm_config_set_in_pins(&c, i2s_DATA);
    pio_gpio_init(pio, i2s_DATA);
    pio_gpio_init(pio, i2s_CLK);
    pio_gpio_init(pio, i2s_LRCLK);
    pio_sm_set_consecutive_pindirs(pio, sm, i2s_DATA, 1, false);
    pio_sm_set_consecutive_pindirs(pio, sm, i2s_CLK, 1, false);
    pio_sm_set_consecutive_pindirs(pio, sm, i2s_LRCLK, 1, false);
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_RX);
    sm_config_set_clkdiv(&c, 1);
    sm_config_set_in_shift(&c, false, true, 32);  // shift left, auto push
    pio_sm_init(pio, sm, offset, &c);
    pio_sm_set_enabled(pio, sm, true);
}

int i2s_ready(void) { return !pio_sm_is_rx_fifo_empty(pio, sm); }

uint32_t i2s_read(void) { return pio_sm_get(pio, sm); }
