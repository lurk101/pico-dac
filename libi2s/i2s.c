
#include "i2s.h"

#include <hardware/dma.h>

#include "i2s.pio.h"
#include "pdm.h"  // just need the pio instance

static uint32_t sm;

volatile uint32_t stereo;

void i2s_begin(uint32_t clk_pin, uint32_t lr_clk_pin, uint32_t data_pin) {
    uint32_t offset = pio_add_program(pio, &i2s_program);
    sm = pio_claim_unused_sm(pio, true);
    // PIO configuration
    pio_sm_config sc = i2s_program_get_default_config(offset);
    sm_config_set_in_pins(&sc, i2s_DATA);
    pio_gpio_init(pio, i2s_DATA);
    pio_gpio_init(pio, i2s_CLK);
    pio_gpio_init(pio, i2s_LRCLK);
    pio_sm_set_consecutive_pindirs(pio, sm, i2s_DATA, 1, false);
    pio_sm_set_consecutive_pindirs(pio, sm, i2s_CLK, 1, false);
    pio_sm_set_consecutive_pindirs(pio, sm, i2s_LRCLK, 1, false);
    sm_config_set_fifo_join(&sc, PIO_FIFO_JOIN_RX);
    sm_config_set_clkdiv(&sc, 1);
    sm_config_set_in_shift(&sc, false, true, 32);  // shift left, auto push
    pio_sm_init(pio, sm, offset, &sc);
    pio_sm_set_enabled(pio, sm, true);

    int chan0 = dma_claim_unused_channel(true);
    int chan1 = dma_claim_unused_channel(true);
    dma_channel_config c = dma_channel_get_default_config(chan0);
    channel_config_set_read_increment(&c, false);
    channel_config_set_write_increment(&c, false);
    channel_config_set_dreq(&c, pio_get_dreq(pio, sm, false));
    channel_config_set_transfer_data_size(&c, DMA_SIZE_32);
    channel_config_set_bswap(&c, true);
    channel_config_set_chain_to(&c, chan1);
    dma_channel_configure(chan0,          // Channel to be configured
                          &c,             // The configuration we just created
                          &stereo,        // The initial write address
                          &pio->rxf[sm],  // The initial read address
                          1000,           // Number of transfers; in this case each is 4 bytes
                          false           // don't start.
    );
    c = dma_channel_get_default_config(chan1);
    channel_config_set_read_increment(&c, false);
    channel_config_set_write_increment(&c, false);
    channel_config_set_dreq(&c, pio_get_dreq(pio, sm, false));
    channel_config_set_transfer_data_size(&c, DMA_SIZE_32);
    channel_config_set_bswap(&c, true);
    channel_config_set_chain_to(&c, chan0);
    dma_channel_configure(chan1,          // Channel to be configured
                          &c,             // The configuration we just created
                          &stereo,        // The initial write address
                          &pio->rxf[sm],  // The initial read address
                          1000,           // Number of transfers; in this case each is 4 bytes
                          true            // Start immediately.
    );
}

int i2s_ready(void) { return !pio_sm_is_rx_fifo_empty(pio, sm); }

uint32_t i2s_read(void) { return pio_sm_get(pio, sm); }
