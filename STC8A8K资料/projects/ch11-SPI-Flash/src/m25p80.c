/*
 * Copyright        : Shendong MCU studio
 * File Description : SPI protocol and M25P80 Flash driver
 * Created          : oliverzhang@shendongmcu.com
 */
#include "global.h"

sbit SPI_FLASH_CS	= P6^1;

#define EEPCMD_WREN        0x06u /*Write Enable*/
#define EEPCMD_WRDI        0x04u /*Write Disable*/
#define EEPCMD_RDSR        0x05u /*Read Status Reg*/
#define EEPCMD_WRSR        0x01u /*Write Status Reg*/
#define EEPCMD_READ        0x03u /*Read*/
#define EEPCMD_FASTREAD    0x0bu /*Fast Read*/
#define EEPCMD_PP          0x02u /*Page Program*/
#define EEPCMD_SE          0xd8u /*Sector Erase*/
#define EEPCMD_BE          0xc7u /*Bulk Erase*/
#define EEPCMD_DP          0xb9u /*Deep Powerdown*/
#define EEPCMD_RES         0xabu /*Release from power down.*/

#define SpiFlashDeselect() do{SPI_FLASH_CS = 1;}while(0)
#define SpiFlashSelect()  do{SPI_FLASH_CS = 0;}while(0)

static void Delay1us()
{
	_nop_();
	_nop_();
	_nop_();
}

/*SPR1 SPR0 @11.0592MHZ
 *  0    0  2.76MHZ     
 *  0    1  0.6912MHZ   691.2KHZ
 *  1    0  0.1728MHZ   172.8KHZ
 *  1    1  0.0864MHZ   86.4KHZ
**/
void spi_Initialization(uint8_t speed)
{
	speed &= 0x03u;
	/*Configure SPI to drive M25P80*/
	SPCTL = 0xdc | speed; /*SSIG SPEN DORD MSTR CPOL CPHA SPR1 SPR0*/
		      /*  1    1    0    1    0    0    0    0 */
		      /*Ignore SS pin,Enable SPI, MSB first,CPOL =0,CPHA = 0,
			clock rate = 1/4 CPU_clk*/
	
	SPSTAT = 0xc0;  /*clear SPIF WCOL flags.*/
}
void spi_SetSpeed(uint8_t speed)
{
	speed &= 0x03u;
	/*Configure SPI to drive M25P80*/
	SPCTL = 0xdc | speed; /*SSIG SPEN DORD MSTR CPOL CPHA SPR1 SPR0*/
		      /*  1    1    0    1    0    0    0    0 */
		      /*Ignore SS pin,Enable SPI, MSB first,CPOL =0,CPHA = 0,clock rate = 1/4 CPU_clk*/
}
uint8_t spi_ExchangeByte(uint8_t val)
{
	SPDAT = val;
	while((SPSTAT & 0x80) == 0);
	SPSTAT = 0x80;
	//if(SPSTAT & 0x40) printf("SPI Err");
	return SPDAT;
}
static void StartCommand(uint8_t cmd)
{
	SpiFlashSelect();
	Delay1us();
	spi_ExchangeByte(cmd);
}

static void EndCommand(void)
{
	SpiFlashDeselect();
	Delay1us();
}

static uint8_t xdata g_eep_error = 0;
static void BusyWait(void)
{
	uint8_t val;
	uint32_t i;
	for(i=0; i<4000ul;i++)
	{
		StartCommand(EEPCMD_RDSR);
		val = spi_ExchangeByte(0);
		EndCommand();
		if((val & 0x01) == 0) return;
		time_HoggingDelayMs(1);
		if(i % 10 == 0) {/*sys_FeedingWatchdog();*/}
	}
	if(g_eep_error < 10) g_eep_error++;
	printf("EEP fatal error.");
}

static void WriteEnable(void)
{
	uint8_t val;
	uint32_t i;

	BusyWait();
	StartCommand(EEPCMD_WREN);
	EndCommand();
	for(i=0;i<4000ul;i++)
	{
		StartCommand(EEPCMD_RDSR);
		val = spi_ExchangeByte(0);
		EndCommand();
		if(val & 0x02) return;
		time_HoggingDelayMs(1);
		if(i % 10 == 0){/*sys_FeedingWatchdog();*/}
	}
	if(g_eep_error < 10) g_eep_error++;
	printf("WriteEnable:EEP fatal error.");
}

