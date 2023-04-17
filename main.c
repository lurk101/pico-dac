#include <hardware/clocks.h>
#include <pico/multicore.h>
#include <pico/stdlib.h>
#include <pico/time.h>
#include <stdio.h>
#include <stdlib.h>
//
#include "pdmAudio.h"

volatile uint32_t scratch;

int main() {
    // set system clock to 96 MHz, this will give
    // divided by 34 gives 2.823529 MHz for I2S clk (44100 * 16 * 2 * 2 = 2,822,400)
    // and divided by 68 gives 1.411764 MHz PCM clk (44100 * 32 = 1,411,200)
    stdio_init_all();
    getchar_timeout_us(250000);
    // clear the screen on VT terminal
    printf("\033[H\033[JPico I2S PDM Stereo DAC\n");
    printf("System clock %f MHz\n", clock_get_hz(clk_sys) / 1.0e6);

    struct pdmAudio pdm_l, pdm_r;
    pdmAudio_begin(&pdm_l, &pdm_r, 14, 15);
    uint32_t t = time_us_32();
    for (int i = 0; i < 44100; i++) {
        multicore_fifo_push_blocking(15);
        scratch = o4_os32_df2(&pdm_r, 14);
        scratch = multicore_fifo_pop_blocking();
    }
    t = time_us_32() - t;
    printf("%d\n", t);
}
