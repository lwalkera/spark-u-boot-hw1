#include <common.h>
#include <command.h>
#include <asm/global_data.h>
#include <../drivers/net/smc91111.h>
#include <../drivers/net/smsc9118.h>

DECLARE_GLOBAL_DATA_PTR;

#ifdef CONFIG_SMC91111_BASE
#define SMC_BASE_ADDRESS CONFIG_SMC91111_BASE

int smc_init(void);

int bcmp(const char *s1, const char *s2, size_t n)
{
	while (*s1++ == *s2++ && --n) ;

	return n;
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

#ifndef CONFIG_MULTI_SMCETH
int do_macaddr ( cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
#else
int smc9111_do_macaddr ( cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
#endif
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
#elif CONFIG_SMSC9118_BASE
#define SMC_BASE_ADDRESS CONFIG_SMSC9118_BASE
#endif //~~CONFIG_SMC91111_BASE

extern void ide_set_reset(int idereset);
extern void ide_power (int on);

enum __smc_eth_type
{
	ETH_UNKNOW,
	ETH_SMC9111,
	ETH_SMC9118,
	ETH_NONE
} ads_eth_type;

/*
   return 0 exist, -1 not exitst.
 */
inline static int smc_exist(void)
{
	if((*BYTE_TEST) == BYTE_TEST_VAL)
		ads_eth_type = ETH_SMC9118;
	else if ((SMC_inw (BANK_SELECT)&0xff00) == 0x3300)
		ads_eth_type = ETH_SMC9111;
	else ads_eth_type = ETH_NONE;

	return ads_eth_type;
}

#if defined(CONFIG_DRIVER_SMSC9118)
int SetMacReg(int Reg, ulong Value);
ulong GetMacReg(int Reg);
int EnableE2P(void);
int E2PSaveMAC(unsigned int addrl, unsigned int addrh);
int ReloadE2P(void);

#define MAC_ADDRH		  			(0x2)
#define MAC_ADDRL		  			(0x3)
static inline int is_multicast_ether_addr(const u8 *addr)
{
	return (0x01 & addr[0]);
}

static inline int is_zero_ether_addr(const u8 *addr)
{
	return !(addr[0] | addr[1] | addr[2] | addr[3] | addr[4] | addr[5]);
}

static inline int is_valid_ether_addr(const u8 *addr)
{
	/* FF:FF:FF:FF:FF:FF is a multicast address so we don't need to
	 * explicitly check for it here. */
	return (!is_multicast_ether_addr(addr)) && (!is_zero_ether_addr(addr));
}

static int get_env_ethaddr(unsigned char * v_env_mac)
{
	int env_size, env_present = 0, reg;
	char *s = NULL, *e, es[] = "11:22:33:44:55:66";
	uchar s_env_mac[64];

	env_size = getenv_r ("ethaddr", s_env_mac, sizeof (s_env_mac));
	if ((env_size > 0) && (env_size < sizeof (es))) {   /* exit if env is bad */
		printf ("\n*** ERROR: ethaddr is not set properly!!\n");
		return (-1);
	}

	if (env_size > 0) {
		env_present = 1;
		s = s_env_mac;
	}

	for (reg = 0; reg < 6; ++reg) { /* turn string into mac value */
		v_env_mac[reg] = s ? simple_strtoul (s, &e, 16) : 0;
		if (s)
			s = (*e) ? e + 1 : e;
	}

	return (env_present);
}

void smc911x_read_mac(char *addr, const char *src, size_t size)
{
	int i;
	unsigned int __v;
	bd_t *bd = gd->bd;

	smc_init();
	__v = GetMacReg(MAC_ADDRL);
	addr[0] = __v; addr[1] = __v >> 8;
	addr[2] = __v >> 16; addr[3] = __v >> 24;
	__v = GetMacReg(MAC_ADDRH);
	addr[4] = __v; addr[5] = __v >> 8;
	if(!is_valid_ether_addr(addr))
	{
		printf("WARN:[");
		for (i = 0; i < 5; i++)
			printf("%2.2x:", addr[i]);
		printf("%2.2x", addr[5]);
		printf("] is invalid ether address, read from flash instead...\n");
		get_env_ethaddr(addr);
	}
	memcpy(bd->bi_enetaddr, addr, size);
	return ;
}

#ifndef CONFIG_MULTI_SMCETH
int do_macaddr ( cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
#else
int smc9118_do_macaddr ( cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
#endif
{
	char addr[6];
	char * argv1;
	int len, i;
	char ethaddr[6*3+1];

	smc_init();

	ReloadE2P();
	if(argc < 2)
	{
		smc911x_read_mac(addr, NULL, 6);
		printf("MAC addr=");
		for (i = 0; i < 5; i++)
			printf("%2.2x:", addr[i]);
		printf("%2.2x\n", addr[5]);
		return 0;
	}

	argv1 = argv[1];
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
		addr[i] = argv1[2*i] << 4 | argv1[2*i+1];
	}
	
	sprintf(ethaddr, "%0.2x:%0.2x:%0.2x:%0.2x:%0.2x:%0.2x",
		addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);


	EnableE2P();
	E2PSaveMAC((*(unsigned int *)addr), (*(unsigned int *)(addr+4)));

	setenv("ethaddr", ethaddr);
	run_command("saveenv", 0);
	SetMacReg(MAC_ADDRL, addr[0] | (addr[1] << 8) | (addr[2] << 16) | (addr[3] << 24));
	SetMacReg(MAC_ADDRH, addr[4] | (addr[5] << 8));
	printf("Successfully wrote MAC address %s to flash\n", ethaddr);
	return 0;
}

#endif //~~defined(CONFIG_DRIVER_SMSC9118)

#include <asm/arch/mx31-regs.h>
#include "board.h"
int smc_init(void)
{
	ide_power(1);//power on 3.3V
	udelay(10000);
	ide_set_reset(1);
	udelay(1000);
	ide_set_reset(0);
	udelay(20000);
	return smc_exist();
}

void smc_close(void )
{
	ide_power(0);
}

#if 0
int do_eth_reset ( cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	smc_init();
}

U_BOOT_CMD(
	eth_reset,	1,	1,	do_eth_reset,
	"eth_reset  - reset ether card.\n",
	"\n"
	"       - reset ether card\n"
);
#endif

#ifdef CONFIG_MULTI_SMCETH
int	(* smc_eth_init)(bd_t *bd);
void (* smc_eth_halt)(void);
int	(* smc_eth_rx)(void);
int	(* smc_eth_send)(volatile void *packet, int length);
int (* smc_do_macaddr) ( cmd_tbl_t * cmdtp, int flag, int argc, char *argv[]);

extern int	smc9111_eth_init(bd_t *bd);
extern void smc9111_eth_halt(void);
extern int	smc9111_eth_rx(void);
extern int	smc9111_eth_send(volatile void *packet, int length);

extern int	smc9118_eth_init(bd_t *bd);
extern void smc9118_eth_halt(void);
extern int	smc9118_eth_rx(void);
extern int	smc9118_eth_send(volatile void *packet, int length);

int	eth_init(bd_t *bd)
{
	if(smc_eth_init)
		return smc_eth_init(bd);
	else
		return -1;
}

void eth_halt(void)
{
	if(smc_eth_halt)
		smc_eth_halt();
}

int	eth_rx(void)
{
	if(smc_eth_rx)
		return smc_eth_rx();
	else
		return -1;
}

int	eth_send(volatile void *packet, int length)
{
	if(smc_eth_send)
		return smc_eth_send(packet, length);
	else
		return -1;
}

int do_macaddr ( cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	if(smc_do_macaddr)
		return smc_do_macaddr(cmdtp, flag, argc, argv);
	else
		return -1;
}

void multi_smc_eth_probe(void )
{
	switch( smc_init())
	{
		case ETH_SMC9111:
			smc_eth_init = smc9111_eth_init;
			smc_eth_halt = smc9111_eth_halt;
			smc_eth_rx = smc9111_eth_rx;
			smc_eth_send = smc9111_eth_send;
			smc_do_macaddr = smc9111_do_macaddr;
			break;

		case ETH_SMC9118:
			smc_eth_init = smc9118_eth_init;
			smc_eth_halt = smc9118_eth_halt;
			smc_eth_rx = smc9118_eth_rx;
			smc_eth_send = smc9118_eth_send;
			smc_do_macaddr = smc9118_do_macaddr;
			break;
		default:
			printf("NO ether chip found!\n");
	}
}
#endif

U_BOOT_CMD(
	macaddr,	2,	1,	do_macaddr,
	"macaddr  - display or store MAC address in eeprom/flash\n",
	"[address]\n"
	"       - address is of form xx:xx:xx:xx:xx:xx or xxxxxxxxxxxx\n"
);
