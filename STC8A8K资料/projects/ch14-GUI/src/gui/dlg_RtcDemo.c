/*
 * Copyright        : Shendong MCU studio
 * File Description : I2C module function demo dialog
 * Created          : oliverzhang@shendongmcu.com
 */

#include "../global.h"

static uint8_t idata g_con_active = 0;

static const struct gui_control code g_con_list[] = {
	{CON_TXTBUTTON,(LCD_WIDTH-16*2)/2,LCD_HEIGHT-20,"返回"},
	{CON_ENDFLAG,0,0,NULL},
};

static void InitRtcDemo(void)
{
	struct dlg_rtc_demo_param *p = &g_all_dlg.rtc_demo;
	memset(p, 0 , sizeof(*p));
	p->oldticks = time_GetTicks();
	p->old_second = 60;
}	

#if 1
static void ShowCalendar(void)
{
	struct dlg_rtc_demo_param *p = &g_all_dlg.rtc_demo;
	char xdata buf[64];
	struct human_time xdata ht;
	uint8_t weekday;
	uint32_t ticks;
	const char code * const code tbl[] = {
		"周日 Sunday",
		"周一 Monday",
		"周二 Tuesday",
		"周三 Wednesday",
		"周四 Thursday",
		"周五 Friday",
		"周六 Saturday",
	};		

	ticks = time_GetTicks();
	if(ticks - p->oldticks < 50) return;
	p->oldticks = ticks;
	rtc_GetCalendar(&ht);
	if(p->old_second == ht.second) return;
	p->old_second = ht.second;
	sprintf(buf,"%04u-%02bu-%02bu",ht.year,ht.month,ht.day);
	lcd_DrawString(10,50,buf,&g_style_normal);
	sprintf(buf,"%02bu:%02bu:%02bu",ht.hour,ht.minute,ht.second);
	lcd_DrawString(10,70,buf,&g_style_normal);
	weekday = rtc_GetWeekdayOfDay(ht.year, ht.month, ht.day);
	lcd_DrawString(10,90,tbl[weekday],&g_style_normal);
}
#endif 

void gui_DlgRtcDemoProc(uint8_t msg_type, uint8_t msg_code)
{
	if(msg_type == WM_TIMER)
	{
		ShowCalendar();
		/*Add your code here.*/
	}
	else if(msg_type == WM_KEYPRESSED || msg_type == WM_KEYREPEATED)
	{
		if(msg_code == KEY_UP)
		{
			gui_SearchPrevControl(g_con_list, &g_con_active);
			return;
		}
		else if(msg_code == KEY_DOWN)
		{
			gui_SearchNextControl(g_con_list, &g_con_active);
			return;
		}
		else if(msg_code == KEY_RETURN)
		{
			gui_DlgMainMenu2Open();
			return;
		}
		else if(msg_code == KEY_OK)
		{
			switch(g_con_active)
			{
				//case 0: gui_DlgEepromDemoOpen(); break;
				case 0: gui_DlgMainMenu2Open(); break;
				default: break;
			}
			return;
		}
	}
	gui_DlgDefaultProc(msg_type, msg_code);
}

void gui_DlgRtcDemoOpen(void)
{
	const struct gui_rect code rect = {5,20,LCD_WIDTH-10,1};
	gui_DlgSetActive(DLG_RTCDEMO);
	memset(g_con_param_list,0x00u,sizeof(g_con_param_list));
	lcd_ClearScreen(COLOR_BLACK);
	lcd_DrawString((LCD_WIDTH-16*7)/2,0,"RTC功能演示",&g_style_normal);
	lcd_DrawSCBitmap(&rect, COLOR_RED);
	gui_DrawAllControls(g_con_list,g_con_active);
	InitRtcDemo();
}
