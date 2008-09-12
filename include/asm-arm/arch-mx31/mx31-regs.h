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
# include <asm/arch/sizes.h>

#define IO_ADDRESS(x) (x)

#ifndef __MX31_REGS_H_
#  define __MX31_REGS_H_

# ifndef __ASSEMBLY__
#  define io_p2v(PhAdd)	   (PhAdd)
# include <asm/io.h>

#define outb(v,p)			__raw_writeb(v,(p))
#define outw(v,p)			__raw_writew(cpu_to_le16(v),(p))
#define outl(v,p)			__raw_writel(cpu_to_le32(v),(p))

#define inb(p)	({ unsigned int __v = __raw_readb((p)); __v; })
#define inw(p)	({ unsigned int __v = le16_to_cpu(__raw_readw((p))); __v; })
#define inl(p)	({ unsigned int __v = le32_to_cpu(__raw_readl((p))); __v; })

#if 0
void outb(uchar val, __u32 addr)
{
	writeb(val, addr);
}

uchar inb(__u32 addr)
{
	return readb(addr);
}
#endif

#define outsw(r,b,l)	({	int __i; 	\
		unsigned short *__b2; 			\
		__b2 = (unsigned short *) b; 	\
		for (__i = 0; __i < l; __i++) { \
		writew( *(__b2 + __i), r); 		\
		} 								\
		})

#define insw(r,b,l) 	({	int __i ;		\
		unsigned short *__b2;  				\
		__b2 = (unsigned short *) b;  		\
		for (__i = 0; __i < l; __i++) {  	\
		*(__b2 + __i) = readw(r);  			\
		};  								\
		})

#  if 0
#  define readb(c) ({ unsigned int __v = __raw_readb(io_p2v(c)); __v; })
#  define readw(c) ({ unsigned int __v = le16_to_cpu(__raw_readw(io_p2v(c))); __v; })
#  define readl(c) ({ unsigned int __v = le32_to_cpu(__raw_readl(io_p2v(c))); __v; })

#  define writeb(v,c)		__raw_writeb(v,io_p2v(c))
#  define writew(v,c)		__raw_writew(cpu_to_le16(v),io_p2v(c))
#  define writel(v,c)		__raw_writel(cpu_to_le32(v),io_p2v(c))
#  endif

#  define memset_io(c,v,l)		_memset_io(io_p2v(c),(v),(l))
#  define memcpy_fromio(a,c,l)		_memcpy_fromio((a),io_p2v(c),(l))
#  define memcpy_toio(c,a,l)		_memcpy_toio(io_p2v(c),(a),(l))

#  define eth_io_copy_and_sum(s,c,l,b) eth_copy_and_sum((s),io_p2v(c),(l),(b))

#  define __REG(x)	(*((volatile u32 *)io_p2v(x)))
#  define __REG16(x)	(*((volatile u16 *)io_p2v(x)))
#  define __REG2(x,y)	(*(volatile u32 *)((u32)&__REG(x) + (y)))

# else
#  define __REG(x) (x)
# endif


/*
    * Default Memory Layout Definitions
	 */

#  define L2CC_BASE_ADDR          0x30000000

/*
 * AIPS 1
 */
#  define AIPS1_BASE_ADDR         0x43F00000
#  define AIPS1_CTRL_BASE_ADDR    AIPS1_BASE_ADDR
#  define MAX_BASE_ADDR           0x43F04000
#  define EVTMON_BASE_ADDR        0x43F08000
#  define CLKCTL_BASE_ADDR        0x43F0C000
#  define ETB_SLOT4_BASE_ADDR     0x43F10000
#  define ETB_SLOT5_BASE_ADDR     0x43F14000
#  define ECT_CTIO_BASE_ADDR      0x43F18000
#  define I2C_BASE_ADDR           0x43F80000
#  define I2C3_BASE_ADDR          0x43F84000
#  define OTG_BASE_ADDR           0x43F88000
#  define ATA_BASE_ADDR           0x43F8C000
#  define UART1_BASE_ADDR         0x43F90000
#  define UART2_BASE_ADDR         0x43F94000
#  define I2C2_BASE_ADDR          0x43F98000
#  define OWIRE_BASE_ADDR         0x43F9C000
#  define SSI1_BASE_ADDR          0x43FA0000
#  define CSPI1_BASE_ADDR         0x43FA4000
#  define KPP_BASE_ADDR           0x43FA8000
#  define IOMUXC_BASE_ADDR        0x43FAC000
#  define UART4_BASE_ADDR         0x43FB0000
#  define UART5_BASE_ADDR         0x43FB4000
#  define ECT_IP1_BASE_ADDR       0x43FB8000
#  define ECT_IP2_BASE_ADDR       0x43FBC000

