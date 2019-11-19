/*
 * Copyright        : Shendong mcu studio
 * File Description : Main Cotntrol Loop.
 * Created          : by oliverzhang@shendongmcu.com
 */

#include "global.h"

#define POT_CHANNEL 6
#define TEMP_CHANNEL 7

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
	if(keycode == KEY_SET && keystate == KEY_PRESSED)
	{
		flash_mode++;
		if(flash_mode > 2) flash_mode = 0;
		led_SetFlashMode(flash_mode);
	}
}

static void TestPotentiometer(void)
{
	uint16_t val;
	float f;
	val = adc_GetChannelValue(POT_CHANNEL);
	val >>= 2;
	printf("---------------Potentiometer----------------\r\n");
	printf("ADC value: %4u\r\n",val);
	f = 3.3f * (float)val / 1024.f;
	printf("POT voltage: %.1f V\r\n",f);
	printf("\r\n");
}

static void TestTemperature(void)
{
	static const uint16_t code tbl[] = {
		1003, 1002, 1001, 999, 998, 996, 995, 993,
		991, 989, 987, 985, 983, 980, 978, 975,
		973, 970, 967, 964, 961, 957, 954, 950,
		947, 943, 939, 935, 930, 926, 921, 916,
		911, 906, 901, 895, 890, 884, 878, 872,
		866, 859, 852, 846, 838, 831, 824, 816,
		809, 801, 793, 785, 777, 768, 760, 751,
		742, 733, 724, 715, 706, 697, 687, 678,
		668, 658, 649, 639, 629, 620, 610, 600,
		590, 580, 570, 561, 551, 541, 531, 522,
		512, 502, 493, 483, 474, 465, 455, 446,
		437, 428, 420, 411, 402, 394, 385, 377,
		369, 361, 353, 345, 337, 330, 322, 315,
		308, 301, 294, 287, 281, 274, 268, 262,
		255, 250, 244, 238, 232, 227, 221, 216,
		211, 206, 201, 196, 192, 187, 183, 178,
		174, 170, 166, 162, 158, 154, 151, 147,
		144, 140, 137, 134, 131, 127, 124, 121,
		119, 116, 113, 111, 108, 105, 103, 101,
		98, 96, 94, 92, 90, 87, 85, 84,
		82, 80, 78, 76, 75, 73, 71, 70,
		68, 67, 65, 64, 62, 61, 60, 58,
		57, 56, 55, 53, 52,
	};
	int index;
	uint16_t val;
	float res;
	val = adc_GetChannelValue(TEMP_CHANNEL);
	val >>= 2;
	printf("---------------Temperature------------------\r\n");
	printf("ADC value: %4u\r\n",val);
	for(index=0; val < tbl[index] && index < 181 ; index++);
	if(index == 0 || index == 181) 
	{
		printf("Error index.\r\n");
		return;
	}
	res = index - 55;
	res -= ((float)val - (float)tbl[index])/((float)tbl[index-1] - (float)tbl[index]);
	printf("temperature is %.1f ÉãÊÏ¶È\r\n",res);
	printf("\r\n");
}
static void TestAdc(void)
{
	static uint32_t oldticks;
	uint32_t ticks = time_GetTicks();

	if(ticks - oldticks < 3*1000ul)return;
	oldticks = ticks;
	TestPotentiometer();
	TestTemperature();
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
	adc_Initialization();

	EA = 1;
	for(;;)
	{
		tick_Task();
		ir_Task();
		led_Task();
		key_Task();
		ProcessKey();
		console_Task();
		TestAdc();
	}
}
