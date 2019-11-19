/*
 * Copyright        : Shendong MCU studio
 * File Description : RTC PCF8563T driver
 * Created          : oliverzhang@shendongmcu.com
 */

#include "global.h"

static uint8_t Bcd2Bin(uint8_t bcd)
{
	return ( ((bcd>>4)&0x0f)*10 + (bcd & 0x0f) );
}

static uint8_t Bin2Bcd(uint8_t bin)
{
	return  (bin/10<<4 | bin%10);
}

extern void SET_IIC_SDA(void);
static int Pcf8563WriteBytes(uint8_t sub_addr,const uint8_t *buf,uint8_t len)
{
	uint8_t i, ack;
	
	i2c_StartCondition();
	ack = i2c_SendByteCheckAck(0xa2);
	if((ack&0x80)!=0)
	{
		i2c_StopCondition();
		return -1;
	}
	ack = i2c_SendByteCheckAck(sub_addr);
	if((ack&0x80)!=0)
	{
		i2c_StopCondition();
		return -2;
	}
	for(i=0;i<len;i++)
	{
		ack = i2c_SendByteCheckAck(buf[i]);
		if((ack&0x80)!=0)
		{
			i2c_StopCondition();
			return -3;
		}
	}
	i2c_StopCondition();
	return 0;
}

static int Pcf8563ReadBytes(uint8_t sub_addr,uint8_t *buf,uint8_t len)
{
	uint8_t i, ack;
	i2c_StartCondition();
	ack = i2c_SendByteCheckAck(0xa2);
	if((ack&0x80)!=0)
	{
		i2c_StopCondition();
		return -1;
	}
	ack = i2c_SendByteCheckAck(sub_addr);/*dummy write end here*/
	if((ack&0x80)!=0)
	{
		i2c_StopCondition();
		return -2;
	}
	SET_IIC_SDA();
	time_HoggingDelayUs(6);
	i2c_StartCondition();
	ack = i2c_SendByteCheckAck(0xa3);
	if((ack&0x80)!=0)
	{
		i2c_StopCondition();
		return -3;
	}
	for(i=0;i<len-1;i++)
	{
		buf[i]=i2c_ReceiveByteWithAck();
	}
	buf[i] = i2c_ReceiveByteWithoutAck();
	i2c_StopCondition();
	return 0;
}

int rtc_Initialization(void)
{
	int ret;
	uint8_t xdata regs[16];
	ret = Pcf8563ReadBytes(13,&regs[13],1);
	if(ret < 0) 
	{
		printf("IIC fatal error.PCF8563 init failed.\r\n");
		return -10;
	}
	if((regs[13] & 0x03) != 0x03)
	{
		regs[0]=0x00;/*Clock is running*/
		regs[1]=0x00;/*disable alarm and timer interrupt*/
		regs[2]= 0x30; /*Second:  30*/
		regs[3]= 0x30; /*Minute:  30*/
		regs[4]= 0x12; /*Hour:    12*/ 
		regs[5]= 0x14; /*Day:     14*/   /*2014-04-14 12:30:30 Monday*/
		regs[6]= 0x01; /*Weekday: Monday*/
		regs[7]= 0x04; /*Month:   4*/
		regs[8]= 0x14; /*year:    2014*/
		regs[9]=0x80;/*minute alarm disabed*/
		regs[10]=0x80;/*hour alarm disabed*/
		regs[11]=0x80;/*day alarm disabed*/
		regs[12]=0x80;/*weekday alarm disabed*/
		/*changed to 3 as reset configuration indication.*/
		regs[13]=0x03;/*CLKOUT is disabled and set to high-impedance*/
		regs[14]=0x00;/*counterdown timer is disabled */
		regs[15]=0x00;/*counterdown timer is disabled */
		ret = Pcf8563WriteBytes(0x00,&regs[0],16);
	}
	if(ret != 0) printf("Init 8563 error:ret = %d\r\n",ret);
	return ret;
}

int rtc_GetCalendar(struct human_time *ptr)
{
	//int i;
	int ret;
	uint8_t xdata regs[16];
	ret = Pcf8563ReadBytes(0x00,regs,16);
	//for(i=0;i<16;i++) printf(" 0x%x",regs[i]); printf("\r\n");
	if(ret != 0)
	{
		printf("Read 8563 error:ret = %d\r\n",ret);
		return ret;
	}
	if(regs[2] & 0x80)
	{
		printf("PCF8563 very low power.");
		return -10;
	}
	if(ptr == NULL)
	{
		printf("parameter error..");
		return -11;
	}
	ptr->second=Bcd2Bin(regs[2] & 0x7f );
	ptr->minute=Bcd2Bin(regs[3] & 0x7f );
	ptr->hour=Bcd2Bin(regs[4] & 0x3f);
	ptr->day=Bcd2Bin(regs[5] & 0x3f);
	ptr->month=Bcd2Bin(regs[7] & 0x1f);
	ptr->year=Bcd2Bin(regs[8]) + 2000;
	return 0;
}