/*
 * SPBA
 */
#  define SPBA_BASE_ADDR          0x50000000
#  define MMC_SDHC1_BASE_ADDR     0x50004000
#  define MMC_SDHC2_BASE_ADDR     0x50008000
#  define UART3_BASE_ADDR         0x5000C000
#  define CSPI2_BASE_ADDR         0x50010000
#  define SSI2_BASE_ADDR          0x50014000
#  define SIM_BASE_ADDR           0x50018000
#  define IIM_BASE_ADDR           0x5001C000
#  define ATA_DMA_BASE_ADDR       0x50020000
#  define SPBA_CTRL_BASE_ADDR     0x5003C000

/*
 * AIPS 2
 */
#  define AIPS2_BASE_ADDR         0x53F00000
#  define AIPS2_CTRL_BASE_ADDR    AIPS2_BASE_ADDR
#  define CCM_BASE_ADDR           0x53F80000
#  define CSPI3_BASE_ADDR     (AIPS2_BASE_ADDR + 0x00084000)
#  define FIRI_BASE_ADDR          0x53F8C000
#  define GPT1_BASE_ADDR          0x53F90000
#  define EPIT1_BASE_ADDR         0x53F94000
#  define EPIT2_BASE_ADDR         0x53F98000
#  define GPIO3_BASE_ADDR         0x53FA4000
#  define SCC_BASE                0x53FAC000
#  define SCM_BASE                0x53FAE000
#  define SMN_BASE                0x53FAF000
#  define RNGA_BASE_ADDR          0x53FB0000
#  define IPU_CTRL_BASE_ADDR      0x53FC0000
#  define AUDMUX_BASE             0x53FC4000
#  define MPEG4_ENC_BASE          0x53FC8000
#  define GPIO1_BASE_ADDR         0x53FCC000
#  define GPIO2_BASE_ADDR         0x53FD0000
#  define SDMA_BASE_ADDR          0x53FD4000
#  define RTC_BASE_ADDR           0x53FD8000
#  define WDOG_BASE_ADDR          0x53FDC000
#  define PWM_BASE_ADDR           0x53FE0000
#  define RTIC_BASE_ADDR          0x53FEC000

/*Watch dog*/

# define WDOG_WCR_REG			(WDOG_BASE_ADDR + 0x00)
# define WDOG_WT(x)				(((x)&0xff) << 8 )
# define WDOG_WOE				(1<< 6)
# define WDOG_WDA				(1<< 5)
# define WDOG_SRS				(1<< 4)
# define WDOG_WRE				(1<< 3)
# define WDOG_WDE				(1<< 2)
# define WDOG_WDBG				(1<< 1)
# define WDOG_WDZST				(1<< 0)

#define WDOG_WSR_REG			(WDOG_BASE_ADDR + 0x02)
#define WDOG_WRSR_REG			(WDOG_BASE_ADDR + 0x02)
#define WDOG_SFTW				(1<<0)
#define WDOG_TOUT				(1<<1)
#define WDOG_CMON				(1<<2)
#define WDOG_EXT				(1<<3)
#define WDOG_PWR				(1<<4)
#define WDOG_JRST				(1<<5)
/*
 * ROMPATCH and AVIC
 */
#  define ROMPATCH_BASE_ADDR      0x60000000
#  define AVIC_BASE_ADDR          0x68000000

/*
 * NAND, SDRAM, WEIM, M3IF, EMI controllers
 */
#  define EXT_MEM_CTRL_BASE       0xB8000000
#  define NFC_BASE                EXT_MEM_CTRL_BASE
#  define ESDCTL_BASE             0xB8001000
#  define WEIM_BASE_ADDR          0xB8002000
#  define WEIM_CTRL_CS0           WEIM_BASE_ADDR
#  define WEIM_CTRL_CS1           (WEIM_BASE_ADDR + 0x10)
#  define WEIM_CTRL_CS2           (WEIM_BASE_ADDR + 0x20)
#  define WEIM_CTRL_CS3           (WEIM_BASE_ADDR + 0x30)
#  define WEIM_CTRL_CS4           (WEIM_BASE_ADDR + 0x40)
#  define WEIM_CTRL_CS5           (WEIM_BASE_ADDR + 0x50)
#  define M3IF_BASE               0xB8003000
#  define PCMCIA_CTL_BASE         0xB8004000

