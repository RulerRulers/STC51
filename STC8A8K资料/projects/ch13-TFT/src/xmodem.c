/*
 * Copyright        : Shendong MCU studio
 * File Description : xmodem protocol implementation
 * Created          : oliverzhang@shendongmcu.com
 */
#include "global.h"

static uint8_t xdata g_xmodem_crc_mode = 1;

enum{CHAR_SOH=0x01,CHAR_EOT=0x04,CHAR_ACK=0x06,CHAR_NAK=0x15,CHAR_CAN=0x18,CHAR_C=0x43,};

static void WriteFlash(uint32_t addr,uint8_t *buf,uint16_t len)
{
	//uart2_Printf("addr=0x%lu,len=%u\r\n",addr,len);
	m25p80_Write(buf,len,addr);
}

static uint8_t PackIsValid(uint8_t *buf,uint8_t packno)
{
	uint8_t i,sum;
	uint8_t status = 0;
	uint8_t crclow, crchigh;
	uint16_t crc;

	//uart2_Printf("~~~~~~~~~~~~~~~~packno = %bu\r\n",packno);
	if(buf[0] != CHAR_SOH) 
	{
		status = 1;
		goto errexit;
	}
	if(packno != buf[1])
	{
		status = 2;
		//uart2_Printf("PackFIALED: buf[1]=%bu,packno=%bu\r\n",buf[1],packno);
		goto errexit;
	}
	if( ((buf[1] + buf[2])&0xff) != 0xff)
	{
		status = 3;
		//uart2_Printf("PackFIALED: buf[1]=0x%bx,buf[2]=0x%bx\r\n",buf[1],buf[2]);
		goto errexit;
	}

	if(g_xmodem_crc_mode == 0)
	{
		for(i=0,sum=0;i<128;i++)
		{
			sum += buf[3+i];
		}
		if(sum != buf[131])
		{
			status = 4;
			//uart2_Printf("PackFIALED: sum=%bu,buf[131]=bu\r\n",sum,buf[131]);
			goto errexit;
		}
		//uart2_Printf("package OK,packno = %bu buf[1]=%bu\r\n",packno,buf[1]);
		return 1;
	}
	else
	{
		crc = crc_CalcCCITT16NibbleVer(&buf[3],128);
		crchigh = crc>>8;
		crclow = crc;
		if(crchigh == buf[131] && crclow == buf[132])
		{
			//uart2_Printf("package OK,packno = %bu buf[1]=%bu\r\n",packno,buf[1]);
			return 1;
		}
		else
		{
			status = 5;
			//uart2_Printf("PackFIALED: crc=0x%x,buf[131]=0xbx,buf[132]=0xbx\r\n",sum,buf[131],buf[131]);
			goto errexit;
		}
	}
errexit:
	//uart2_Printf("error status = %bu\r\n",status);
	return 0;
}

static void SendAck(uint8_t ack, uint8_t debug_id)
{
	debug_id = debug_id;
	if(ack == CHAR_ACK)
		;//uart2_Printf("send ACK %bu.\r\n",debug_id);
	else if(ack == CHAR_NAK)
		;//uart2_Printf("send NAK %bu.\r\n",debug_id);
	else if(ack == CHAR_CAN)
		;//uart2_Printf("send CAN %bu.\r\n",debug_id);
	uart1_Write(&ack,1);
	time_HoggingDelayMs(20);
}

