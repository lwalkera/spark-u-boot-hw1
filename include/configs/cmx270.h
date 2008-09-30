/*
 * (C) Copyright 2008
 * Laine Walker-Avina, PASCO scientific, lwakera@pasco.com
 *
 * Configuation settings for CM-x270'esque boards.
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
#define CONFIG_CMX270		1	/* on a CM-x270'esque board  */
#define BOARD_LATE_INIT		1

#undef CONFIG_USE_IRQ			/* we don't need IRQ/FIQ stuff */
#undef RTC

/*
 * Size of malloc() pool
 */
#define CFG_MALLOC_LEN	    (CFG_ENV_SIZE + 1024*1024)
#define CFG_GBL_DATA_SIZE	128	/* size in bytes reserved for initial data */

/*
 * select serial console configuration
 */
#define CONFIG_FFUART	       1       /* we use FFUART the cm-x270 */

/* allow to overwrite serial and ethaddr */
#define CONFIG_ENV_OVERWRITE
#define CONFIG_BAUDRATE	       115200
#define CONFIG_DOS_PARTITION   1


/*
 * Command line configuration.
 */
#include <config_cmd_default.h>

#define CONFIG_CMD_ASKENV
#define CONFIG_CMD_FAT
#define CONFIG_CMD_EXT2
#define CONFIG_CMD_IDE
#define CONFIG_CMD_PCMCIA
#define CONFIG_CMD_USB
#define CONFIG_CMD_FLASH
#define CONFIG_CMD_MAP

#define CONFIG_LCD
#define CONFIG_CMD_BMP
#define CONFIG_VIDEO_BMP_GZIP
#define CONFIG_PXA_VGA
#define LCD_BPP LCD_COLOR16
#define CONFIG_SPLASH_SCREEN
#define CFG_VIDEO_LOGO_MAX_SIZE 0xe3000

#undef CONFIG_CMD_NET

#undef CONFIG_CMD_AUTOSCRIPT
#undef CONFIG_SHOW_BOOT_PROGRESS

#define CONFIG_BOOT_RETRY_TIME -1
#define CONFIG_BOOT_RETRY_MIN 1
#define CONFIG_RESET_TO_RETRY

#define CONFIG_SILENT_CONSOLE
#define CONFIG_BOOTDELAY	5

#define CONFIG_BOOTCOMMAND	"run start"
#define CONFIG_BOOTARGS		"console=ttyS0,115200"\
				" rw root=/dev/hda1 rootdelay=5s "   \
				" " CONFIG_MTD_PARTS

#define CONFIG_MTD_PARTS	"mtdparts=flash0"			\
				":256k(boot)"				\
				",256k(bootvars)"			\
				",512k(bootlogo)"
				/*",-(flashfs1)"*/

#define CONFIG_EXTRA_ENV_SETTINGS                                         \
   "board=cm-x270\0"                                                      \
   "FLBLOCKSZ=256k\0"                                                     \
   "splashimage=0x80000\0"                                                \
   "tty_args=console=ttyS0,115200\0"                                      \
   "eraseenv="	                                                          \
     "protect off 0x00040000 0x0007ffff;"                                 \
     "erase 0x00040000 0x0007ffff\0"                                      \
   "usb_reprogram_uboot="                                                 \
     "if usb start && fatload usb 0 scratch_buffer u-boot.bin;"           \
     "then "                                                              \
       "protect off 0x00000000 0x0003ffff;"                               \
       "erase 0x00000000 0x0003ffff;"	                                  \
       "cp.b scratch_buffer 0x00000000 ${filesize};"                      \
     "fi\0"                                                               \
   "execute_txt_file_usb="                                                \
     "test -n ${usb_ide_ok} && "                                          \
     "fatenv usb 0 ${txt_file} ${txt_file} && "                           \
     "run ${txt_file} &&     "                                            \
     "setenv ${txt_file}\0"                                               \
   "execute_txt_file="                                                    \
     "echo Attempting to execute ${txt_file}.;"                           \
     "run execute_txt_file_usb || "                                       \
     "echo Did not execute ${txt_file}.\0"                                \
   "boot_linux="                                                          \
     "echo Booting linux;"\
     "if test -n ${cf_ide_ok};"\
     "then ext2load ide 0:2 zImage_buffer /zImage;"\
       "bootm zImage_buffer;"\
     "else echo Failed to load kernel;"\
	 "fi\0"\
   "media_init="                                                          \
     "cf_ide_ok=;if pinit on && ide reset; then cf_ide_ok=y;fi;"          \
     "usb_ide_ok=0;\0"                                                    \
   "media_init_long="                                                     \
     "cf_ide_ok=;if pinit on && ide reset; then cf_ide_ok=y;fi;"          \
     "usb_ide_ok=;if usb start; then usb_ide_ok=y;fi\0"                   \
   "start="                                                               \
     "run media_init_long;"                                               \
     "txt_file=start.txt; run execute_txt_file;"                          \
     "test -n ${boot_linux} && run boot_linux\0"                          \
   "load_dram_cf="                                                        \
     "test -n ${cf_ide_ok} && fatload ide 0 ${load_buffer} ${load_file}\0"\
   "load_dram_usb="                                                       \
     "test -n ${usb_ide_ok} && fatload usb 0 ${load_buffer} ${load_file}\0"\
   "load_dram="                                                           \
     "run load_dram_cf || run load_dram_usb\0"                            \
   "load_flash="                                                          \
    "if run load_dram && "                                                \
      "protect off ${flash_partition} && "                                \
      "erase ${flash_partition} && "                                      \
      "cp.b ${load_buffer} ${flash_partition} ${filesize}; "              \
        "then echo Loaded ${load_file} to flash; "                        \
    "else echo Failed to load ${load_file}.;"                             \
    "fi\0"                                                                

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

