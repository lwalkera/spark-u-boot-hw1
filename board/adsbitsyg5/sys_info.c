#include <common.h>
#include <asm/types.h>
#include <linux/ctype.h>
#include "board.h"
#include "adsversion.h"

u32 get_base(void)
{
	extern u8 * u_boot_addr;
	u32  val;

	val = (u32 )u_boot_addr & 0xF0000000;
	return(val);
}

int ads_version = CFG_VERSION ;
int ads_patchlevel = CFG_PATCHLEVEL;
int ads_sublevel = CFG_SUBLEVEL;
int ads_extaversion = CFG_EXTRAVERSION; /*"t"-test "r"-release*/

#define ADS_BOAD_VERSION() (((ads_version&0xFF)<<24) |			\
		((ads_patchlevel&0xFF)<<16) | ((ads_sublevel&0xFF)<<8)	\
		| (ads_extaversion&0xFF))

static void ads_get_boardrev(int * m, int * p, int * s, int * e)
{
	char * adsversion = getenv("ADSVERSION");
	char * p1, *p2, tmp[32];

	if(!adsversion)
		return ;

	p1 = p2 = adsversion;
	while(*p1 && !isdigit(*p1))
		p1 ++ ;
	if(!*p1) return;

	p2 = p1+1;
	while(*p2 && isdigit(*p2))
		p2 ++;
	strncpy(tmp, p1, sizeof(tmp));
	tmp[p2-p1]='\0';
	*m = (int) simple_strtoul (tmp, NULL, 10);


	p1 = p2;
	if(!*p1) return;
	while(*p1 && !isdigit(*p1))
		p1 ++ ;
	if(!*p1) return;
	p2 = p1+1;
	while(*p2 && isdigit(*p2))
		p2 ++;
	strncpy(tmp, p1, sizeof(tmp));
	tmp[p2-p1]='\0';
	*p = (int) simple_strtoul (tmp, NULL, 10);


	if(!*p2) return ;
	while(*p2 && !isalpha(*p2))
		p2 ++;
	if(!*p2) return ;
	*e = *p2;


	p1 = p2;
	while(*p1 && !isdigit(*p1))
		p1 ++;
	if(!*p1) return;
	p2 = p1+1;
	while(*p2 && isdigit(*p2))
		p2 ++;
	strncpy(tmp, p1, sizeof(tmp));
	tmp[p2-p1]='\0';
	*s = (int) simple_strtoul (tmp, NULL, 10);
	return ;
}

int checkboard(void)
{
	ads_get_boardrev(&ads_version, &ads_patchlevel, 
			&ads_sublevel, &ads_extaversion);

	printf("Machine: %s v%d.%d%c%02d",CFG_BOARD_NAME, ads_version,
		  ads_patchlevel, ads_extaversion, ads_sublevel);
	printf(" [REV%d]", ADSBITSYG5_CPLD_REV & 0xff);
	switch(get_base())
	{
		case CS0_BASE_ADDR:
			puts ("\t[NOR Flash]\n");
			break;
		case CSD0_BASE_ADDR:
		case CSD1_BASE_ADDR:
			puts ("\t[SDRAM]\n");
			break;
		default:
			puts ("\t[NAND Flash]\n");
	}
	return 0;
}

#ifdef CONFIG_REVISION_TAG  
/*In i.mx31 linux kernel BSP 
  ATAG_REVISION used by cpu revision.
 */
#define SYSTEM_SREV_REG     IO_ADDRESS(IIM_BASE_ADDR + 0x24)
u32 get_board_rev(void)
{
	/*0x31014*/
	return (0x31000 |__raw_readl(SYSTEM_SREV_REG) & 0xFF);
}
#endif

#ifdef CONFIG_SERIAL_TAG
static u32 ADS_BOAD_SERIAL(void)
{
	char tmp[64];
	u32 sn = getenv_r ("SERIALSN", tmp, sizeof (tmp));
	if(sn)
		sn = (u32) simple_strtoul (tmp, NULL, 16);
	return sn;
}

void get_board_serial(struct tag_serialnr *serialnr)
{
	/*
	   high	used as board revision.
	   low	used as board serial.
	 */
	serialnr->high = ADS_BOAD_VERSION();
	serialnr->low = ADS_BOAD_SERIAL();
}
#endif
