/*
 * Copyright        : General Public Domain.
 * File Description : Console engine. 
 * Created          : 2013-07-04 by 13510503508
 */

#include "global.h"

extern void iap_TestMain(uint8_t argc, char *argv[]);

static void ProcessTest(unsigned char argc, char *argv[])
{
	printf("argc = %bu, argv[0]=%s\r\n",argc,argv[0]);
}

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
	if (strcmp(argv[0], "?") == 0 || 0 == strcmp(argv[0], "help"))
	{
		printf("Support Command List:\r\n");
		printf("help or ?: command list\r\n");
		printf("test: just a demo test, you can add your own command here\r\n");
		printf("clear or cls: clear screen\r\n");
		printf("iap: iap eeprom read/write test\r\n");
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
	else if (strcmp(argv[0], "test") == 0)
	{
		ProcessTest(argc,argv);
	}
	else if (strcmp(argv[0], "iap") == 0)
	{
		iap_TestMain(argc,argv);
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