#define CFG_LOAD_ADDR		0xa0100000	/* default load address */

#define CFG_HZ			3686400		/* incrementer freq: 3.6864 MHz */
						/* valid baudrates */
#define CFG_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200 }

#define CFG_MEM_MAP_ITEMS {{"zImage_buffer",  0xa0100000, 0x00200000}, \
                           {"scratch_buffer", 0xa0208000, 0x01c00000}}
/*
 * Stack sizes
 *
 * The stack sizes are set up in start.S using the settings below
 */
#define CONFIG_STACKSIZE	(256*1024)	/* regular stack */
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

#define CFG_GPSR0_VAL		0x0c001880
#define CFG_GPSR1_VAL		0x0004f002
#define CFG_GPSR2_VAL		0x6021c000
#define CFG_GPSR3_VAL		0x00002000

#define CFG_GPCR0_VAL		0x00202000
#define CFG_GPCR1_VAL		0x00200000
#define CFG_GPCR2_VAL		0x00002000
#define CFG_GPCR3_VAL		0x00000000

#define CFG_GPDR0_VAL		0xc021aa00
#define CFG_GPDR1_VAL		0xfcef2b82
#define CFG_GPDR2_VAL		0x6021ffff
#define CFG_GPDR3_VAL		0x00020104

#define CFG_GAFR0_L_VAL		0x80000000
#define CFG_GAFR0_U_VAL		0xa5000010

#define CFG_GAFR1_L_VAL		0x999a9558
#define CFG_GAFR1_U_VAL		0xaaa5a0aa

#define CFG_GAFR2_L_VAL		0xaaaaaaaa
#define CFG_GAFR2_U_VAL		0x00000402

#define CFG_GAFR3_L_VAL		0x00010010 /*0x00000010*/
#define CFG_GAFR3_U_VAL		0x00000008

#define CFG_PSSR_VAL		0x20

/*
 * Clock settings
 */
#define CFG_CKEN		0x01ffffff
/*#define CFG_CCCR		0x02000290   520Mhz */
/* #define CFG_CCCR		0x02000210   416 Mhz */
#define CFG_CCCR		0x02000190   /* 320 Mhz */
/* #define CFG_CCCR		0x02000110   208 Mhz */
/* #define CFG_CCCR		0x02000108   104 Mhz */
#define CFG_CPUSPEED    CFG_CCCR

/*
 * Memory settings
 */
    
#define CFG_MSC0_VAL		0x7ff47ff8
#define CFG_MSC1_VAL		0x7ffc1364
#define CFG_MSC2_VAL		0x7ff023f4
#if !defined(MEMSIZE) || (MEMSIZE == 64)
#define CFG_MDCNFG_VAL		0x00001ac9
#elif MEMSIZE == 128
#define CFG_MDCNFG_VAL		0x00001a53
#else
#error "Unsupported memory size"
#endif
#define CFG_MDREFR_VAL		0x200bc015
#define CFG_MDMRS_VAL		0x00320032

#define CFG_FLYCNFG_VAL		0x00000000
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
#define CFG_USB_OHCI_SLOT_NAME		"cm-x270"
#define CONFIG_USB_STORAGE
#endif


/*
 * PCMCIA and CF Interfaces
 */
#define CFG_MECR_VAL		0x00000002
#define CFG_MCMEM0_VAL		0x0004ca84/*0x00040420*/
#define CFG_MCMEM1_VAL		0x00000000
#define CFG_MCATT0_VAL		0x0004ca84/*0x00040420*/
#define CFG_MCATT1_VAL		0x00000000
#define CFG_MCIO0_VAL		0x0004ca88/*0x00040420*/
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
