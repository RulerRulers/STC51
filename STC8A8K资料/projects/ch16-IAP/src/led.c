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

void led_TurnOn(unsigned char on)
{
	if(on)
	{		
		LED_RUN = 0;
	}
	else 
	{
		LED_RUN = 1;
	}
}

static struct _led_mode_param_s_ {
	unsigned int on_span; 
	unsigned int off_span;
	unsigned char mode_changed;
} idata g_led_param = {500,500,0};	
	
void led_SetFlashMode(unsigned char mode)
{
	static unsigned char idata old_mode = 0xff;
	struct _led_mode_param_s_ idata *p = &g_led_param;
	if(mode == 0)
	{
		p->on_span = 500;
		p->off_span = 500;
	}
	else if(mode == 1)
	{
		p->on_span = 200;
		p->off_span = 2800;
	}
	else if(mode == 2)
	{
		p->on_span = 200;
		p->off_span = 800;
	}
	else 
	{
		printf("error param.\r\n");
		return;
	}
	if(old_mode != mode)
	{
		old_mode = mode;
		p->mode_changed = 1;
	}
}

void led_Task(void)
{
	static unsigned long idata oldticks;
	static unsigned char idata state;
	unsigned long ticks;
	struct _led_mode_param_s_ idata *p = &g_led_param;

	if(p->mode_changed)
	{
		state = 0;
		p->mode_changed = 0;
	}
	ticks = time_GetTicks();
	switch (state)
	{
	case 0:
		led_TurnOn(1);
		oldticks = ticks;
		state = 1;
		break;
	case 1:
		if (ticks - oldticks < p->on_span) break;
		oldticks = ticks;
		led_TurnOn(0);
		state = 2;
		break;
	case 2:
		if (ticks - oldticks < p->off_span) break;
		led_TurnOn(1);
		oldticks = ticks;
		state = 1;
		break;
	default:
		break;
	}
}
