/*
 * Copyright        : Shendong MCU studio
 * File Description : Globally Included head file.
 * Created          : oliverzhang@shendongmcu.com
 */

#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <stdio.h>
#include <intrins.h>
#include <string.h>
#include <stdlib.h>
#include "stc8axx.h"

#define CFG_SYSFREQ 22118400UL

typedef unsigned char uint8_t;
typedef signed char int8_t;

typedef unsigned short uint16_t;
typedef signed short int16_t;

typedef unsigned long uint32_t;
typedef signed long int32_t;

#define __disable_irq() do{EA = 0;}while(0)
#define __enable_irq() do{EA = 1;}while(0)

/*led.c*/
extern void led_Initialization(void);
extern void led_TurnOn(unsigned char on);
extern void led_Task(void);
extern void led_SetFlashMode(unsigned char mode);

/*uart1.c*/
extern void uart1_Initialization(void);
extern unsigned char uart1_Read(void *vbuf,unsigned char len);
extern void uart1_Write(const void *vbuf, unsigned char len);
extern void uart1_PrintStr(const char *str);
extern void uart1_PrintStr2(const char *str1,const char *str2);
extern void uart1_PrintStr3(const char *str1,const char *str2,const char *str3);

/*console.c*/
extern void console_Task(void);

/*time.c*/
extern void time_Initialization(void);
extern unsigned long time_GetTicks(void);
extern void tick_Task(void);
extern void time_HoggingDelayMs(unsigned int ms);
extern void time_HoggingDelayUs(unsigned int us);

/*key.c*/
enum _key_code_e_{  KEY_ONBD,
	KEY_F1,KEY_F2,KEY_F3,KEY_F4,
	KEY_1,KEY_2,KEY_3,KEY_4,KEY_5,KEY_6,KEY_7,KEY_8,KEY_9,KEY_0,
	KEY_ASTERISK,KEY_POUND,
	KEY_LEFT,KEY_RIGHT,KEY_UP,KEY_DOWN,
	
	KEY_SET,KEY_MENU,KEY_MUTE,KEY_TVAV,KEY_DIGIT,KEY_AUTO,KEY_OK,
	KEY_POWER,KEY_PLUS,KEY_MINUS,KEY_RETURN,KEY_CLEAR,KEY_CHANGEPIC,
	KEY_EXIT,KEY_PREVPAGE,KEY_NEXTPAGE,KEY_CHANNELLIST,
	KEY_PROGRAMGUIDE,KEY_PROGRAMINFO,KEY_INFOSERVICE,
	KEY_RED,KEY_GREEN,KEY_YELLOW,KEY_BLUE,
	
	KEY_UNKNOWN,KEY_NONE};
enum  _key_type_e_ {KEY_PRESSED,KEY_REPEATED,KEY_RELEASED};
extern void key_Initialization(void);
extern void key_Task(void);
extern unsigned char key_InsertKeyCode(unsigned char key_code, unsigned char state);
extern unsigned char key_GetKeyCode(unsigned char *pcode,unsigned char *pstate);

/*ir.c*/
extern void ir_Task(void);
extern void ir_Initialization(void);

/*eeprom.c*/
extern void i2c_StartCondition(void);
extern uint8_t i2c_SendByteCheckAck(uint8_t uc);
extern uint8_t i2c_ReceiveByteWithAck(void);
extern uint8_t i2c_ReceiveByteWithoutAck(void);
extern void i2c_StopCondition(void);
extern int eeprom_Read(void *vbuf, uint16_t len, uint16_t addr);
extern int eeprom_Write(const void *vbuf, uint16_t len, uint16_t addr);

/*pcf8563.c*/
struct human_time {
	uint16_t year;/*1970-*/
	uint8_t month;/*1--12*/
	uint8_t day;/*1--31*/
	uint8_t hour;/*0--23*/
	uint8_t minute;/*0--59*/
	uint8_t second;/*0--59*/
};
extern int rtc_Initialization(void);
extern int rtc_GetCalendar(struct human_time *pcal);
extern int rtc_SetCalendar(const struct human_time *pcal);
extern uint8_t rtc_GetMonthDays(uint16_t year, uint8_t month);
extern uint8_t rtc_GetWeekdayOfDay(uint16_t year, uint8_t month, uint8_t day);

