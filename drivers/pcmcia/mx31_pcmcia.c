#undef DEBUG
#include <common.h>
#include <command.h>

#ifdef CONFIG_MX31_PCMCIA

#include <pcmcia.h>
#include <asm/io.h>
#include <asm/arch/mx31-regs.h>
#include <asm/arch/iomux.h>

#ifndef CFG_PCMCIA_MEM_ADDR
#define CFG_PCMCIA_MEM_ADDR 0xbc000000
#endif
#ifndef CFG_PCMCIA_MEM_SIZE
#define CFG_PCMCIA_MEM_SIZE 0x1000000
#endif

extern void pcmcia_uninit (void);
extern int pcmcia_init(void );

int pcmcia_off(void)
{
	pcmcia_uninit ();
	printf ("Disable PCMCIA " PCMCIA_SLOT_MSG "\n");
	return 0;
}

extern void gpio_pcmcia_active(void);
extern int check_ide_device (int slot);

int pcmcia_on(void)
{
	debug ("Enable PCMCIA " PCMCIA_SLOT_MSG "\n");

	gpio_pcmcia_active();
	pcmcia_init();
#define PCMCIA_PGCR __REG(0xb8004060)
#define PCMCIA_PGSR __REG(0xb8004064)
#define PCMCIA_PSCR __REG(0xb8004004)
	writel(0x00, 0xb8004008);//PER

	writel(0x00, 0xb8004028);//POR0
	writel(0x00, 0xb800402c);//POR1
	writel(0x00, 0xb8004030);//POR2
#if 1
	writel(CFG_PCMCIA_MEM_ADDR, 0xb800400c);//PBR0
	writel(CFG_PCMCIA_MEM_ADDR+CFG_PCMCIA_MEM_SIZE, 0xb8004010);//PBR1
	writel(CFG_PCMCIA_MEM_ADDR+2*CFG_PCMCIA_MEM_SIZE, 0xb8004014);//PBR2
#endif

#if 0
	writel(0x240c31f4, 0xb8004028);//POR0 16 bits attribute space
	writel(0x2632baf4, 0xb800402c);//POR1 16 bits i/o space
	writel(0x20595554, 0xb8004030);//POR2 Common memory space. 16 bits port
#else
	writel(0x24F11974, 0xb8004028);//POR0 16 bits attribute space
	writel(0x26F11974, 0xb800402c);//POR1 16 bits i/o space
	writel(0x20F11974, 0xb8004030);//POR2 Common memory space. 16 bits port
#endif

	writel(0x00, 0xb8004044);//POFR0
	writel(0x00, 0xb8004048);//POFR1
	writel(0x00, 0xb800404c);//POFR2

	//	writel(0x02, 0xb8004060);//PGCR
	PCMCIA_PGSR = PCMCIA_PGSR;
	PCMCIA_PSCR = PCMCIA_PSCR;
	udelay(200000);
	PCMCIA_PGCR |= 0x1;
	udelay(300000);
	PCMCIA_PGCR &= ~(0x9);/*RESET and LPMEN*/
	PCMCIA_PGCR |= 0x2;/*Enable POE*/
	udelay(200000);

	PCMCIA_PGSR = PCMCIA_PGSR;
	udelay(200000);
	return check_ide_device(0);
}

#if 0
#define	MAX_TUPEL_SZ	512
#define MAX_FEATURES	4

extern char *indent ;
static int identify  (volatile uchar *p);
static int check_ide_device (int slot);
static void print_funcid (int func);
static void print_fixed (volatile uchar *p);

int ide_devices_found;

static int check_ide_device (int slot)
{
	volatile uchar *ident = NULL;
	volatile uchar *feature_p[MAX_FEATURES];
	volatile uchar *p, *start, *addr;
	int n_features = 0;
	uchar func_id = ~0;
	uchar code, len;
	ushort config_base = 0;
	int found = 0;
	int i;

	addr = (volatile uchar *)(CFG_PCMCIA_MEM_ADDR +
				  CFG_PCMCIA_MEM_SIZE * (slot * 4));

	start = p = (volatile uchar *) addr;

	while ((p - start) < MAX_TUPEL_SZ) {

		code = *p; p += 2;

		if (code == 0xFF) { /* End of chain */
			break;
		}

		len = *p; p += 2;
#ifdef DEBUG
		{ volatile uchar *q = p;
			printf ("\nTuple code %02x  length %d\n\tData:",
				code, len);

			for (i = 0; i < len; ++i) {
				printf ("%#x: %02x",q, *q);
				q+= 2;
			}
		}
#endif
		switch (code) {
		case CISTPL_VERS_1:
			ident = p + 4;
			break;
		case CISTPL_FUNCID:
			/* Fix for broken SanDisk which may have 0x80 bit set */
			func_id = *p & 0x7F;
			break;
		case CISTPL_FUNCE:
			if (n_features < MAX_FEATURES)
				feature_p[n_features++] = p;
			break;
		case CISTPL_CONFIG:
			config_base = (*(p+6) << 8) + (*(p+4));
			debug ("\n## Config_base = %04x ###\n", config_base);
		default:
			break;
		}
		p += 2 * len;
	}

	found = identify (ident);

	if (func_id != ((uchar)~0)) {
		print_funcid (func_id);

		if (func_id == CISTPL_FUNCID_FIXED)
			found = 1;
		else
			return (1);	/* no disk drive */
	}

	for (i=0; i<n_features; ++i) {
		print_fixed (feature_p[i]);
	}

	if (!found) {
		printf ("unknown card type\n");
		return (1);
	}

	ide_devices_found |= (1 << slot);

#if CONFIG_CPC45
#else
	/* set I/O area in config reg -> only valid for ARGOSY D5!!! */
	*((uchar *)(addr + config_base)) = 0x2;
#endif
#ifdef DEBUG
	printf("\n## Config_base = %04x ###\n", config_base);
	printf("Configuration Option Register: %02x @ %x\n", readb(addr + config_base), addr + config_base);
	printf("Card Configuration and Status Register: %02x\n", readb(addr + config_base + 2));
	printf("Pin Replacement Register Register: %02x\n", readb(addr + config_base + 4));
	printf("Socket and Copy Register: %02x\n", readb(addr + config_base + 6));
#endif
	return (0);
}

