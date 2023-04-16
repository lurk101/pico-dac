#include <hardware/clocks.h>
#include <pico/stdlib.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    // set system clock to 90.39548 MHz, this will give
    // closest to 44.1KHz * 64 for 32X oversampling.
    // actual ends up at 44.13841797 KHz
    const uint32_t f = 90395480;
    clock_configure(clk_sys, CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLKSRC_CLK_SYS_AUX,
                    CLOCKS_CLK_SYS_CTRL_AUXSRC_VALUE_CLKSRC_PLL_SYS, 125000000, f);
    uint32_t a = clock_get_hz(clk_sys);
    stdio_init_all();
    getchar_timeout_us(250000);
    // clear the screen on VT terminal
    printf("\033[H\033[J");
    if (a != f) {
        printf("clk mismatch\n");
        exit(-1);
    }
    printf("sys clk set to %d Hz.\n", clock_get_hz(clk_sys));
}
