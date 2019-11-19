/*
 * Copyright        : General Public Domain.
 * File Description : Globally Included head file.
 * Created          : by oliverzhang@shendongmcu.com
 */

#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <stdio.h>
#include <intrins.h>
//#include <reg52.h>
#include <STC8Axx.h>

/*led.c*/
extern void led_Initialization(void);
extern void led_TurnOn(void);
extern void led_TurnOff(void);
#endif
