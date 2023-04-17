#include <stdio.h>
#include <stdlib.h>
//
#include <pico/multicore.h>
#include <pico/stdlib.h>
#include <pico/time.h>
//
#include <hardware/clocks.h>
//
#include "pdm.h"

static const uint32_t PDM_LEFT_GPIO = 14;
static const uint32_t PDM_RIGHT_GPIO = 15;

int main() {
    // set system clock to 96 MHz, this will give
    // divided by 34 gives 2.823529 MHz for I2S clk (44100 * 16 * 2 * 2 = 2,822,400)
    // and divided by 68 gives 1.411764 MHz PCM clk (44100 * 32 = 1,411,200)
    stdio_init_all();
    getchar_timeout_us(250000);
    // clear the screen on VT terminal
    printf("\033[H\033[JPico I2S PDM Stereo DAC\n");
    printf("System clock %f MHz\n", clock_get_hz(clk_sys) / 1.0e6);

    printf("Starting PCM TX\n");
    pdm_data pdm_l, pdm_r;
    pdm_begin(&pdm_l, &pdm_r, PDM_LEFT_GPIO, PDM_RIGHT_GPIO);
    uint32_t t = time_us_32();
    for (int i = 0; i < 44100; i++) {
        multicore_fifo_push_blocking(15);
        uint32_t r = pdm_o4_os32_df2(&pdm_r, 14);
        uint32_t l = multicore_fifo_pop_blocking();
        while (pio_sm_is_tx_fifo_full(pio, pdm_r.sm))
            ;
        pio->txf[pdm_r.sm] = r;
        while (pio_sm_is_tx_fifo_full(pio, pdm_l.sm))
            ;
        pio->txf[pdm_l.sm] = l;
    }
    while (!pio_sm_is_tx_fifo_empty(pio, pdm_l.sm))
        ;
    while (!pio_sm_is_tx_fifo_empty(pio, pdm_r.sm))
        ;
    t = time_us_32() - t;
    printf("PCM sync test, error %.3f%%\n", (1000000 - t) / 10000.0);

    printf("Starting I2S RX\n");
}
