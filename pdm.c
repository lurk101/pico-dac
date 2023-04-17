#include <string.h>
//
#include "pico/multicore.h"
//
#include "hardware/gpio.h"
#include "hardware/pio.h"
//
#include "pdm.h"
//
#include "pdm.pio.h"

static const int32_t pos_error_04 = 32767 * 3 * 2;
static const int32_t vmin_04 = -32767 * 3;

static volatile uint32_t scratch;

static uint32_t o2_os32(struct pdm_data* pdm, int16_t sig) {
    uint32_t out = 0;
    int32_t d = -32767 - sig;
    int32_t etmp;
    for (int j = 0; j < 32; j++) {
        etmp = d + 2 * pdm->buff[0] - pdm->buff[1];
        pdm->buff[1] = pdm->buff[0];
        pdm->buff[0] = etmp;
        // checks if current error minises sum of squares error
        // if not it changes the deltas and errors.
        out <<= 1;
        if (etmp < 0) {
            pdm->buff[0] += 65534;
            out |= 1;
        }
    }
    return out;
}

uint32_t pdm_o4_os32_df2(struct pdm_data* pdm, int16_t sig) {
    uint32_t out = 0;
    int32_t d = vmin_04 - sig;  // vmin_o4  = -32767*3, therefore output is 3.3v/3 = 1.1v pk-pk
    const int32_t test = vmin_04 * 1024;
    for (int j = 0; j < 32; j++) {
        int32_t wn = d + 4 * (pdm->w[0] + pdm->w[2]) - 6 * pdm->w[1] - pdm->w[3];
        // direct form 2 feedback
        int32_t etmp =
            -3035 * pdm->w[0] + 3477 * pdm->w[1] - 1809 * pdm->w[2] + 359 * pdm->w[3] + 1024 * wn;
        // update previous values
        pdm->w[3] = pdm->w[2];
        pdm->w[2] = pdm->w[1];
        pdm->w[1] = pdm->w[0];
        pdm->w[0] = wn;
        // checks if current error minises sum of squares error
        out <<= 1;
        if (etmp < test) {
            pdm->w[0] += pos_error_04;
            out |= 1;
        }
    }
}

static void core1_worker() {
    struct pdm_data* pdm = (struct pdm_data*)multicore_fifo_pop_blocking();
    uint32_t a = 0;
    // startup pop suppression
    for (int i = -32767; i < 0; i++) {
        a = o2_os32(pdm, i);
        while (pio_sm_is_tx_fifo_full(pio, pdm->sm)) {
        }
        pio->txf[pdm->sm] = a;
    }
    multicore_fifo_push_blocking(0);

    for (;;) multicore_fifo_push_blocking(pdm_o4_os32_df2(pdm, multicore_fifo_pop_blocking()));
}

void config_sm(struct pdm_data* pdm, uint32_t pin) {
    uint offset = pio_add_program(pio, &pdm_program);
    pdm->sm = pio_claim_unused_sm(pio, true);
    // PIO configuration
    pio_sm_config c = pdm_program_get_default_config(offset);
    sm_config_set_out_pins(&c, pin, 1);
    pio_gpio_init(pio, pin);
    pio_sm_set_consecutive_pindirs(pio, pdm->sm, pin, 1, true);

    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_TX);
    sm_config_set_clkdiv(&c, 68);
    sm_config_set_out_shift(&c, true, true, 32);
    pio_sm_init(pio, pdm->sm, offset, &c);
    pio_sm_set_enabled(pio, pdm->sm, true);
}

void pdm_begin(struct pdm_data* pdm_l, struct pdm_data* pdm_r, uint32_t pl, uint32_t pr) {
    memset(pdm_l, 0, sizeof(struct pdm_data));
    memset(pdm_r, 0, sizeof(struct pdm_data));
    config_sm(pdm_l, pl);
    config_sm(pdm_r, pr);
    multicore_launch_core1(core1_worker);
    multicore_fifo_push_blocking((uint32_t)&pdm_l);
    // wait for core 1 ready
    multicore_fifo_pop_blocking();
}
