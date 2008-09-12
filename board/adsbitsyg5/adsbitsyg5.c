/*
 * (C) Copyright 2004
 * Texas Instruments, <www.ti.com>
 * Richard Woodruff <r-woodruff2@ti.com>
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
#include <asm/arch/mx31-regs.h>
#include <asm/io.h>
#include <asm/arch/bits.h>
#include <asm/arch/mux.h>
#include <asm/arch/sys_proto.h>
#include <asm/arch/sys_info.h>
#include <asm/arch/mem.h>
#include <i2c.h>
#include <asm/mach-types.h>
#include <asm/arch/iomux.h>
#include "board.h"

int pmic_write_reg(int , unsigned int *);

static void board_gpio_init(void)
{
	/* config CS4 */
	iomux_config_mux(MX31_PIN_CS4, OUTPUTCONFIG_FUNC, INPUTCONFIG_FUNC);

	/*enable ISP1504 clock*/
	iomux_config_mux(ADSBITSYG5_USBHSH_ENABLE, OUTPUTCONFIG_GPIO, INPUTCONFIG_GPIO);
	mxc_set_gpio_direction(ADSBITSYG5_USBHSH_ENABLE, GPIO_OUTPUT);/*set output*/
	mxc_set_gpio_dataout(ADSBITSYG5_USBHSH_ENABLE, GPIO_LOW_LEVEL);/*enable ISP1504 set low*/

	iomux_config_mux(ADSBITSYG5_LCD_ENABLE_PIN, OUTPUTCONFIG_GPIO, INPUTCONFIG_NONE);
	mxc_set_gpio_direction(ADSBITSYG5_LCD_ENABLE_PIN, GPIO_OUTPUT);
	mxc_set_gpio_dataout(ADSBITSYG5_LCD_ENABLE_PIN, GPIO_LOW_LEVEL);

	iomux_config_mux(ADSBITSYG5_LED0_PIN, OUTPUTCONFIG_GPIO, INPUTCONFIG_NONE);
	mxc_set_gpio_direction(ADSBITSYG5_LED0_PIN, GPIO_OUTPUT);
	LED0OFF();
}

void uart_gpio_init(int port)
{
	switch (port) 
	{
		/* UART 2 IOMUX Configs */
		case 2:
			ADSBITSYG5_CONTROL0 |= ADSBITSYG5_COM2_ON;

			iomux_config_mux(MX31_PIN_TXD2, OUTPUTCONFIG_FUNC,
					INPUTCONFIG_FUNC);
			iomux_config_mux(MX31_PIN_RXD2, OUTPUTCONFIG_FUNC,
					INPUTCONFIG_FUNC);
			iomux_config_mux(MX31_PIN_RTS2, OUTPUTCONFIG_FUNC,
					INPUTCONFIG_FUNC);
			iomux_config_mux(MX31_PIN_CTS2, OUTPUTCONFIG_FUNC,
					INPUTCONFIG_FUNC);
			iomux_config_mux(MX31_PIN_DTR_DCE2, OUTPUTCONFIG_FUNC,
                INPUTCONFIG_FUNC);

			break;
		case 3:
			ADSBITSYG5_CONTROL0 |= ADSBITSYG5_COM13_ON;
			iomux_config_mux(MX31_PIN_CSPI3_MOSI, OUTPUTCONFIG_ALT1,
					INPUTCONFIG_ALT1);
			iomux_config_mux(MX31_PIN_CSPI3_MISO, OUTPUTCONFIG_ALT1,
					INPUTCONFIG_ALT1);
#if 0
			iomux_config_mux(MX31_PIN_CSPI3_SCLK, OUTPUTCONFIG_ALT1,
					INPUTCONFIG_ALT1);
			iomux_config_mux(MX31_PIN_CSPI3_SPI_RDY, OUTPUTCONFIG_ALT1,
					INPUTCONFIG_ALT1);
#else
			iomux_config_mux(MX31_PIN_CSPI3_SCLK, OUTPUTCONFIG_FUNC,
					INPUTCONFIG_FUNC);
			iomux_config_mux(MX31_PIN_CSPI3_SPI_RDY, OUTPUTCONFIG_FUNC,
					INPUTCONFIG_FUNC);
#endif
			break;
		default:

		case 1:
			/* UART 1 IOMUX Configs */
			ADSBITSYG5_CONTROL0 |= ADSBITSYG5_COM13_ON;

			iomux_config_mux(MX31_PIN_RXD1, OUTPUTCONFIG_FUNC,
					INPUTCONFIG_FUNC);
			iomux_config_mux(MX31_PIN_TXD1, OUTPUTCONFIG_FUNC,
					INPUTCONFIG_FUNC);
#if 0
			iomux_config_mux(MX31_PIN_RTS1, OUTPUTCONFIG_FUNC,
					INPUTCONFIG_FUNC);
			iomux_config_mux(MX31_PIN_CTS1, OUTPUTCONFIG_FUNC,
					INPUTCONFIG_FUNC);
#else
			mxc_set_gpio_direction(MX31_PIN_RTS1, GPIO_INPUT);
			mxc_set_gpio_direction(MX31_PIN_CTS1, GPIO_INPUT);
			iomux_config_mux(MX31_PIN_RTS1, OUTPUTCONFIG_GPIO,
					INPUTCONFIG_GPIO);
			iomux_config_mux(MX31_PIN_CTS1, OUTPUTCONFIG_GPIO,
					INPUTCONFIG_GPIO);
#endif
			iomux_config_mux(MX31_PIN_DTR_DCE1, OUTPUTCONFIG_FUNC,
					INPUTCONFIG_FUNC);
			iomux_config_mux(MX31_PIN_DSR_DCE1, OUTPUTCONFIG_FUNC,
					INPUTCONFIG_FUNC);
			iomux_config_mux(MX31_PIN_RI_DCE1, OUTPUTCONFIG_FUNC,
					INPUTCONFIG_FUNC);
			iomux_config_mux(MX31_PIN_DCD_DCE1, OUTPUTCONFIG_FUNC,
					INPUTCONFIG_FUNC);
			break;
	}//~~switch().
}//~~uart_gpio_init()

