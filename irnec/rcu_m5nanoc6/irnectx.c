/*
    Moddable SDK Module: Transmit IR NEC code
    based on: espressif/esp-idf/examples/peripherals/rmt/ir_nec_transceiver
*/

#include "xsmc.h"
#include "mc.xs.h"
#include "driver/rmt_tx.h"
#include "ir_nec_encoder.h"

static rmt_channel_handle_t tx_channel = NULL;
static rmt_encoder_handle_t nec_encoder = NULL;
static rmt_transmit_config_t transmit_config = {};
static int txdone_wait_timeout = 300;
// default:300ms   0: ignore wait function, -1: wait unlimited

void xs_irnectx_destructor(void *data)
{
}

void xs_irnectx(xsMachine *the)
{
    // create RMT TX channel
    rmt_tx_channel_config_t tx_channel_cfg = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = 1000000, // 1MHz resolution, 1 tick = 1us
        .mem_block_symbols = 64, // amount of RMT symbols that the channel can store at a time
        .trans_queue_depth = 4, // number of transactions that allowed to pending in the background
        .gpio_num = 0, // TX GPIO pin
    };

    // set RX IR gpio pin
	xsmcVars(1);
	xsmcGet(xsVar(0), xsArg(0), xsID_pin);
	tx_channel_cfg.gpio_num = xsmcToInteger(xsVar(0));

    // set txdone_wait_timeout
 	if (xsmcHas(xsArg(0), xsID_timeout)) {
		xsmcGet(xsVar(0), xsArg(0), xsID_timeout);
		txdone_wait_timeout = xsmcToInteger(xsVar(0));
    }
   
    // rmt_channel_handle_t tx_channel = NULL;
    rmt_new_tx_channel(&tx_channel_cfg, &tx_channel);

    // modulate carrier to TX channel
    rmt_carrier_config_t carrier_cfg = {
        .duty_cycle = 0.33,
        .frequency_hz = 38000, // 38KHz
    };
    rmt_apply_carrier(tx_channel, &carrier_cfg);

    // rmt_transmit_config_t -this example won't send NEC frames in a loop
    transmit_config.loop_count = 0; // no loop

    // install IR NEC encoder
    ir_nec_encoder_config_t nec_encoder_cfg = {
        .resolution = 1000000,   // 1MHz resolution, 1 tick = 1us
    };
    // rmt_encoder_handle_t
    rmt_new_ir_nec_encoder(&nec_encoder_cfg, &nec_encoder);

    // enable RMT TX channel
    rmt_enable(tx_channel);

}

void xs_irnectx_write(xsMachine *the)
{
    ir_nec_scan_code_t nec_code = {
        .address = 0x0,
        .command = 0x0,
    };

    if (xsmcArgc == 2) {
        nec_code.address = xsmcToInteger(xsArg(0));
        nec_code.command = xsmcToInteger(xsArg(1));
    }

    rmt_transmit(tx_channel, nec_encoder, &nec_code, sizeof(nec_code), &transmit_config);
    if(txdone_wait_timeout != 0) {
        rmt_tx_wait_all_done(tx_channel, txdone_wait_timeout);
    }
}
