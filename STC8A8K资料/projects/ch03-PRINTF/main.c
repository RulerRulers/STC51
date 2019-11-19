/*
 * Copyright        : Shendong mcu studio
 * File Description : Main Cotntrol Loop.
 * Created          : by oliverzhang@shendongmcu.com
 */

#include "global.h"

static void InitSTC8A(void)
{
	P0M1 = 0x00;
	P0M0 = 0x00;
	P1M1 = 0x00;
	P1M0 = 0x00;
	P2M1 = 0x00;
	P2M0 = 0x00;
	P3M1 = 0x00;
	P3M0 = 0x00;
	P4M1 = 0x00;
	P4M0 = 0x00;
	P5M1 = 0x00;
	P5M0 = 0x00;
	P6M1 = 0x00;
	P6M0 = 0x00;
	P7M1 = 0x00;
	P7M0 = 0x00;
}

void main(void)
{
	unsigned int i;
	InitSTC8A();
	led_Initialization();
	time_HoggingDelayMs(1000);
	uart1_Initialization();
	printf("Hello world.\r\n");
	for(i=0;;i++)
	{
		led_TurnOn();
		printf("LED ON: count %u\r\n",i);
		time_HoggingDelayMs(500);
		led_TurnOff();
		printf("LED OFF: count %u\r\n",i);
		time_HoggingDelayMs(500);
	}
}
