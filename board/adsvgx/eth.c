
#include <common.h>
#include <command.h>
#include <asm/arch/pxa-regs.h>
#include "adsvgx.h"
#include <../drivers/net/smc91111.h>

static void read_mac_from_chip(char maddr[6]);

#define SMC_BASE_ADDRESS (PXA_CS4_PHYS + 0x300)

int bcmp(const char *s1, const char *s2, size_t n)
{
	while (*s1++ == *s2++ && --n) ;

	return n;
}

void smc_init(void)
{
	char maddr[6];
	char ethaddr[6*3+1];

	read_mac_from_chip(maddr);

	sprintf(ethaddr, "%0.2x:%0.2x:%0.2x:%0.2x:%0.2x:%0.2x",
		maddr[0], maddr[1], maddr[2], maddr[3], maddr[4], maddr[5]);
	setenv("ethaddr", ethaddr);
}


static int read_eeprom_reg (int reg)
{
	int timeout;

	SMC_SELECT_BANK (2);
	SMC_outw (reg, PTR_REG);

	SMC_SELECT_BANK (1);
	SMC_outw (SMC_inw (CTL_REG) | CTL_EEPROM_SELECT | CTL_RELOAD,
		  CTL_REG);
	timeout = 100;
	while ((SMC_inw (CTL_REG) & CTL_RELOAD) && --timeout)
		udelay (100);
	if (timeout == 0) {
		printf ("Timeout Reading EEPROM register %02x\n", reg);
		return 0;
	}

	return SMC_inw (GP_REG);

}

static int write_eeprom_reg (int value, int reg)
{
	int timeout;

	SMC_SELECT_BANK (2);
	SMC_outw (reg, PTR_REG);

	SMC_SELECT_BANK (1);
	SMC_outw (value, GP_REG);
	SMC_outw (SMC_inw (CTL_REG) | CTL_EEPROM_SELECT | CTL_STORE, CTL_REG);
	timeout = 100;
	while ((SMC_inw (CTL_REG) & CTL_STORE) && --timeout)
		udelay (100);
	if (timeout == 0) {
		printf ("Timeout Writing EEPROM register %02x\n", reg);
		return 0;
	}

	return 1;

}

static void write_mac_to_chip(char maddr[6])
{
	write_eeprom_reg(maddr[0] | (maddr[1] << 8), 0x20);
	write_eeprom_reg(maddr[2] | (maddr[3] << 8), 0x21);
	write_eeprom_reg(maddr[4] | (maddr[5] << 8), 0x22);
}

static void read_mac_from_chip(char maddr[6])
{
	int val;
	val = read_eeprom_reg(0x20);
	maddr[0] = val & 0xff;
	maddr[1] = (val >> 8) & 0xff;
	val = read_eeprom_reg(0x21);
	maddr[2] = val & 0xff;
	maddr[3] = (val >> 8) & 0xff;
	val = read_eeprom_reg(0x22);
	maddr[4] = val & 0xff;
	maddr[5] = (val >> 8) & 0xff;
}

int do_macaddr ( cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	int len, i;
	char *argv1 = argv[1];
	char maddr[6];
	char maddr_check[6];
	char ethaddr[6*3+1];

	if (argc > 2) {
		printf("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

	smc_init();

	if (argc == 1) {

		read_mac_from_chip(maddr);

		printf("MAC address: %0.2x:%0.2x:%0.2x:%0.2x:%0.2x:%0.2x\n",
		       maddr[0], maddr[1], maddr[2], maddr[3], maddr[4], maddr[5]);
		return 0;
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
		maddr[i] = argv1[2*i] << 4 | argv1[2*i+1];
	}
	

	write_mac_to_chip(maddr);

	read_mac_from_chip(maddr_check);

	if (bcmp(maddr, maddr_check, 6)) {
		printf("Failed to read same MAC address back from eeprom\n");
		return 1;
	}

	sprintf(ethaddr, "%0.2x:%0.2x:%0.2x:%0.2x:%0.2x:%0.2x",
		maddr[0], maddr[1], maddr[2], maddr[3], maddr[4], maddr[5]);
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
