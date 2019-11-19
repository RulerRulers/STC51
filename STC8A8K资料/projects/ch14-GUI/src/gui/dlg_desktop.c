/*
 * Copyright        : Shendong MCU studio
 * File Description : Desktop Dialog
 * Created          : oliverzhang@shendongmcu.com
 */

#include "../global.h"

static void InitDesktop(void)
{
	struct dlg_desktop_param *p = &g_all_dlg.desktop_param;
	memset(p,0,sizeof(*p));
	p->oldticks = time_GetTicks();
}

static void ShowLogo(void)
{
	struct dlg_desktop_param *p = &g_all_dlg.desktop_param;
	uint32_t ticks = time_GetTicks();

	if(ticks - p->oldticks < 60*1000ul) return;
	gui_DlgMainMenuOpen();
}

void gui_DlgDesktopProc(uint8_t msg_type, uint8_t msg_code)
{
	msg_code = msg_code;
	if(msg_type == WM_KEYPRESSED)
	{
		gui_DlgMainMenuOpen();
	}
	else if(msg_type == WM_TIMER)
	{
		ShowLogo();
	}
}

void gui_DlgDesktopOpen(void)
{
	lcd_DrawBitmapFromFlash(0,0,OFFSET_DESKTOP_BIN);
	gui_DlgSetActive(DLG_DESKTOP);
	InitDesktop();
}
