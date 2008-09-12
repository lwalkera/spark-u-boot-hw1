/*
 * (C) Copyright 2005
 * Robert Whaley, Applied Data Systems, Inc. rwhaley@applieddata.net
 *
 * (C) Copyright 2002
 * Kyle Harris, Nexus Technologies, Inc. kharris@nexus-tech.net
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * Configuation settings for the ADSBITSYX board.
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef __CONFIG_H
#define __CONFIG_H

/*
 * High Level Configuration Options
 * (easy to change)
 */
#define CONFIG_PXA27X		1	/* This is an PXA27x CPU    */
#define CONFIG_ADSBITSYXB	1	/* on a Ads BitsyBx Board     */
#define BOARD_LATE_INIT		1

#undef CONFIG_USE_IRQ			/* we don't need IRQ/FIQ stuff */

#define RTC

/*
 * Size of malloc() pool
 */
#define CFG_MALLOC_LEN	    (CFG_ENV_SIZE + 128*1024)
#define CFG_GBL_DATA_SIZE	128	/* size in bytes reserved for initial data */

/*
 * Hardware drivers
 */
#define CONFIG_DRIVER_SMC91111
#define CONFIG_SMC91111_BASE    	0x3c000300

#undef CONFIG_SMC91111_EXT_PHY
#undef  CONFIG_SMC_USE_32_BIT

/*
 * select serial console configuration
 */
#define CONFIG_BTUART	       1       /* we use BTUART on ADS BitsyXb */

/* allow to overwrite serial and ethaddr */
#define CONFIG_ENV_OVERWRITE

#define CONFIG_BAUDRATE	       38400

#define CONFIG_DOS_PARTITION   1


/*
 * Command line configuration.
 */
#include <config_cmd_default.h>

#define CONFIG_CMD_ASKENV
#define CONFIG_CMD_FAT
#define CONFIG_CMD_IDE
#define CONFIG_CMD_MAP
#define CONFIG_CMD_MMC
#define CONFIG_CMD_PCMCIA
#define CONFIG_CMD_USB

#undef CONFIG_CMD_AUTOSCRIPT


#define CONFIG_BOOT_RETRY_TIME -1
#define CONFIG_BOOT_RETRY_MIN 1
#define CONFIG_RESET_TO_RETRY

#undef CONFIG_SHOW_BOOT_PROGRESS

#define CONFIG_SILENT_CONSOLE

#define CONFIG_BOOTDELAY	3

#define CONFIG_BOOTCOMMAND	"run start"

#define CONFIG_BOOTARGS		"console=ttyS1,38400 SERIALGETTY=ttyS1 "\
				" SERIALBAUD=38400"                     \
				" rw root=/dev/ram initrd=0xa0600000,5m"\
				" ramdisk_size=12288 rootfstype=ext2"   \
				" " CONFIG_MTD_PARTS

#define CONFIG_MTD_PARTS	"mtdparts=flash0"			\
				":256k(boot)"				\
				",256k(bootvars)"			\
				",2m(zImage)"				\
				",5m(ramdisk.gz)"			\
				",-(flashfs1)"

