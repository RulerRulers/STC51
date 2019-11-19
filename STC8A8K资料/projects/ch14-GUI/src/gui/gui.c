/*
 * Copyright        : Shendong MCU studio
 * File Description : GUI frame implementation.
 * Created          : oliverzhang@shendongmcu.com
 */
#include "../global.h"

static uint8_t idata g_current_dlg;

void gui_DlgSetActive(uint8_t dlg_index)
{
	g_current_dlg = dlg_index;
}

void gui_Task(void)
{
	uint32_t xdata ticks;
	static uint32_t xdata oldticks;
	uint8_t xdata ret,key_code,key_state;
	uint8_t msg_type = 0xffu, msg_code = 0xffu;
	
	ticks = time_GetTicks();
	if(ticks - oldticks < 10) {/*Do nothing.*/} else {
		oldticks = ticks;
		msg_type = WM_TIMER;
		msg_code = 0;
	}
	if(msg_type == 0xffu)
	{
		ret = key_GetKeyCode(&key_code,&key_state);
		if(ret != 0)
		{
			msg_code = key_code;
			msg_type = key_state;
		}
	}
	if(msg_type != 0xffu)
	{
		switch(g_current_dlg)
		{
			case 0: break;
			case DLG_DESKTOP:	gui_DlgDesktopProc(msg_type,msg_code); break;
			case DLG_MAINMENU:	gui_DlgMainMenuProc(msg_type,msg_code); break;
			case DLG_MAINMENU2:	gui_DlgMainMenu2Proc(msg_type,msg_code); break;
			case DLG_FLASHDEMO:	gui_DlgFlashDemoProc(msg_type,msg_code); break;
			case DLG_SDCARDDEMO:	gui_DlgSDCardDemoProc(msg_type,msg_code); break;
			case DLG_TFTDEMO:	gui_DlgTftDemoProc(msg_type,msg_code); break;
			case DLG_KEYDEMO:	gui_DlgKeyDemoProc(msg_type,msg_code); break;
			case DLG_LEDDEMO:	gui_DlgLedDemoProc(msg_type,msg_code); break;
			case DLG_IRDEMO:	gui_DlgIrDemoProc(msg_type,msg_code); break;
			case DLG_ADCDEMO:	gui_DlgAdcDemoProc(msg_type,msg_code); break;
			case DLG_RTCDEMO:	gui_DlgRtcDemoProc(msg_type,msg_code); break;
			case DLG_EEPROMDEMO:	gui_DlgEepromDemoProc(msg_type,msg_code); break;
			default: printf("No dialog proc.\r\n");break;
		}
	}
}

void gui_DlgDefaultProc(uint8_t msg_type, uint8_t msg_code)
{
	/*Add your default proc here.*/
	msg_type = msg_type;
	msg_code = msg_code;
}


const char *gui_GetMsgTypeString(uint8_t msg_type)
{
	switch(msg_type)
	{
	case WM_KEYPRESSED: return "1"; 
	case WM_KEYREPEATED: return "2"; 
	case WM_KEYRELEASED: return "3"; 
	case WM_TIMER: return "WM_TIMER"; 
	default: return "unknown type";
	}
}