void gpio_usb_active(void )
{
	iomux_config_mux(MX31_PIN_USBH2_CLK, OUTPUTCONFIG_FUNC, INPUTCONFIG_FUNC);
	iomux_config_mux(MX31_PIN_USBH2_DIR, OUTPUTCONFIG_FUNC, INPUTCONFIG_FUNC);
	iomux_config_mux(MX31_PIN_USBH2_NXT, OUTPUTCONFIG_FUNC, INPUTCONFIG_FUNC);
	iomux_config_mux(MX31_PIN_USBH2_STP, OUTPUTCONFIG_FUNC, INPUTCONFIG_FUNC) ;
	iomux_config_mux(MX31_PIN_USBH2_DATA0, OUTPUTCONFIG_FUNC, INPUTCONFIG_FUNC) ;
	iomux_config_mux(MX31_PIN_USBH2_DATA1, OUTPUTCONFIG_FUNC, INPUTCONFIG_FUNC) ;
	iomux_config_mux(MX31_PIN_STXD3, OUTPUTCONFIG_GPIO, INPUTCONFIG_NONE);
	iomux_config_mux(MX31_PIN_SRXD3, OUTPUTCONFIG_GPIO, INPUTCONFIG_NONE);
	iomux_config_mux(MX31_PIN_SCK3,	OUTPUTCONFIG_GPIO, INPUTCONFIG_NONE);
	iomux_config_mux(MX31_PIN_SFS3,	OUTPUTCONFIG_GPIO, INPUTCONFIG_NONE);
	iomux_config_mux(MX31_PIN_STXD6, OUTPUTCONFIG_GPIO, INPUTCONFIG_NONE);
	iomux_config_mux(MX31_PIN_SRXD6, OUTPUTCONFIG_GPIO, INPUTCONFIG_NONE);

#define H2_PAD_CFG (PAD_CTL_DRV_MAX | PAD_CTL_SRE_FAST | PAD_CTL_HYS_CMOS | PAD_CTL_ODE_CMOS | PAD_CTL_100K_PU)
	mxc_iomux_set_pad(MX31_PIN_USBH2_CLK, H2_PAD_CFG);
	mxc_iomux_set_pad(MX31_PIN_USBH2_DIR, H2_PAD_CFG);
	mxc_iomux_set_pad(MX31_PIN_USBH2_NXT, H2_PAD_CFG);
	mxc_iomux_set_pad(MX31_PIN_USBH2_STP, H2_PAD_CFG);
	mxc_iomux_set_pad(MX31_PIN_USBH2_DATA0, H2_PAD_CFG);
	mxc_iomux_set_pad(MX31_PIN_USBH2_DATA1, H2_PAD_CFG);
	mxc_iomux_set_pad(MX31_PIN_SRXD6, H2_PAD_CFG);
	mxc_iomux_set_pad(MX31_PIN_STXD6, H2_PAD_CFG);
	mxc_iomux_set_pad(MX31_PIN_SFS3, H2_PAD_CFG);
	mxc_iomux_set_pad(MX31_PIN_SCK3, H2_PAD_CFG);
	mxc_iomux_set_pad(MX31_PIN_SRXD3, H2_PAD_CFG);
	mxc_iomux_set_pad(MX31_PIN_STXD3, H2_PAD_CFG);	
#undef H2_PAD_CFG
	mxc_iomux_set_gpr(MUX_PGP_UH2, 1);
}

