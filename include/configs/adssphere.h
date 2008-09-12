/*
 * (C) Copyright 2005
 * Robert Whaley, Applied Data Systems, Inc. rwhaley@applieddata.net
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 * Gary Jennejohn <gj@denx.de>
 * David Mueller <d.mueller@elsoft.ch>
 *
 * Configuation settings for the ADS Sphere board.
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

#ifndef __CONFIG_H
#define __CONFIG_H

/*
 * High Level Configuration Options
 * (easy to change)
 */
#define CONFIG_ARM920T		1	/* This is an ARM920T Core	*/
#define CONFIG_EP93XX		1	/* in a Cirrus Logic 93XX SoC     */
#define CONFIG_ADSSPHERE        1       /* on an ADS Sphere board */
#define BOARD_LATE_INIT		1

/* Cirrus Logic header */
#define CONFIG_CIRRUS_HEADER_LE         /* little-endian */
#undef CONFIG_CIRRUS_HEADER_BE          /* big-endian */

/* input clock of PLL */
#define CONFIG_SYS_CLK_FREQ	3686400 /* the EP9312 has 3.6864MHz input clock */


/* #define USE_920T_MMU		1*/
#undef CONFIG_USE_IRQ			/* we don't need IRQ/FIQ stuff */

/*
 * Size of malloc() pool
 */
#define CFG_MALLOC_LEN		(CFG_ENV_SIZE + 128*1024)
#define CFG_GBL_DATA_SIZE	128	/* size in bytes reserved for initial data */

/*
 * Hardware drivers
 */

#define CONFIG_DRIVER_EP93XX_MAC
#define CONFIG_MII_SUPPRESS_PREAMBLE
#define CONFIG_MII
#define CFG_MAC_ADDR_IN_EEPROM
// #define CONFIG_PHY_ADDR			(1)



/*
 * select serial console configuration
 */
#define CONFIG_SERIAL1          1	/* we use SERIAL 1 */

/* allow to overwrite serial and ethaddr */
#define CONFIG_ENV_OVERWRITE

#define CONFIG_BAUDRATE		38400


/*
 * Command line configuration.
 */
#include <config_cmd_default.h>

#define CONFIG_CMD_ASKENV
#define CONFIG_CMD_DHCP
#define CONFIG_CMD_FAT
#define CONFIG_CMD_I2C
#define CONFIG_CMD_IDE
#define CONFIG_CMD_MAP
#define CONFIG_CMD_MII
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

#define CONFIG_BOOTARGS		"console=ttyAM0,38400 SERIALGETTY=ttyAM0"\
				" SERIALBAUD=38400"                     \
				" rw root=/dev/ram initrd=0x00600000,5m"\
				" ramdisk_size=12288 rootfstype=ext2"   \
				" " CONFIG_MTD_PARTS

#define CONFIG_MTD_PARTS	"mtdparts=flash0"			\
				":256k(boot)"				\
				",256k(bootvars)"			\
				",2m(zImage)"				\
				",5m(ramdisk.gz)"			\
				",-(flashfs1)"


#define CONFIG_EXTRA_ENV_SETTINGS                                         \
   "board=adssphere\0"                                                    \
   "FLBLOCKSZ=256k\0"                                                     \
   "INITRD=0x00600000\0"                                                  \
   "tty_args=console=ttyAM0,38400 SERIALGETTY=ttyAM0 SERIALBAUD=38400\0"  \
   "eraseenv="	                                                          \
     "protect off 0x60040000 0x6007ffff;"                                 \
     "erase 0x60040000 0x6007ffff\0"                                      \
   "p_reprogram_uboot="	                                                  \
     "if pinit on && ide reset && fatload ide 0 scratch_buffer u-boot.bin;"\
     "then "			                                          \
       "protect off 0x60000000 0x6003ffff;"		                  \
       "erase 0x60000000 0x6003ffff;"	                                  \
       "cp.b scratch_buffer 0x60000000 ${filesize};"                      \
     "fi\0"							          \
   "usb_reprogram_uboot="	                                          \
     "if usb start && fatload usb 0 scratch_buffer u-boot.bin;"           \
     "then "			                                          \
       "protect off 0x60000000 0x6003ffff;"		                  \
       "erase 0x60000000 0x6003ffff;"	                                  \
       "cp.b scratch_buffer 0x60000000 ${filesize};"                      \
     "fi\0"							          \
   "execute_txt_file_cf="                                                 \
     "test -n ${cf_ide_ok} && "                                           \
     "fatenv ide 0 ${txt_file} ${txt_file} && "                           \
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
      "run execute_txt_file_usb || "                                      \
      "echo Did not execute ${txt_file}.\0"                               \
   "media_init="                                                          \
     "cf_ide_ok=;if pinit on && ide reset; then cf_ide_ok=y;fi;"          \
     "usb_ide_ok=;if usb start; then usb_ide_ok=y;fi\0"                   \
   "start="                                                               \
     "run media_init;"                                                    \
     "txt_file=start.txt; run execute_txt_file;"                          \
     "test -n ${boot_linux} && run boot_linux\0"                          \
   "load_dram_cf="                                                        \
     "test -n ${cf_ide_ok} && fatload ide 0 ${load_buffer} ${load_file}\0"\
   "load_dram_usb="                                                        \
     "test -n ${usb_ide_ok} && fatload usb 0 ${load_buffer} ${load_file}\0"\
   "load_dram="                                                           \
     "run load_dram_cf || run load_dram_usb\0"                            \
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
#define CONFIG_KGDB_BAUDRATE	115200		/* speed to run kgdb serial port */
#define CONFIG_KGDB_SER_INDEX	1		/* which serial port to use */
#endif

