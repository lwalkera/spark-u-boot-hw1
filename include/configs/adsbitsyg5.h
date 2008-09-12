/*
 *  adsbitsyg5.h
 *
 *  Author:		jl.shan@vantrontech.com.cn
 *  Created:	May 9, 2007
 *  Copyright:	Applied Data Systems, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *
 *
 */
#ifndef __CONFIG_H
#define __CONFIG_H

#include <asm/arch/mx31-regs.h>		/* get chip and board defs */
/*
 * High Level Configuration Options
 */
#define CONFIG_MX31		1	/* This is an arm1136 CPU core */
#define CONFIG_ADSBITSYG5	1	/* on a Adsbitsy G5 Board      */
#define CONFIG_MX31_PCMCIA	1

#define CONFIG_PMIC		1
#define CONFIG_SPI		1
#define CFG_CPU_SPEED		399

/*ChangeLog: Seee board/adsbitsyg5/ChangeLog.*/

#undef CONFIG_USE_IRQ /* no support for IRQs */

/* the MX31 ads use 32K clock */
#define CFG_HZ 32768

/*
 * Hardware drivers
 */

#define CFG_PCMCIA_MEM_ADDR	0xbc000000
#define CFG_PCMCIA_MEM_SIZE	0x1000000

/*-----------------------------------------------------------------------
 * IDE/ATA stuff
 *-----------------------------------------------------------------------
 */
#define CFG_IDE_MAXBUS		1	/* max. 1 IDE busses    */
#define CFG_IDE_MAXDEVICE	1
#define CFG_ATA_IDE0_OFFSET	0x0000

#define CFG_ATA_BASE_ADDR	0xbd000000
/* Offset for data I/O			*/
#define CFG_ATA_DATA_OFFSET	0x1f0

/* Offset for normal register accesses	*/
#define CFG_ATA_REG_OFFSET	0x1f0

/* Offset for alternate registers	*/
#define CFG_ATA_ALT_OFFSET	0x3f0

/*
 * Ether Configuration
 */
#if 1
#define BOARD_LATE_INIT
#define CONFIG_MULTI_SMCETH
#define CONFIG_DRIVER_SMC91111
#define CONFIG_SMC91111_BASE	(CS5_BASE_ADDR+0x300)
#endif
#define CONFIG_DRIVER_SMSC9118
#define CONFIG_SMSC9118_BASE	(CS5_BASE_ADDR)

/*
 * Serial Configuration
 */
#define CFG_MX31_UART
#define CONFIG_SERIAL		3 /*Select serial.UART3*/
#define CONFIG_BAUDRATE		38400

/*
 * Size of malloc() pool
 */
#define CFG_MALLOC_LEN		(CFG_ENV_SIZE + SZ_128K)
#define CFG_GBL_DATA_SIZE	128 /* size in bytes reserved for initial data */

/* allow to overwrite serial and ethaddr */
#define CONFIG_ENV_OVERWRITE
#define CFG_BAUDRATE_TABLE	{9600, 19200, 38400, 57600, 115200}

#define CONFIG_DOS_PARTITION	1

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


#define CONFIG_BOOT_RETRY_TIME	-1
#define CONFIG_BOOT_RETRY_MIN	1
#define CONFIG_RESET_TO_RETRY

#undef CONFIG_SHOW_BOOT_PROGRESS
#define CONFIG_BOOTCOMMAND	"run start"

#define CONFIG_BOOTARGS		"console=ttyS2,38400 SERIALGETTY=ttyS2 "\
				" SERIALBAUD=38400"                     \
				" rw root=/dev/ram initrd=0x80600000,5m"\
				" ramdisk_size=12288 rootfstype=ext2"   \
				" " CONFIG_MTD_PARTS

/*
 * Since u-boot is > 128k, we need a 256k boot partiton.
 * The bootvars partition could be 128k but 256k anyway
 * so standard root_config.txt files can be used.
 */
#define CONFIG_MTD_PARTS	"mtdparts=flash0"			\
				":256k(boot)"				\
				",256k(bootvars)"			\
				",2m(zImage)"				\
				",5m(ramdisk.gz)"			\
				",-(flashfs1)"

/*
 * FLBLOCKSZ is really 128k but u-boot needs two blocks so use
 * 256k below so standard root_config.txt files can be used.
 */
#define CONFIG_EXTRA_ENV_SETTINGS                                         \
   "board=adsbitsyg5\0"                                                   \
   "console=ttyS2\0"	                                                  \
   "FLBLOCKSZ=256k\0"                                                     \
   "INITRD=0x80600000\0"                                                  \
   "tty_args=console=ttyS2,38400 SERIALGETTY=ttyS2 SERIALBAUD=38400\0"    \
   "eraseenv="	                                                          \
     "protect off 0xa0040000 0xa007ffff;"                                 \
     "erase 0xa0040000 0xa007ffff\0"                                      \
   "p_reprogram_uboot="	                                                  \
     "if pinit on && ide reset && fatload ide 0 scratch_buffer u-boot.bin;"\
     "then "			                                          \
       "protect off 0xa0000000 0xa003ffff;"		                  \
       "erase 0xa0000000 0xa003ffff;"	                                  \
       "cp.b scratch_buffer 0xa0000000 ${filesize};"                      \
     "fi\0"							          \
   "mmc_reprogram_uboot="                                                 \
     "if mmcinit && fatload mmc 0 scratch_buffer u-boot.bin;"             \
     "then "			                                          \
       "protect off 0xa0000000 0xa003ffff;"		                  \
       "erase 0xa0000000 0xa003ffff;"	                                  \
       "cp.b scratch_buffer 0xa0000000 ${filesize};"                      \
     "fi\0"							          \
   "usb_reprogram_uboot="                                                 \
     "if usb start && fatload usb 0 scratch_buffer u-boot.bin;"           \
     "then "			                                          \
       "protect off 0xa0000000 0xa003ffff;"		                  \
       "erase 0xa0000000 0xa003ffff;"	                                  \
       "cp.b scratch_buffer 0xa0000000 ${filesize};"                      \
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


