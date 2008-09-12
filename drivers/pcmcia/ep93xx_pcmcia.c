#include <common.h>
#include <config.h>

#ifdef CONFIG_EP93XX_PCMCIA

#include <pcmcia.h>
#include <asm/arch/ep93xx.h>
#include <asm/io.h>

static int hardware_enable (int slot)
{
	return 0;	/* No hardware to enable */
}

static int hardware_disable(int slot)
{
	return 0;	/* No hardware to disable */
}

static int voltage_set(int slot, int vcc, int vpp)
{
	return 0;
}

void msWait(unsigned msVal)
{
	udelay(msVal*1000);
}

int pcmcia_on (void)
{
	int rc;

	ulong gpiof;

	PFDDR = 0x00000000;

	if (PFDR & 0x06) {
		/* no drive inserted */
		return 1;
	}
	/* timing */
	PC1ATTRIB = CFG_PC1ATTRIB;
	PC1COMMON = CFG_PC1COMMON;
	PC1IO =     CFG_PC1IO;

#if defined(CONFIG_ADSSPHERE)
	void pcmcia_power_on(void);

	pcmcia_power_on();
#endif

	/* enable CF slot */
	PCCNTR = 0x01;

	udelay(1000);

	/* enable reset */
	PCCNTR = 0x15;
	udelay(300000);
	
	/* disable reset and use timing */
	PCCNTR = 0x11;
	udelay(200000);

	rc = check_ide_device (0);	/* use just slot 0 */

	return rc;
}

int pcmcia_off (void)
{
#if defined(CONFIG_ADSSPHERE)
	void pcmcia_power_off(void);
	pcmcia_power_off();
#endif
	return 0;
}

#endif /* CONFIG_EP93XX_PCMCIA */
