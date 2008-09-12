
#include <common.h>
#include <asm/arch/pxa-regs.h>
#include "cmx270.h"

#ifdef CONFIG_PCMCIA_SLOT_A
extern void pxa_gpio_mode(int gpio_mode);

void pcmcia_power_on(void)
{
	GPSR(GPIO49_nPWE) = GPIO_bit(GPIO49_nPWE);
	pxa_gpio_mode(GPIO49_nPWE | GPIO_OUT);

	//reset card
	GPCR(GPIO49_nPWE) = GPIO_bit(GPIO49_nPWE);
	GPSR(GPIO53_nPCE_2) = GPIO_bit(GPIO53_nPCE_2);
	udelay(10);
	GPCR(GPIO53_nPCE_2) = GPIO_bit(GPIO53_nPCE_2);
	GPSR(GPIO49_nPWE) = GPIO_bit(GPIO49_nPWE);

	pxa_gpio_mode(GPIO49_nPWE_MD);
}

void pcmcia_power_off(void)
{
#if 0
	ADSBITSYXB_PCMCIA_CR &= ~(ADSBITSYXB_CR_CARDA_5V | ADSBITSYXB_CR_CARDA_3V);
#endif
}

#endif