void gpio_spi_active(int mod)
{
	switch (mod) {
	case 0:
		/* SPI1 */
		iomux_config_mux(MX31_PIN_CSPI1_MISO, OUTPUTCONFIG_FUNC,
				  INPUTCONFIG_FUNC);
		iomux_config_mux(MX31_PIN_CSPI1_MOSI, OUTPUTCONFIG_FUNC,
				  INPUTCONFIG_FUNC);
		iomux_config_mux(MX31_PIN_CSPI1_SCLK, OUTPUTCONFIG_FUNC,
				  INPUTCONFIG_FUNC);
		iomux_config_mux(MX31_PIN_CSPI1_SPI_RDY, OUTPUTCONFIG_FUNC,
				  INPUTCONFIG_FUNC);
		iomux_config_mux(MX31_PIN_CSPI1_SS0, OUTPUTCONFIG_FUNC,
				  INPUTCONFIG_FUNC);
		iomux_config_mux(MX31_PIN_CSPI1_SS1, OUTPUTCONFIG_FUNC,
				  INPUTCONFIG_FUNC);
		iomux_config_mux(MX31_PIN_CSPI1_SS2, OUTPUTCONFIG_FUNC,
				  INPUTCONFIG_FUNC);
		break;
	case 1:
		/* SPI2 */
		iomux_config_mux(MX31_PIN_CSPI2_MISO, OUTPUTCONFIG_FUNC,
				  INPUTCONFIG_FUNC);
		iomux_config_mux(MX31_PIN_CSPI2_MOSI, OUTPUTCONFIG_FUNC,
				  INPUTCONFIG_FUNC);
		iomux_config_mux(MX31_PIN_CSPI2_SCLK, OUTPUTCONFIG_FUNC,
				  INPUTCONFIG_FUNC);
		iomux_config_mux(MX31_PIN_CSPI2_SPI_RDY, OUTPUTCONFIG_FUNC,
				  INPUTCONFIG_FUNC);
		iomux_config_mux(MX31_PIN_CSPI2_SS2, OUTPUTCONFIG_FUNC,
				  INPUTCONFIG_FUNC);
		break;
	case 2:
		/* SPI3 */
		iomux_config_mux(MX31_PIN_CSPI3_MOSI, OUTPUTCONFIG_FUNC,
				  INPUTCONFIG_FUNC);
		iomux_config_mux(MX31_PIN_CSPI3_MISO, OUTPUTCONFIG_FUNC,
				  INPUTCONFIG_FUNC);
		iomux_config_mux(MX31_PIN_CSPI3_SCLK, OUTPUTCONFIG_FUNC,
				  INPUTCONFIG_FUNC);
		iomux_config_mux(MX31_PIN_CSPI3_SPI_RDY, OUTPUTCONFIG_FUNC,
				  INPUTCONFIG_FUNC);
		iomux_config_mux(MX31_PIN_CSPI2_SS0, OUTPUTCONFIG_ALT1,
				  INPUTCONFIG_ALT1);
		iomux_config_mux(MX31_PIN_CSPI2_SS1, OUTPUTCONFIG_ALT1,
				  INPUTCONFIG_ALT1);
		break;
	default:
		break;
	}
}

int board_init (void)
{
	DECLARE_GLOBAL_DATA_PTR;

	/* in SRAM or SDRM, finish GPMC */
	gd->bd->bi_arch_number = MACH_TYPE_ADSBITSYG5;		/* board id for linux */
	/* adress of boot parameters */
	gd->bd->bi_boot_params = (CSD0_BASE_ADDR+0x100);	

	board_gpio_init();

	return 0;
}

int dram_init (void)
{
	DECLARE_GLOBAL_DATA_PTR;
	gd->bd->bi_dram[0].start = PHYS_SDRAM_1;
	gd->bd->bi_dram[0].size = PHYS_SDRAM_1_SIZE;
	return 0;
}

#ifdef CONFIG_SHOW_BOOT_PROGRESS
void show_boot_progress(int arg)
{
  printf("boot progress: %d\n", arg);
}
#endif

