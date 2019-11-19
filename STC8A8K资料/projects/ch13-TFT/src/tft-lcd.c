/*
 * Copyright        : Shendong MCU studio
 * File Description : TFT LCD module Driver
 * Created          : oliverzhang@shendongmcu.com
 */
#include "global.h"

sbit LCD_CS = P6^5;
sbit LCD_RST = P6^4;
sbit LCD_RS = P6^3;
sbit LCD_BL = P6^2;

#if 0
sbit SPI_MISO = P1^6;
sbit SPI_MOSI = P1^5;
sbit SPI_SCK = P1^7;
static uint8_t spi_ExchangeByte(uint8_t dat)
{
	unsigned char i;
	for(i=0; i<8; i++)			
	{  
		if( (dat&0x80)!=0 ) 
			SPI_MOSI = 1;
		else 
			SPI_MOSI = 0;
		dat <<= 1;
		SPI_SCK = 0;
		SPI_SCK = 0;
		SPI_SCK = 1;			
		SPI_SCK = 1;			
	}
	return 0;
}
#endif

static void WriteCommand(uint8_t val)
{
	LCD_RS = 0;
	LCD_CS = 0;
	spi_ExchangeByte(val);
	LCD_CS = 1;
}
static void WriteData(uint8_t val)
{
	LCD_RS = 1;
	LCD_CS = 0;
	spi_ExchangeByte(val);
	LCD_CS = 1;
}

static void WriteWord(uint16_t val)
{
	LCD_RS = 1;
	LCD_CS = 0;
	spi_ExchangeByte(val>>8);
	spi_ExchangeByte(val);
	LCD_CS = 1;
}

static uint8_t SetWindow(uint16_t x, uint16_t y, uint16_t w, uint16_t h) 
{
	uint8_t xstart,xend,ystart,yend;
	if(w == 0 || h == 0)
	{
		/*remove printf to save limited DATA memory space.*/
		uart1_PrintStr("Set Window : input parameter error.\r\n");
		return 0;
	}
	if(x + w > LCD_WIDTH  || y + h > LCD_HEIGHT)
	{
		uart1_PrintStr("Set Window : beyond bondary.\r\n");
		return 0;
	}
	xstart = x;
	xend = x + w -1;
	ystart = y;
	yend = y + h -1;

	WriteCommand(0x2A);
	WriteData(xstart>>8);
	WriteData(xstart);
	WriteData(xend>>8);
	WriteData(xend);

	WriteCommand(0x2B);
	WriteData(ystart>>8);
	WriteData(ystart);
	WriteData(yend>>8);
	WriteData(yend);
	WriteCommand(0x2C);
	return 1;
}

void lcd_Initialization(void)
{
	printf("LCD init start.......\r\n");

	P6M0 |= 0x04U; /*Set LCD_BL to push-pull state*/
	LCD_BL = 1;
	LCD_CS = 1;
	time_HoggingDelayMs(3);
	LCD_RST = 0;
	time_HoggingDelayMs(20);
	LCD_RST = 1;
	time_HoggingDelayMs(20);

	WriteCommand(0x11); //Sleep out 
	time_HoggingDelayMs (120);//Delay 120ms 
	WriteCommand(0x11); //Exit Sleep 
	time_HoggingDelayMs(120); 
	WriteCommand(0x26); //Set Default Gamma 
	WriteData(0x04); 
	WriteCommand(0xB1); 
	WriteData(0x08); 
	WriteData(0x10); 
	WriteCommand(0xC0); //Set VRH1[4:0] & VC[2:0] for VCI1 & GV
	WriteData(0x02); 
	WriteData(0x00); 
	WriteCommand(0xC1); //Set BT[2:0] for AVDD & VCL & VGH & V
	WriteData(0x02); 
	WriteCommand(0xC5); //Set VMH[6:0] & VML[6:0] for VOMH & V
	WriteData(0x4C); 
	WriteData(0x5E); 
	WriteCommand(0xC7); 
	WriteData(0x00); 
	WriteCommand(0x3a); //Set Color Format 
	WriteData(0x05); 
	WriteCommand(0x2A); //Set Column Address 
	WriteData(0x00); 
	WriteData(0x00); 
	WriteData(0x00); 
	WriteData(0x7F); 
	WriteCommand(0x2B); //Set Page Address 
	WriteData(0x00); 
	WriteData(0x00); 
	WriteData(0x00); 
	WriteData(0x9F); 
	WriteCommand(0x36); //Set Scanning Direction 
	WriteData(0xC8); //C0
	WriteCommand(0xB7); //Set Source Output Direction 
	WriteData(0x00); 
	WriteCommand(0xEC); //Set pumping clock frequence 
	WriteData(0x0C); 
	WriteCommand(0xf2); //Enable Gamma bit 
	WriteData(0x01); 
	WriteCommand(0xE0); 
	WriteData(0x3F);//p1 
	WriteData(0x22);//p2 
	WriteData(0x20);//p3 
	WriteData(0x24);//p4 
	WriteData(0x20);//p5 
	WriteData(0x0C);//p6 
	WriteData(0x4E);//p7 
	WriteData(0xB7);//p8 
	WriteData(0x3C);//p9 
	WriteData(0x19);//p10 
	WriteData(0x22);//p11 
	WriteData(0x1E);//p12 
	WriteData(0x02);//p13 
	WriteData(0x01);//p14 
	WriteData(0x00);//p15 
	WriteCommand(0xE1); 
	WriteData(0x00);//p1 
	WriteData(0x1B);//p2 
	WriteData(0x1F);//p3 
	WriteData(0x0B);//p4 
	WriteData(0x0F);//p5 
	WriteData(0x13);//p6 
	WriteData(0x31);//p7 
	WriteData(0x84);//p8 
	WriteData(0x43);//p9 
	WriteData(0x06);//p10 
	WriteData(0x1D);//p11 
	WriteData(0x21);//p12 
	WriteData(0x3D);//p13 
	WriteData(0x3E);//p14 
	WriteData(0x3F);//p15 
	WriteCommand(0x29);//Display On 
	lcd_ClearScreen(COLOR_BLACK);
}

