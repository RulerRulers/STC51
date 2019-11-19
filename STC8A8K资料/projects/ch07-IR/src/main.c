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

void ProcessKey(void)
{
	static unsigned char idata flash_mode = 0;
	unsigned char ret,keycode,keystate;
	
	ret = key_GetKeyCode(&keycode, &keystate);
	if(ret == 0) return;
	switch(keycode)
	{
		case KEY_F1: printf("KEY_F1");break;
		case KEY_F2: printf("KEY_F2");break;
		case KEY_F3: printf("KEY_F3");break;
		case KEY_F4: printf("KEY_F4");break;
		case KEY_1: printf("KEY_1");break;
		case KEY_2: printf("KEY_2");break;
		case KEY_3: printf("KEY_3");break;
		case KEY_4: printf("KEY_4");break;
		case KEY_5: printf("KEY_5");break;
		case KEY_6: printf("KEY_6");break;
		case KEY_7: printf("KEY_7");break;
		case KEY_8: printf("KEY_8");break;
		case KEY_9: printf("KEY_9");break;
		case KEY_0: printf("KEY_0");break;
		case KEY_ASTERISK: printf("KEY_ASTERISK");break;
		case KEY_POUND: printf("KEY_POUND");break;
		case KEY_LEFT: printf("KEY_LEFT");break;
		case KEY_UP: printf("KEY_UP");break;
		case KEY_DOWN: printf("KEY_DOWN");break;
		case KEY_RIGHT: printf("KEY_RIGHT");break;
		case KEY_SET: printf("KEY_SET");break;
		case KEY_MENU: printf("KEY_MENU");break;
		case KEY_MUTE: printf("KEY_MUTE");break;
		case KEY_TVAV: printf("KEY_TVAV");break;
		case KEY_DIGIT: printf("KEY_DIGIT");break;
		case KEY_AUTO: printf("KEY_AUTO");break;
		case KEY_OK: printf("KEY_OK");break;
		case KEY_POWER: printf("KEY_POWER");break;
		case KEY_PLUS: printf("KEY_PLUS");break;
		case KEY_MINUS: printf("KEY_MINUS");break;
		case KEY_RETURN: printf("KEY_RETURN");break;
		case KEY_CLEAR: printf("KEY_CLEAR");break;
		case KEY_CHANGEPIC: printf("KEY_CHANGEPIC");break;
		case KEY_EXIT: printf("KEY_EXIT");break;
		case KEY_PREVPAGE: printf("KEY_PREVPAGE");break;
		case KEY_NEXTPAGE: printf("KEY_NEXTPAGE");break;
		case KEY_CHANNELLIST: printf("KEY_CHANNELLIST");break;
		case KEY_PROGRAMGUIDE: printf("KEY_PROGRAMGUIDE");break;
		case KEY_PROGRAMINFO: printf("KEY_PROGRAMINFO");break;
		case KEY_INFOSERVICE: printf("KEY_INFOSERVICE");break;
		case KEY_RED: printf("KEY_RED");break;
		case KEY_GREEN: printf("KEY_GREEN");break;
		case KEY_YELLOW: printf("KEY_YELLOW");break;
		case KEY_BLUE: printf("KEY_BLUE");break;
		default: printf("unknown key");break;
	}
	printf("  ");
	switch(keystate)
	{
		case KEY_PRESSED: printf("~~pressed.");break;
		case KEY_REPEATED:printf("<>repeated.");break;
		case KEY_RELEASED:printf("**released.");break;
		default: printf("impossible to run here.");break;
	}
	printf("\r\n");
	if(keycode == KEY_SET && keystate == KEY_PRESSED)
	{
		flash_mode++;
		if(flash_mode > 2) flash_mode = 0;
		led_SetFlashMode(flash_mode);
	}
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
	printf("Hello world.\r\n");
	EA = 1;
	for(;;)
	{
		tick_Task();
		ir_Task();
		led_Task();
		key_Task();
		ProcessKey();
	}
}
