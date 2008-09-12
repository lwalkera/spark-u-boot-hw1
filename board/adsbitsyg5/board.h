#ifndef __ADSBIGSYG5_BOAD_H
#define __ADSBIGSYG5_BOAD_H

#include <asm/arch/mx31-regs.h>

#define ADSBITSYG5_CPLD_PHYS    (CS4_BASE_ADDR)
# define __ADS_REG(x)       (*((volatile u8 *)(x)))

#define SDRAM_BASE_ADDR             CSD0_BASE_ADDR
#define SDRAM_SIZE                  0x08000000

#define __CPLD_REV              0x00
#define ADSBITSYG5_CPLD_REV     __ADS_REG(ADSBITSYG5_CPLD_PHYS+__CPLD_REV)

#define __BOARD_CONTROL0        0x20
#define ADSBITSYG5_CONTROL0 __ADS_REG(ADSBITSYG5_CPLD_PHYS+__BOARD_CONTROL0)
#define ADSBITSYG5_COM2_ON  (1<<7)
#define ADSBITSYG5_PS2_ON   (1<<6)
#define ADSBITSYG5_IRDA_ON  (1<<5)
#define ADSBITSYG5_USB_RECONN   (1<<4)
#define ADSBITSYG5_COM13_ON (1<<2)
#define ADSBITSYG5_AUDIOPA_ON   (1<<0)

#define __BOARD_CONTROL1        0x28
#define ADSBITSYG5_CONTROL1 __ADS_REG(ADSBITSYG5_CPLD_PHYS+__BOARD_CONTROL1)
#define ADSBITSYG5_SPI_SEL2 (1<<5)
#define ADSBITSYG5_SPI_SEL1 (1<<4)
#define ADSBITSYG5_VEE_ON   (1<<1)
#define ADSBITSYG5_PANEL_ON (1<<0)

#define __BOARD_CONTROL2        0x30
#define ADSBITSYG5_CONTROL2 __ADS_REG(ADSBITSYG5_CPLD_PHYS+__BOARD_CONTROL2)
#define ADSBITSYG5_AVR_WAKEUPKP (1<<6)
#define ADSBITSYG5_AVR_WAKEUP   (1<<2)
#define ADSBITSYG5_AVR_RESET    (1<<1)

#define __PCMCIA_CONTROL        0x60
#define ADSBITSYG5_PCMCIA       __ADS_REG(ADSBITSYG5_CPLD_PHYS+__PCMCIA_CONTROL)
#define ADSBITSYG5_RST_CF       (1<<4) /**/
#define ADSBITSYG5_PCMCIA_5V    (0x2<<2)
#define ADSBITSYG5_PCMCIA_OFF   (0x0<<2)
#define ADSBITSYG5_PCMCIA_33V   (0x1<<2)

#define ADSBITSYG5_CF_OFF       (0x0)
#define ADSBITSYG5_CF_33V       (0x1)
#define ADSBITSYG5_CF_5V        (0x2)


#ifndef __ASSEMBLY__

#include <asm/arch/iomux.h>
#define ADSBITSYG5_LCD_ENABLE_PIN   MX31_PIN_SD_D_I
#define ADSBITSYG5_USBHSH_ENABLE MX31_PIN_SCK6 /*GPIO1_25*/
#define ADSBITSYG5_LED0_PIN MX31_PIN_LCS0
static inline void LED0OFF(void)
{
	mxc_set_gpio_dataout(ADSBITSYG5_LED0_PIN, GPIO_LOW_LEVEL);/*Light off led0*/
}

static inline void LED0ON(void)
{
	mxc_set_gpio_dataout(ADSBITSYG5_LED0_PIN, GPIO_HIGH_LEVEL);/*Light off led0*/
}
#endif


#endif/*__ADSBIGSYG5_BOAD_H*/