void lcd_ClearScreen(uint16_t color)
{
	uint32_t i,len = (uint32_t)LCD_WIDTH * LCD_HEIGHT;
	if(0==SetWindow(0,0,LCD_WIDTH,LCD_HEIGHT)) return;
	for (i = 0; i < len; i++) 
	{
		WriteWord(color);
	}
}

void lcd_GetBitmapHeadFromFlash(HEADCOLOR *ptr,uint32_t flash_offset)
{
	if(ptr == NULL) return;
	m25p80_Read(ptr,sizeof(*ptr),flash_offset);
}

void lcd_DrawBitmapFromFlash(uint16_t x,uint16_t y, uint32_t flash_offset) 
{
	HEADCOLOR xdata head;
	uint32_t xdata len;
	uint16_t xdata buf[32];
	uint16_t i,n;

	m25p80_Read(&head,sizeof(head),flash_offset);
	//printf("head.w = %u, head.h=%u\r\n",head.w, head.h);
	len = (uint32_t)head.w * head.h;
	if(0 == SetWindow(x,y,head.w,head.h))return;
	flash_offset += sizeof(head);
	while(len)
	{
		if(len > 32)
			n = 32;
		else 
			n = len;
		m25p80_Read(buf,n<<1,flash_offset);
		for (i = 0; i < n; i++) 
		{
			WriteWord(buf[i]);
		}
		flash_offset += n<<1;
		len -= n;
	}
}

void lcd_GetBitmapHeadFromFile(HEADCOLOR *ptr,const char *pathname)
{
	FRESULT res;
	uint16_t xdata nread;
	
	if(ptr == NULL) return;
	res = pf_open(pathname);
	if(res != FR_OK)
	{
		printf("open file error: %s\r\n",pathname);
		return;
	}
	res = pf_read(ptr,sizeof(*ptr),&nread);
	if(res != FR_OK){printf("read file error: %s\r\n",pathname);return;}
}

void lcd_DrawBitmapFromFile(uint16_t x,uint16_t y, const char *pathname) 
{
	HEADCOLOR xdata head;
	uint32_t xdata len;
	uint16_t xdata buf[64];
	uint16_t xdata i,n,nread;
	FRESULT res;
	
	res = pf_open(pathname);
	if(res != FR_OK)
	{
		printf("open file error: %s\r\n",pathname);
		return;
	}
	res = pf_read(&head,sizeof(head),&nread);
	if(res != FR_OK){printf("read file error: %s\r\n",pathname);return;}
	//printf("head.w = %u, head.h=%u\r\n",head.w, head.h);
	if(head.w > LCD_WIDTH || head.h > LCD_HEIGHT) 
	{
		printf("too large image. exit.\r\n");
		return;
	}
	x = (LCD_WIDTH - head.w) / 2;
	y = (LCD_HEIGHT - head.h) / 2;
	len = (uint32_t)head.w * head.h;
	if(0 == SetWindow(x,y,head.w,head.h))return;
	while(len)
	{
		if(len > 64)
			n = 64;
		else 
			n = len;
		res = pf_read(buf,n<<1,&nread);
		if(res != FR_OK){printf("read file error: %s\r\n",pathname);return;}
		for (i = 0; i < n; i++) 
		{
			WriteWord(buf[i]);
		}
		len -= n;
	}
}

