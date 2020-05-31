#ifndef	LED_H
#define	LED_H 


#include <unistd.h> // usleep()


void init_led(short * address);
void led_clear();
void led_all();
void led_stage();

#endif