static void WriteStatusReg(uint8_t comm)
{
	BusyWait();
	WriteEnable();

	StartCommand(EEPCMD_WRSR);
	spi_ExchangeByte(comm);
	EndCommand();
}

static void SectorErase(uint32_t addr)
{
	BusyWait();
	WriteEnable();

	StartCommand(EEPCMD_SE);
	spi_ExchangeByte(addr >> 16);
	spi_ExchangeByte(addr >> 8);
	spi_ExchangeByte(addr >> 0);
	EndCommand();
}
#if 0
static void WriteDisable(void)
{
	BusyWait();
	StartCommand(EEPCMD_WRDI);
	EndCommand();
}

static void BulkErase(void)
{
	BusyWait();
	WriteEnable();

	StartCommand(EEPCMD_BE);
	EndCommand();
}

static uint8_t ReadSignature(void)
{
	uint8_t val;
	StartCommand(EEPCMD_RES);
	spi_ExchangeByte(0);
	spi_ExchangeByte(0);
	spi_ExchangeByte(0);
	val = spi_ExchangeByte(0);
	EndCommand();
	return val;
}
#endif
static void PageProgram(uint16_t page,uint16_t offset,const uint8_t *buffer, uint16_t size) large
{
	uint16_t i;
	uint32_t addr;

	addr = (((uint32_t)page)<<8) + offset;
	if((addr & 0xffff) == 0) SectorErase(addr);
	BusyWait();
	WriteEnable();
	StartCommand(EEPCMD_PP);
	spi_ExchangeByte(addr >> 16);
	spi_ExchangeByte(addr >> 8);
	spi_ExchangeByte(addr >> 0);
	for(i=0 ; i<size; i++) spi_ExchangeByte(buffer[i]);
	EndCommand();
}

void m25p80_Initialization(void)
{
	spi_Initialization(SPI_SPEED_MOSTFAST);
	WriteStatusReg(0x00);
	spi_SetSpeed(SPI_SPEED_MOSTFAST);
}

static char xdata g_virtual_eep[10];
void m25p80_Write(const void *vbuf, uint16_t len, uint32_t eep_addr) large
{
	const uint8_t * buffer = (const uint8_t *)vbuf;
	uint16_t page,startpos;
	uint16_t nleft,nwrite;

	if(g_eep_error >= 10)
	{
		if(len < sizeof(g_virtual_eep)) memcpy(g_virtual_eep,vbuf,len);
		return;
	}
	page = eep_addr / 256;
	startpos = eep_addr % 256;
	nleft=len;
	for(;nleft;)
	{
		if(nleft <= 256 - startpos)
			nwrite = nleft;
		else
			nwrite = 256 - startpos;
		PageProgram(page,startpos,buffer,nwrite);
		buffer += nwrite;
		nleft -= nwrite;
		page ++;
		startpos=0;
	}
}

/*for MSP430 sizeof(void *) == 2 !!!  16 bit pointer */
void m25p80_Read(void *vbuf, uint16_t len, /*void * */uint32_t eep_addr)
{
	uint16_t i;
	uint8_t *buf = (uint8_t *)vbuf;

	if(g_eep_error >= 10)
	{
		if(len < sizeof(g_virtual_eep)) memcpy(vbuf,g_virtual_eep,len);
		return;
	}
	BusyWait();
	StartCommand(EEPCMD_READ);
	spi_ExchangeByte(eep_addr >> 16);
	spi_ExchangeByte(eep_addr >> 8);
	spi_ExchangeByte(eep_addr >> 0);
	for(i=0 ; i<len; i++)
	{
		buf[i]=spi_ExchangeByte(0);
	}
	EndCommand();
}

void m25p80_EraseChip(void)
{
	uint32_t secaddr;
	if(g_eep_error >= 10)return;
	for(secaddr=0;secaddr<0xffffful;secaddr+=0x10000ul)
	{
		SectorErase(secaddr);
	}
}

void m25p80_TestMain(uint8_t argc, char *argv[])
{
	const char code *str = "Hello world. Now Test M25P80 read/write/erase function.";
	char xdata buf[64];
	
	argc = argc;
	argv = argv;
	memset(buf,0,sizeof(buf));
	printf("Begin Erase M25P80.\r\n");
	m25p80_EraseChip();
	printf("Erase M25P80 End. then write:\r\n");
	m25p80_Write(str,strlen(str)+1,0x500);
	m25p80_Read(buf,sizeof(buf),0x500);
	buf[sizeof(buf)-1] = 0;
	printf("Content read back:\r\n");
	printf("%s\r\n",buf);
}

