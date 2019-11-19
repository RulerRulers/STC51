/*
 * Copyright        : Shendong MCU studio
 * File Description : Main Menu dialog
 * Created          : oliverzhang@shendongmcu.com
 */
#include "../global.h"

static uint8_t idata g_con_active = 0;

static const struct gui_control code g_con_list[] = {
	{CON_TXTBUTTON,10,30,"演示Flash读写"},
	{CON_TXTBUTTON,10,50,"演示SD卡读写"},
	{CON_TXTBUTTON,10,70,"演示TFT显示"},
	{CON_TXTBUTTON,10,90,"演示按键"},
	{CON_TXTBUTTON,10,110,"演示LED灯"},
	{CON_TXTBUTTON,10,130,"下一页"},
	{CON_ENDFLAG,0,0,NULL},
};

void gui_DlgMainMenuProc(uint8_t msg_type, uint8_t msg_code)
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
				case 0: gui_DlgFlashDemoOpen(); break;
				case 1: gui_DlgSDCardDemoOpen(); break;
				case 2: gui_DlgTftDemoOpen(); break;
				case 3: gui_DlgKeyDemoOpen(); break;
				case 4: gui_DlgLedDemoOpen(); break;
				case 5: gui_DlgMainMenu2Open(); break;
				default: break;
			}
			return;
		}
	}
	gui_DlgDefaultProc(msg_type, msg_code);
}

void gui_DlgMainMenuOpen(void)
{
	const struct gui_rect code rect = {5,20,LCD_WIDTH-10,1};
	lcd_ClearScreen(COLOR_BLACK);
	gui_DlgSetActive(DLG_MAINMENU);
	memset(g_con_param_list,0x00u,sizeof(g_con_param_list));
	//g_con_active = 0;
	lcd_DrawString((LCD_WIDTH-16*3)/2, 0, "主菜单", &g_style_normal);
	lcd_DrawSCBitmap(&rect, COLOR_RED);
	gui_DrawAllControls(g_con_list,g_con_active);
}