#define CONFIG_EXTRA_ENV_SETTINGS                                         \
   "board=adsbitsyxb\0"                                                   \
   "FLBLOCKSZ=256k\0"                                                     \
   "INITRD=0xa0600000\0"                                                  \
   "tty_args=console=ttyS1,38400 SERIALGETTY=ttyS1 SERIALBAUD=38400\0"    \
   "eraseenv="	                                                          \
     "protect off 0x00040000 0x0007ffff;"                                 \
     "erase 0x00040000 0x0007ffff\0"                                      \
   "p_reprogram_uboot="	                                                  \
     "if pinit on && ide reset && fatload ide 0 scratch_buffer u-boot.bin;"\
     "then "			                                          \
       "protect off 0x00000000 0x0003ffff;"		                  \
       "erase 0x00000000 0x0003ffff;"	                                  \
       "cp.b scratch_buffer 0x00000000 ${filesize};"                      \
     "fi\0"							          \
   "mmc_reprogram_uboot="                                                 \
     "if mmcinit && fatload mmc 0 scratch_buffer u-boot.bin;"             \
     "then "			                                          \
       "protect off 0x00000000 0x0003ffff;"		                  \
       "erase 0x00000000 0x0003ffff;"	                                  \
       "cp.b scratch_buffer 0x00000000 ${filesize};"                      \
     "fi\0"							          \
   "usb_reprogram_uboot="                                                 \
     "if usb start && fatload usb 0 scratch_buffer u-boot.bin;"           \
     "then "			                                          \
       "protect off 0x00000000 0x0003ffff;"		                  \
       "erase 0x00000000 0x0003ffff;"	                                  \
       "cp.b scratch_buffer 0x00000000 ${filesize};"                      \
     "fi\0"							          \
   "execute_txt_file_cf="                                                 \
     "test -n ${cf_ide_ok} && "                                           \
     "fatenv ide 0 ${txt_file} ${txt_file} && "                           \
     "run ${txt_file} &&     "                                            \
     "setenv ${txt_file}\0"                                               \
   "execute_txt_file_mmc="                                                \
     "test -n ${mmc_ide_ok} && "                                          \
     "fatenv mmc 0 ${txt_file} ${txt_file} && "                           \
     "run ${txt_file} &&     "                                            \
     "setenv ${txt_file}\0"                                               \
   "execute_txt_file_usb="                                                \
     "test -n ${usb_ide_ok} && "                                          \
     "fatenv usb 0 ${txt_file} ${txt_file} && "                           \
     "run ${txt_file} &&     "                                            \
     "setenv ${txt_file}\0"                                               \
   "execute_txt_file="                                                    \
      "echo Attempting to execute ${txt_file}.;"                          \
      "run execute_txt_file_cf || "                                       \
      "run execute_txt_file_mmc || "                                      \
      "run execute_txt_file_usb || "                                      \
      "echo Did not execute ${txt_file}.\0"                               \
   "media_init="                                                          \
     "cf_ide_ok=;if pinit on && ide reset; then cf_ide_ok=y;fi;"          \
     "mmc_ide_ok=;if mmcinit; then mmc_ide_ok=y;fi;"                      \
     "usb_ide_ok=;if usb start; then usb_ide_ok=y;fi\0"                   \
   "start="                                                               \
     "run media_init;"                                                    \
     "txt_file=start.txt; run execute_txt_file;"                          \
     "test -n ${boot_linux} && run boot_linux\0"                          \
   "load_dram_cf="                                                        \
     "test -n ${cf_ide_ok} && fatload ide 0 ${load_buffer} ${load_file}\0"\
   "load_dram_mmc="                                                       \
     "test -n ${mmc_ide_ok} && fatload mmc 0 ${load_buffer} ${load_file}\0"\
   "load_dram_usb="                                                       \
     "test -n ${usb_ide_ok} && fatload usb 0 ${load_buffer} ${load_file}\0"\
   "load_dram="                                                           \
     "run load_dram_cf || run load_dram_mmc || run load_dram_usb\0"       \
   "load_flash="                                                          \
     "if run load_dram && "                                               \
	"protect off ${flash_partition} && "                              \
	"erase ${flash_partition} && "                                    \
	"cp.b ${load_buffer} ${flash_partition} ${filesize}; "            \
     "then echo Loaded ${load_file} to flash; "                           \
     "else echo Failed to load ${load_file}.;"                            \
     "fi\0"                                                               \

#define CONFIG_SETUP_MEMORY_TAGS 1
#define CONFIG_CMDLINE_TAG	 1	/* enable passing of ATAGs	*/

#if defined(CONFIG_CMD_KGDB)
#define CONFIG_KGDB_BAUDRATE	230400	/* speed to run kgdb serial port */
#define CONFIG_KGDB_SER_INDEX	2	/* which serial port to use */
#endif

#define CONFIG_AUTO_COMPLETE	1

/*
 * Miscellaneous configurable options
 */
#define CFG_HUSH_PARSER		1
#define CFG_PROMPT_HUSH_PS2	"> "

#define CFG_LONGHELP				/* undef to save memory	*/
#ifdef CFG_HUSH_PARSER
#define CFG_PROMPT		"$ "		/* Monitor Command Prompt */
#else
#define CFG_PROMPT		"=> "		/* Monitor Command Prompt */
#endif
#define CFG_CBSIZE		256		/* Console I/O Buffer Size */
#define CFG_PBSIZE (CFG_CBSIZE+sizeof(CFG_PROMPT)+16) /* Print Buffer Size */
#define CFG_MAXARGS		32		/* max number of command args	*/
#define CFG_BARGSIZE		CFG_CBSIZE	/* Boot Argument Buffer Size	*/
#define CFG_DEVICE_NULLDEV	1

#define CFG_MEMTEST_START	0xa0400000	/* memtest works on	*/
#define CFG_MEMTEST_END		0xa0800000	/* 4 ... 8 MB in DRAM	*/

#undef	CFG_CLKS_IN_HZ		/* everything, incl board info, in Hz */

#define CFG_LOAD_ADDR		0xa0008000	/* default load address */