void xmodem_ReceiveFile(uint32_t addr)
{
	uint8_t state = 0,old_state = 0xff;
	uint8_t xdata buf[134];
	uint8_t count = 0;
	uint32_t oldticks,ticks;
	uint8_t nread;
	uint8_t packno;
	uint8_t retry_count = 0;
	uint8_t packlen;

	if( g_xmodem_crc_mode == 0)
		packlen = 132;
	else 
		packlen = 133;
	
	while(1)
	{
		tick_Task();
		ticks = time_GetTicks();
		if(old_state != state)
		{
			old_state = state;
			//uart2_Printf("--------------------%bd------------------\r\n",old_state);
		}
		switch(state)
		{
		case 0:
			if(g_xmodem_crc_mode==0)
				SendAck(CHAR_NAK,1);
			else 
				SendAck(CHAR_C,1);
			oldticks = ticks;
			state = 1;
			break;
		case 1:
			if(ticks - oldticks < 3*1000ul)
			{
				nread = uart1_Read(buf,1);
				if(nread == 1)
				{
					if(buf[0] == CHAR_SOH)
					{
						oldticks = ticks;
						packno = 1;
						count = 1;
						state = 2;
						break;
					}
				}
				break;
			}
			state = 0;
			break;
		case 2:
			if(ticks - oldticks < 10*1000ul)
			{
				nread = uart1_Read(buf+count,packlen-count);
				count += nread;
				if(count == packlen)
				{
					if(PackIsValid(buf,packno))
					{
						WriteFlash(addr,buf+3,128);
						addr += 128;
						SendAck(CHAR_ACK,2);
						count = 0;
						packno++;
						state = 3;
						retry_count = 0;
						oldticks = ticks;
					}
					else
					{
						retry_count ++;
						SendAck(CHAR_NAK,3);
						state = 1;
						if(retry_count >= 10)
						{
							SendAck(CHAR_CAN,4);
							goto errexit;
						}
					}
					break;
				}
				break;
			}
			SendAck(CHAR_NAK,5);
			retry_count++;
			state = 1;
			if(retry_count >= 10)
			{
				SendAck(CHAR_CAN,6);
				goto errexit;
			}
			break;
		case 3: /*receive next package head.*/
			if(ticks - oldticks < 10*1000ul)
			{
				nread = uart1_Read(buf,1);
				if(nread == 1)
				{
					if(buf[0] == CHAR_SOH)
					{
						oldticks = ticks;
						count = 1;
						state = 4;
						break;
					}
					else if(buf[0] == CHAR_EOT)
					{
						SendAck(CHAR_ACK,7);
						//uart2_Printf("file receive finished ok.\r\n");
						goto errexit;
					}
				}
				break;
			}
			retry_count++;
			if(retry_count == 10)
			{
				SendAck(CHAR_CAN,8);
				goto errexit;
			}
			SendAck(CHAR_NAK,9);
			oldticks = ticks;
			break;
		case 4: /*receive remained part of next package.*/
			if(ticks - oldticks < 10*1000ul)
			{
				nread = uart1_Read(buf+count,packlen-count);
				count += nread;
				if(count == packlen)
				{
					if(PackIsValid(buf,packno))
					{
						WriteFlash(addr,buf+3,128);
						addr += 128;
						SendAck(CHAR_ACK,10);
						count = 0;
						packno++;
						state = 3;
						retry_count = 0;
						oldticks = ticks;
					}
					else
					{
						retry_count ++;
						SendAck(CHAR_NAK,11);
						state = 3;
						if(retry_count >= 10)
						{
							SendAck(CHAR_CAN,12);
							goto errexit;
						}
					}
					break;
				}
				break;
			}
			SendAck(CHAR_NAK,0);
			retry_count++;
			state = 3;
			if(retry_count >= 10)
			{
				SendAck(CHAR_CAN,0);
				goto errexit;
			}
			break;
		default:
			break;
		}
	}
errexit:
	return;
}

