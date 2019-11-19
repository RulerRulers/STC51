/*
 * Copyright        : Shendong MCU studio
 * File Description : Console Task
 * Created          : oliverzhang@shendongmcu.com
 */
#include "global.h"

extern void eeprom_TestMain(unsigned char argc, char *argv[]);
extern void rtc_TestMain(unsigned char argc, char *argv[]);
extern void xmodem_Main(uint8_t argc,char *argv[]);

#define MAX_ARG_NUM 5
extern char *strsep(char **s, const char *ct);
static void ProcessOneLine(char xdata *cmd_line)
{
	unsigned char argc;
	static char * xdata argv[MAX_ARG_NUM];
	char *pline;
	char *ret;
	for (argc = 0, pline = cmd_line;;)
	{
		ret = strsep(&pline, " \t\n\r");
		if (ret == NULL) break;
		if (strcmp(ret, "") == 0) continue;
		argv[argc] = ret;
		argc++;
		if (argc >= MAX_ARG_NUM) break;
	}
	if (argc == 0) return;

	if (strcmp(argv[0], "test-eeprom") == 0)
	{
		eeprom_TestMain(argc,argv);
	}
	else if(strcmp(argv[0], "test-rtc") == 0)
	{
		rtc_TestMain(argc,argv);
	}
	else if(strcmp(argv[0], "test-m25p80") == 0)
	{
		m25p80_TestMain(argc,argv);
	}
	else if(strcmp(argv[0], "xmodem") == 0)
	{
		xmodem_Main(argc,argv);
	}
	else if (strcmp(argv[0], "?") == 0 || 0 == strcmp(argv[0], "help"))
	{
		printf("Support Command List:\r\n");
		printf("test-eeprom: test eeprom read and write function.\r\n");
		printf("test-rtc: test RTC read and write function.\r\n");
		printf("test-m25p80: m25p80 test function.\r\n");
		printf("xmodem: transmit file to flash or dump file from flash through xmodem protocol.\r\n");
		printf("help or ?: command list\r\n");
		printf("clear or cls: clear screen\r\n");
	}
	else if (strcmp(argv[0], "clear") == 0 || 0 == strcmp(argv[0], "cls"))
	{
		unsigned char i;
		for (i = 0; i < 30; i++)
		{
			printf("\r\n");
		}
		return;
	}
	else 
	{
		printf("command %s not supported.\r\n", argv[0]);
	}
}

void console_Task(void)
{
	char buf[8];
	unsigned char i, ret;
	static char xdata line[80];
	static unsigned char xdata state;

	ret = uart1_Read(buf, sizeof(buf));
	if (ret <= 0) return;
	for (i = 0; i < ret; i++)
	{
		uart1_Write(&buf[i], 1);
		if (state < sizeof(line)-1)
		{
			if (buf[i] != '\b')
			{
				line[state] = buf[i];
				state++;
			}
			else
			{
				if (state>0)state--;
			}
		}
		if (buf[i] == '\n')
		{
			line[state] = '\0';
			ProcessOneLine(line);
			printf("$>");
			state = 0;
		}
	}
}
