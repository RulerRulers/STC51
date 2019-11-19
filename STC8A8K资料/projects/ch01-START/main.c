/*
 * Copyright        : General Public Domain.
 * File Description : Main Cotntrol Loop.
 * Created          : by oliverzhang@shendongmcu.com
 */

#include "global.h"

static void Delay1ms() //STC8A@22.1184MHz
{
	unsigned char i, j;

	i = 29;
	j = 183;
	do
	{
		while (--j);
	} while (--i);
}

static void DelayMs(unsigned int ms)
{
	for(;ms;ms--)
	{
		Delay1ms();
	}
}

void main(void)
{
	led_Initialization();
	for(;;)
	{
		led_TurnOn();
		DelayMs(500);
		led_TurnOff();
		DelayMs(500);
	}
}
