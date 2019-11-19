/*
 * Copyright        : Shendong MCU studio
 * File Description : ADC driver
 * Created          : oliverzhang@shendongmcu.com
 */

#include "global.h"

void adc_Initialization(void)
{
	/*Init ADC input.*/
	P1M1 |= 0xc0u; /*P1.7 P1.6 as High-Z input for ADC.*/
	P1M0 &= 0x3fu;
	ADCCFG = 0x24u; /*Result right-justified, speed: 160 cpu clock*/
	ADC_CONTR = 0x80; /*ADC module power on*/
}

uint16_t adc_GetChannelValue(uint8_t ch)
{
	uint16_t val;
	ch &= 0xf;
	
	ADC_CONTR = (0xc0 + ch); /*Start sample. and set channel.*/
	_nop_();
	_nop_();
	for(;((ADC_CONTR & 0x20u) == 0););
	val = ADC_RES;
	val <<= 8;
	val |= ADC_RESL;
	ADC_CONTR = 0x80u;
	time_HoggingDelayUs(100);
	return val;
}
