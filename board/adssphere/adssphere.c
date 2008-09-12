/*
 * (C) Copyright 2002 2003
 * Network Audio Technologies, Inc. <www.netaudiotech.com>
 * Adam Bezanson <bezanson@netaudiotech.com>
 *
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <command.h>
#include <asm/arch/ep93xx.h>
#include "clocks.h"
#include <linux/byteorder/swab.h>
#include <i2c.h>
#include "adssphere.h"

static void mac_addr_init (void);

#define HAL_WRITE_UINT32( _register_, _value_ ) \
        (*((volatile unsigned long *)(_register_)) = (_value_))

DECLARE_GLOBAL_DATA_PTR;

/* ------------------------------------------------------------------------- */

/*
 * Miscellaneous platform dependent initialisations
 */


int board_init (void)
{
	DECLARE_GLOBAL_DATA_PTR;
	unsigned long ClkSet1;
	unsigned long ClkSet2;
	
	
	//
	// Set the output of PLL2 to 192Mhz
	//
	ClkSet2 = 0x300dc317;
	
#ifdef CONFIG_EP9301
	//
	// Set the output of the PLL to 332Mhz
	//
	ClkSet1 = EP9312_CLKSET1_NBYP | 0x00fa5a;
	
	//
	// Set the FCLKDIV value to divide by 2 (166Mhz).
	//
	ClkSet1 |= (1 << EP9312_CLKSET1_FCLKDIV_SHIFT);
	
	//
	// Set the HCLKDIV value to divide by 5 (66Mhz).
	//
	ClkSet1 |= (3 << EP9312_CLKSET1_HCLKDIV_SHIFT);
#else
	//
	// Set the output of the PLL to 400Mhz
	//
	ClkSet1 = EP9312_CLKSET1_NBYP | 0x00e39e;
	
	//
	// Set the FCLK to 400/2 or 200Mhz
	//
	ClkSet1 |= (1 << EP9312_CLKSET1_FCLKDIV_SHIFT);
	
	//
	// Set the HCLK to 400/4 or 100Mhz
	//
	ClkSet1 |= (2 << EP9312_CLKSET1_HCLKDIV_SHIFT);
#endif // CONFIG_EP9301
	
	//
	// Set PCLKDIV so that PCLK = HCLK / 2
	//
	ClkSet1 |= (1 << EP9312_CLKSET1_PCLKDIV_SHIFT);
	
	HAL_WRITE_UINT32(EP9312_CLKSET1, ClkSet1);
	
	//
	// Do the five required nops to keep us clean.
	//
	__asm ("nop");
	__asm ("nop");
	__asm ("nop");
	__asm ("nop");
	__asm ("nop");
	
	//
	// Write out the value to ClkSet 2
	//
	HAL_WRITE_UINT32(EP9312_CLKSET2, ClkSet2);
	
	//
	// Go to Async mode
	//
	__asm ("mrc p15, 0, r0, c1, c0, 0");
	__asm ("orr r0, r0, #0xc0000000");
	__asm ("mcr p15, 0, r0, c1, c0, 0");
	
	icache_enable();

#ifdef USE_920T_MMU
	dcache_enable();
#endif

	/* make sure the PCMCIA power is turnned off */
	*((ulong *)ADSSPHERE_CR0) = ~(ADSSPHERE_CR0_CF_50 | ADSSPHERE_CR0_CF_33);

	/* arch number */
	gd->bd->bi_arch_number = MACH_TYPE_ADSSPHERE;

	/* adress of boot parameters */
	gd->bd->bi_boot_params = CONFIG_BOOT_PARAM_ADDR;

	/* We have a console */
	gd->have_console = 1;

	return 0;
}

int board_late_init(void)
{
	DECLARE_GLOBAL_DATA_PTR;

	setenv("stdout", "serial");
	setenv("stderr", "serial");

	if (gd->flags & GD_FLG_SILENT) {
		SysconScrReg0 = 0xffffffff;
	}
	else {
#if defined(CONFIG_SERIAL1)
		SysconScrReg0 = (unsigned long) (&rUART1_DR);
#elif defined(CONFIG_SERIAL2)
		SysconScrReg0 = (unsigned long) (&rUART2_DR);
#elif defined(CONFIG_SERIAL3)
		SysconScrReg0 = (unsigned long) (&rUART3_DR);
#else
#error "Must have a uart for console!"
#endif
	}

#if defined(CONFIG_CMD_NET)

	/* this should avoid problems where the ethernet is still
	   scribling on DRAM after the board has rebooted */
	eth_halt();

#ifdef CFG_MAC_ADDR_IN_EEPROM
	mac_addr_init();
#endif

#endif

	return 0;
}

int dram_init (void)
{
	DECLARE_GLOBAL_DATA_PTR;

	gd->bd->bi_dram[0].start = PHYS_SDRAM_1;
	gd->bd->bi_dram[0].size = PHYS_SDRAM_SIZE_1;
	gd->bd->bi_dram[1].start = PHYS_SDRAM_2;
	gd->bd->bi_dram[1].size = PHYS_SDRAM_SIZE_2;
	gd->bd->bi_dram[2].start = PHYS_SDRAM_3;
	gd->bd->bi_dram[2].size = PHYS_SDRAM_SIZE_3;
	gd->bd->bi_dram[3].start = PHYS_SDRAM_4;
	gd->bd->bi_dram[3].size = PHYS_SDRAM_SIZE_4;

	return 0;
}

