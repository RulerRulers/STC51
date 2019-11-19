/*
 * Copyright        : Shendong MCU studio
 * File Description : Main Menu dialog
 * Created          : oliverzhang@shendongmcu.com
 */
#include "../global.h"

static uint8_t idata g_con_active = 0;

static const struct gui_control code g_con_list[] = {
	{CON_TXTBUTTON,10,30,"演示红外遥控器"},
	{CON_TXTBUTTON,10,50,"演示ADC功能"},
	{CON_TXTBUTTON,10,70,"演示EEPROM功能"},
	{CON_TXTBUTTON,10,90,"演示RTC功能"},
	{CON_TXTBUTTON,10,130,"上一页"},
	{CON_ENDFLAG,0,0,NULL},
};

void gui_DlgMainMenu2Proc(uint8_t msg_type, uint8_t msg_code)
{
	if(msg_type == WM_TIMER)
	{
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
			/*gui_DlgDesktopOpen();*/
			return;
		}
		else if(msg_code == KEY_OK)
		{
			switch(g_con_active)
			{
				case 0: gui_DlgIrDemoOpen(); break;
				case 1: gui_DlgAdcDemoOpen(); break;
				case 2: gui_DlgEepromDemoOpen(); break;
				case 3: gui_DlgRtcDemoOpen(); break;
				case 4: gui_DlgMainMenuOpen(); break;
				default: break;
			}
			return;
		}
	}
	gui_DlgDefaultProc(msg_type, msg_code);
}

void gui_DlgMainMenu2Open(void)
{
	const struct gui_rect code rect = {5,20,LCD_WIDTH-10,1};
	lcd_ClearScreen(COLOR_BLACK);
	gui_DlgSetActive(DLG_MAINMENU2);
	memset(g_con_param_list,0x00u,sizeof(g_con_param_list));
	//g_con_active = 0;
	lcd_DrawString((LCD_WIDTH-16*3)/2, 0, "主菜单", &g_style_normal);
	lcd_DrawSCBitmap(&rect, COLOR_RED);
	gui_DrawAllControls(g_con_list,g_con_active);
}
