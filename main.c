#include <hardware/clocks.h>
#include <pico/stdlib.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    // set system clock to 96 MHz, this will give
    // divided by 34 gives 2.823529 MHz for I2S clk (44100 * 16 * 2 * 2 = 2,822,400)
    // and divided by 68 gives 1.411764 MHz PCM clk (44100 * 32 = 1,411,200)
    stdio_init_all();
    getchar_timeout_us(250000);
    // clear the screen on VT terminal
    printf("\033[H\033[JPico I2S PDM Stereo DAC\n");
    printf("System clock %f MHz\n", clock_get_hz(clk_sys) / 1.0e6);
}
