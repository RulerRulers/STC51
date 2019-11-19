/*
 * Copyright        : Shendong MCU studio
 * File Description : TFT display demo dialog
 * Created          : oliverzhang@shendongmcu.com
 */
#include "../global.h"

static uint8_t idata g_con_active = 0;
static const struct gui_control code g_con_list[] = {
	{CON_TXTBUTTON,(LCD_WIDTH-16*2)/2,LCD_HEIGHT-20,"返回"},
	{CON_ENDFLAG,0,0,NULL},
};

static void InitTFTDemo(void)
{
	struct dlg_tft_demo_param *p = &g_all_dlg.tft_demo;
	//const struct gui_style code s = {COLOR_BLACK,COLOR_WHITE, FONT_16};
	memset(p, 0 , sizeof(*p));
	p->oldticks = time_GetTicks();
	//lcd_ClearScreen(COLOR_BLACK);
	//lcd_DrawString(0,40,"按OK键退出",&s);
	
}
static void TestTFTDisplay(void)
{
	struct dlg_tft_demo_param *p = &g_all_dlg.tft_demo;
	const struct gui_style code s = {COLOR_BLACK,COLOR_WHITE, FONT_16};
	const struct gui_rect code rc = {20,70,55,55};
	uint32_t xdata ticks;
	HEADCOLOR xdata head; 
	
	ticks = time_GetTicks();
	if(ticks - p->oldticks < 3*1000) return;
	p->oldticks = ticks;
	switch(p->state)
	{
	case 0:
		lcd_ClearScreen(COLOR_BLACK);
		lcd_DrawString(0,40,"按OK键退出",&s);
		lcd_DrawSCBitmap(&rc, COLOR_YELLOW);
		break;
	case 1:
		lcd_ClearScreen(COLOR_BLACK);
		lcd_GetBitmapHeadFromFlash(&head,OFFSET_GIRL1_BIN);
		if(head.w > LCD_WIDTH || head.h > LCD_HEIGHT)
			lcd_DrawString(0,40,"Size too large.",&s);
		else 
			lcd_DrawBitmapFromFlash((LCD_WIDTH-head.w)/2,(LCD_HEIGHT-head.h)/2,OFFSET_GIRL1_BIN);
		break;
	case 2:
		lcd_GetBitmapHeadFromFlash(&head,OFFSET_GIRL2_BIN);
		if(head.w > LCD_WIDTH || head.h > LCD_HEIGHT)
			lcd_DrawString(0,40,"Size too large.",&s);
		else 
			lcd_DrawBitmapFromFlash((LCD_WIDTH-head.w)/2,(LCD_HEIGHT-head.h)/2,OFFSET_GIRL2_BIN);
		break;
	case 3:
		lcd_GetBitmapHeadFromFlash(&head,OFFSET_GIRL3_BIN);
		if(head.w > LCD_WIDTH || head.h > LCD_HEIGHT)
			lcd_DrawString(0,40,"Size too large.",&s);
		else 
			lcd_DrawBitmapFromFlash((LCD_WIDTH-head.w)/2,(LCD_HEIGHT-head.h)/2,OFFSET_GIRL3_BIN);
		break;
	case 4:
		lcd_GetBitmapHeadFromFlash(&head,OFFSET_GIRL4_BIN);
		if(head.w > LCD_WIDTH || head.h > LCD_HEIGHT)
			lcd_DrawString(0,40,"Size too large.",&s);
		else 
			lcd_DrawBitmapFromFlash((LCD_WIDTH-head.w)/2,(LCD_HEIGHT-head.h)/2,OFFSET_GIRL4_BIN);
		break;
	case 5:
		lcd_GetBitmapHeadFromFlash(&head,OFFSET_GIRL5_BIN);
		if(head.w > LCD_WIDTH || head.h > LCD_HEIGHT)
			lcd_DrawString(0,40,"Size too large.",&s);
		else 
			lcd_DrawBitmapFromFlash((LCD_WIDTH-head.w)/2,(LCD_HEIGHT-head.h)/2,OFFSET_GIRL5_BIN);
		break;
	case 6:
		lcd_GetBitmapHeadFromFlash(&head,OFFSET_DESKTOP_BIN);
		if(head.w > LCD_WIDTH || head.h > LCD_HEIGHT)
			lcd_DrawString(0,40,"Size too large.",&s);
		else 
			lcd_DrawBitmapFromFlash((LCD_WIDTH-head.w)/2,(LCD_HEIGHT-head.h)/2,OFFSET_DESKTOP_BIN);
		break;
	default:
		break;
	}
	p->state++;
	if(p->state > 6) p->state = 0;
}

void gui_DlgTftDemoProc(uint8_t msg_type, uint8_t msg_code)
{
	if(msg_type == WM_TIMER)
	{
		TestTFTDisplay();
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

void gui_DlgTftDemoOpen(void)
{
	const struct gui_rect code rect = {5,20,LCD_WIDTH-10,1};
	gui_DlgSetActive(DLG_TFTDEMO);
	memset(g_con_param_list,0x00u,sizeof(g_con_param_list));
	//g_con_active = 0;
	lcd_ClearScreen(COLOR_BLACK);
	lcd_DrawString((LCD_WIDTH-16*8)/2,0,"演示TFT显示功能",&g_style_normal);
	lcd_DrawSCBitmap(&rect, COLOR_RED);
	gui_DrawAllControls(g_con_list,g_con_active);
	InitTFTDemo();
}
