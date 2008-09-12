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

#include <asm/arch/ixp425.h>
#include <common.h>
#include <i2c.h>
#include <pci.h>
#include <command.h>

DECLARE_GLOBAL_DATA_PTR;

/* ------------------------------------------------------------------------- */


/* local prototypes */

void show_boot_progress(int x)
{
	printf("sbp: %x\n", x);
}

/*
 * Miscelaneous platform dependent initialisations
 */

/**********************************************************/

int board_post_init (void)
{
	return (0);
}

/**********************************************************/

int board_init (void)
{
	DECLARE_GLOBAL_DATA_PTR;


	/* arch number of Portal */
	gd->bd->bi_arch_number = MACH_TYPE_ADSPORTAL;

	/* adress of boot parameters */
	gd->bd->bi_boot_params = 0x00000100;

	return 0;
}

/**********************************************************/

int board_late_init(void)
{
	setenv("stdout", "serial");
	setenv("stderr", "serial");
	return 0;
}

/**********************************************************/

int dram_init (void)
{
	DECLARE_GLOBAL_DATA_PTR;

	gd->bd->bi_dram[0].start = PHYS_SDRAM_1;
	gd->bd->bi_dram[0].size  = PHYS_SDRAM_1_SIZE;

	return (0);
}

/**********************************************************/

static struct pci_device_id sm501[] = {
	{0x126f, 0x0501},
	{0, 0}
};

void sm501_map(void)
{
	pci_dev_t devno;

	if ((devno = pci_find_devices (sm501, 0)) < 0) {
		printf("SM501 not found on PCI bus\n");
		return;
	}

	pci_write_config_dword(devno, PCI_BASE_ADDRESS_1, 0x4a800000);

	pci_write_config_dword(devno, PCI_COMMAND, PCI_COMMAND_MEMORY  |
				PCI_COMMAND_IO | PCI_COMMAND_WAIT);
}

extern struct pci_controller hose;

void pci_init_board(void)
{
	extern void pci_ixp_init (struct pci_controller *hose);

	pci_ixp_init(&hose);

	sm501_map();
}

#ifdef CFG_MAC_ADDR_IN_EEPROM

static int write_mac_to_chip(int offset, char *maddr)
{
	int i;

	for (i=0; i<8; i++) {
	
		if(i2c_write(0x50, offset+i, 2, maddr+i, 1) != 0) {
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
		udelay(20000);
	}
	return 0;
	
}


static int read_macaddr (int offset)
{
	char maddr[8];
	int ret;

	ret = i2c_read(0x50, offset, 2, maddr, sizeof(maddr));

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

static int write_macaddr (char *argv1, int offset)
{
	int len, i;
	char maddr[8];
	char maddr_check[8];
	char ethaddr[6*3+1];

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

	if (write_mac_to_chip(offset, maddr)) {
		printf("Failed to write to eeprom.\n");
		return 1;
	}

	if (i2c_read(0x50, offset, 2, maddr_check, sizeof(maddr_check))) {
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

int do_macaddr ( cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	int ret = 0;

	switch (argc) {

	default:
		printf("Usage:\n%s\n", cmdtp->usage);
		ret = 1;
		break;

	case 1:
		ret |= read_macaddr(0);
		ret |= read_macaddr(8);
		break;


	case 2:
		ret |= write_macaddr(argv[1], 0);
		break;

	case 3:
		ret |= write_macaddr(argv[1], 0);
		ret |= write_macaddr(argv[2], 8);
		break;
	}

	return ret;
}

U_BOOT_CMD(
	macaddr,	3,	1,	do_macaddr,
	"macaddr  - display or store MAC addresses in eeprom\n",
	"[address] [address]\n"
	"       - address is of form xx:xx:xx:xx:xx:xx or xxxxxxxxxxxx\n"
);

#endif
