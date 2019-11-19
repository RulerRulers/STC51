/*
 * Copyright        : Shendong MCU studio
 * File Description : LED Driver.
 * Created          : by oliverzhang@shendongmcu.com
 */

#include "global.h"

sbit LED_RUN = P4^2;

void led_Initialization(void)
{
	LED_RUN = 0;
}

void led_TurnOn(void)
{
	LED_RUN = 0;
}

void led_TurnOff(void)
{
	LED_RUN = 1;
}