const char *gui_GetMsgCodeString(uint8_t msg_code)
{
	switch(msg_code)
	{
	case KEY_ONBD:		return("KEY_ONBD: ");break;
	case KEY_1:		return("KEY_1: ");break;
	case KEY_2:		return("KEY_2: ");break;
	case KEY_3:		return("KEY_3: ");break;
	case KEY_4:		return("KEY_4: ");break;
	case KEY_5:		return("KEY_5: ");break;
	case KEY_6:		return("KEY_6: ");break;
	case KEY_7:		return("KEY_7: ");break;
	case KEY_8:		return("KEY_8: ");break;
	case KEY_9:		return("KEY_9: ");break;
	case KEY_0:		return("KEY_0: ");break;
	case KEY_ASTERISK:	return("KEY_ASTERISK: ");break;
	case KEY_POUND:		return("KEY_POUND: ");break;
	case KEY_LEFT:		return("KEY_LEFT: ");break;
	case KEY_UP:		return("KEY_UP: ");break;
	case KEY_DOWN:		return("KEY_DOWN: ");break;
	case KEY_RIGHT:		return("KEY_RIGHT: ");break;
	case KEY_F1:		return("KEY_F1: ");break;
	case KEY_F2:		return("KEY_F2: ");break;
	case KEY_F3:		return("KEY_F3: ");break;
	case KEY_F4:		return("KEY_F4: ");break;
	case KEY_MENU:		return("KEY_MENU: ");break;
	case KEY_MUTE:		return("KEY_MUTE: ");break;
	case KEY_TVAV:		return("KEY_TVAV: ");break;
	case KEY_DIGIT:		return("KEY_DIGIT: ");break;
	case KEY_OK:		return("KEY_OK: ");break;
	case KEY_AUTO:		return("KEY_AUTO: ");break;
	case KEY_POWER:		return("KEY_POWER: ");break;
	case KEY_PLUS:		return("KEY_PLUS: ");break;
	case KEY_MINUS:		return("KEY_MINUS: ");break;
	case KEY_RETURN:	return("KEY_RETURN: ");break;
	case KEY_EXIT:		return("KEY_EXIT: ");break;		
	case KEY_PREVPAGE:      return("KEY_PREVPAGE: ");break;    
	case KEY_NEXTPAGE:      return("KEY_NEXTPAGE: ");break;    
	case KEY_CHANNELLIST:   return("KEY_CHANNELLIST: ");break; 
	case KEY_PROGRAMGUIDE:	return("KEY_PROGRAMGUIDE: ");break;
	case KEY_PROGRAMINFO:   return("KEY_PROGRAMINFO: ");break; 
	case KEY_INFOSERVICE:   return("KEY_INFOSERVICE: ");break; 
	case KEY_RED:           return("KEY_RED: ");break;         
	case KEY_GREEN:		return("KEY_GREEN: ");break;		
	case KEY_YELLOW:        return("KEY_YELLOW: ");break;      
	case KEY_BLUE:          return("KEY_BLUE: ");break;        
	case KEY_NONE:		return("KEY_NONE or unknown: ");break;
	default: 		return("impossible to run here:"); break;
	}
}

void gui_DrawAllControls(const struct gui_control *plist,uint8_t active)
{
	uint8_t i;
	if(plist == NULL) return;
	for(i=0;plist[i].type != CON_ENDFLAG && i < CON_MAX_ITEMS; i++)
	{
		gui_DrawControl(&plist[i], (i == active));
	}
}

void gui_DrawControl(const struct gui_control *pcon,uint8_t is_active)
{
	if(pcon == NULL) return;
	if(pcon->type == CON_TXTBUTTON)
	{
		lcd_DrawString(pcon->x,pcon->y,pcon->str,(is_active) ? &g_style_reverse : &g_style_normal);
	}
	else if(pcon->type == CON_BMPBUTTON)
	{
		
	}
}

void gui_SearchPrevControl(const struct gui_control *plist,uint8_t *pactive)
{
	uint8_t end,active = *pactive;
	if(plist == NULL) return;
	gui_DrawControl(&plist[active],0);
	for(end=0;plist[end].type!=CON_ENDFLAG && end < CON_MAX_ITEMS;end++);
	if(active == 0) active = end - 1;
	else active --;
	gui_DrawControl(&plist[active],1);
	*pactive = active;
}

void gui_SearchNextControl(const struct gui_control *plist,uint8_t *pactive)
{
	uint8_t active = *pactive;
	if(plist == NULL) return;
	gui_DrawControl(&plist[active],0);
	active ++;
	if(plist[active].type == CON_ENDFLAG) active = 0;
	gui_DrawControl(&plist[active],1);
	*pactive = active;
}

const struct gui_style code g_style_reverse = {COLOR_BLACK,COLOR_WHITE, FONT_16};
const struct gui_style code g_style_normal = {COLOR_WHITE,COLOR_BLACK, FONT_16};

uint8_t idata g_con_param_list[CON_MAX_ITEMS];
union gui_all_dlg xdata g_all_dlg;



