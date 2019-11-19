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

static void Delay1us() //STC8A@22.1184MHz
{
	unsigned char i;

	_nop_();
	i = 5;
	while (--i);
}

static void Delay10us()//STC8A@22.1184MHz
{
	unsigned char i;

	_nop_();
	_nop_();
	i = 71;
	while (--i);
}

static void Delay100us()//STC8A@22.1184MHz
{
	unsigned char i, j;

	i = 3;
	j = 221;
	do
	{
		while (--j);
	} while (--i);
}

void time_HoggingDelayUs(unsigned int us)
{
	for(;us>=100;us-=100) Delay100us();
	for(;us>=10;us-=10) Delay10us();
	for(;us>0;us--) Delay1us();
}

void time_HoggingDelayMs(unsigned int ms)
{
	for(;ms;ms--)
	{
		Delay1ms();
	}
}