#define CONFIG_AUTO_COMPLETE	1

/*
 * Miscellaneous configurable options
 */
#define CFG_HUSH_PARSER		1
#define CFG_PROMPT_HUSH_PS2	"> "

#define	CFG_LONGHELP				/* undef to save memory		*/
#ifdef CFG_HUSH_PARSER
#define CFG_PROMPT		"$ "		/* Monitor Command Prompt */
#else
#define CFG_PROMPT		"=> "		/* Monitor Command Prompt */
#endif
#define	CFG_CBSIZE		256		/* Console I/O Buffer Size	*/
#define	CFG_PBSIZE (CFG_CBSIZE+sizeof(CFG_PROMPT)+16) /* Print Buffer Size */
#define	CFG_MAXARGS		32		/* max number of command args	*/
#define CFG_BARGSIZE		CFG_CBSIZE	/* Boot Argument Buffer Size	*/

#define CFG_MEMTEST_START	0x00000000	/* memtest works on	*/
#define CFG_MEMTEST_END		0x00f00000	/*  MB in DRAM	*/

#undef  CFG_CLKS_IN_HZ		/* everything, incl board info, in Hz */

#define	CFG_LOAD_ADDR		0x00008000	/* default load address	*/

/* Timer 3 set for 2KHz operation */
#define	CFG_HZ			2048

/* valid baudrates */
#define CFG_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200 }

#define CFG_MEM_MAP_ITEMS {{"zImage_buffer",  0x00008000, 0x00200000}, \
                           {"scratch_buffer", 0x00208000, 0x01c00000}}
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

/*-----------------------------------------------------------------------
 * Physical Memory Map
 */
#if !defined(MEMSIZE)
#define MEMSIZE 64
#endif
#if (MEMSIZE == 64)
#define CONFIG_NR_DRAM_BANKS	4	   /* we have 2 banks of DRAM */
#define PHYS_SDRAM_1			0x00000000 /* Physycal address (Sync memory boot) */
#define PHYS_SDRAM_SIZE_1		0x02000000 /* 32 MB */
#define PHYS_SDRAM_2			0x04000000 /* Physycal address (Sync memory boot) */
#define PHYS_SDRAM_SIZE_2		0x02000000 /* 32 MB */
#define PHYS_SDRAM_3			0x00000000 /* Physycal address (Sync memory boot) */
#define PHYS_SDRAM_SIZE_3		0x00000000 /* 0 MB */
#define PHYS_SDRAM_4			0x00000000 /* Physycal address (Sync memory boot) */
#define PHYS_SDRAM_SIZE_4		0x00000000 /* 0 MB */
#elif MEMSIZE == 32
#define CONFIG_NR_DRAM_BANKS	4	   /* we have 1 banks of DRAM */
#define PHYS_SDRAM_1			0x00000000 /* Physycal address (Sync memory boot) */
#define PHYS_SDRAM_SIZE_1		0x02000000 /* 32 MB */
#define PHYS_SDRAM_2			0x00000000 /* Physycal address (Sync memory boot) */
#define PHYS_SDRAM_SIZE_2		0x00000000 /* 0 MB */
#define PHYS_SDRAM_3			0x00000000 /* Physycal address (Sync memory boot) */
#define PHYS_SDRAM_SIZE_3		0x00000000 /* 0 MB */
#define PHYS_SDRAM_4			0x00000000 /* Physycal address (Sync memory boot) */
#define PHYS_SDRAM_SIZE_4		0x00000000 /* 0 MB */
#elif MEMSIZE == 128
#define CONFIG_NR_DRAM_BANKS	4	   /* we have 2 banks of DRAM */
#define PHYS_SDRAM_1			0x00000000 /* Physycal address (Sync memory boot) */
#define PHYS_SDRAM_SIZE_1		0x04000000 /* 64 MB */
#define PHYS_SDRAM_2			0x04000000 /* Physycal address (Sync memory boot) */
#define PHYS_SDRAM_SIZE_2		0x04000000 /* 64 MB */
#define PHYS_SDRAM_3			0x00000000 /* Physycal address (Sync memory boot) */
#define PHYS_SDRAM_SIZE_3		0x00000000 /* 0 MB */
#define PHYS_SDRAM_4			0x00000000 /* Physycal address (Sync memory boot) */
#define PHYS_SDRAM_SIZE_4		0x00000000 /* 0 MB */
#else
#error "Unsupported memory size"
#endif

