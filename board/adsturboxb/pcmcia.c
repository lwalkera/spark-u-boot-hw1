
#include <common.h>
#include <asm/arch/pxa-regs.h>

void pcmcia_power_on(void)
{
	//__REG(0x8000000) = 0x10;

	pxa_gpio_mode(81|GPIO_OUT); /*CARDAON*/
	pxa_gpio_mode(83|GPIO_OUT); /*CARDAON_5V*/
	pxa_gpio_mode(108|GPIO_OUT); /*CARDAON_5V*/
	pxa_gpio_mode(10|GPIO_OUT); /*CARDAON_5V*/
	pxa_gpio_mode(52|GPIO_IN); /*CARDAON_5V*/


	// configure PC Card interface
	GPSR(GPIO48_nPOE) = GPIO_bit(GPIO48_nPOE) |
		GPIO_bit(GPIO49_nPWE) |
		GPIO_bit(GPIO50_nPIOR) |
		GPIO_bit(GPIO51_nPIOW) |
		GPIO_bit(GPIO54_nPCE_2);
	GPSR(GPIO85_nPCE_1) =   GPIO_bit(GPIO85_nPCE_1);

	pxa_gpio_mode(GPIO48_nPOE_MD);
	pxa_gpio_mode(GPIO49_nPWE_MD);
	pxa_gpio_mode(GPIO50_nPIOR_MD);
	pxa_gpio_mode(GPIO51_nPIOW_MD);
	pxa_gpio_mode(GPIO85_nPCE_1_MD);
	pxa_gpio_mode(GPIO54_nPCE_2_MD);
	pxa_gpio_mode(GPIO79_pSKTSEL_MD);
	pxa_gpio_mode(GPIO55_nPREG_MD);
	pxa_gpio_mode(GPIO56_nPWAIT_MD);
	pxa_gpio_mode(GPIO57_nIOIS16_MD);
}

void pcmcia_power_off(void)
{
#if 0
	ADSBITSYXB_PCMCIA_CR &= ~(ADSBITSYXB_CR_CARDA_5V | ADSBITSYXB_CR_CARDA_3V);
#endif
}