void lcd_DrawSCBitmap(const struct gui_rect *prect, uint16_t color)
{
	uint32_t i, len = (uint32_t)prect->width * prect->height;
	if(0 == SetWindow(prect->x,prect->y,prect->width,prect->height))return;
	for (i = 0; i < len; i++) 
	{
		WriteWord(color);
	}
}

/*Draw ANSI String aux functions*/
static void GetFontBmpFrom16X16(uint8_t *fontbmp,uint16_t loc)
{
#ifdef STORE_FONT16X16_IN_CODE
	memcpy(fontbmp,&g_font_16x16_song[loc][0],32);
#else
	uint32_t offset = loc;
	offset <<= 5;
	m25p80_Read(fontbmp,32,OFFSET_GB2312_SONG_16X16_FON + offset);
#endif
}
static void GetFontBmpFrom24X24(uint8_t *fontbmp, uint16_t loc)
{
#ifdef STORE_FONT24X24_IN_CODE
	memcpy(fontbmp,&g_font_24x24_song[loc][0],72);
#else
	loc = loc;
	memset(fontbmp, 0xff, 72);
#endif
}
static void FontRamWriteLine(uint16_t left,uint16_t top,uint16_t length,uint16_t *linebuf)
{
	uint16_t i;
	if(0 == SetWindow(left,top,length,1));
	for (i = 0; i < length; i++) 
	{
		WriteWord(linebuf[i]);
	}
}
static void En8x16FillOneLine(uint8_t *dot_line,uint16_t bkcolor,uint16_t ftcolor,uint16_t *linebuf)
{
	uint8_t dots,i;

	dots=*dot_line;
	for(i=0;i<8;i++)
	{
		if(dots&0x80)
			linebuf[i]=ftcolor;
		else
			linebuf[i]=bkcolor;
		dots<<=1;
	}
}
static void En12x24FillOneLine(uint8_t *dot_line,uint16_t bkcolor,uint16_t ftcolor,uint16_t *linebuf)
{
	uint8_t dots,i;

	dots=*dot_line++;
	for(i=0;i<8;i++)
	{
		if(dots&0x80)
			linebuf[i]=ftcolor;
		else
			linebuf[i]=bkcolor;
		dots<<=1;
	}
	dots=*dot_line;
	for(i=8;i<12;i++)
	{
		if(dots&0x80)
			linebuf[i]=ftcolor;
		else
			linebuf[i]=bkcolor;
		dots<<=1;
	}
}

static void Ch16x16FillOneLine(uint8_t *dot_line,uint16_t bkcolor,uint16_t ftcolor,uint16_t *linebuf)
{
	uint8_t dots,i;

	dots=*dot_line++;
	for(i=0;i<8;i++)
	{
		if(dots&0x80)
			linebuf[i]=ftcolor;
		else
			linebuf[i]=bkcolor;
		dots<<=1;
	}
	dots=*dot_line++;
	for(i=8;i<16;i++)
	{
		if(dots&0x80)
			linebuf[i]=ftcolor;
		else
			linebuf[i]=bkcolor;
		dots<<=1;
	}
}
static void Ch24x24FillOneLine(uint8_t *dot_line,uint16_t bkcolor,uint16_t ftcolor,uint16_t *linebuf)
{
	uint8_t dots,i;

	dots=*dot_line++;
	for(i=0;i<8;i++)
	{
		if(dots&0x80)
			linebuf[i]=ftcolor;
		else
			linebuf[i]=bkcolor;
		dots<<=1;
	}
	dots=*dot_line++;
	for(i=8;i<16;i++)
	{
		if(dots&0x80)
			linebuf[i]=ftcolor;
		else
			linebuf[i]=bkcolor;
		dots<<=1;
	}
	dots=*dot_line++;
	for(i=16;i<24;i++)
	{
		if(dots&0x80)
			linebuf[i]=ftcolor;
		else
			linebuf[i]=bkcolor;
		dots<<=1;
	}
}