#define CFG_HZ			3686400		/* incrementer freq: 3.6864 MHz */
#define CFG_CPUSPEED		0x207		/* need to look more closely, I think this is Turbo = 2x, L=91Mhz */

						/* valid baudrates */
#define CFG_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200 }

#define CFG_MEM_MAP_ITEMS {{"zImage_buffer",  0xa0008000, 0x00200000}, \
                           {"scratch_buffer", 0xa0208000, 0x01c00000}}
/*
 * Stack sizes
 *
 * The stack sizes are set up in start.S using the settings below
 */
#define CONFIG_STACKSIZE	(128*1024)	/* regular stack */
#ifdef CONFIG_USE_IRQ
#define CONFIG_STACKSIZE_IRQ	(4*1024)	/* IRQ stack */
#define CONFIG_STACKSIZE_FIQ	(4*1024)	/* FIQ stack */
#endif

/*
 * Physical Memory Map
 */
#if !defined(MEMSIZE) || (MEMSIZE == 64)
#define CONFIG_NR_DRAM_BANKS	4	   /* we have 2 banks of DRAM */
#define PHYS_SDRAM_1		0xa0000000 /* SDRAM Bank #1 */
#define PHYS_SDRAM_1_SIZE	0x04000000 /* 64 MB */
#define PHYS_SDRAM_2		0xb0000000 /* SDRAM Bank #2 */
#define PHYS_SDRAM_2_SIZE	0x00000000 /* 0 MB */
#define PHYS_SDRAM_3		0x80000000 /* SDRAM Bank #3 */
#define PHYS_SDRAM_3_SIZE	0x00000000 /* 0 MB */
#define PHYS_SDRAM_4		0x90000000 /* SDRAM Bank #4 */
#define PHYS_SDRAM_4_SIZE	0x00000000 /* 0 MB */
#elif MEMSIZE == 128
#define CONFIG_NR_DRAM_BANKS	4	   /* we have 2 banks of DRAM */
#define PHYS_SDRAM_1		0xa0000000 /* SDRAM Bank #1 */
#define PHYS_SDRAM_1_SIZE	0x08000000 /* 64 MB */
#define PHYS_SDRAM_2		0xb0000000 /* SDRAM Bank #2 */
#define PHYS_SDRAM_2_SIZE	0x00000000 /* 0 MB */
#define PHYS_SDRAM_3		0x80000000 /* SDRAM Bank #3 */
#define PHYS_SDRAM_3_SIZE	0x00000000 /* 0 MB */
#define PHYS_SDRAM_4		0x90000000 /* SDRAM Bank #4 */
#define PHYS_SDRAM_4_SIZE	0x00000000 /* 0 MB */
#else
#error "Unsupported memory size"
#endif

#define PHYS_FLASH_1		0x00000000 /* Flash Bank #1 */

#define CFG_DRAM_BASE		0xa0000000
#define CFG_DRAM_SIZE		0x04000000

#define CFG_FLASH_BASE		PHYS_FLASH_1

#define CFG_UNLOCK_WHOLE_AMNESIAC_FLASH_AT_BOOT /* Intel K3 & P30 flash powers up with all blocks locked */

/*
 * GPIO settings
 */

#define CFG_GPSR0_VAL		0x0000C000
#define CFG_GPSR1_VAL		0x004F0080
#define CFG_GPSR2_VAL		0x1220C000
#define CFG_GPSR3_VAL		0x0003E000
#define CFG_GPCR0_VAL		0x08B90018
#define CFG_GPCR1_VAL		0x00300002
#define CFG_GPCR2_VAL		0x0C010000
#define CFG_GPCR3_VAL		0x01E0000A
#define CFG_GPDR0_VAL		0xCBB9F018
#define CFG_GPDR1_VAL		0xFCFFAB83
#define CFG_GPDR2_VAL		0x1EFBFFFF
#define CFG_GPDR3_VAL		0x01E3EF0A
#define CFG_GAFR0_L_VAL		0x24800000
#define CFG_GAFR0_U_VAL		0xA51A8042
#define CFG_GAFR1_L_VAL		0x699A9552
#define CFG_GAFR1_U_VAL		0xAAA5A0AA
#define CFG_GAFR2_L_VAL		0x6AAAAAAA
#define CFG_GAFR2_U_VAL		0x0109A554
#define CFG_GAFR3_L_VAL		0x54000000
#define CFG_GAFR3_U_VAL		0x00000009

#define CFG_PSSR_VAL		0x20

/*
 * Clock settings
 */
