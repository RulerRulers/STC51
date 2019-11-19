/*
 * Copyright        : Shendong MCU studio
 * File Description : ADC demo dialog
 * Created          : oliverzhang@shendongmcu.com
 */
#include "../global.h"

static uint8_t idata g_con_active = 0;
static const struct gui_control code g_con_list[] = {
	{CON_TXTBUTTON,(LCD_WIDTH-16*2)/2,LCD_HEIGHT-20,"返回"},
	{CON_ENDFLAG,0,0,NULL},
};

static void InitAdcDemo(void)
{
	const struct gui_rect code txt_rect= {5, 40, LCD_WIDTH - 10, LCD_HEIGHT - 100};
	struct dlg_adc_demo_param *p = &g_all_dlg.adc_demo;

	memset(p, 0 , sizeof(*p));
	p->oldticks = time_GetTicks();
	lcd_DrawSCBitmap(&txt_rect,COLOR_YELLOW);
}

static void UpdateSampleValue(void)
{
	struct dlg_adc_demo_param *p = &g_all_dlg.adc_demo;
	struct gui_style style = {COLOR_BLUE, COLOR_YELLOW, FONT_16};
	uint32_t ticks = time_GetTicks();
	char xdata buf[64];
	//float temp;
	
	if(ticks - p->oldticks < 2000) return;
	p->oldticks = ticks;
	p->x = 5;
	p->y = 40;
	sprintf(buf,"电位器: %4u",adc_GetChannelValue(6));
	lcd_DrawString(p->x,p->y,buf,&style);
	p->y+= 20;
	sprintf(buf,"热敏电阻: %4u",adc_GetChannelValue(7));
	lcd_DrawString(p->x,p->y,buf,&style);
	p->y+= 20;
#if 0
	temp = ds18b20_GetValue() * 0.0625f;
	sprintf(buf,"DS18B20读\xca\xfd: %4.1f",temp);
	lcd_DrawString(p->x,p->y,buf,&style);
#endif
}

void gui_DlgAdcDemoProc(uint8_t msg_type, uint8_t msg_code)
{
	if(msg_type == WM_TIMER)
	{
		UpdateSampleValue();
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
			gui_DlgMainMenuOpen();
			return;
		}
		else if(msg_code == KEY_OK)
		{
			switch(g_con_active)
			{
				case 0: gui_DlgMainMenu2Open(); break;
				default: break;
			}
			return;
		}
	}
	gui_DlgDefaultProc(msg_type, msg_code);
}

void gui_DlgAdcDemoOpen(void)
{
	const struct gui_rect code rect = {5,20,LCD_WIDTH-10,1};
	gui_DlgSetActive(DLG_ADCDEMO);
	memset(g_con_param_list,0x00u,sizeof(g_con_param_list));
	lcd_ClearScreen(COLOR_BLACK);
	lcd_DrawString((LCD_WIDTH-16*7)/2,0,"ADC模块功能",&g_style_normal);
	lcd_DrawSCBitmap(&rect, COLOR_RED);
	gui_DrawAllControls(g_con_list,g_con_active);
	InitAdcDemo();
}