void lcd_DrawString(uint16_t x, uint16_t y, const char* cstr, const struct gui_style *ps) large
{
	uint8_t xdata fontbmp[72];
	uint16_t xdata linebuf[24];
	uint16_t i,ch,area,offset,loc,xbk;
	const uint8_t *strbak;
	const uint8_t *str = (const uint8_t *)cstr;

	xbk = x;
	strbak = str;
	for(;*str;)
	{
		if( (*str & 0x80) == 0)/*English*/
		{
			ch = (*str)&0xff;
			if(ch == '\n')
			{
				if(ps->ftsize == FONT_16) y += 16;
				else if(ps->ftsize == FONT_24) y += 24;
				x = xbk;
				str++;
				continue;
			}
			if(ps->ftsize == FONT_16)
			{
				if(ch < 0x20) ch = 0x20;
				ch -= 0x20;
				//GetFontBmpFrom8X16(fontbmp,ch);
				memcpy(fontbmp,&g_font_8x16_song[ch][0],16);
				for(i=0; i<16; i++)
				{
					En8x16FillOneLine(&fontbmp[i],ps->bgcolor,ps->ftcolor,linebuf);
					FontRamWriteLine(x, y + i, 8, linebuf);
				}
				x += 8;
			}
			else if(ps->ftsize == FONT_24)
			{
				if(ch < 0x20) ch = 0x20;
				ch -= 0x20;
				//GetFontBmpFrom12X24(fontbmp,ch);
				memcpy(fontbmp,&g_font_12x24_song[ch][0],48);
				for(i=0; i<24; i++)
				{
					En12x24FillOneLine(&fontbmp[i*2],ps->bgcolor,ps->ftcolor,linebuf);
					FontRamWriteLine(x, y + i, 12, linebuf);
				}
				x += 12;
			}
			str++;
		}
		else  /*Chinese characters*/
		{
			area = (*str) & 0xff;
			str++;
			offset = (*str) & 0xff;
			//printf("0x%x 0x%x\r\n",area, offset);
			if((offset & 0x80)== 0)
			{
				uart1_PrintStr("Invalid String");
				str++;
				return;
			}
			area -= 0xa1;
			offset -= 0xa1;
			loc = area * 94 + offset;
			if(ps->ftsize == FONT_16)
			{
				GetFontBmpFrom16X16(fontbmp,loc);
				for(i=0;i<16;i++)
				{
					Ch16x16FillOneLine(&fontbmp[2*i],ps->bgcolor,ps->ftcolor,linebuf);
					FontRamWriteLine(x, y + i, 16, linebuf);
				}
				x += 16;
			}
			else if(ps->ftsize == FONT_24)
			{
				GetFontBmpFrom24X24(fontbmp,loc);
				for(i=0;i<24;i++)
				{
					Ch24x24FillOneLine(&fontbmp[3*i],ps->bgcolor,ps->ftcolor,linebuf);
					FontRamWriteLine(x, y + i, 24, linebuf);
				}
				x += 24;
			}
			str++;
		}
	}
}

#if 0
static void PrintCharToLCD(char cc) 
{
	uint8_t c = (uint8_t) cc;
	static uint16_t xdata x = 0, y = 0;
	static unsigned char xdata buf[3]={0,0,0};
	const struct gui_style code s = {COLOR_WHITE, COLOR_BLACK, FONT_16};
	const struct gui_rect code rect = {0,0,LCD_WIDTH,LCD_HEIGHT};
	const uint16_t code xm = LCD_WIDTH/8, ym = LCD_HEIGHT/16;
	
	if((c & 0x80u) == 0)
	{
		if(c == '\r') return;
		if(c == '\n')
		{
			x = 0;
			y+= 1;
			if(y> ym-1)
			{
				lcd_DrawSCBitmap(&rect, s.bgcolor);
				x = 0;
				y = 0;
			}
			return;
		}
		buf[0] = c;
		buf[1] = '\0';
		lcd_DrawString(x<<3, y<<4, buf, &s);
		memset(buf,0,sizeof(buf));
		x+=1;
		if(x> xm-2)
		{
			x = 0;
			y += 1;
			if(y> ym -1)
			{
				lcd_DrawSCBitmap(&rect, s.bgcolor);
				y = 0;
			}
		}
	}
	else 
	{
		if(buf[0] == 0)
		{
			buf[0] = c;
			return;
		}
		buf[1] = c;
		buf[2] = '\0';
		lcd_DrawString(x<<3,y<<4,buf,&s);
		memset(buf,0,sizeof(buf));
		x+= 2;
		if(x> xm-2)
		{
			x = 0;
			y += 1;
			if(y> ym -1)
			{
				lcd_DrawSCBitmap(&rect, s.bgcolor);
				y = 0;
			}
		}
	}
}

