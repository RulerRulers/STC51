/*
 * Copyright        : shendong MCU studio
 * File Description : IAP/EEPROM driver
 * Created          : by oliverzhang@shendongmcu.com
 */

#include "global.h"

#define IAP_CMD_IDLE	0
#define IAP_CMD_READ	1
#define IAP_CMD_PROGRAM	2
#define IAP_CMD_ERASE	3
#define IAP_WAIT_CLOCK_SETTING	0x81 /*for 22.1184MHZ*/

static const uint8_t code g_trig_first = 0x5au;
static const uint8_t code g_trig_second = 0xa5u;

static void CheckFail(uint8_t id)
{
	id = IAP_CONTR;
	if(id & 0x10)
	{
		printf("IAP opeartion fail id = %bu\r\n",id);
		IAP_CONTR &= 0xefu;
	}
}

static void IapDisable(void)
{
	IAP_CONTR = 0;
	IAP_CMD = IAP_CMD_IDLE;
	IAP_TRIG = 0;
	IAP_ADDRH =  0xffu;
	IAP_ADDRL = 0xffu;
}

static void IapEraseSector(uint16_t addr)
{
	IAP_ADDRH = addr >> 8;
	IAP_ADDRL = addr;
	IAP_CONTR = IAP_WAIT_CLOCK_SETTING;
	IAP_CMD = IAP_CMD_ERASE;
	IAP_TRIG = g_trig_first;
	IAP_TRIG = g_trig_second;
	_nop_();
	CheckFail(IAP_CMD_ERASE);
	
}

static uint8_t IapReadByte(uint16_t addr)
{
	uint8_t val;
	IAP_ADDRH = addr >> 8;
	IAP_ADDRL = addr;
	IAP_CONTR = IAP_WAIT_CLOCK_SETTING;
	IAP_CMD = IAP_CMD_READ;
	IAP_TRIG = g_trig_first;
	IAP_TRIG = g_trig_second;
	_nop_();
	val = IAP_DATA;
	CheckFail(IAP_CMD_READ);
	return val;
}

static void IapProgramByte(uint16_t addr, uint8_t val)
{
	IAP_ADDRH = addr >> 8;
	IAP_ADDRL = addr;
	IAP_CONTR = IAP_WAIT_CLOCK_SETTING;
	IAP_CMD = IAP_CMD_PROGRAM;
	IAP_DATA = val;
	IAP_TRIG = g_trig_first;
	IAP_TRIG = g_trig_second;
	_nop_();
	CheckFail(IAP_CMD_PROGRAM);
}

void iap_WriteSector(uint16_t sec_start_addr, const void *vbuf, uint16_t len)
{
	uint16_t i;
	const uint8_t *buf = (const uint8_t *)vbuf;
	if((sec_start_addr & 0x1ffu) != 0)
	{
		printf("not sector start addr.\r\n");
		return;
	}
	if(len > 512)
	{
		printf("size too big! trim to 512 byte..\r\n");
		len = 512;
	}
	IapEraseSector(sec_start_addr);
	for(i=0;i<len;i++)
		IapProgramByte(sec_start_addr + i, buf[i]);
	IapDisable();
}

void iap_Read(uint16_t addr, void *vbuf, uint16_t len)
{
	uint16_t i;
	uint8_t *buf = (uint8_t *)vbuf;
	for(i=0;i<len;i++)
		buf[i] = IapReadByte(addr + i);
	IapDisable();
}

#define EEPROM_START_ADDR  0x0000u /*for IAP15W4K61S4*/

void iap_TestMain(uint8_t argc, char *argv[])
{
	char xdata buf[64];
	const char *str="Hello world, Test IAP EEPROM now.";

	argc=argc;
	argv=argv;
	iap_WriteSector(EEPROM_START_ADDR + 0x000u, str, strlen(str)+1);
	iap_Read(EEPROM_START_ADDR + 0x000u,buf,sizeof(buf));
	printf("------------------test iap eeprom----------------\r\n");
	printf("buf[]=\"%s\"\r\n",buf);
	if(strcmp(buf,str)==0) 
	{
		printf("Test OK.\r\n");
	}
	else 
	{
		printf("Test Failed.\r\n");
	}
}
