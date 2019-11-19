/*
 * Copyright        : Shendong mcu studio
 * File Description : Time related function.
 * Created          : by oliverzhang@shendongmcu.com
 */

#include "global.h"

/*System ticks*/
static volatile unsigned long idata g_sys_ticks;
static unsigned int code g_timer0_init_val = 65536 - (22118400UL/12/1000);
/*Timer 0 as system tick timer. 1KHZ */
void time_Initialization(void)
{
	/*Timer0 at 16-bit timer mode.*/
	TMOD = (TMOD & 0xf0u) | 0x01u;
	TL0 = g_timer0_init_val & 0xff;
	TH0 = g_timer0_init_val >> 8; 
	ET0 = 1; /*Enable timer0 interrupt.*/
	TR0 = 1; /*Start running.*/
}

void timer0_ISR(void) interrupt 1
{
	TL0 = g_timer0_init_val & 0xff;
	TH0 = g_timer0_init_val >> 8; 
	g_sys_ticks ++;
}

unsigned long time_GetTicks(void)
{
	unsigned long ticks;
	EA = 0;
	ticks = g_sys_ticks;
	EA = 1;
	return ticks;
}

/*Software delay*/
static void Delay1ms()
{
	unsigned char i, j;

	_nop_();
	_nop_();
	i = 22;
	j = 128;
	do
	{
		while (--j);
	} while (--i);
}
static void Delay1us()
{
	unsigned char i;

	i = 3;
	while (--i);
}
static void Delay10us()
{
	unsigned char i;

	_nop_();
	_nop_();
	_nop_();
	i = 52;
	while (--i);
}
static void Delay100us()
{
	unsigned char i, j;

	i = 3;
	j = 35;
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

void time_HoggingDelayUs(unsigned int us)
{
	for(;us>=100;us-=100) Delay100us();
	for(;us>=10;us-=10) Delay10us();
	for(;us>0;us--) Delay1us();
}
