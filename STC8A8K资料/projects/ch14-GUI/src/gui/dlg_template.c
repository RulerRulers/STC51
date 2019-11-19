/*
 * Copyright        : Shendong MCU studio
 * File Description : Temlate dialog
 * Created          : oliverzhang@shendongmcu.com
 */
#if 0

#include "../global.h"

static uint8_t idata g_con_active = 0;
static const struct gui_control code g_con_list[] = {
	{CON_TXTBUTTON,(LCD_WIDTH-16*2)/2,LCD_HEIGHT-20,"返回"},
	{CON_ENDFLAG,0,0,NULL},
};

void gui_DlgXXXProc(uint8_t msg_type, uint8_t msg_code)
{
	if(msg_type == WM_TIMER)
	{
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
			gui_DlgMainMenuOpen();
			return;
		}
		else if(msg_code == KEY_OK)
		{
			switch(g_con_active)
			{
				case 0: gui_DlgMainMenuOpen(); break;
				default: break;
			}
			return;
		}
	}
	gui_DlgDefaultProc(msg_type, msg_code);
}

void gui_DlgXXXOpen(void)
{
	const struct gui_rect code rect = {5,20,LCD_WIDTH-10,1};
	gui_DlgSetActive(DLG_XXX);
	memset(g_con_param_list,0x00u,sizeof(g_con_param_list));
	lcd_ClearScreen(COLOR_BLACK);
	lcd_DrawString((LCD_WIDTH-16*7)/2,0,"演示模版对话框",&g_style_normal);
	lcd_DrawSCBitmap(&rect, COLOR_RED);
	gui_DrawAllControls(g_con_list,g_con_active);
}


#endif
