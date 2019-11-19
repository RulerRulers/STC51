/*
 * Copyright        : Shendong mcu studio
 * File Description : IR driver.
 * Created          : by oliverzhang@shendongmcu.com
 */

#include "global.h"

#if (CFG_SYSFREQ == 11059200UL)
/*12443 -- 13.5ms*/
#define MIN_13_5MS_TICKS	12143
#define MAX_13_5MS_TICKS	12543
/*1037---1.125 ms*/
#define MIN_1_125MS_TICKS	737
#define MAX_1_125MS_TICKS	1137
/*2074---2.25ms*/
#define MIN_2_25MS_TICKS	1774
#define MAX_2_25MS_TICKS	2174
/*10372--11.25 ms*/
#define MIN_11_25MS_TICKS	10072
#define MAX_11_25MS_TICKS	10472
/*12443 -- 13.5ms*/
#define MIN_13_5MS_TICKS	12143
#define MAX_13_5MS_TICKS	12543
/*88848 -- 96.75ms*/
#define MIN_96_75MS_TICKS	88548
#define MAX_96_75MS_TICKS	88948
/*29043 -- 45639 : 31.5ms -- 49.5ms */
#define MIN_31_5MS_TICKS	28743
#define MAX_49_5MS_TICKS	45739

#elif (CFG_SYSFREQ == 22118400UL)

/*12443x2 -- 13.5ms*/
#define MIN_13_5MS_TICKS	(12143*2)
#define MAX_13_5MS_TICKS	(12543*2)
/*1037x2---1.125 ms*/
#define MIN_1_125MS_TICKS	(737*2)
#define MAX_1_125MS_TICKS	(1137*2)
/*2074x2---2.25ms*/
#define MIN_2_25MS_TICKS	(1774*2)
#define MAX_2_25MS_TICKS	(2174*2)
/*10372x2--11.25 ms*/
#define MIN_11_25MS_TICKS	(10072*2)
#define MAX_11_25MS_TICKS	(10472*2)
/*12443x2 -- 13.5ms*/
#define MIN_13_5MS_TICKS	(12143*2)
#define MAX_13_5MS_TICKS	(12543*2)
/*88848x2 -- 96.75ms*/
#define MIN_96_75MS_TICKS	(88548*2)
#define MAX_96_75MS_TICKS	(88948*2)
/*29043x2 -- 45639x2 : 31.5ms -- 49.5ms */
#define MIN_31_5MS_TICKS	(28743*2)
#define MAX_49_5MS_TICKS	(45739*2)
#else
	#error "must have a choice."
#endif

#define IR_BUFLEN 50
typedef struct _ir_param_struct_tag{
	unsigned char head;
	unsigned char rear;
	unsigned char th0[IR_BUFLEN];
	unsigned char tl0[IR_BUFLEN];
	unsigned int timer[IR_BUFLEN];
} ir_param_t;
static volatile ir_param_t xdata g_ir_param;
static const unsigned char code g_enable_debug = 0;

/*IR Input pin initialization.*/
sbit IR_INPUT = P3^2;
void ir_Initialization(void)
{
	IR_INPUT = 1;
	IT0 = 1; /*Negative edge trigger external interrupt.*/
	IE0 = 0;
	EX0 = 1;
}

static unsigned char IrCodeToKeyCodeZhongJiu(unsigned char ir_code)
{
	switch(ir_code)
	{
		case 0x0: return KEY_POWER;break;
		case 0x1: return KEY_MUTE;break;
		case 0x11: return KEY_1;break;
		case 0x12: return KEY_2;break;
		case 0x13: return KEY_3;break;
		case 0x14: return KEY_4;break;
		case 0x15: return KEY_5;break;
		case 0x16: return KEY_6;break;
		case 0x17: return KEY_7;break;
		case 0x18: return KEY_8;break;
		case 0x19: return KEY_9;break;
		case 0x1b: return KEY_TVAV;break;
		case 0x10: return KEY_0;break;
		case 0x1a: return KEY_RETURN;break;
		case 0x2: return KEY_MENU;break;
		case 0x3: return KEY_RETURN;break;
		case 0x5: return KEY_UP;break;
		case 0x7: return KEY_LEFT;break;
		case 0x4: return KEY_OK;break;
		case 0x8: return KEY_RIGHT;break;
		case 0x6: return KEY_DOWN;break;
		case 0x9: return KEY_PREVPAGE;break;
		case 0xa: return KEY_NEXTPAGE;break;
		case 0x20: return KEY_CHANNELLIST;break;
		case 0x21: return KEY_PROGRAMGUIDE;break;
		case 0x22: return KEY_PROGRAMINFO;break;
		case 0x23: return KEY_INFOSERVICE;break;
		case 0x24: return KEY_RED;break;
		case 0x25: return KEY_GREEN;break;
		case 0x26: return KEY_YELLOW;break;
		case 0x27: return KEY_BLUE;break;
		case 0x28: return KEY_F1;break;
		case 0x29: return KEY_F2;break;
		case 0x2a: return KEY_F3;break;
		case 0x2b: return KEY_F4;break;
		default: return KEY_UNKNOWN;break;
	}
	return KEY_NONE;
}

