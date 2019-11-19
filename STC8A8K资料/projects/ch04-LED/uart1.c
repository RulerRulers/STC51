/*
 * Copyright        : shendong MCU studio
 * File Description : UART1 driver
 * Created          : by oliverzhang@shendongmcu.com
 */

#include "global.h"

void uart1_Initialization(void)
{
	//__enable_xfr();
	//P3PU |= 0x02u;
	//__disable_xfr();
	/*Default setting: Timer2 as baud rate generator for UART1.  */
	T2L = (65536ul - 4);
	T2H = (65536ul - 4) >> 8;/*22118400/12/4/115200 = 4 */
	AUXR |= 0x11u; /*Start timer2. Timer2 as timer. 12T mode.*/
	SCON = 0x50u;/*UART1 at mode 1:8-N-1 115200 , Enable Receiver.*/
	//ES = 1; /*Enable UART1 interrupt.*/
}


/*implemented for printf() */
char putchar(char c)  
{
	SBUF = c;
	while (TI == 0);
	TI = 0;
	return c;
}
