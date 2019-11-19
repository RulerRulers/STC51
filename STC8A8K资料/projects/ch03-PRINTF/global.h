/*
 * Copyright        : Shendong MCU studio
 * File Description : Globally Included head file.
 * Created          : oliverzhang@shendongmcu.com
 */

#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <stdio.h>
#include <intrins.h>
#include "STC8AXX.h"

/*led.c*/
extern void led_Initialization(void);
extern void led_TurnOn(void);
extern void led_TurnOff(void);

/*uart1.c*/
extern void uart1_Initialization(void);

/*time.c*/
extern void time_HoggingDelayMs(unsigned int ms);

#endif
