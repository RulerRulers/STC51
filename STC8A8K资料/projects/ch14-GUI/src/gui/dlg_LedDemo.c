/*
 * Copyright        : Shendong MCU studio
 * File Description : LED dmeo dialog
 * Created          : oliverzhang@shendongmcu.com
 */
#include "../global.h"

static uint8_t idata g_con_active = 0;
static const struct gui_control code g_con_list[] = {
	{CON_TXTBUTTON,10,50,"模式0"},
	{CON_TXTBUTTON,10,70,"模式1"},
	{CON_TXTBUTTON,10,90,"模式2"},
	{CON_TXTBUTTON,(LCD_WIDTH-16*2)/2,LCD_HEIGHT-20,"返回"},
	{CON_ENDFLAG,0,0,NULL},
};

static void InitLEDDemo(void)
{
	const struct gui_rect code txt_rect= {5, 120, LCD_WIDTH - 10, LCD_HEIGHT - 160};
	struct gui_style style = {COLOR_BLUE, COLOR_YELLOW, FONT_16};

	lcd_DrawSCBitmap(&txt_rect,COLOR_YELLOW);
	switch(g_con_active)
	{
		case 0:
			led_SetFlashMode(0);
			lcd_DrawString(5,100+16,"当前模式为0",&style);
			break;
		case 1:
			led_SetFlashMode(1);
			lcd_DrawString(5,100+16,"当前模式为1",&style);
			break;
		default:
			led_SetFlashMode(2);
			lcd_DrawString(5,100+16,"当前模式为2",&style);
			break;
	}
}

void gui_DlgLedDemoProc(uint8_t msg_type, uint8_t msg_code)
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
			if(g_con_active == 3) 
			{
				gui_DlgMainMenuOpen();
			}
			else 
			{
				InitLEDDemo();
			}
			return;
		}
	}
	gui_DlgDefaultProc(msg_type, msg_code);
}

void gui_DlgLedDemoOpen(void)
{
	const struct gui_rect code rect = {5,20,LCD_WIDTH-10,1};
	gui_DlgSetActive(DLG_LEDDEMO);
	memset(g_con_param_list,0x00u,sizeof(g_con_param_list));
	//g_con_active = 0;
	lcd_ClearScreen(COLOR_BLACK);
	lcd_DrawString((LCD_WIDTH-16*6)/2,0,"演示LED功能",&g_style_normal);
	lcd_DrawSCBitmap(&rect, COLOR_RED);
	gui_DrawAllControls(g_con_list,g_con_active);
	InitLEDDemo();
}
