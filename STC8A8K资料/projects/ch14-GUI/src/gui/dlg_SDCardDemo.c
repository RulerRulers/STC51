/*
 * Copyright        : Shendong MCU studio
 * File Description : SD Card demo dialog
 * Created          : oliverzhang@shendongmcu.com
 */
#include "../global.h"

static uint8_t idata g_con_active = 0;
static const struct gui_control code g_con_list[] = {
	{CON_TXTBUTTON,(LCD_WIDTH-16*2)/2,LCD_HEIGHT-20,"返回"},
	{CON_ENDFLAG,0,0,NULL},
};

static void InitSDDemo(void)
{
	struct dlg_sd_demo_param *p = &g_all_dlg.sd_demo;
	memset(p, 0 , sizeof(*p));
	p->oldticks = time_GetTicks();
}

static void TestSDCard(void)
{
	const struct gui_style code s = {COLOR_WHITE, COLOR_BLACK, FONT_16};
	struct dlg_sd_demo_param *p = &g_all_dlg.sd_demo;
	uint32_t xdata ticks;
	char xdata pathname[20];
	static DIR xdata dir;
	FILINFO xdata file;
	FRESULT res;
	HEADCOLOR xdata head; 
	
	ticks = time_GetTicks();

	switch(p->state)
	{
	case 0:
		res = pf_opendir(&dir,"/girl");
		if(res == FR_OK)
		{
			lcd_DrawString(0,40,"按OK键退出",&s);
			p->oldticks = ticks;
			p->state = 3;
		}
		else
		{
			printf("Error open dir %s\r\n",pf_GetErrorString(res));
			lcd_DrawString(0,40,"打开文件夹出错",&s);
			p->state = 100;
		}
		break;
	case 1:
		res = pf_readdir(&dir,&file);
		if(res != FR_OK)
		{
			printf("Error read dir %s\r\n",pf_GetErrorString(res));
			lcd_ClearScreen(COLOR_BLACK);
			lcd_DrawString(0,40,"读取文件夹出错",&s);
			p->state = 100;
			break;
		}
		if(file.fname[0] == 0)
		{
			printf("end of dir.\r\n");
			lcd_ClearScreen(COLOR_BLACK);
			p->state = 0;
			break;
		}
		if(file.fattrib & AM_DIR)
		{
			printf("Not a file.\r\n");
			break;
		}
		if(strstr(file.fname,".BIN") == NULL)
		{
			printf("Not an image file.%s\r\n",file.fname);
			break;
		}
		sprintf(pathname,"/girl/%-12s",file.fname);
		lcd_GetBitmapHeadFromFile(&head,pathname);
		if(head.w > LCD_WIDTH || head.h > LCD_HEIGHT)
		{
			lcd_DrawString(0,40,"Size too large.",&s);
		}
		else 
		{
			res = pf_open(pathname);
			if(res == FR_OK)
			{
				lcd_DrawBitmapFromFile((LCD_WIDTH-head.w)/2,(LCD_HEIGHT-head.h)/2,pathname);
			}
			else
			{
				printf("Error open file %s %s\r\n",pathname,pf_GetErrorString(res));
				lcd_DrawString(0,40,"open file error.",&s);
			}
		}
		//lcd_DrawString(0,0,pathname,&s);
		p->oldticks = ticks;
		p->state = 2;
		break;
	case 2:
		if(ticks - p->oldticks < 3000ul) break;
		p->state = 1;
		break;
	case 3:
		if(ticks - p->oldticks < 2000ul) break;
		lcd_ClearScreen(COLOR_BLACK);
		p->state = 1;
		break;
	default:
		break;
	}
}

void gui_DlgSDCardDemoProc(uint8_t msg_type, uint8_t msg_code)
{
	if(msg_type == WM_TIMER)
	{
		TestSDCard();
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

void gui_DlgSDCardDemoOpen(void)
{
	const struct gui_rect code rect = {5,20,LCD_WIDTH-10,1};
	gui_DlgSetActive(DLG_SDCARDDEMO);
	memset(g_con_param_list,0x00u,sizeof(g_con_param_list));
	//g_con_active = 0;
	lcd_ClearScreen(COLOR_BLACK);
	lcd_DrawString((LCD_WIDTH-16*8)/2,0,"演示SD卡读写功能",&g_style_normal);
	lcd_DrawSCBitmap(&rect, COLOR_RED);
	gui_DrawAllControls(g_con_list,g_con_active);
	InitSDDemo();
}
