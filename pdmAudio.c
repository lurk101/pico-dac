#include <string.h>

#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "pico/multicore.h"
//
#include "pdmAudio.h"

static const int32_t pos_error_04 = 32767 * 3 * 2;
static const int32_t vmin_04 = -32767 * 3;

static volatile uint32_t scratch;

static uint32_t o2_os32(struct pdmAudio* pdm, int16_t sig) {
    uint32_t out = 0;
    int32_t d = -32767 - sig;
    int32_t etmp;
    for (int j = 0; j < 32; j++) {
        etmp = d + 2 * pdm->buff[0] - pdm->buff[1];
        pdm->buff[1] = pdm->buff[0];
        pdm->buff[0] = etmp;
        // checks if current error minises sum of squares error
        // if not it changes the deltas and errors.
        if (etmp < 0) {
            pdm->buff[0] += 65534;
            out += (1 << j);
        }
    }
    return out;
}

uint32_t o4_os32_df2(struct pdmAudio* pdm, int16_t sig) {
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
    struct pdmAudio* pdm = (struct pdmAudio*)multicore_fifo_pop_blocking();
    uint32_t a = 0;
    // startup pop suppression
    for (int i = -32767; i < 0; i++) {
        a = o2_os32(pdm, i);
        // while (pio_sm_is_tx_fifo_full(pio, sm)) {}
        // pio->txf[sm] = a;
    }
    multicore_fifo_push_blocking(0);

    while (1) {
        // if fifo empty modulate the previous value to keep voltage constant
        //  helps prevents clicks and pops I think

        // if (pio_sm_is_tx_fifo_empty(pio, sm))
        //     pio->txf[sm] = a;

        // if other core sends value
        if (multicore_fifo_rvalid())
            multicore_fifo_push_blocking(o4_os32_df2(pdm, multicore_fifo_pop_blocking()));
    }
}

void pdmAudio_begin(struct pdmAudio* pdm_l, struct pdmAudio* pdm_r, uint32_t pl, uint32_t pr) {
    memset(pdm_l, 0, sizeof(struct pdmAudio));
    memset(pdm_r, 0, sizeof(struct pdmAudio));

    multicore_launch_core1(core1_worker);
    multicore_fifo_push_blocking((uint32_t)&pdm_l);
    // wait for core 1 ready
    multicore_fifo_pop_blocking();
}
