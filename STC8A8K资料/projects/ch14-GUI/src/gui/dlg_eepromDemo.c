/*
 * Copyright        : Shendong MCU studio
 * File Description : EEPROM demo dialog
 * Created          : oliverzhang@shendongmcu.com
 */
#include "../global.h"

static uint8_t idata g_con_active = 0;
static const struct gui_control code g_con_list[] = {
	{CON_TXTBUTTON,(LCD_WIDTH-16*2)/2,LCD_HEIGHT-20,"返回"},
	{CON_ENDFLAG,0,0,NULL},
};

static void InitEepromDemo(void)
{
	const struct gui_rect code txt_rect= {5, 40, LCD_WIDTH - 10, LCD_HEIGHT - 80};
	struct dlg_eeprom_demo_param *p = &g_all_dlg.eeprom_demo;

	memset(p, 0 , sizeof(*p));
	p->oldticks = time_GetTicks();
	lcd_DrawSCBitmap(&txt_rect,COLOR_YELLOW);
	p->x = 5;
	p->y = 40;
}

static void TestEeprom(void)
{
	struct dlg_eeprom_demo_param *p = &g_all_dlg.eeprom_demo;
	struct gui_style style = {COLOR_BLUE, COLOR_YELLOW, FONT_16};
	static const char code *str = "Hello world. Now Test EEPROM read/write function.";
	uint32_t ticks = time_GetTicks();
	
	if(ticks - p->oldticks < 200) return;
	p->oldticks = ticks;
	if(p->state == 0)
	{
		lcd_DrawString(p->x,p->y,"开始测试...",&style);
		p->y+= 20;
	}
	else if(p->state == 5)
	{
		eeprom_Write(str,strlen(str)+1,100);
		lcd_DrawString(p->x,p->y,"写入字符串...",&style);
		p->x+= 88;
	}
	else if(p->state == 6)
	{
		//lcd_DrawString(p->x,p->y,".",&style);
		//p->x+= 8;
	}
	else if(p->state == 7)
	{
		//lcd_DrawString(p->x,p->y,".",&style);
		//p->x+= 8;
	}
	else if(p->state == 8)
	{
		lcd_DrawString(p->x,p->y,".",&style);
		p->x+= 8;
	}
	else if(p->state == 9)
	{
		lcd_DrawString(p->x,p->y,".",&style);
		p->x = 5;
		p->y+= 20;
	}
	else if(p->state == 10)
	{
		char xdata buf[64];
		memset(buf,0,sizeof(buf));
		eeprom_Read(buf,sizeof(buf),100);
		buf[sizeof(buf)-1] = 0;
		printf("%s\r\n",buf);
		if(strcmp(buf,str) == 0)
		{
			lcd_DrawString(p->x,p->y,"\xca\xfd据校验成功..",&style);
		}
		else 
		{
			lcd_DrawString(p->x,p->y,"\xca\xfd据校验失败..",&style);
		}
	}
	if(p->state < 20) p->state++;
}

void gui_DlgEepromDemoProc(uint8_t msg_type, uint8_t msg_code)
{
	if(msg_type == WM_TIMER)
	{
		TestEeprom();
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
				case 0: gui_DlgMainMenu2Open(); break;
				default: break;
			}
			return;
		}
	}
	gui_DlgDefaultProc(msg_type, msg_code);
}

void gui_DlgEepromDemoOpen(void)
{
	const struct gui_rect code rect = {5,20,LCD_WIDTH-10,1};
	gui_DlgSetActive(DLG_EEPROMDEMO);
	memset(g_con_param_list,0x00u,sizeof(g_con_param_list));
	lcd_ClearScreen(COLOR_BLACK);
	lcd_DrawString((LCD_WIDTH-16*7)/2,0,"演示EEPROM读写功能",&g_style_normal);
	lcd_DrawSCBitmap(&rect, COLOR_RED);
	gui_DrawAllControls(g_con_list,g_con_active);
	InitEepromDemo();
}