/*
 * Memory regions and CS
 */
#  define IPU_MEM_BASE_ADDR       0x70000000
#  define CSD0_BASE_ADDR          0x80000000
#  define CSD1_BASE_ADDR          0x90000000
#  define CS0_BASE_ADDR           0xA0000000
#  define CS1_BASE_ADDR           0xA8000000
#  define CS2_BASE_ADDR           0xB0000000
#  define CS3_BASE_ADDR           0xB2000000
#  define CS4_BASE_ADDR           0xB4000000
#  define CS4_BASE_PSRAM          0xB5000000
#  define CS5_BASE_ADDR           0xB6000000
#  define PCMCIA_MEM_BASE_ADDR    0xC0000000

#  define INTERNAL_ROM_VA         0xF0000000

/* General Purpose Timers */
#  define GPTCR 				__REG(0x53f90000)
#  define GPTPR 				__REG(0x53f90004)
#  define GPTSR 				__REG(0x53f90008)
#  define GPTIR 				__REG(0x53f9000c)
#  define GPTOCR1 				__REG(0x53f90010)
#  define GPTOCR2 				__REG(0x53f90014)
#  define GPTOCR3 				__REG(0x53f90018)
#  define GPTICR1 				__REG(0x53f9001c)
#  define GPTICR2 				__REG(0x53f90020)
#  define GPTCNT 				__REG(0x53f90024)

#  define GPT_FO3			(1<<31)
#  define GPT_FO2			(1<<30)
#  define GPT_FO1			(1<<29)
#  define GPT_OM3(x)			((x&0x07)<<26)
#  define GPT_OM2(x)			((x&0x07)<<23)
#  define GPT_OM1(x)			((x&0x07)<<20)
#  define GPT_IM2(x)			((x&0x03)<<18)
#  define GPT_IM1(x)			((x&0x03)<<16)
#  define GPT_SWR			(1<<15)
#  define GPT_FRR			(1<<9)
#  define GPT_CLKSRC(x)		((x&0x07)<<6)
#  define GPT_STOPEN			(1<<5)
#  define GPT_DOZEN			(1<<4)
#  define GPT_WAITEN			(1<<3)
#  define GPT_DBGEN			(1<<2)
#  define GPT_ENMOD			(1<<1)
#  define GPT_EN			1

/*
 * IRQ Controller Register Definitions.
 */
#  define AVIC_NIMASK                     __REG(AVIC_BASE_ADDR + (0x04))
#  define AVIC_INTTYPEH                   __REG(AVIC_BASE_ADDR + (0x18))
#  define AVIC_INTTYPEL                   __REG(AVIC_BASE_ADDR + (0x1C))

/* L210 */
#  define L2CC_BASE_ADDR                  0x30000000
#  define L2_CACHE_LINE_SIZE              32
#  define L2_CACHE_CTL_REG                0x100
#  define L2_CACHE_AUX_CTL_REG            0x104
#  define L2_CACHE_SYNC_REG               0x730
#  define L2_CACHE_INV_LINE_REG           0x770
#  define L2_CACHE_INV_WAY_REG            0x77C
#  define L2_CACHE_CLEAN_LINE_REG         0x7B0
#  define L2_CACHE_CLEAN_INV_LINE_REG     0x7F0

/* CCM */
#  define CLKCTL_CCMR                     0x00
#  define CLKCTL_PDR0                     0x04
#  define CLKCTL_PDR1                     0x08
#  define CLKCTL_PDR2                     0x64
#  define CLKCTL_RCSR                     0x0C
#  define CLKCTL_MPCTL                    0x10
#  define CLKCTL_UPCTL                    0x14
#  define CLKCTL_SPCTL                    0x18
#  define CLKCTL_COSR                     0x1C
#  define FREQ_26MHZ                      26000000
#  define FREQ_27MHZ                      27000000
#  define FREQ_32768HZ                    (32768 * 512)
#  define FREQ_32000HZ                    (32000 * 512)
#  define PLL_REF_CLK                     FREQ_26MHZ
//#  define PLL_REF_CLK  FREQ_32768HZ
//#  define PLL_REF_CLK  FREQ_32000HZ

