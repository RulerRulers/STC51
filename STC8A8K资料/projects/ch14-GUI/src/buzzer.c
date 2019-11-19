/*
 * Copyright        : Shendong MCU studio
 * File Description : Buzzer Driver.
 * Created          : by oliverzhang@shendongmcu.com
 */

#include "global.h"
sbit BUZZER = P4^4;

void buz_Initialization(void)
{
	P4M0 |= 0x10u; /*push-pull output*/
	BUZZER = 0;
}

void buz_TurnOn(unsigned char on)
{
	if(on)
	{		
		//BUZZER = 1;
		BUZZER = 0;
	}
	else 
	{
		BUZZER = 0;
	}
}
