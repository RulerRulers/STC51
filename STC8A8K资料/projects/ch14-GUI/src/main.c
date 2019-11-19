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
	InitSTC8A();
	time_HoggingDelayMs(1000);
	time_HoggingDelayUs(1);
	time_Initialization();
	led_Initialization();
	key_Initialization();
	ir_Initialization();
	uart1_Initialization();
	printf("Hello world.\r\n$>");
	rtc_Initialization();
	pfs_Initialization();
	m25p80_Initialization();
	lcd_Initialization();
	adc_Initialization();
	buz_Initialization();
	gui_DlgDesktopOpen();
	
	EA = 1;
	for(;;)
	{
		gui_Task();
		tick_Task();
		ir_Task();
		led_Task();
		key_Task();
		//key_TestMain();
		console_Task();
		pfs_Task();
	}
}