/* WEIM - CS0 */
#  define CSCRU                           0x00
#  define CSCRL                           0x04
#  define CSCRA                           0x08
#  define CHIP_REV_1_0            0x0      /* PASS 1.0 */
#  define CHIP_REV_1_1            0x1      /* PASS 1.1 */
#  define CHIP_REV_2_0            0x2      /* PASS 2.0 */

#  define IIM_STAT_OFF            0x00
#  define IIM_STAT_BUSY           (1 << 7)
#  define IIM_STAT_PRGD           (1 << 1)
#  define IIM_STAT_SNSD           (1 << 0)
#  define IIM_STATM_OFF           0x04
#  define IIM_ERR_OFF             0x08
#  define IIM_ERR_PRGE            (1 << 7)
#  define IIM_ERR_WPE         (1 << 6)
#  define IIM_ERR_OPE         (1 << 5)
#  define IIM_ERR_RPE         (1 << 4)
#  define IIM_ERR_WLRE        (1 << 3)
#  define IIM_ERR_SNSE        (1 << 2)
#  define IIM_ERR_PARITYE     (1 << 1)
#  define IIM_EMASK_OFF           0x0C
#  define IIM_FCTL_OFF            0x10
#  define IIM_UA_OFF              0x14
#  define IIM_LA_OFF              0x18
#  define IIM_SDAT_OFF            0x1C
#  define IIM_PREV_OFF            0x20
#  define IIM_SREV_OFF            0x24
#  define IIM_PREG_P_OFF          0x28
#  define IIM_SCS0_OFF            0x2C
#  define IIM_SCS1_P_OFF          0x30
#  define IIM_SCS2_OFF            0x34
#  define IIM_SCS3_P_OFF          0x38

#  define EPIT_BASE_ADDR          EPIT1_BASE_ADDR
#  define EPITCR                  __REG(EPIT1_BASE_ADDR + 0x00)
#  define EPITSR                  __REG(EPIT1_BASE_ADDR + 0x04)
#  define EPITLR                  __REG(EPIT1_BASE_ADDR + 0x08)
#  define EPITCNR                 __REG(EPIT1_BASE_ADDR + 0x10)

#  define GPT_BASE_ADDR           GPT1_BASE_ADDR
#if 0
#  define GPTCR                   0x00
#  define GPTPR                   0x04
#  define GPTSR                   0x08
#  define GPTIR                   0x0C
#  define GPTOCR1                 0x10
#  define GPTOCR2                 0x14
#  define GPTOCR3                 0x18
#  define GPTICR1                 0x1C
#  define GPTICR2                 0x20
#  define GPTCNT                  0x24
#endif

/* ESDCTL */
#  define ESDCTL_ESDCTL0                  0x00
#  define ESDCTL_ESDCFG0                  0x04
#  define ESDCTL_ESDCTL1                  0x08
#  define ESDCTL_ESDCFG1                  0x0C
#  define ESDCTL_ESDMISC                  0x10

#if (PLL_REF_CLK != 26000000)
#error Wrong PLL reference clock! The following macros will not work.
#endif

/* Assuming 26MHz input clock */
/*                            PD             MFD              MFI          MFN */
#  define MPCTL_PARAM_208     (((2-1) << 26) + ((1 -1) << 16) + (8  << 10) + (0  << 0))
#  define MPCTL_PARAM_399     (((1-1) << 26) + ((52-1) << 16) + (7  << 10) + (35 << 0))
#  define MPCTL_PARAM_462     (((1-1) << 26) + ((26-1) << 16) + (8  << 10) + (23 << 0))
#  define MPCTL_PARAM_528     (((1-1) << 26) + ((13-1) << 16) + (10  << 10) + (2 << 0))
#  define MPCTL_PARAM_532     (((1-1) << 26) + ((52-1) << 16) + (10 << 10) + (12 << 0))
#  define MPCTL_PARAM_665     (((1-1) << 26) + ((52-1) << 16) + (12 << 10) + (41 << 0))
#  define MPCTL_PARAM_532_27  (((1-1) << 26) + ((15-1) << 16) + (9  << 10) + (13 << 0))

/* UPCTL                      PD             MFD              MFI          MFN */
#  define UPCTL_PARAM_288     (((1-1) << 26) + ((13-1) << 16) + (5  << 10) + (7  << 0))
#  define UPCTL_PARAM_240     (((2-1) << 26) + ((13-1) << 16) + (9  << 10) + (3  << 0))
#  define UPCTL_PARAM_240_27  (((2-1) << 26) + ((9 -1) << 16) + (8  << 10) + (8  << 0))