int rtc_SetCalendar(const struct human_time *pcal)
{
	int ret;
	uint8_t xdata regs[9];
	if(pcal == NULL)
	{
		printf("param error.");
		return -10;
	}
	regs[0]=0x00;/*Clock is running*/
	regs[1]=0x00;/*disable alarm and timer interrupt*/
	regs[2]=Bin2Bcd(pcal->second);
	regs[3]=Bin2Bcd(pcal->minute);
	regs[4]=Bin2Bcd(pcal->hour);
	regs[5]=Bin2Bcd(pcal->day);
	regs[6]= 0 ; /*weekday is not implemented */
	regs[7]=Bin2Bcd(pcal->month);
	regs[8]=Bin2Bcd(pcal->year-2000);
	ret = Pcf8563WriteBytes(0x00,regs,9);
	if(ret != 0) printf("Write 8563 error:ret = %d\r\n",ret);
	return ret;
}
static uint8_t IsLeapYear(uint16_t year)
{
	return ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0);
}
/*2000-1-1 is Saturday. calc weekday of first day of each year.*/
static uint8_t GetFirstWeekDayOfYear(uint16_t year)
{
	uint16_t i;
	uint32_t total_day = (year - 2000) * 365;

	for (i = 2001; i < year; i++)
	{
		/*Consider leap years.*/
		total_day += IsLeapYear(i);
	}
	return ((total_day + 6) % 7);
}

/*Calc out weekday of calendar day.*/
uint8_t rtc_GetWeekdayOfDay(uint16_t year, uint8_t month, uint8_t day)
{
	uint32_t days = 0;
	days = GetFirstWeekDayOfYear(year);
	switch (month - 1)
	{
	case 12:
		days += 31;
	case 11:
		days += 30;
	case 10:
		days += 31;
	case 9:
		days += 30;
	case 8:
		days += 31;
	case 7:
		days += 31;
	case 6:
		days += 30;
	case 5:
		days += 31;
	case 4:
		days += 30;
	case 3:
		days += 31;
	case 2:
		if (IsLeapYear(year))
			days += 29;
		else
			days += 28;
	case 1:
		days += 31;
	}
	days += day;
	return (days % 7);
}
#if 0
/*Get total days of a given month.*/
uint8_t rtc_GetMonthDays(uint16_t year, uint8_t month)
{
	switch (month)
	{
	case 1:
	case 3:
	case 5:
	case 7:
	case 8:
	case 10:
	case 12:
		return 31;
	case 4:
	case 6:
	case 9:
	case 11:
		return 30;
	case 2:
		if (IsLeapYear(year))
			return 29;
		else
			return 28;
	}
	return 0;
}
#endif

void rtc_TestMain(unsigned char argc, char *argv[])
{
	struct human_time ht;
	char c;
	unsigned char ret;
	
	if(argc == 1) goto _print_usage_;

	if(strcmp(argv[1],"get") == 0)
	{
		printf("press any key to abort...\r\n");
		for(;;)
		{
			time_HoggingDelayMs(1000);
			rtc_GetCalendar(&ht);
			printf("%u-%bu-%bu ",ht.year,ht.month,ht.day);
			printf("WEEK %bu ",rtc_GetWeekdayOfDay(ht.year,ht.month,ht.day));
			printf("%bu:%bu:%bu \r\n",ht.hour,ht.minute,ht.second);
			ret = uart1_Read(&c,1);
			if(ret == 1) break;
		}
		return;
	}
	else if(strcmp(argv[1],"set") == 0)
	{
		int ret;
		if(argc != 4) goto _print_usage_;
		ret = sscanf(argv[2],"%u-%bu-%bu",&ht.year,&ht.month,&ht.day);
		if(ret != 3)  goto _print_usage_;
		ret = sscanf(argv[3],"%bu:%bu:%bu",&ht.hour,&ht.minute,&ht.second);
		if(ret != 3)  goto _print_usage_;
		rtc_SetCalendar(&ht);
		printf("Set Calendar OK\r\n");
		return;
	}
_print_usage_:
	printf("----------------usage----------------------\r\n");
	printf("%s get\r\n----Get current calendar time.\r\n",argv[0]);
	printf("%s set 2016-8-3 12:30:45\r\n----Set calendar time.\r\n",argv[0]);
}