void lcd_PrintString(const char *str)
{
	for(;*str;str++)
	{
		PrintCharToLCD(*str);
	}
}
#endif

#if 1
void lcd_TestMain(void)
{
	static uint8_t xdata state;
	static uint32_t xdata oldticks;
	const struct gui_style code s = {COLOR_BLACK,COLOR_WHITE, FONT_16};
	const struct gui_rect code rc = {20,20,55,75};
	uint32_t xdata ticks;
	HEADCOLOR xdata head; 
	
	ticks = time_GetTicks();
	if(ticks - oldticks < 3*1000) return;
	oldticks = ticks;
	switch(state)
	{
	case 0:
		printf("display string and single-color-bitmap\r\n");
		lcd_ClearScreen(COLOR_BLACK);
		lcd_DrawString(0,0,"TEST",&s);
		lcd_DrawSCBitmap(&rc, COLOR_YELLOW);
		break;
	case 1:
		printf("display GIRL1\r\n");
		lcd_ClearScreen(COLOR_BLACK);
		lcd_GetBitmapHeadFromFlash(&head,OFFSET_GIRL1_BIN);
		if(head.w > LCD_WIDTH || head.h > LCD_HEIGHT)
			lcd_DrawString(10,40,"Image size too large.",&s);
		else 
			lcd_DrawBitmapFromFlash((LCD_WIDTH-head.w)/2,(LCD_HEIGHT-head.h)/2,OFFSET_GIRL1_BIN);
		break;
	case 2:
		printf("display GIRL2\r\n");
		lcd_GetBitmapHeadFromFlash(&head,OFFSET_GIRL2_BIN);
		if(head.w > LCD_WIDTH || head.h > LCD_HEIGHT)
			lcd_DrawString(10,40,"Image size too large.",&s);
		else 
			lcd_DrawBitmapFromFlash((LCD_WIDTH-head.w)/2,(LCD_HEIGHT-head.h)/2,OFFSET_GIRL2_BIN);
		break;
	case 3:
		printf("display GIRL3\r\n");
		lcd_GetBitmapHeadFromFlash(&head,OFFSET_GIRL3_BIN);
		if(head.w > LCD_WIDTH || head.h > LCD_HEIGHT)
			lcd_DrawString(10,40,"Image size too large.",&s);
		else 
			lcd_DrawBitmapFromFlash((LCD_WIDTH-head.w)/2,(LCD_HEIGHT-head.h)/2,OFFSET_GIRL3_BIN);
		break;
	case 4:
		printf("display GIRL4\r\n");
		lcd_GetBitmapHeadFromFlash(&head,OFFSET_GIRL4_BIN);
		if(head.w > LCD_WIDTH || head.h > LCD_HEIGHT)
			lcd_DrawString(10,40,"Image size too large.",&s);
		else 
			lcd_DrawBitmapFromFlash((LCD_WIDTH-head.w)/2,(LCD_HEIGHT-head.h)/2,OFFSET_GIRL4_BIN);
		break;
	case 5:
		printf("display GIRL5\r\n");
		lcd_GetBitmapHeadFromFlash(&head,OFFSET_GIRL5_BIN);
		if(head.w > LCD_WIDTH || head.h > LCD_HEIGHT)
			lcd_DrawString(10,40,"Image size too large.",&s);
		else 
			lcd_DrawBitmapFromFlash((LCD_WIDTH-head.w)/2,(LCD_HEIGHT-head.h)/2,OFFSET_GIRL5_BIN);
		break;
	case 6:
		printf("display DESKTOP\r\n");
		lcd_GetBitmapHeadFromFlash(&head,OFFSET_DESKTOP_BIN);
		if(head.w > LCD_WIDTH || head.h > LCD_HEIGHT)
			lcd_DrawString(10,40,"Image size too large.",&s);
		else 
			lcd_DrawBitmapFromFlash((LCD_WIDTH-head.w)/2,(LCD_HEIGHT-head.h)/2,OFFSET_DESKTOP_BIN);
		break;
	case 7:
		printf("display /girl/128-1.bin\r\n");
		lcd_GetBitmapHeadFromFile(&head,"/girl/128-1.bin");
		if(head.w > LCD_WIDTH || head.h > LCD_HEIGHT)
			lcd_DrawString(10,40,"Image size too large.",&s);
		else 
			lcd_DrawBitmapFromFile((LCD_WIDTH-head.w)/2,(LCD_HEIGHT-head.h)/2,"/girl/128-1.bin");
		break;	
	default:
		break;
	}
	state++;
	if(state > 7) state = 0;
}
#endif