void xmodem_SendFile(uint32_t addr,uint32_t len)
{
	uint8_t state = 0,old_state = 0xff;
	uint8_t xdata buf[134];
	uint8_t i,sum,count = 0;
	uint32_t oldticks,ticks;
	uint8_t nwrite,nread;
	uint8_t packno = 1;
	uint8_t retry_count = 0;
	uint8_t packlen = 132;
	uint8_t crc_mode = 0;
	uint16_t crc;
	while(1)
	{
		tick_Task();
		ticks = time_GetTicks();
		if(old_state != state)
		{
			old_state = state;
			//uart2_Printf("--------------------%bd------------------\r\n",old_state);
		}
		switch(state)
		{
		case 0:
			retry_count =  0;
			oldticks = ticks;
			state = 1;
			break;
		case 1:
			if(ticks - oldticks < 10*1000ul)
			{
				nread = uart1_Read(buf,1);
				if(nread == 1)
				{
					if(buf[0] == CHAR_C)
					{
						crc_mode = 1;
						state = 2;
						packlen = 133;
						packno = 1;
					}
					else if(buf[0] ==  CHAR_NAK)
					{
						crc_mode = 0;
						state = 2;
						packlen = 132;
						packno = 1;
					}
					else if(buf[0] ==  CHAR_CAN)
					{
						printf("user cancels communication.\r\n");
						state = 100;
					}
					else
					{
						printf("error communication.\r\n");
						state = 100;
					}
				}
			}
			else 
			{
				oldticks = ticks;
				retry_count++;
				if(retry_count>= 5)
				{
					printf("can not receive response more than 50 second.exit\r\n");
					state = 100;
				}
			}
			break;
		case 2: /*send packets.*/
			if(len == 0)
			{
				SendAck(CHAR_EOT,0);
				retry_count = 0;
				oldticks = ticks;
				state = 4;
			}
			else
			{
				if(len >= 128) nwrite = 128;
				else
				{
					nwrite = len;
					memset(buf,0x1a,sizeof(buf));
				}
				buf[0] = CHAR_SOH;
				buf[1] = packno;
				buf[2] = packno ^ 0xffu;
				m25p80_Read(&buf[3],nwrite,addr);
				if(crc_mode)
				{
					crc = crc_CalcCCITT16NibbleVer(&buf[3],128);
					buf[131] = crc >> 8;
					buf[132] = crc;
				}
				else 
				{
					for(i=0,sum=0;i<128;i++)
					{
						sum += buf[3+i];
					}
					buf[131] = sum;
				}
				uart1_Write(buf,packlen);
				oldticks = ticks;
				retry_count = 10;
				state = 3;
			}
			break;
		case 3: /*check response*/
			if(ticks - oldticks < 10*1000ul)
			{
				nread = uart1_Read(buf,1);
				if(nread == 1)
				{
					if(buf[0] == CHAR_ACK)
					{
						addr += nwrite;
						len -= nwrite;
						packno++;
						state = 2;
					}
					else if(buf[0] ==  CHAR_NAK)
					{
						state = 2;
					}
					else if(buf[0] ==  CHAR_CAN)
					{
						printf("user cancels communication.\r\n");
						state = 100;
					}
					else
					{
						printf("error communication.\r\n");
						state = 100;
					}
				}
			}
			else 
			{
				oldticks = ticks;
				retry_count++;
				if(retry_count>= 5)
				{
					printf("can not receive response more than 50 second.exit\r\n");
					state = 100;
				}
			}
			break;
		case 4: /*Check the last ack.*/
			if(ticks - oldticks < 10*1000ul)
			{
				nread = uart1_Read(buf,1);
				if(nread == 1)
				{
					if(buf[0] == CHAR_ACK)
					{
						state = 100;
					}
					else if(buf[0] ==  CHAR_CAN)
					{
						printf("user cancels communication.\r\n");
						state = 100;
					}
					else
					{
						printf("error communication.\r\n");
						state = 100;
					}
				}
			}
			else 
			{
				oldticks = ticks;
				retry_count++;
				if(retry_count>= 5)
				{
					printf("can not receive response more than 50 second.exit\r\n");
					state = 100;
				}
			}
			break;
		case 100:
			goto errexit;
			break;
		default:
			break;
		}
	}
	errexit:
	;
}

void xmodem_Main(uint8_t argc,char *argv[])
{
	int32_t addr;
	int32_t len;

	if(argc == 1) goto _print_usage_;
	if(strcmp(argv[1],"w") == 0)
	{
		if(argc != 3) goto _print_usage_;
		addr = atol(argv[2]);
		if(addr < 0 || addr > 0xf0000 || (addr & 0xffff)!=0 )goto _print_usage_;
		xmodem_ReceiveFile(addr);
		return;
	}
	else if(strcmp(argv[1],"r") == 0)
	{
		if(argc != 4) goto _print_usage_;
		addr = atol(argv[2]);
		if(addr < 0 || addr > 0xfffff)goto _print_usage_;
		len = atol(argv[3]);
		if(len <= 0)goto _print_usage_;
		xmodem_SendFile(addr,len);
		return;
	}
_print_usage_:
	printf("-----------------usage---------------------\r\n");
	printf("usage: %s w <addr>\r\n",argv[0]);
	printf("to write content to spi flash.\r\n");
	printf("For example: xmodem w 0x10000\r\n");
	printf("addr must be on the boundary of sectors.\r\n");
	
	printf("usage: %s r <addr> <len>\r\n",argv[0]);
	printf("to read content from spi flash.\r\n");
	printf("For example: xmodem r 0x10011 55\r\n");
}
