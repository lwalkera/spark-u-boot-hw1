
#include <common.h>
#include <command.h>
#include <asm/arch/pxa-regs.h>
#include <../drivers/net/smc91111.h>

int get_env_ethaddr(unsigned char * v_env_mac)
{
	int env_size, env_present = 0, reg;
	char *s = NULL, *e, es[] = "11:22:33:44:55:66";
	uchar s_env_mac[64];

	env_size = getenv_r ("ethaddr", s_env_mac, sizeof (s_env_mac));
	if ((env_size > 0) && (env_size < sizeof (es))) {	/* exit if env is bad */
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

void smc911x_read_mac(char *addr, const char *src, size_t size)
{
	memcpy(addr, src, size);
}
int smc_init(void) { return 1; }
void smc_close(void) {}

int do_macaddr ( cmd_tbl_t * cmdtp, int flag, int argc, char *argv[])
{
	char addr[6];
	char * argv1;
	int len, i;
	char ethaddr[6*3+1];

	ReloadE2P();
	if(argc < 2)
	{
		unsigned int __v;   

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
	SetMacReg(MAC_ADDRL, addr[0] | (addr[1] << 8) | (addr[2] << 16) | (addr[3] << 24));
	SetMacReg(MAC_ADDRH, addr[4] | (addr[5] << 8));
	printf("Successfully wrote MAC address %s to flash\n", ethaddr);
	return 0;
}

U_BOOT_CMD(
	macaddr,	2,	1,	do_macaddr,
	"macaddr  - display or store MAC address in flash\n",
	"[address]\n"
	"       - address is of form xx:xx:xx:xx:xx:xx or xxxxxxxxxxxx\n"
);
