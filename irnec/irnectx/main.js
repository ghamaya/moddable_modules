/*
    ESP32 Remote Control Transceiver
    Sample: transmit IR NEC code
    gpio_pin -> register + IR LED -> GND
*/

import Timer from "timer";
import Irnectx from "irnectx";

const address = 0x2222; //IR NEC code
const command = 0xF20D;

let rmt = new Irnectx({pin: 13, timeout: 300});
// pin: out put gpio pin
// timeout: tx done wait time
//   default: 300ms, 0: ignore wait function, -1: wait unlimited
//   The actual tx done time is 200ms.

Timer.repeat(id => {

    rmt.write(address, command);

}, 1000)