#define CFG_SMCBCR0_VAL			0x2000ffef
#define CFG_SMCBCR6_VAL			0x28001d20 /* determined experimentally for J3 */

#define PHYS_FLASH_1		0x60000000 /* Flash Bank #1 */

#define CFG_FLASH_BASE		PHYS_FLASH_1

#define CFG_UNLOCK_WHOLE_AMNESIAC_FLASH_AT_BOOT /* Intel K3 & P30 flash powers up with all blocks locked */

/* Boot parameters address */
/* This must reflect what's inside the kernel */
#define CONFIG_BOOT_PARAM_ADDR (PHYS_SDRAM_1 + 0x100)

/*-----------------------------------------------------------------------
 * FLASH and environment organization
 */

#define CFG_FLASH_CFI
#define CFG_FLASH_CFI_DRIVER	1

#define CFG_MONITOR_BASE	PHYS_FLASH_1
#define CFG_MONITOR_LEN		0x40000

#define CFG_MAX_FLASH_BANKS	1         /* max number of memory banks		*/
#define CFG_MAX_FLASH_SECT	(4 + 255) /* max number of sectors on one chip	*/

/* timeout values are in ticks */
#define CFG_FLASH_ERASE_TOUT	(2*CFG_HZ) /* Timeout for Flash Erase */
#define CFG_FLASH_WRITE_TOUT	(2*CFG_HZ) /* Timeout for Flash Write */

/* write flash less slowly */
#define CFG_FLASH_USE_BUFFER_WRITE 1

#define	CFG_ENV_IS_IN_FLASH	1
#define CFG_ENV_ADDR		(PHYS_FLASH_1 + 0x40000)	/* Addr of Environment Sector	*/
#define CFG_ENV_SIZE		0x40000	/* Total Size of Environment Sector	*/

/* Flash banks JFFS2 should use */
#define CFG_JFFS2_FIRST_BANK    0
#define CFG_JFFS2_FIRST_SECTOR	20
#define CFG_JFFS2_NUM_BANKS     1


#if defined(CONFIG_CMD_USB)
/*
 * USB Interface
 */
#define LITTLEENDIAN			/* used by usb_ohci.c		*/
#define CONFIG_USB_OHCI_NEW
#define CFG_USB_OHCI_CPU_INIT
#define CFG_USB_OHCI_MAX_ROOT_PORTS	3
#define CFG_USB_OHCI_REGS_BASE		USB_BASE
#define CFG_USB_OHCI_SLOT_NAME		"adssphere"
#define CONFIG_USB_STORAGE
#endif


/* PCMCIA stuff */

/* EP93xx PCMCIA register settings */
#define CFG_PC1ATTRIB		0x801d0f1d
#define CFG_PC1IO		0x80100f1d
#define CFG_PC1COMMON		0x801d0f1d

#define CONFIG_EP93XX_PCMCIA 1

#define CONFIG_DOS_PARTITION   1
#define CONFIG_PCMCIA_SLOT_A 1

#define CFG_PCMCIA_MEM_ADDR     0x48000000
#define CFG_PCMCIA_MEM_SIZE     0x04000000

#define CFG_IDE_MAXBUS		1        /* max. 1 IDE bus		*/
#define CFG_IDE_MAXDEVICE	1        /* max. 1 drive per IDE bus	*/

#define CFG_ATA_IDE0_OFFSET	0x0000

#define CFG_ATA_BASE_ADDR	0x40000000
#define CFG_ATA_REG_OFFSET	0x1f0
#define CFG_ATA_DATA_OFFSET	0x1f0


/* I2C settings */

#define CONFIG_SOFT_I2C

#define CFG_I2C_SPEED		40000
#define CFG_I2C_SLAVE		0xFE

#define SCL 0x01
#define SDA 0x02

#define I2C_INIT	(PGDDR |=  SCL)
#define I2C_ACTIVE	(PGDDR |=  SDA)
#define I2C_TRISTATE	(PGDDR &= ~SDA)
#define I2C_READ	((PGDR & SDA) != 0)
#define I2C_SDA(bit)	if(bit) PGDR |=  SDA; \
			else    PGDR &= ~SDA
#define I2C_SCL(bit)	if(bit) PGDR |=  SCL; \
			else    PGDR &= ~SCL
#define I2C_DELAY	udelay(5)

#endif	/* __CONFIG_H */