#define CFG_CKEN		0x00400201
#define CFG_CCCR		0x02000290   /* 520Mhz */
/* #define CFG_CCCR		0x02000210   416 Mhz */
/* #define CFG_CCCR		0x02000110   208 Mhz */
/* #define CFG_CCCR		0x02000108   104 Mhz */

/* #define CFG_CPU_VOLTAGE	23      DAC value for 1.55V 624Mhz */
#define CFG_CPU_VOLTAGE		63	/* DAC value for 1.45V 520Mhz */

/*
 * Memory settings
 */

#define CFG_MSC0_VAL		0x15d215d0
#define CFG_MSC1_VAL		0x7ff0CDD1
#define CFG_MSC2_VAL		0x7ff0B884
#if !defined(MEMSIZE) || (MEMSIZE == 64)
#define CFG_MDCNFG_VAL		0x88000AC8
#elif MEMSIZE == 128
#define CFG_MDCNFG_VAL		0x88000AD0
#else
#error "Unsupported memory size"
#endif
#define CFG_MDREFR_VAL		0x00000031
#define CFG_MDMRS_VAL		0x00000000

#define CFG_FLYCNFG_VAL		0x00010001
#define CFG_SXCNFG_VAL		0x40044004

#if defined(CONFIG_CMD_MMC)
/*
 * MMC Interface
 */
#define CONFIG_MMC
#define CFG_MMC_BASE		0xF0000000
#endif


#if defined(CONFIG_CMD_USB)
/*
 * USB Interface
 */
#include <asm/arch/pxa-regs.h>		/* for OHCI_REGS_BASE 		*/
#define LITTLEENDIAN			/* used by usb_ohci.c		*/
#define CONFIG_USB_OHCI_NEW
#define CFG_USB_OHCI_CPU_INIT
#define CFG_USB_OHCI_BOARD_INIT
#define CFG_USB_OHCI_MAX_ROOT_PORTS	3
#define CFG_USB_OHCI_REGS_BASE		OHCI_REGS_BASE
#define CFG_USB_OHCI_SLOT_NAME		"adsbitsyxb"
#define CONFIG_USB_STORAGE
#endif


/*
 * PCMCIA and CF Interfaces
 */
#define CFG_MECR_VAL		0x00000002
#define CFG_MCMEM0_VAL		0x00004204
#define CFG_MCMEM1_VAL		0x00000000
#define CFG_MCATT0_VAL		0x00010504
#define CFG_MCATT1_VAL		0x00000000
#define CFG_MCIO0_VAL		0x00008407
#define CFG_MCIO1_VAL		0x00000000

#define CONFIG_PXA_PCMCIA 1
#define CONFIG_PXA_IDE 1

#define CONFIG_PCMCIA_SLOT_A 1
/* just to keep build system happy  */

#define CFG_PCMCIA_MEM_ADDR     0x28000000
#define CFG_PCMCIA_MEM_SIZE     0x04000000


#define CFG_IDE_MAXBUS		1
/* max. 1 IDE bus		*/
#define CFG_IDE_MAXDEVICE	1
/* max. 1 drive per IDE bus	*/

#define CFG_ATA_IDE0_OFFSET	0x0000

#define CFG_ATA_BASE_ADDR	0x20000000

/* Offset for data I/O			*/
#define CFG_ATA_DATA_OFFSET	0x1f0

/* Offset for normal register accesses	*/
#define CFG_ATA_REG_OFFSET	0x1f0

/* Offset for alternate registers	*/
#define CFG_ATA_ALT_OFFSET	0x3f0

/*
 * FLASH and environment organization
 */

#define CFG_FLASH_CFI
#define CFG_FLASH_CFI_DRIVER	1

#define CFG_MONITOR_BASE	PHYS_FLASH_1
#define CFG_MONITOR_LEN		0x40000

#define CFG_MAX_FLASH_BANKS	1         /* max number of memory banks		*/
#define CFG_MAX_FLASH_SECT	(4 + 255) /* max number of sectors on one chip  */

/* timeout values are in ticks */
#define CFG_FLASH_ERASE_TOUT	(25*CFG_HZ) /* Timeout for Flash Erase */
#define CFG_FLASH_WRITE_TOUT	(25*CFG_HZ) /* Timeout for Flash Write */

/* write flash less slowly */
#define CFG_FLASH_USE_BUFFER_WRITE 1

/* Flash environment locations */
#define CFG_ENV_IS_IN_FLASH	1
#define CFG_ENV_ADDR		(PHYS_FLASH_1 + CFG_MONITOR_LEN)	/* Addr of Environment Sector	*/
#define CFG_ENV_SIZE		0x40000	/* Total Size of Environment Sector	*/


#endif	/* __CONFIG_H */