#define CONFIG_DISPLAY_BOARDINFO
#define CONFIG_BOOTDELAY		3
#define CONFIG_SETUP_MEMORY_TAGS	1
//#define CONFIG_REVISION_TAG		1
#define CONFIG_SERIAL_TAG		1
#define CONFIG_CMDLINE_TAG		1 /* enable passing of ATAGs */
#define CONFIG_NETMASK			255.255.255.0
#define CONFIG_IPADDR			192.168.1.158

#define CONFIG_BOOTFILE			"zImage"

/*
 * Miscellaneous configurable options
 */
#define CFG_HUSH_PARSER		1
#define CONFIG_AUTO_COMPLETE	1
#define CFG_PROMPT_HUSH_PS2	"> "

#define CFG_LONGHELP				/* undef to save memory		*/
#ifdef CFG_HUSH_PARSER
#define CFG_PROMPT		"$ "		/* Monitor Command Prompt */
#else
#define CFG_PROMPT		"=> "		/* Monitor Command Prompt */
#endif
#define CFG_CBSIZE		256		/* Console I/O Buffer Size	*/
#define CFG_PBSIZE (CFG_CBSIZE+sizeof(CFG_PROMPT)+16) /* Print Buffer Size */
#define CFG_MAXARGS		32		/* max number of command args	*/
#define CFG_BARGSIZE		CFG_CBSIZE	/* Boot Argument Buffer Size	*/
#define CFG_DEVICE_NULLDEV	1
#undef	CFG_CLKS_IN_HZ		/* everything, incl board info, in Hz */

#define CFG_MEMTEST_START	(CSD0_BASE_ADDR+0x04000000) /* memtest works on */
#define CFG_MEMTEST_END		(CFG_MEMTEST_START+0x800000)
#define CFG_LOAD_ADDR		(CSD0_BASE_ADDR+0x8000)	/* default load address */

#define CFG_MEM_MAP_ITEMS {{"zImage_buffer",  0x80008000, 0x00200000}, \
                           {"scratch_buffer", 0x80208000, 0x01c00000}}

/*
 * Physical Memory Map
 */
#define CONFIG_NR_DRAM_BANKS	1
#define PHYS_SDRAM_1		CSD0_BASE_ADDR
#define PHYS_SDRAM_1_SIZE	SZ_128M

/*-----------------------------------------------------------------------
 * Stack sizes
 *
 * The stack sizes are set up in start.S using the settings below
 */
#define CONFIG_STACKSIZE	SZ_128K	/* regular stack */
#ifdef CONFIG_USE_IRQ
#define CONFIG_STACKSIZE_IRQ	SZ_4K	/* IRQ stack */
#define CONFIG_STACKSIZE_FIQ	SZ_4K	/* FIQ stack */
#endif

/*-----------------------------------------------------------------------
 * FLASH and environment organization
 */
#define CFG_FLASH_CFI
#define CFG_FLASH_CFI_DRIVER	1

#define PHYS_FLASH_1		CS0_BASE_ADDR	/* Flash Bank #1 */
//#define PHYS_FLASH_SIZE_1	SZ_32M

/* write flash less slowly */
#define CFG_FLASH_USE_BUFFER_WRITE 1
#define CFG_UNLOCK_WHOLE_AMNESIAC_FLASH_AT_BOOT /* Intel K3 & P30 flash powers up with all blocks locked */

/* Flash environment locations */
#define CFG_ENV_IS_IN_FLASH	1
#define CFG_MONITOR_LEN		SZ_256K		/* Reserve 1 sector */
#define CFG_ENV_ADDR		(PHYS_FLASH_1 + CFG_MONITOR_LEN)	/* Addr of Environment Sector	*/
#define CFG_ENV_SIZE		SZ_256K		/* Total Size of Environment Sector */

#define CFG_FLASH_BASE		PHYS_FLASH_1
#define CFG_MAX_FLASH_BANKS	1		/* max number of memory banks */
#define CFG_MAX_FLASH_SECT	(255+4+4)	/* max number of sectors on one chip */
#define CFG_MONITOR_BASE	CFG_FLASH_BASE	/* Monitor at beginning of flash */

#if defined(CONFIG_CMD_MMC)
/*
 * MMC Interface
 */
#define CONFIG_MMC
#define CFG_MXC_SDHC		0x50004000
#define CFG_MMC_BASE		0xc0000000
#define CFG_MMC_MAX_BUS_WIDTH	1		/* in bits */
#endif

#if defined(CONFIG_CMD_USB)
/*
 * USB Interface
 */
#define CONFIG_USB_EHCI
#define LITTLEENDIAN			    /* used by usb_ehci.c */
#define CONFIG_USB_STORAGE
#define CFG_USB_EHCI_BASE	0x43f88500  /* ehci register base. */
#endif

#endif	/* __CONFIG_H */
