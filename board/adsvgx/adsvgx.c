/*
 * (C) Copyright 2002
 * Kyle Harris, Nexus Technologies, Inc. kharris@nexus-tech.net
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
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
#include "adsvgx.h"
#include <asm/arch/pxa-regs.h>

DECLARE_GLOBAL_DATA_PTR;

/* ------------------------------------------------------------------------- */

#ifdef CONFIG_SHOW_BOOT_PROGRESS
void show_boot_progress(int arg)
{
  printf("boot progress: %d\n", arg);
}
#endif

DECLARE_GLOBAL_DATA_PTR;

/*
 * Miscelaneous platform dependent initialisations
 */

int board_init (void)
{
	/* memory and cpu-speed are setup before relocation */
	/* so we do _nothing_ here */

	/* arch number of ADS Vgx Board */
	gd->bd->bi_arch_number = MACH_TYPE_ADSVGX;

	/* adress of boot parameters */
	gd->bd->bi_boot_params = 0xa000003c;

	/* turn on serial ports and nothing else */
	ADSVGX_CPLD_CR1 |=
	  ADSVGX_CPLD_CR1_COM3EN |
	  ADSVGX_CPLD_CR1_COM2EN |
	  ADSVGX_CPLD_CR1_COM1EN;

	/* reset the SM501 and the ethernet */
	GPSR(0) = 0x00000400;
        ADSVGX_CPLD_CR3 |= ADSVGX_CPLD_CR3_DRESETP;

	/* udelay(10); */
	OSCR = 0;
	while (OSCR < 37);

	ADSVGX_CPLD_CR3 &= ~ADSVGX_CPLD_CR3_DRESETP;

	/* mdelay(10); */
	OSCR = 0;
	while (OSCR < 36846);

	return 0;
}

int board_late_init(void)
{
	DECLARE_GLOBAL_DATA_PTR;

	setenv("stdout", "serial");
	setenv("stderr", "serial");

	if (gd->flags & GD_FLG_SILENT) {
		PSPR = 0xffffffff;
	}
	else {
#if defined(CONFIG_FFUART)
		PSPR = (unsigned long) (&FFRBR);
#elif defined(CONFIG_BTUART)
		PSPR = (unsigned long) (&BTRBR);
#elif defined(CONFIG_STUART)
		PSPR = (unsigned long) (&STRBR);
#else
#error "Must have a uart for console!"
#endif
	}

	return 0;
}

int dram_init (void)
{
	DECLARE_GLOBAL_DATA_PTR;

	gd->bd->bi_dram[0].start = PHYS_SDRAM_1;
	gd->bd->bi_dram[0].size = PHYS_SDRAM_1_SIZE;
	gd->bd->bi_dram[1].start = PHYS_SDRAM_2;
	gd->bd->bi_dram[1].size = PHYS_SDRAM_2_SIZE;
	gd->bd->bi_dram[2].start = PHYS_SDRAM_3;
	gd->bd->bi_dram[2].size = PHYS_SDRAM_3_SIZE;
	gd->bd->bi_dram[3].start = PHYS_SDRAM_4;
	gd->bd->bi_dram[3].size = PHYS_SDRAM_4_SIZE;

	return 0;
}
