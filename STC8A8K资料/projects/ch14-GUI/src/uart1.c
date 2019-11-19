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
	ES = 1; /*Enable UART1 interrupt.*/
}

/*implemented for printf() */
char putchar(char c)  
{
	SBUF = c;
	while (TI == 0);
	TI = 0;
	return c;
}

#define UART1_MAXBUFLEN	(140)
#if (UART1_MAXBUFLEN>255)
#error "UART buffer must be less than 256 bytes because of head/rear data size is 8-bit unsigned char."
#error "if xmodem function is enabled, LENGTH must be great than 133 bytes."
#endif
static volatile struct _uart1_param_tag{
	unsigned char head;
	unsigned char rear;
	char buffer[UART1_MAXBUFLEN];
} xdata g_uart1_param;

unsigned char uart1_Read(void *vbuf,unsigned char len)
{
	unsigned char i = 0;
	char *buf = (char *)vbuf;
	for(i=0; i<len; i++)
	{
		EA = 0; 
		if(g_uart1_param.rear == g_uart1_param.head)
		{
			EA = 1;
			break;
		}
		buf[i] = g_uart1_param.buffer[g_uart1_param.rear];
		g_uart1_param.rear ++;
		if(g_uart1_param.rear >= UART1_MAXBUFLEN)
			g_uart1_param.rear = 0;
		EA = 1;
		_nop_();
	}
	return i;
}

void UART1_IRQHandler(void) interrupt 4
{
	unsigned char val;
	if(RI)
	{
		val = SBUF;
		RI = 0;
		if( (g_uart1_param.head+1) % UART1_MAXBUFLEN == g_uart1_param.rear )
		{
			/*TODO: Add error handling here.*/
			//debug_Assert(0,DPFL,"UART0:buffer full.");
		}
		else
		{
			g_uart1_param.buffer[g_uart1_param.head] = val;
			g_uart1_param.head++;
			if(g_uart1_param.head >= UART1_MAXBUFLEN)
				g_uart1_param.head = 0;
		}
	}
}

void uart1_Write(const void *vbuf, unsigned char len)
{
	unsigned char i;
	const char *buf = (const char *)vbuf;
	for(i=0;i<len;i++)
	{
		SBUF = buf[i];
		while(TI == 0);
		TI = 0;
	}
}
void uart1_PrintStr(const char *str)
{
	for(;*str;str++)
	{
		SBUF = *str;
		while(TI == 0);
		TI = 0;
	}
}
#if 0
void uart1_PrintStr2(const char *str1,const char *str2)
{
	for(;*str1;str1++)
	{
		SBUF = *str1;
		while(TI == 0);
		TI = 0;
	}
	for(;*str2;str2++)
	{
		SBUF = *str2;
		while(TI == 0);
		TI = 0;
	}
}
#endif
void uart1_PrintStr3(const char *str1,const char *str2,const char *str3)
{
	for(;*str1;str1++)
	{
		SBUF = *str1;
		while(TI == 0);
		TI = 0;
	}
	for(;*str2;str2++)
	{
		SBUF = *str2;
		while(TI == 0);
		TI = 0;
	}
	for(;*str3;str3++)
	{
		SBUF = *str3;
		while(TI == 0);
		TI = 0;
	}
}

