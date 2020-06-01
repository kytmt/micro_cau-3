#include "led.h"

static short * led;

void init_led(short * address) {
	led = address;
}

void led_clear() {
	*led = (short)~0;
}

void led_all() {
	*led = (short)~0xFF;
}

void led_stage() {
	if (sel.temperature==1){
    *led = (short)~0x03;
	}
	else if (sel.sidemenu==2){
	*led = (short)~0x1B;
	}
	else if (sel.payment==3){
	*led = (short)~0xDB;
	}
}