void sdhc_gpio_active(int index)
{
	iomux_config_mux(MX31_PIN_SD1_CLK, OUTPUTCONFIG_FUNC,
			INPUTCONFIG_FUNC);
	iomux_config_mux(MX31_PIN_SD1_CMD, OUTPUTCONFIG_FUNC,
			INPUTCONFIG_FUNC);
	iomux_config_mux(MX31_PIN_SD1_DATA0, OUTPUTCONFIG_FUNC,
			INPUTCONFIG_FUNC);
	iomux_config_mux(MX31_PIN_SD1_DATA1, OUTPUTCONFIG_FUNC,
			INPUTCONFIG_FUNC);
	iomux_config_mux(MX31_PIN_SD1_DATA2, OUTPUTCONFIG_FUNC,
			INPUTCONFIG_FUNC);
	iomux_config_mux(MX31_PIN_SD1_DATA3, OUTPUTCONFIG_FUNC,
			INPUTCONFIG_FUNC);

	mxc_iomux_set_pad(MX31_PIN_SD1_CLK,
			(PAD_CTL_DRV_MAX | PAD_CTL_SRE_FAST));
	mxc_iomux_set_pad(MX31_PIN_SD1_CMD,
			(PAD_CTL_DRV_MAX | PAD_CTL_SRE_FAST));
	mxc_iomux_set_pad(MX31_PIN_SD1_DATA0,
			(PAD_CTL_DRV_MAX | PAD_CTL_SRE_FAST));
	mxc_iomux_set_pad(MX31_PIN_SD1_DATA1,
			(PAD_CTL_DRV_MAX | PAD_CTL_SRE_FAST));
	mxc_iomux_set_pad(MX31_PIN_SD1_DATA2,
			(PAD_CTL_DRV_MAX | PAD_CTL_SRE_FAST));
	mxc_iomux_set_pad(MX31_PIN_SD1_DATA3,
			(PAD_CTL_DRV_MAX | PAD_CTL_SRE_FAST));

	/*SDHC power on GPIO*/
	iomux_config_mux(MX31_PIN_ATA_CS1, OUTPUTCONFIG_GPIO, INPUTCONFIG_NONE);
	mxc_set_gpio_direction(MX31_PIN_ATA_CS1, 0);/*set as output*/
}

void sdhc_power(int index, int on)
{ 
	/*SD PWMEN set low*/
	if(on)
		mxc_set_gpio_dataout(MX31_PIN_ATA_CS1, 0);
	else
		mxc_set_gpio_dataout(MX31_PIN_ATA_CS1, 1);
}

int mmc_board_init(unsigned long sdhc)
{
    sdhc_gpio_active(0);
	sdhc_power(0, 1);	
	return 0;
}
	
#ifdef CONFIG_USB_EHCI

#include <asm/arch/arc_otg.h>

static int ehci_platform_set_host_mode(void )
{
	UH2_USBMODE |= USBMODE_CM_HOST;
	return 0;
}

static int usb_set_ulpi_xcvr(void )
{   
	USBCTRL &= ~(UCTRL_H2SIC_MASK | UCTRL_BPE); /* disable bypass mode */
	USBCTRL |= UCTRL_H2WIE |    /* wakeup intr enable */
		UCTRL_H2UIE |   /* ULPI intr enable */
		UCTRL_H2DT |    /* disable H2 TLL */
		UCTRL_H2PM;     /* power mask */    

	UH2_PORTSC1 &= ~PORTSC_PTS_MASK;    /* set ULPI xcvr */
	UH2_PORTSC1 |= PORTSC_PTS_ULPI;

	return 0;
}

void isp1504_set(u8 bits, int reg, volatile u32 * view);
void isp1504_clear(u8 bits, int reg, volatile u32 * view);

static void platform_set_vbus(int power)
{
	if(power)
		isp1504_set(1<<6, 0xa, &UH2_ULPIVIEW);
	else
		isp1504_clear(1<<6, 0xa, &UH2_ULPIVIEW);
	return ;
}

int ehci_setup(u32 base)
{
	platform_set_vbus(1);
	return 0;
}

int usb_board_init(u32 base)
{
	gpio_usb_active();
	ehci_platform_set_host_mode();
	usb_set_ulpi_xcvr();
	ehci_setup(base);
	return 0;
}

int usb_board_stop(u32 base)
{
	return 0;
}
#endif

extern void multi_smc_eth_probe(void );

int board_late_init(void)
{
	unsigned int tmp;

	multi_smc_eth_probe();

	/* assert 13783 GPO4*/
	tmp = 0x1000;
	pmic_write_reg(0x22, &tmp);
	return 0;
}