/* PDR0 */
#  define PDR0_208_104_52     0xFF870D48  /* ARM=208MHz, HCLK=104MHz, IPG=52MHz */
#  define PDR0_399_66_66      0xFF872B28  /* ARM=399MHz, HCLK=IPG=66.5MHz */
/* define PDR0_399_133_66 was 0xFF871650 but 1550 is the value used in linux */
#  define PDR0_399_133_66     0xFF871550  /* ARM=399MHz, HCLK=133MHz, IPG=66.5MHz */
#  define PDR0_528_133_66     0xFF871E58  /* ARM=528MHz, HCLK=132MHz, IPG=66MHz */
#  define PDR0_532_133_66     0xFF871E58  /* ARM=532MHz, HCLK=133MHz, IPG=66MHz */
#  define PDR0_665_83_42      0xFF873D78  /* ARM=665MHz, HCLK=83MHz, IPG=42MHz */
#  define PDR0_665_133_66     0xFF872660  /* ARM=665MHz, HCLK=133MHz, IPG=66MHz */

#  define NAND_REG_BASE                   (NFC_BASE + 0xE00)
#  define NFC_BUFSIZE_REG_OFF             (0 + 0x00)
#  define RAM_BUFFER_ADDRESS_REG_OFF      (0 + 0x04)
#  define NAND_FLASH_ADD_REG_OFF          (0 + 0x06)
#  define NAND_FLASH_CMD_REG_OFF          (0 + 0x08)
#  define NFC_CONFIGURATION_REG_OFF       (0 + 0x0A)
#  define ECC_STATUS_RESULT_REG_OFF       (0 + 0x0C)
#  define ECC_RSLT_MAIN_AREA_REG_OFF      (0 + 0x0E)
#  define ECC_RSLT_SPARE_AREA_REG_OFF     (0 + 0x10)
#  define NF_WR_PROT_REG_OFF              (0 + 0x12)
#  define UNLOCK_START_BLK_ADD_REG_OFF    (0 + 0x14)
#  define UNLOCK_END_BLK_ADD_REG_OFF      (0 + 0x16)
#  define NAND_FLASH_WR_PR_ST_REG_OFF     (0 + 0x18)
#  define NAND_FLASH_CONFIG1_REG_OFF      (0 + 0x1A)
#  define NAND_FLASH_CONFIG2_REG_OFF      (0 + 0x1C)
#  define RAM_BUFFER_ADDRESS_RBA_3        0x3
#  define NFC_BUFSIZE_1KB                 0x0
#  define NFC_BUFSIZE_2KB                 0x1
#  define NFC_CONFIGURATION_UNLOCKED      0x2
#  define ECC_STATUS_RESULT_NO_ERR        0x0
#  define ECC_STATUS_RESULT_1BIT_ERR      0x1
#  define ECC_STATUS_RESULT_2BIT_ERR      0x2
#  define NF_WR_PROT_UNLOCK               0x4
#  define NAND_FLASH_CONFIG1_FORCE_CE     (1 << 7)
#  define NAND_FLASH_CONFIG1_RST          (1 << 6)
#  define NAND_FLASH_CONFIG1_BIG          (1 << 5)
#  define NAND_FLASH_CONFIG1_INT_MSK      (1 << 4)
#  define NAND_FLASH_CONFIG1_ECC_EN       (1 << 3)
#  define NAND_FLASH_CONFIG1_SP_EN        (1 << 2)
#  define NAND_FLASH_CONFIG2_INT_DONE     (1 << 15)
#  define NAND_FLASH_CONFIG2_FDO_PAGE     (0 << 3)
#  define NAND_FLASH_CONFIG2_FDO_ID       (2 << 3)
#  define NAND_FLASH_CONFIG2_FDO_STATUS   (4 << 3)
#  define NAND_FLASH_CONFIG2_FDI_EN       (1 << 2)
#  define NAND_FLASH_CONFIG2_FADD_EN      (1 << 1)
#  define NAND_FLASH_CONFIG2_FCMD_EN      (1 << 0)
#  define FDO_PAGE_SPARE_VAL              0x8

