/*
 * Copyright        : Shendong MCU studio
 * File Description : Console Task
 * Created          : oliverzhang@shendongmcu.com
 */
#include "global.h"

static void ProcessTestEEPROM(unsigned char argc, char *argv[])
{
	char xdata buf[20];
	if(argc != 2)
	{
		printf("Usage: %s string-ro-write\r\n",argv[0]);
		return;
	}
	printf("write \"%s\" to address 0x111\r\n");
	eeprom_Write(argv[1],strlen(argv[1]),0x111);
	memset(buf,0,sizeof(buf));
	printf("then read it back and checking:\r\n");
	eeprom_Read(buf,sizeof(buf)-1,0x111);
	printf("%s\r\n",buf);
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

	if (strcmp(argv[0], "test-eeprom") == 0)
	{
		ProcessTestEEPROM(argc,argv);
	}
	else if (strcmp(argv[0], "?") == 0 || 0 == strcmp(argv[0], "help"))
	{
		printf("Support Command List:\r\n");
		printf("test-eeprom: test eeprom read and write function.\r\n");
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
