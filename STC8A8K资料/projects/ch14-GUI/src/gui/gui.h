/*
 * Copyright        : Shendong MCU studio
 * File Description : GUI sub-system header
 * Created          : oliverzhang@shendongmcu.com
 */

#ifndef __GUI_H__
#define __GUI_H__

extern void gui_Task(void);
enum {WM_KEYPRESSED,WM_KEYREPEATED,WM_KEYRELEASED,WM_TIMER};
extern const char *gui_GetMsgCodeString(uint8_t msg_code);
extern const char *gui_GetMsgTypeString(uint8_t msg_type);
extern void gui_DlgSetActive(uint8_t dlg_index);
extern void gui_DlgDefaultProc(uint8_t msg_type, uint8_t msg_code);

enum { DLG_DESKTOP=1, DLG_MAINMENU, DLG_MAINMENU2, DLG_FLASHDEMO,
	DLG_SDCARDDEMO,DLG_TFTDEMO,DLG_KEYDEMO,
	DLG_LEDDEMO,DLG_IRDEMO,DLG_ADCDEMO,DLG_RTCDEMO,DLG_MOTORDEMO,
	DLG_DACDEMO,DLG_EEPROMDEMO,DLG_ADJUSTRTC,
};
extern void gui_DlgDesktopOpen(void);
extern void gui_DlgDesktopProc(uint8_t msg_type, uint8_t msg_code);

extern void gui_DlgMainMenuOpen(void);
extern void gui_DlgMainMenuProc(uint8_t msg_type, uint8_t msg_code);

extern void gui_DlgMainMenu2Open(void);
extern void gui_DlgMainMenu2Proc(uint8_t msg_type, uint8_t msg_code);

extern void gui_DlgFlashDemoOpen(void);
extern void gui_DlgFlashDemoProc(uint8_t msg_type, uint8_t msg_code);

extern void gui_DlgSDCardDemoOpen(void);
extern void gui_DlgSDCardDemoProc(uint8_t msg_type, uint8_t msg_code);

extern void gui_DlgTftDemoOpen(void);
extern void gui_DlgTftDemoProc(uint8_t msg_type, uint8_t msg_code);

extern void gui_DlgKeyDemoOpen(void);
extern void gui_DlgKeyDemoProc(uint8_t msg_type, uint8_t msg_code);

extern void gui_DlgLedDemoOpen(void);
extern void gui_DlgLedDemoProc(uint8_t msg_type, uint8_t msg_code);

extern void gui_DlgIrDemoOpen(void); 
extern void gui_DlgIrDemoProc(uint8_t msg_type, uint8_t msg_code);

extern void gui_DlgAdcDemoOpen(void); 
extern void gui_DlgAdcDemoProc(uint8_t msg_type, uint8_t msg_code);

extern void gui_DlgRtcDemoOpen(void); 
extern void gui_DlgRtcDemoProc(uint8_t msg_type, uint8_t msg_code);

extern void gui_DlgEepromDemoOpen(void); 
extern void gui_DlgEepromDemoProc(uint8_t msg_type, uint8_t msg_code);

enum {CON_ENDFLAG,CON_TXTBUTTON,CON_BMPBUTTON,CON_CHECKBOX,CON_LIST,};
struct gui_control {
	uint8_t type;
	uint16_t x;
	uint16_t y;
	const char *str;
};
#define CON_MAX_ITEMS 10
extern uint8_t idata g_con_param_list[CON_MAX_ITEMS];

extern void gui_DrawControl(const struct gui_control *pcon, uint8_t is_active);
extern void gui_DrawAllControls(const struct gui_control *plist, uint8_t active);
extern void gui_SearchPrevControl(const struct gui_control *plist, uint8_t *pactive);
extern void gui_SearchNextControl(const struct gui_control *plist, uint8_t *pactive);


struct dlg_desktop_param {
	uint32_t oldticks;
};
struct dlg_flash_demo_param {
	uint8_t state;
	uint32_t oldticks;
	uint16_t x;
	uint16_t y;
};
struct dlg_ir_demo_param {
	uint8_t state;
	uint32_t oldticks;
	uint16_t x;
	uint16_t y;
};
struct dlg_key_demo_param {
	uint8_t state;
	uint32_t oldticks;
	uint16_t x;
	uint16_t y;
};
struct dlg_led_demo_param {
	uint8_t dummy;
};
struct dlg_sd_demo_param {
	uint8_t state;
	uint32_t oldticks;
	uint16_t x;
	uint16_t y;
};
struct dlg_tft_demo_param {
	uint8_t state;
	uint32_t oldticks;
	uint16_t x;
	uint16_t y;
};
struct dlg_adc_demo_param {
	uint8_t state;
	uint32_t oldticks;
	uint16_t x;
	uint16_t y;
};
struct dlg_rtc_demo_param {
	uint8_t state;
	uint32_t oldticks;
	uint16_t x;
	uint16_t y;
	uint8_t old_second;
};
struct dlg_eeprom_demo_param {
	uint8_t state;
	uint32_t oldticks;
	uint16_t x;
	uint16_t y;
};
union gui_all_dlg {
	struct dlg_desktop_param desktop_param;
	struct dlg_flash_demo_param flash_demo;
	struct dlg_ir_demo_param ir_demo;
	struct dlg_key_demo_param key_demo;
	struct dlg_led_demo_param led_demo;
	struct dlg_sd_demo_param sd_demo;
	struct dlg_tft_demo_param tft_demo;
	struct dlg_adc_demo_param adc_demo;
	struct dlg_rtc_demo_param rtc_demo;
	struct dlg_eeprom_demo_param eeprom_demo;
};
extern union gui_all_dlg xdata g_all_dlg;

#endif
