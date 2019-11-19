/*
 * Copyright        : Shendong mcu studio
 * File Description : Time related function.
 * Created          : by oliverzhang@shendongmcu.com
 */

#include "global.h"

static void Delay1ms()//STC8A@22.1184MHz
{
	unsigned char i, j;

	i = 29;
	j = 183;
	do
	{
		while (--j);
	} while (--i);
}


void time_HoggingDelayMs(unsigned int ms)
{
	for(;ms;ms--)
	{
		Delay1ms();
	}
}

