#ifndef LED_H
#define LED_H

#include "WS2812B.h"

void LED_init();
void LED_system_start(float a);
void LED_off ();
void LED_go();
void LED_stop();
void LED_left();
void LED_right();

#endif