/*
 * Copyright        : Shendong MCU studio
 * File Description : Globally Included head file.
 * Created          : oliverzhang@shendongmcu.com
 */

#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <stdio.h>
#include <intrins.h>
#include "stc8axx.h"

/*led.c*/
extern void led_Initialization(void);
extern void led_TurnOn(unsigned char on);
extern void led_Task(void);
extern void led_SetFlashMode(unsigned char mode);

/*uart1.c*/
extern void uart1_Initialization(void);

/*time.c*/
extern void time_Initialization(void);
extern unsigned long time_GetTicks(void);
extern void time_HoggingDelayMs(unsigned int ms);
extern void time_HoggingDelayUs(unsigned int us);

/*key.c*/
enum _key_code_e_{ KEY_SET,KEY_PLUS,KEY_MINUS,KEY_RETURN,
	KEY_F1,KEY_F2,KEY_F3,KEY_F4,
	KEY_1,KEY_2,KEY_3,KEY_4,KEY_5,KEY_6,KEY_7,KEY_8,KEY_9,KEY_0,
	KEY_ASTERISK,KEY_POUND,
	KEY_LEFT,KEY_RIGHT,KEY_UP,KEY_DOWN,
	KEY_UNKNOWN,KEY_NONE};
enum  _key_type_e_ {KEY_PRESSED,KEY_REPEATED,KEY_RELEASED};
extern void key_Initialization(void);
extern void key_Task(void);
extern unsigned char key_InsertKeyCode(unsigned char key_code, unsigned char state);
extern unsigned char key_GetKeyCode(unsigned char *pcode,unsigned char *pstate);

#endif
