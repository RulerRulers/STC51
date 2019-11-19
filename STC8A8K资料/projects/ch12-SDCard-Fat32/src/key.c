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