#ifdef CFG_MAC_ADDR_IN_EEPROM
static void mac_addr_init (void)
{
	unsigned char mac[8];
	int err=0;
	char ethaddr[6*3+1];

	err = i2c_read(0x50, 0, 1, mac, sizeof(mac));

	if (err) {
		printf("Warning: cannot read from eeprom that should contain MAC address.\n");
		return;
	}

	if (mac[0] != 0x55 || mac[7] != 0xaa) {
		printf("Warning: MAC address in eeprom is missing magic numbers.\n");
	}

	sprintf(ethaddr, "%0.2x:%0.2x:%0.2x:%0.2x:%0.2x:%0.2x", mac[1], mac[2], mac[3], mac[4], mac[5], mac[6]);
	setenv("ethaddr", ethaddr);

	
}

static int write_mac_to_chip(char *maddr)
{
	int i;

	for (i=0; i<8; i++) {
	
		if(i2c_write(0x50, i, 1, maddr+i, 1) != 0) {
			return 1;
		}
		/*
		 * Wait for the write to complete.  The write can take
		 * up to 10mSec (we allow a little more time).
		 *
		 * On some chips, while the write is in progress, the
		 * chip doesn't respond.  This apparently isn't a
		 * universal feature so we don't take advantage of it.
		 */
		udelay(11000);
	}
	return 0;
	
}

int do_macaddr ( cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	int len, i;
	char *argv1 = argv[1];
	char maddr[8];
	char maddr_check[8];
	int ret;
	char ethaddr[6*3+1];

	if (argc > 2) {
		printf("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

	if (argc == 1) {
		ret = i2c_read(0x50, 0, 1, maddr, sizeof(maddr));

		if (ret) {
			printf("Warning: cannot read from eeprom that should contain MAC address.\n");
			return 1;
		}

		if (maddr[0] != 0x55 || maddr[7] != 0xaa) {
			printf("MAC address uninitialized (missing magic numbers).\n");
			return 1;
		}
		else {
			printf("MAC address: %0.2x:%0.2x:%0.2x:%0.2x:%0.2x:%0.2x\n",
			       maddr[1], maddr[2], maddr[3], maddr[4], maddr[5], maddr[6]);
			return 0;
		}
	}

	len = strnlen(argv1, 20);

	if (len != 12 && len != 17) {
		printf("Illegal address format: %s\n", argv1);
		return 1;
	}

	if (len == 17) {
		if ((argv1[ 2] != ':' && argv1[ 2] != '-') || 
		    (argv1[ 5] != ':' && argv1[ 5] != '-') || 
		    (argv1[ 8] != ':' && argv1[ 8] != '-') ||
		    (argv1[11] != ':' && argv1[11] != '-') ||
		    (argv1[14] != ':' && argv1[14] != '-')) {
			printf("Illegal address format: %s\n", argv1);
			return 1;
		}

		/* convert drop the ':' */
		argv1[ 2] = argv1[ 3];
		argv1[ 3] = argv1[ 4];	
		argv1[ 4] = argv1[ 6];
		argv1[ 5] = argv1[ 7];
		argv1[ 6] = argv1[ 9];
		argv1[ 7] = argv1[10];
		argv1[ 8] = argv1[12];
		argv1[ 9] = argv1[13];
		argv1[10] = argv1[15];
		argv1[11] = argv1[16];
		argv1[13] = 0;
	}

	/* check and convert to binary */
	for (i=0; i<12; i++) {
		if (argv1[i] >= '0' && argv1[i] <= '9') {
			argv1[i] -= '0';
		}
		else if (argv1[i] >= 'a' && argv1[i] <= 'f') {
			argv1[i] -= 'a';
			argv1[i] += 0xa;
		}
		else if (argv1[i] >= 'A' && argv1[i] <= 'F') {
			argv1[i] -= 'A';
			argv1[i] += 0xa;
		}
		else {
			printf("Invalid address symbol: %c (%#x)\n", argv1[i], argv1[i]);
			return 1;
		}
	}

	for (i=0; i<6; i++) {
		maddr[i+1] = argv1[2*i] << 4 | argv1[2*i+1];
	}
	

	/* add in the signature */
	maddr[0] = 0x55;
	maddr[7] = 0xaa;

	if (write_mac_to_chip(maddr)) {
		printf("Failed to write to eeprom.\n");
		return 1;
	}

	if (i2c_read(0x50, 0, 1, maddr_check, sizeof(maddr_check))) {
		printf("Failed to read back from eeprom\n");
		return 1;
	}

	if (strncmp(maddr, maddr_check, 8)) {
		printf("Failed to read same MAC address back from eeprom\n");
		return 1;
	}

	sprintf(ethaddr, "%0.2x:%0.2x:%0.2x:%0.2x:%0.2x:%0.2x",
		maddr[1], maddr[2], maddr[3], maddr[4], maddr[5], maddr[6]);
	setenv("ethaddr", ethaddr);

	printf("Successfully wrote MAC address %s to eeprom\n", ethaddr);

	return 0;
}

U_BOOT_CMD(
	macaddr,	2,	1,	do_macaddr,
	"macaddr  - display or store MAC address in eeprom\n",
	"[address]\n"
	"       - address is of form xx:xx:xx:xx:xx:xx or xxxxxxxxxxxx\n"
);

#endif