/*m25p80.c*/
extern void m25p80_Initialization(void);
enum {SPI_SPEED_MOSTFAST=0, SPI_SPEED_FAST,SPI_SPEED_MIDDLE,SPI_SPEED_LOW};
extern void spi_Initialization(uint8_t speed);
extern uint8_t spi_ExchangeByte(uint8_t val);
extern void spi_SetSpeed(uint8_t speed);
extern void m25p80_EraseChip(void);
extern void m25p80_Read(void *vbuf, uint16_t len, uint32_t eep_addr);
extern void m25p80_Write(const void *vbuf, uint16_t len, uint32_t eep_addr) large;
extern void m25p80_TestMain(uint8_t argc, char *argv[]);

/*util.c*/
extern uint16_t crc_CalcCCITT16NibbleVer(void *vptr, uint16_t len);
extern const char *util_UintToStr(unsigned int val);

/*petitFS*/
#include "pff/pff.h"
extern void pfs_Task(void);
extern void pfs_Initialization(void);

/*tft-lcd.c*/
#define COLOR_BLACK           0x0000      /*   0,   0,   0 */
#define COLOR_NAVY            0x000F      /*   0,   0, 128 */
#define COLOR_DARKGREEN       0x03E0      /*   0, 128,   0 */
#define COLOR_DARKCYAN        0x03EF      /*   0, 128, 128 */
#define COLOR_MAROON          0x7800      /* 128,   0,   0 */
#define COLOR_PURPLE          0x780F      /* 128,   0, 128 */
#define COLOR_OLIVE           0x7BE0      /* 128, 128,   0 */
#define COLOR_LIGHTGREY       0xC618      /* 192, 192, 192 */
#define COLOR_DARKGREY        0x7BEF      /* 128, 128, 128 */
#define COLOR_BLUE            0x001F      /*   0,   0, 255 */
#define COLOR_GREEN           0x07E0      /*   0, 255,   0 */
#define COLOR_CYAN            0x07FF      /*   0, 255, 255 */
#define COLOR_RED             0xF800      /* 255,   0,   0 */
#define COLOR_MAGENTA         0xF81F      /* 255,   0, 255 */
#define COLOR_YELLOW          0xFFE0      /* 255, 255, 0   */
#define COLOR_WHITE           0xFFFF      /* 255, 255, 255 */

struct gui_rect {
	uint16_t x; 
	uint16_t y;
	uint16_t width;
	uint16_t height;
};
struct gui_style {
	uint16_t ftcolor;
	uint16_t bgcolor;
	uint8_t ftsize; 
};
#define LCD_WIDTH       128
#define LCD_HEIGHT      160
enum {FONT_16, FONT_24, FONT_MAX};
extern void lcd_Initialization(void);
extern void lcd_DrawBitmapFromFlash(uint16_t x, uint16_t y, uint32_t flash_offset);
extern void lcd_DrawBitmapFromFile(uint16_t x, uint16_t y, const char *pathname);
extern void lcd_DrawString(uint16_t x, uint16_t y,const char *str, const struct gui_style *ps) large;
extern void lcd_DrawSCBitmap(const struct gui_rect *prect, uint16_t color);
extern void lcd_PrintString(const char *str);
extern void lcd_ClearScreen(uint16_t color);
typedef struct _HEADCOLOR_
{
   unsigned char scan;
   unsigned char gray;
   unsigned short w;
   unsigned short h;
   unsigned char is565;
   unsigned char rgb;
}HEADCOLOR; 
extern void lcd_GetBitmapHeadFromFile(HEADCOLOR *ptr,const char *pathname);
extern void lcd_GetBitmapHeadFromFlash(HEADCOLOR *ptr,uint32_t flash_offset);

extern const struct gui_style code g_style_normal;
extern const struct gui_style code g_style_reverse;
extern void lcd_TestMain(void);

/*font.c*/
#include "res/flash-offset.h"
//#define STORE_FONT16X16_IN_CODE
//#define STORE_FONT24X24_IN_CODE
extern const char code g_font_8x16_song[][16];
extern const char code g_font_12x24_song[][48];
extern const char code g_font_16x16_song[][32];
extern const char code g_font_24x24_song[][72];

#endif
