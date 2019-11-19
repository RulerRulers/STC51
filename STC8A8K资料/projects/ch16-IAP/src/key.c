/*
 * Copyright        : ShenDong MCU studio.
 * Description      : keyboard driver  
 * Created          : by oliverzhang@shendongmcu.com
 */

#include "global.h"

sbit LINE_F1 = P4^0;
sbit LINE_F2 = P4^1;
sbit LINE_F3 = P7^5;


static unsigned char ScanKeyCode(void)
{
	/*TEST_KEY on the head board.*/
	if(LINE_F1 == 0) return KEY_SET;
	if(LINE_F2 == 0) return KEY_MINUS;
	if(LINE_F3 == 0) return KEY_PLUS;
	/*No key pressed, return key_none.*/
	return KEY_NONE;
}

void key_Initialization(void)
{
	LINE_F1 = 1;
	LINE_F2 = 1;
	LINE_F3 = 1;
}

void key_Task(void)
{
	unsigned char key_code;
	unsigned long ticks = time_GetTicks();
	static unsigned long idata oldticks;
	static unsigned char idata old_code,state = 0;
	switch(state)
	{
	case 0:
		old_code = ScanKeyCode();
		if(old_code == KEY_NONE) break;
		oldticks = ticks;
		state = 1;
		break;
	case 1: /*de-bounce*/
		if(ticks - oldticks < 20) break;
		key_code = ScanKeyCode();
		if(key_code == old_code)
		{
			key_InsertKeyCode(key_code, KEY_PRESSED);
			oldticks = ticks;
			state = 2;
		}
		else
		{
			state = 0;
		}
		break;
	case 2: /*Check for repeat*/
		key_code = ScanKeyCode();
		if(key_code == KEY_NONE) 
		{
			oldticks = ticks;
			state = 3;
		}
		else
		{
			if(ticks - oldticks > 250)
			{
				key_InsertKeyCode(old_code, KEY_REPEATED);
				oldticks = ticks;
			}
		}
		break;
	case 3: /*check for release*/
		if(ticks - oldticks < 20) break;
		key_code = ScanKeyCode();
		if(key_code == KEY_NONE)
		{
			key_InsertKeyCode(old_code, KEY_RELEASED);
			state = 0;
		}
		else 
		{
			oldticks = ticks;
		}
		break;
	default:
		break;
	}
}

#define KEY_FIFOLEN	4
static struct _key_fifo_tag_{
	unsigned char keycode[KEY_FIFOLEN];
	unsigned char state[KEY_FIFOLEN];
	unsigned char rear;
	unsigned char top;
} idata g_key_fifo;

unsigned char key_GetKeyCode(unsigned char *pcode,unsigned char *pstate)
{
	unsigned char rear = g_key_fifo.rear;
	if(rear != g_key_fifo.top)
	{
		if(pcode != NULL)*pcode = g_key_fifo.keycode[rear];
		if(pstate!= NULL)*pstate = g_key_fifo.state[rear];
		rear++;
		if(rear >= KEY_FIFOLEN) rear = 0;
		g_key_fifo.rear = rear;
		return 1;
	}
	return 0;
}

unsigned char key_InsertKeyCode(unsigned char keycode, unsigned char state)
{
	unsigned char top,next;

	//printf("insert key: code = %bu state=%bu\r\n",keycode,state);
	top = g_key_fifo.top;
	next = g_key_fifo.top + 1;
	if(next >= KEY_FIFOLEN) next = 0;
	if(next != g_key_fifo.rear)
	{
		g_key_fifo.keycode[top] = keycode;
		g_key_fifo.state[top] = state;
		g_key_fifo.top = next;
		return 1;
	}
	return 0;
}