static unsigned char IrCodeToKeyCode(unsigned char ir_code)
{
	switch(ir_code)
	{
		case 0x00: return KEY_POWER;break;
		case 0x01: return KEY_UNKNOWN;break;
		case 0x02: return KEY_MENU;break;
		case 0x04: return KEY_PLUS;break;
		case 0x05: return KEY_UP;break;
		case 0x06: return KEY_RETURN;break;
		case 0x08: return KEY_LEFT;break;
		case 0x09: return KEY_OK;break;
		case 0x0A: return KEY_RIGHT;break;
		case 0x0C: return KEY_MINUS;break;
		case 0x0D: return KEY_DOWN;break;
		case 0x0E: return KEY_0;break;
		case 0x10: return KEY_1;break;
		case 0x11: return KEY_2;break;
		case 0x12: return KEY_3;break;
		case 0x14: return KEY_4;break;
		case 0x15: return KEY_5;break;
		case 0x16: return KEY_6;break;
		case 0x18: return KEY_7;break;
		case 0x19: return KEY_8;break;
		case 0x1A: return KEY_9;break;
		default: return KEY_UNKNOWN;break;
	}
	return KEY_NONE;
}

void ir_Task(void)
{
	static unsigned char idata state;
	static unsigned long idata old_tp;
	static unsigned long idata tp_ticks;
	static unsigned char idata ir_buffer[4];
	static unsigned char idata key_code = KEY_NONE,repeat_counter;
	unsigned long tp;
	unsigned char th0,tl0,rear,i;
	unsigned int timer;
	unsigned char record_ok = 0;
	unsigned long span;

	EA = 0;
	if(g_ir_param.head != g_ir_param.rear)
	{
		rear = g_ir_param.rear;
		if(g_ir_param.head != rear)
		{
			timer = g_ir_param.timer[rear];
			tl0 = g_ir_param.tl0[rear];
			th0 = g_ir_param.th0[rear];
			record_ok = 1;
			rear ++;
			if(rear >= IR_BUFLEN) rear = 0;
			g_ir_param.rear = rear;
		}
	}
	EA =1;
	if(record_ok)
	{
		tp = timer;
		tp <<= 16;
		timer = th0;
		timer <<= 8;
		timer |= tl0;
		tp |= timer;
		tp_ticks = time_GetTicks();
		if(g_enable_debug)printf("tp = %lu\r\n",tp);
	}
	if(state != 0)
	{
		if(time_GetTicks() - tp_ticks < 150) /*>= 108ms with some tolerance*/
		{
			/*Do nothing.*/
		}
		else 
		{
			if(record_ok == 0) /*There MUST not be un-processed records.*/
			{
				if(key_code != KEY_NONE)
				{
					key_InsertKeyCode(key_code, KEY_RELEASED);
				}
				state = 0;
				if(g_enable_debug)printf("return to state 0.\r\n");
			}
		}
	}
	
	if(record_ok == 0) return;
	span = tp - old_tp;
	old_tp = tp;
	if(g_enable_debug)printf("state=%bu,span=%lu\r\n",state,span);
	
	switch(state)
	{
	case 0:
		key_code = KEY_NONE;
		ir_buffer[0]=0;
		ir_buffer[1]=0;
		ir_buffer[2]=0;
		ir_buffer[3]=0;
		state = 1;
		break;
	case 1:
		if(span > MIN_13_5MS_TICKS && span < MAX_13_5MS_TICKS) /*12443 -- 13.5ms*/
		{
			state = 2;
		}
		else
		{
			state = 0;
		}
		break;
	default:
		if(span>MIN_1_125MS_TICKS && span<MAX_1_125MS_TICKS)/*1037---1.125 ms*/
		{
			i=0;
		}
		else if(span>MIN_2_25MS_TICKS && span<MAX_2_25MS_TICKS)/*2074---2.25ms*/
		{
			i=1;
		}
		else
		{
			if(state < 34)
			{
				if(g_enable_debug)printf("error span, not 0 or 1, return to 0.\r\n");
				state = 0;
				break;
			}
		}
		if(state<10)
		{
			if(i==1)
			{
				ir_buffer[0] |= 0x01u<<(state-2);
			}
			state++;
		}
		else if(state<18)
		{
			if(i==1)
			{
				ir_buffer[1] |= 0x01u<<(state-10);
			}
			state++;
		}
		else if(state<26)
		{
			if(i==1)
			{
				ir_buffer[2] |= 0x01u<<(state-18);
			}
			state++;
		}
		else if(state<34)
		{
			if(i==1)
			{
				ir_buffer[3] |= 0x01u<<(state-26);
			}
			state++;
			if(state == 34)
			{
				if(ir_buffer[0] == 32 && ir_buffer[1] == 223)
				{
					if(ir_buffer[2] + ir_buffer[3]==255)
					{
						key_code = IrCodeToKeyCode(ir_buffer[2]);
						key_InsertKeyCode(key_code, KEY_PRESSED);
						if(g_enable_debug)printf("ir_code = 0x%bx\r\n",ir_buffer[2]);
					}
				}
				else if(ir_buffer[0] == 0xcc && ir_buffer[1] == 0x1d) /*Zhong Jiu*/
				{
					if(ir_buffer[2] + ir_buffer[3]==255)
					{
						key_code = IrCodeToKeyCodeZhongJiu(ir_buffer[2]);
						key_InsertKeyCode(key_code, KEY_PRESSED);
						if(g_enable_debug) printf("ir_code = 0x%bx\r\n",ir_buffer[2]);
					}
				}
				else 
				{
					/*for debug purpose only.*/
					key_code = IrCodeToKeyCodeZhongJiu(ir_buffer[2]);
					key_InsertKeyCode(key_code, KEY_PRESSED);
					//printf("ir[0]=0x%02bx\r\n",ir_buffer[0]);
					//printf("ir[1]=0x%02bx\r\n",ir_buffer[1]);
					//printf("ir[2]=0x%02bx\r\n",ir_buffer[2]);
					//printf("ir[3]=0x%02bx\r\n",ir_buffer[3]);
				}
				repeat_counter = 0;
			}
		}
		else if(state==34)
		{
			if(span>MIN_11_25MS_TICKS && span<MAX_11_25MS_TICKS) /*10372--11.25 ms*/
			{
				if(g_enable_debug)printf("repeat\r\n");
				repeat_counter++;
				if(repeat_counter>=4)
				{
					if(key_code != KEY_NONE)key_InsertKeyCode(key_code, KEY_REPEATED);
					repeat_counter=2;
				}
			}
			else if(span > MIN_13_5MS_TICKS && span < MAX_13_5MS_TICKS) /*12443 -- 13.5ms*/
			{
				if(g_enable_debug)printf("restart\r\n");
				if(key_code != KEY_NONE)key_InsertKeyCode(key_code, KEY_RELEASED);
				key_code = KEY_NONE;
				ir_buffer[0]=0;
				ir_buffer[1]=0;
				ir_buffer[2]=0;
				ir_buffer[3]=0;
				state = 2;
			}
			else if(span > MIN_96_75MS_TICKS && span < MAX_96_75MS_TICKS) /*88848 -- 96.75ms*/
			{
				if(g_enable_debug)printf("repeat gap.\r\n");
			}
			else if(span > MIN_31_5MS_TICKS && span < MAX_49_5MS_TICKS) /*29043 -- 45639 : 31.5ms -- 49.5ms */
			{
				if(g_enable_debug)printf("start gap.\r\n");
			}
			else
			{
				if(g_enable_debug)printf("impossible here..\r\n");
			}
		}
		break;
	}
}

/*ISR MUST be kept simple.*/
void eint0_ISR(void) interrupt 0
{
	if( (g_ir_param.head+1) % IR_BUFLEN != g_ir_param.rear )
	{
		extern volatile unsigned int idata g_high_timer;
		TR0 = 0;
		g_ir_param.th0[g_ir_param.head] = TH0;
		g_ir_param.tl0[g_ir_param.head] = TL0;
		g_ir_param.timer[g_ir_param.head] = g_high_timer;
		if(TF0)g_ir_param.timer[g_ir_param.head]++;
		TR0 = 1;
		g_ir_param.head++;
		if(g_ir_param.head >= IR_BUFLEN)
			g_ir_param.head = 0;
	}
}
