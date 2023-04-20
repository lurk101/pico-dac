#include <stdio.h>
#include <stdlib.h>
//
#include <pico/multicore.h>
#include <pico/stdlib.h>
#include <pico/time.h>
//
#include <hardware/clocks.h>
//
#include "i2s.h"
#include "pdm.h"

static const uint32_t PDM_LEFT_GPIO = 14;
static const uint32_t PDM_RIGHT_GPIO = 15;
static const uint32_t PDM_SM_CLK_DIV = 90;
static const uint32_t I2S_DATA = 17;
static const uint32_t I2S_LR_CLK = 18;
static const uint32_t I2S_CLK = 26;

int main() {
    // set system clock to 127 MHz, this will give
    // divided by 90 gives 1.4111111 MHz PCM clk (44100 * 32 = 1,411,200)
    stdio_init_all();
    getchar_timeout_us(250000);
    // clear the screen on VT terminal
    printf("\033[H\033[JPico I2S PDM Stereo DAC\n");
    uint32_t sys_clk = clock_get_hz(clk_sys);
    printf("System clock %f MHz\n", sys_clk / 1.0e6);

    printf("Starting PCM TX\n");
    pdm_data pdm_l, pdm_r;
    pdm_begin(&pdm_l, &pdm_r, PDM_LEFT_GPIO, PDM_RIGHT_GPIO, PDM_SM_CLK_DIV);
    uint32_t r, l, t = time_us_32();
    for (int i = 0; i < 44100; i++) {
        multicore_fifo_push_blocking(0);
        r = pdm_o4_os32_df2(&pdm_r, 0);
        l = multicore_fifo_pop_blocking();
        while (pio_sm_is_tx_fifo_full(pio, pdm_r.sm))
            ;
        pio_sm_put(pio, pdm_r.sm, r);
        while (pio_sm_is_tx_fifo_full(pio, pdm_l.sm))
            ;
        pio_sm_put(pio, pdm_l.sm, l);
    }
    while (!pio_sm_is_tx_fifo_empty(pio, pdm_l.sm))
        ;
    while (!pio_sm_is_tx_fifo_empty(pio, pdm_r.sm))
        ;
    t = time_us_32() - t;
    printf("- PCM sync test, elapsed %f seconds, error %.3f%%\n", t / 1000000.0,
           (t - 1000000) / 10000.0);
    printf("- Ideal PCM freq. %d Hz, actual %d Hz\n", 44100 * 32, sys_clk / PDM_SM_CLK_DIV);

    printf("Starting I2S RX\n");
    i2s_begin(I2S_CLK, I2S_LR_CLK, I2S_DATA);
    printf("Ready\n");
    for (;;)
        if (!pio_sm_is_tx_fifo_full(pio, pdm_r.sm) && !pio_sm_is_tx_fifo_full(pio, pdm_l.sm)) {
            uint32_t s = stereo;
            multicore_fifo_push_blocking((int16_t)(s >> 16));
            r = pdm_o4_os32_df2(&pdm_r, (int16_t)s);
            l = multicore_fifo_pop_blocking();
            pio_sm_put(pio, pdm_r.sm, r);
            pio_sm_put(pio, pdm_l.sm, l);
        }
}
