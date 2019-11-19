/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for Petit FatFs (C)ChaN, 2014      */
/*-----------------------------------------------------------------------*/
#include "../global.h"
#include "pff.h"

sbit SD_CARD_DETECT = P6^7;

const char code *pf_GetErrorString(FRESULT res)
{
	switch(res)
	{
		case FR_OK: return "FR_OK"; break;
		case FR_DISK_ERR: return "FR_DISK_ERR"; break;
		case FR_NOT_READY: return "FR_NOT_READY"; break;
		case FR_NO_FILE: return "FR_NO_FILE"; break;
		case FR_NOT_OPENED: return "FR_NOT_OPENED"; break;
		case FR_NOT_ENABLED: return "FR_NOT_ENABLED"; break;
		case FR_NO_FILESYSTEM: return "FR_NO_FILESYSTEM"; break;
		default: return "Unknown error.";break;
	}
}

static FATFS xdata g_fatfs;
/*!!! ATTENTION!!! this function must be executed before any other SPI device initialization.*/
void pfs_Initialization(void)
{
	FRESULT res;
	if(SD_CARD_DETECT) return;
	uart1_PrintStr("SD card inserted, now init SDCARD.\r\n");
	res = pf_mount(&g_fatfs);
	if(res == FR_OK)
	{
		printf("Done.\r\n$>");
		return;
	}
	uart1_PrintStr3("mount sd card error ",pf_GetErrorString(res),"\r\n$>");
	time_HoggingDelayMs(1000);
	res = pf_mount(&g_fatfs);
	uart1_PrintStr3("mount sd card ",pf_GetErrorString(res),"\r\n$>");
}

void pfs_Task(void)
{
	static uint8_t xdata state = 0;
	static uint32_t xdata oldticks; 
	uint32_t xdata ticks;
	FRESULT res;

	switch(state)
	{
	case 0:
		SD_CARD_DETECT = 1;
		state = 1;
		break;
	case 1: /*Wait for CARD inserted.*/
		if(SD_CARD_DETECT == 1)
			break;
		oldticks = time_GetTicks();
		state = 2;
		break;
	case 2:
		ticks = time_GetTicks();
		if(ticks - oldticks < 200)break;
		if(SD_CARD_DETECT == 1)
		{
			state = 1;
			break;
		}
		uart1_PrintStr("SD card inserted, now init SDCARD.\r\n");
		res = pf_mount(&g_fatfs);
		if(res != FR_OK)
		{
			uart1_PrintStr3("mount sd card error ",pf_GetErrorString(res),"\r\n$>");
			oldticks = time_GetTicks();
			state = 100;
			break;
		}
		uart1_PrintStr("Done.\r\n$>");
		state = 3;
		break;
	case 3: /*wait for push-out.*/
		if(SD_CARD_DETECT == 0)
			break;
		oldticks = time_GetTicks();
		state = 4;
		break;
	case 4: 
		ticks = time_GetTicks();
		if(ticks - oldticks < 200)break;
		if(SD_CARD_DETECT == 0)
		{
			state = 3;
			break;
		}
		uart1_PrintStr("SD card pulled out, now De-Init SDCARD.\r\n");
		res = pf_mount(&g_fatfs);
		uart1_PrintStr3("unmount sd card : ",pf_GetErrorString(res),"\r\n$>");
		state = 0;
		break;
	case 100: /*Fatal error. wait for 5 second.*/
		ticks = time_GetTicks();
		if(ticks - oldticks < 5 * 1000u) break;
		state = 0;
		break;
	default:
		uart1_PrintStr("impossible here.\r\n");
		break;
	}
}

static void ListDir(char *dir_name)
{
	DIR xdata dir;
	FILINFO xdata info;
	FRESULT res;

	res = pf_opendir(&dir,dir_name);
	if(res != FR_OK)
	{
		uart1_PrintStr3("read dir error ",pf_GetErrorString(res),"\r\n");
		return;
	}
	for(;;)
	{
		res = pf_readdir(&dir,&info);
		if(res != FR_OK || info.fname[0]==0)break;
		uart1_PrintStr(info.fname);
		uart1_PrintStr("\r\n");
        }
}

static void CatFile(char *pathname)
{
	FRESULT res;
	char xdata buff[16];
	UINT br;
	
	res = pf_open(pathname);
	pf_lseek(0);
	if(res != FR_OK)
	{
		printf("open file error: ");
		printf(pf_GetErrorString(res));
		printf("\r\n");
		return;
	}
	for(;;)
	{
		res = pf_read(buff, 16, &br);
		if(res != FR_OK)
		{
			uart1_PrintStr(pf_GetErrorString(res));
			return;
		}
		uart1_Write(buff,br);
		if(br != 16)break;
	}
	printf("\r\n\r\n");
}

void pfs_Main(uint8_t argc,char *argv[]) 
{
	if(argc == 1) goto _print_usage_;
	if(strcmp(argv[1],"dir") == 0)
	{
		if(argc < 3) goto _print_usage_;
		ListDir(argv[2]);
		return;
	}
	else if(strcmp(argv[1],"cat") == 0)
	{
		if(argc < 3) goto _print_usage_;
		CatFile(argv[2]);
		return;
	}
	else if(strcmp(argv[1],"never") == 0)
	{
		pf_write(NULL,0,NULL);
		return;
	}

_print_usage_:
	printf("---------usage-------------\r\n");
	printf("%s dir <dir-name>\t\tlist dir items\r\n",argv[0]);
	printf("%s cat <file-name>\t\tdump file content to screen.\r\n",argv[0]);
}