#define MAX_IDENT_CHARS		64
#define	MAX_IDENT_FIELDS	4

static uchar *known_cards[] = {
	(uchar *)"ARGOSY PnPIDE D5",
	NULL
};

static int identify  (volatile uchar *p)
{
	uchar id_str[MAX_IDENT_CHARS];
	uchar data;
	uchar *t;
	uchar **card;
	int i, done;

	if (p == NULL)
		return (0);	/* Don't know */

	t = id_str;
	done =0;

	for (i=0; i<=4 && !done; ++i, p+=2) {
		while ((data = *p) != '\0') {
			if (data == 0xFF) {
				done = 1;
				break;
			}
			*t++ = data;
			if (t == &id_str[MAX_IDENT_CHARS-1]) {
				done = 1;
				break;
			}
			p += 2;
		}
		if (!done)
			*t++ = ' ';
	}
	*t = '\0';
	while (--t > id_str) {
		if (*t == ' ')
			*t = '\0';
		else
			break;
	}
	puts ((char *)id_str);
	putc ('\n');

	for (card=known_cards; *card; ++card) {
		debug ("## Compare against \"%s\"\n", *card);
		if (strcmp((char *)*card, (char *)id_str) == 0) {	/* found! */
			debug ("## CARD FOUND ##\n");
			return (1);
		}
	}

	return (0);	/* don't know */
}

static void print_funcid (int func)
{
	puts (indent);
	switch (func) {
	case CISTPL_FUNCID_MULTI:
		puts (" Multi-Function");
		break;
	case CISTPL_FUNCID_MEMORY:
		puts (" Memory");
		break;
	case CISTPL_FUNCID_SERIAL:
		puts (" Serial Port");
		break;
	case CISTPL_FUNCID_PARALLEL:
		puts (" Parallel Port");
		break;
	case CISTPL_FUNCID_FIXED:
		puts (" Fixed Disk");
		break;
	case CISTPL_FUNCID_VIDEO:
		puts (" Video Adapter");
		break;
	case CISTPL_FUNCID_NETWORK:
		puts (" Network Adapter");
		break;
	case CISTPL_FUNCID_AIMS:
		puts (" AIMS Card");
		break;
	case CISTPL_FUNCID_SCSI:
		puts (" SCSI Adapter");
		break;
	default:
		puts (" Unknown");
		break;
	}
	puts (" Card\n");
}

static void print_fixed (volatile uchar *p)
{
	if (p == NULL)
		return;

	puts(indent);

	switch (*p) {
	case CISTPL_FUNCE_IDE_IFACE:
	    {   uchar iface = *(p+2);

		puts ((iface == CISTPL_IDE_INTERFACE) ? " IDE" : " unknown");
		puts (" interface ");
		break;
	    }
	case CISTPL_FUNCE_IDE_MASTER:
	case CISTPL_FUNCE_IDE_SLAVE:
	    {   uchar f1 = *(p+2);
		uchar f2 = *(p+4);

		puts ((f1 & CISTPL_IDE_SILICON) ? " [silicon]" : " [rotating]");

		if (f1 & CISTPL_IDE_UNIQUE)
			puts (" [unique]");

		puts ((f1 & CISTPL_IDE_DUAL) ? " [dual]" : " [single]");

		if (f2 & CISTPL_IDE_HAS_SLEEP)
			puts (" [sleep]");

		if (f2 & CISTPL_IDE_HAS_STANDBY)
			puts (" [standby]");

		if (f2 & CISTPL_IDE_HAS_IDLE)
			puts (" [idle]");

		if (f2 & CISTPL_IDE_LOW_POWER)
			puts (" [low power]");

		if (f2 & CISTPL_IDE_REG_INHIBIT)
			puts (" [reg inhibit]");

		if (f2 & CISTPL_IDE_HAS_INDEX)
			puts (" [index]");

		if (f2 & CISTPL_IDE_IOIS16)
			puts (" [IOis16]");

		break;
	    }
	}
	putc ('\n');
}
#endif

#endif //~~CONFIG_MX31_PCMCIA
