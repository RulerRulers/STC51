/*
 * Copyright        : Shendong MCU studio
 * File Description : Keyboard demo dialog
 * Created          : oliverzhang@shendongmcu.com
 */
#include "../global.h"

static uint8_t idata g_con_active = 0;
static const struct gui_control code g_con_list[] = {
	{CON_TXTBUTTON,(LCD_WIDTH-16*2)/2,LCD_HEIGHT-20,"返回"},
	{CON_ENDFLAG,0,0,NULL},
};

static void InitKeyDemo(void)
{
	const struct gui_rect code txt_rect= {5, 40, LCD_WIDTH - 10, LCD_HEIGHT - 80};
	struct dlg_key_demo_param *p = &g_all_dlg.key_demo;
	
	lcd_DrawSCBitmap(&txt_rect,COLOR_YELLOW);
	memset(p, 0 , sizeof(*p));
	p->oldticks = time_GetTicks();
	p->x = 5;
	p->y = 40;
}

static void DumpKeyInfo(uint8_t msg_type, uint8_t msg_code)
{
	const struct gui_rect code txt_rect= {5, 40, LCD_WIDTH - 10, LCD_HEIGHT - 80};
	const struct gui_style code style = {COLOR_BLUE, COLOR_YELLOW, FONT_16};
	
	struct dlg_key_demo_param *p = &g_all_dlg.key_demo;

	lcd_DrawString(p->x, p->y, gui_GetMsgCodeString(msg_code),&style);
	p->x += 8*9;
	lcd_DrawString(p->x, p->y, gui_GetMsgTypeString(msg_type),&style);
	p->y+= 20;
	p->x = 5;
	if(p->y >= LCD_HEIGHT - 40)
	{
		p->y = 40;
		lcd_DrawSCBitmap(&txt_rect,COLOR_YELLOW);
	}
}

void gui_DlgkeyDemoProc(uint8_t msg_type, uint8_t msg_code)
{
	if(msg_type == WM_KEYPRESSED || msg_type == WM_KEYRELEASED || msg_type == WM_KEYREPEATED)
	{
		DumpKeyInfo(msg_type, msg_code);
	}
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

void gui_DlgKeyDemoOpen(void)
{
	const struct gui_rect code rect = {5,20,LCD_WIDTH-10,1};
	gui_DlgSetActive(DLG_KEYDEMO);
	memset(g_con_param_list,0x00u,sizeof(g_con_param_list));
	//g_con_active = 0;
	lcd_ClearScreen(COLOR_BLACK);
	lcd_DrawString((LCD_WIDTH-16*6)/2,0,"演示按键功能",&g_style_normal);
	lcd_DrawSCBitmap(&rect, COLOR_RED);
	gui_DrawAllControls(g_con_list,g_con_active);

	InitKeyDemo();
}
