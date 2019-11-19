/*
 * Copyright        : Shendong MCU studio
 * File Description : I2C protocol and EEPROM driver
 * Created          : oliverzhang@shendongmcu.com
 */
#include "global.h"

sbit SCL = P7^7;
sbit SDA = P7^6;

static void SET_IIC_SCL(void)
{
	SCL = 1;
}
static void CLR_IIC_SCL(void)
{
	SCL = 0;
}
/*required by other file: PCF8563 driver.*/
/*static */void SET_IIC_SDA(void)
{
	SDA = 1;
}
static void CLR_IIC_SDA(void)
{
	SDA = 0;
}
static uint8_t IIC_SDA_IS_HIGH(void)
{
	return (SDA == 1);
}
void i2c_StartCondition(void) /*20us */
{
	SET_IIC_SCL();
	SET_IIC_SDA();
	time_HoggingDelayUs(6);
	SET_IIC_SDA();
	time_HoggingDelayUs(6);
	CLR_IIC_SDA();
	time_HoggingDelayUs(6);
}
uint8_t i2c_SendByteCheckAck(uint8_t uc)/*135 us*/
{
	uint8_t i, ack = 0;
	for(i=0;i<8;i++)
	{
		CLR_IIC_SCL();
		time_HoggingDelayUs(1);
		if(uc&0x80)
			SET_IIC_SDA();
		else 
			CLR_IIC_SDA();
		uc<<=1;
		time_HoggingDelayUs(6);
		SET_IIC_SCL();
		time_HoggingDelayUs(6);
	}
	CLR_IIC_SCL();
	time_HoggingDelayUs(1);
	SET_IIC_SDA();
	time_HoggingDelayUs(6);
	SET_IIC_SCL();
	time_HoggingDelayUs(6);
	if(IIC_SDA_IS_HIGH())
	{
		ack|=0x80;
	}
	else 
	{
		ack&=0x7f;
	}
	CLR_IIC_SCL();
	time_HoggingDelayUs(1);
	return ack;
}

uint8_t i2c_ReceiveByteWithAck(void)/*135 us*/
{
	uint8_t i,uc;
	for(i=0;i<8;i++)
	{
		CLR_IIC_SCL();
		time_HoggingDelayUs(1);
		SET_IIC_SDA();
		time_HoggingDelayUs(6);
		SET_IIC_SCL();
		time_HoggingDelayUs(6);
		uc<<=1;
		if(IIC_SDA_IS_HIGH())uc|=0x01;
	}
	CLR_IIC_SCL();
	time_HoggingDelayUs(1);
	CLR_IIC_SDA();
	time_HoggingDelayUs(6);
	SET_IIC_SCL();
	time_HoggingDelayUs(6);

	CLR_IIC_SCL();
	time_HoggingDelayUs(1);
	return uc;
}

uint8_t i2c_ReceiveByteWithoutAck(void)/*135us*/
{
	uint8_t i,uc=0;
	for(i=0;i<8;i++)
	{
		CLR_IIC_SCL();
		time_HoggingDelayUs(1);
		SET_IIC_SDA();
		time_HoggingDelayUs(6);
		SET_IIC_SCL();
		time_HoggingDelayUs(6);
		uc<<=1;
		if(IIC_SDA_IS_HIGH())uc|=0x01;
	}

	CLR_IIC_SCL();
	time_HoggingDelayUs(1);
	SET_IIC_SDA();
	time_HoggingDelayUs(6);
	SET_IIC_SCL();
	time_HoggingDelayUs(6);
	CLR_IIC_SCL();
	time_HoggingDelayUs(1);
	return uc;
}

void i2c_StopCondition(void) /*30us*/
{
	CLR_IIC_SCL();
	time_HoggingDelayUs(1);
	CLR_IIC_SDA();
	time_HoggingDelayUs(6);
	SET_IIC_SCL();
	
	CLR_IIC_SDA();
	time_HoggingDelayUs(6);
	SET_IIC_SDA();
	time_HoggingDelayUs(6);
	
	SET_IIC_SDA();
	SET_IIC_SCL();
	time_HoggingDelayUs(6);
}

int eeprom_Read(void *vbuf, uint16_t len, uint16_t addr)
{
	uint8_t ack;
	uint8_t *buf = (uint8_t *)vbuf;
	uint16_t i;
	if(len ==0) return 0;
	for(i=0;i<10;i++)
	{
		i2c_StartCondition();
		ack =i2c_SendByteCheckAck(0xa0);
		if((ack&0x80)==0) break;
		i2c_StopCondition();
		time_HoggingDelayMs(1);
	}
	if(i != 0)
	{		
		printf("eeprom_Read try %u times.\r\n",i);
	}
	if(i == 10) 
	{
		i2c_StopCondition();
		return -1;
	}
	ack = i2c_SendByteCheckAck(addr>>8);
	if((ack&0x80)!=0)
	{
		i2c_StopCondition();
		return -2;
	}
	ack = i2c_SendByteCheckAck(addr);
	if((ack&0x80)!=0)
	{
		i2c_StopCondition();
		return -3;
	}
	SET_IIC_SDA();
	time_HoggingDelayUs(6);
	/*Restart for read*/
	i2c_StartCondition();
	ack = i2c_SendByteCheckAck(0xa1);
	if((ack&0x80)!=0)
	{
		i2c_StopCondition();
		return -4;
	}
	for(i=0;i<len-1;i++)
	{
		buf[i]=i2c_ReceiveByteWithAck();
	}
	buf[i] = i2c_ReceiveByteWithoutAck();
	i2c_StopCondition();
	return 0;
}

static int PageProgram(uint16_t page,uint16_t offset,const uint8_t *buf, uint16_t len)
{
	uint8_t ack;
	uint16_t i;
	uint16_t addr;
	addr = (page << 5) + offset;

	for(i=0;i<10;i++)
	{
		i2c_StartCondition();
		ack = i2c_SendByteCheckAck(0xa0);
		if((ack&0x80)==0) break;
		i2c_StopCondition();
		time_HoggingDelayMs(1);
	}
	if(i != 0)
	{
		printf("eeprom_Write try %u times.\r\n",i);
	}
	if(i == 10) 
	{
		i2c_StopCondition();
		return -1;
	}
	ack = i2c_SendByteCheckAck(addr>>8);
	if((ack&0x80)!=0)
	{
		i2c_StopCondition();
		return -2;
	}
	ack = i2c_SendByteCheckAck(addr);
	if((ack&0x80)!=0)
	{
		i2c_StopCondition();
		return -3;
	}
	for(i=0;i<len;i++)
	{
		ack = i2c_SendByteCheckAck(buf[i]);
		if((ack&0x80)!=0)
		{
			i2c_StopCondition();
			return -4;
		}
	}
	i2c_StopCondition();
	return 0;
}

int eeprom_Write(const void *vbuf, uint16_t len, uint16_t addr)
{
	const uint8_t *buf = (const uint8_t *)vbuf;
	uint16_t page,startpos;
	uint16_t nleft,nwrite;
	int ret;

	page = addr / 32;
	startpos = addr % 32;
	nleft=len;
	for(;nleft;)
	{
		if(nleft <= 32 - startpos)
			nwrite = nleft;
		else
			nwrite = 32 - startpos;
		ret = PageProgram(page,startpos,buf,nwrite);
		if(ret != 0) return ret;
		buf += nwrite;
		nleft -= nwrite;
		page ++;
		startpos=0;
	}
	return 0;
}

void eeprom_TestMain(unsigned char argc, char *argv[])
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