#  define MXC_NAND_BASE_DUMMY             0xE0000000
#  define NOR_FLASH_BOOT                  0
#  define NAND_FLASH_BOOT                 0x10000000
#  define SDRAM_NON_FLASH_BOOT            0x20000000
#  define MXCBOOT_FLAG_REG                (AVIC_BASE_ADDR + 0x100)
#  define MXCFIS_NOTHING                  0x00000000
#  define MXCFIS_NAND                     0x10000000
#  define MXCFIS_NOR                      0x20000000
#  define MXCFIS_FLAG_REG                 (AVIC_BASE_ADDR + 0x104)

#  define IS_BOOTING_FROM_NAND()          (readl(MXCBOOT_FLAG_REG) == NAND_FLASH_BOOT)
#  define IS_BOOTING_FROM_NOR()           (readl(MXCBOOT_FLAG_REG) == NOR_FLASH_BOOT)
#  define IS_BOOTING_FROM_SDRAM()         (readl(MXCBOOT_FLAG_REG) == SDRAM_NON_FLASH_BOOT)

#ifndef MXCFLASH_SELECT_NAND
#  define IS_FIS_FROM_NAND()              0
#else
#  define IS_FIS_FROM_NAND()              (readl(MXCFIS_FLAG_REG) == MXCFIS_NAND)
#endif

#ifndef MXCFLASH_SELECT_NOR
#  define IS_FIS_FROM_NOR()               0
#else
#  define IS_FIS_FROM_NOR()               (!IS_FIS_FROM_NAND())
#endif

#  define MXC_ASSERT_NOR_BOOT()           writel(MXCFIS_NOR, MXCFIS_FLAG_REG)
#  define MXC_ASSERT_NAND_BOOT()          writel(MXCFIS_NAND, MXCFIS_FLAG_REG)

/*
 * This macro is used to get certain bit field from a number
 */
#  define MXC_GET_FIELD(val, len, sh)          ((val >> sh) & ((1 << len) - 1))

/*
 * This macro is used to set certain bit field inside a number
 */
#  define MXC_SET_FIELD(val, len, sh, nval)    ((val & ~(((1 << len) - 1) << sh)) | (nval << sh))

#  define L2CC_ENABLED
#  define UART_WIDTH_32         /* internal UART is 32bit access only */

# ifndef __ASSEMBLY__
enum plls {
	MCU_PLL = CCM_BASE_ADDR + CLKCTL_MPCTL,
	USB_PLL = CCM_BASE_ADDR + CLKCTL_UPCTL,
	SER_PLL = CCM_BASE_ADDR + CLKCTL_SPCTL,
};

enum main_clocks {
	CPU_CLK,
	AHB_CLK,
	MBX_CLK,
	IPG_CLK,
	IPG_PER_CLK,
	NFC_CLK,
	USB_CLK,
	HSP_CLK,
};

enum peri_clocks {
	UART1_BAUD,
	UART2_BAUD,
	UART3_BAUD,
	UART4_BAUD,
	UART5_BAUD,
	FIRI_BAUD,
	SIM_BAUD,
	SSI1_BAUD,
	SSI2_BAUD,
	CSI_BAUD,
	MSTICK1_CLK,
	MSTICK2_CLK,
};

#endif

#define SZ_DEC_1M       1000000
#define PLL_PD_MAX      16      //actual pd+1
#define PLL_MFI_MAX     15
#define PLL_MFI_MIN     5
#define PLL_MFD_MAX     1024    //actual mfd+1
#define PLL_MFN_MAX     511
#define PRESC_MAX       8
#define IPG_DIV_MAX     4
#define AHB_DIV_MAX     8
#define HSP_PODF_MAX    8
#define NFC_PODF_MAX    8

#if (PLL_REF_CLK == FREQ_32768HZ) || (PLL_REF_CLK == FREQ_32000HZ)
#define PLL_MFD_FIXED   1024
#endif
#if (PLL_REF_CLK == FREQ_26MHZ)
#define PLL_MFD_FIXED   (26 * 16)       // =416
#endif

#define PLL_FREQ_MAX    (2 * PLL_REF_CLK * PLL_MFI_MAX)
#define PLL_FREQ_MIN    ((2 * PLL_REF_CLK * (PLL_MFI_MIN - 1)) / PLL_PD_MAX)
#define AHB_CLK_MAX     133333333
#define IPG_CLK_MAX     (AHB_CLK_MAX / 2)
#define NFC_CLK_MAX     25000000
// IPU-HSP clock is independent of the HCLK and can go up to 177MHz but requires
// higher voltage support. For simplicity, limit it to 133MHz
#define HSP_CLK_MAX     133333333

#endif
