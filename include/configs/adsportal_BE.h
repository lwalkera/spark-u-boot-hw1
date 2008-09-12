/*
 * (C) Copyright 2005
 * Robert Whaley, Applied Data Systems, Inc. rwhaley@applieddata.net
 *
 * (C) Copyright 2003
 * Kyle Harris, Nexus Technologies, Inc. kharris@nexus-tech.net
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * Configuation settings for the ADSPORTAL board.
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
#define CONFIG_IXP425              1       /* This is an IXP425 CPU    */
#define CONFIG_ADSPORTAL           1       /* on an ADS Portal Board   */
#define BOARD_LATE_INIT		   1

#undef CONFIG_USE_IRQ                   /* we don't need IRQ/FIQ stuff */

/*
 * Size of malloc() pool
 */
#define CFG_MALLOC_LEN      (CFG_ENV_SIZE + 128*1024)
#define CFG_GBL_DATA_SIZE	128	/* size in bytes reserved for initial data */

#define CFG_MAC_ADDR_IN_EEPROM

/* allow to overwrite serial and ethaddr */
#define CONFIG_ENV_OVERWRITE

#define CONFIG_BAUDRATE         38400

#define CONFIG_DOS_PARTITION   1


/*
 * Command line configuration.
 */
#include <config_cmd_default.h>

#define CONFIG_CMD_ASKENV
#define CONFIG_CMD_FAT
#define CONFIG_CMD_IDE
#define CONFIG_CMD_MAP
#define CONFIG_CMD_PCMCIA
#define CONFIG_CMD_I2C

#undef CONFIG_CMD_AUTOSCRIPT
#undef CONFIG_CMD_NET
#undef CONFIG_CMD_LOADB


#define CONFIG_BOOT_RETRY_TIME -1
#define CONFIG_BOOT_RETRY_MIN 1
#define CONFIG_RESET_TO_RETRY

#define CONFIG_PCI

#define __mem_pci(a) ((unsigned long) (a))
 
// #define CONFIG_NET_MULTI
#define CONFIG_EEPRO100

#define CONFIG_BOOTDELAY        3

#define CONFIG_BOOTCOMMAND	"run start"

#define CONFIG_BOOTARGS		"console=ttyS1,38400 SERIALGETTY=ttyS1 "\
				" SERIALBAUD=38400"                     \
				" rw root=/dev/ram initrd=0x00600000,5m"\
				" ramdisk_size=12288 rootfstype=ext2"   \
				" " CONFIG_MTD_PARTS

#define CONFIG_MTD_PARTS	"mtdparts=flash0"			\
				":128k(boot)"				\
				",128k(bootvars)"			\
                                ",256k(ixp425netinit)"			\
				",2m(zImage)"				\
				",5m(ramdisk.gz)"			\
				",-(flashfs1)"

#ifndef ENDIAN
#define ENDIAN endian=big
#endif

#define CONFIG_EXTRA_ENV_SETTINGS                                         \
   "board=adsportal\0"                                                    \
   "endian=big\0"                                                         \
   "pcidelay=100\0"						          \
   "FLBLOCKSZ=128k\0"                                                     \
   "INITRD=0x00600000\0"                                                  \
   "tty_args=console=ttyS1,38400 SERIALGETTY=ttyS1 SERIALBAUD=38400\0"    \
   "eraseenv="	                                                          \
     "protect off 0x50020000 0x5003ffff;"                                 \
     "erase 0x50020000 0x5003ffff\0"                                      \
   "p_reprogram_uboot="	                                                  \
     "if pinit on && ide reset && fatload ide 0 scratch_buffer u-boot.bin;"\
     "then "			                                          \
       "protect off 0x50000000 0x5001ffff;"		                  \
       "erase 0x50000000 0x5001ffff;"	                                  \
       "cp.b scratch_buffer 0x50000000 ${filesize};"                      \
     "fi\0"							          \
   "execute_txt_file_cf="                                                 \
     "test -n ${cf_ide_ok} && "                                           \
     "fatenv ide 0 ${txt_file} ${txt_file} && "                           \
     "run ${txt_file} &&     "                                            \
     "setenv ${txt_file}\0"                                               \
   "execute_txt_file="                                                    \
      "echo Attempting to execute ${txt_file}.;"                          \
      "run execute_txt_file_cf || "                                       \
      "echo Did not execute ${txt_file}.\0"                               \
   "media_init="                                                          \
     "cf_ide_ok=;if pinit on && ide reset; then cf_ide_ok=y;fi\0"         \
   "start="                                                               \
     "run media_init;"                                                    \
     "txt_file=start.txt; run execute_txt_file;"                          \
     "test -n ${boot_linux} && run boot_linux\0"                          \
   "load_dram_cf="                                                        \
     "test -n ${cf_ide_ok} && fatload ide 0 ${load_buffer} ${load_file}\0"\
   "load_dram="                                                           \
     "run load_dram_cf\0"                                                 \
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
#define CONFIG_KGDB_BAUDRATE    230400          /* speed to run kgdb serial port */
#define CONFIG_KGDB_SER_INDEX   2               /* which serial port to use */
#endif

#define CONFIG_AUTO_COMPLETE	1

/*
 * Miscellaneous configurable options
 */
#define CFG_HUSH_PARSER		1
#define CFG_PROMPT_HUSH_PS2	"> "

#define CFG_LONGHELP                            /* undef to save memory         */
#ifdef CFG_HUSH_PARSER
#define CFG_PROMPT		"$ "		/* Monitor Command Prompt */
#else
#define CFG_PROMPT		"=> "		/* Monitor Command Prompt */
#endif
#define CFG_CBSIZE              256             /* Console I/O Buffer Size      */
#define CFG_PBSIZE (CFG_CBSIZE+sizeof(CFG_PROMPT)+16) /* Print Buffer Size */
#define CFG_MAXARGS             32              /* max number of command args   */
#define CFG_BARGSIZE            CFG_CBSIZE      /* Boot Argument Buffer Size    */
#define CFG_DEVICE_NULLDEV	1

#define CFG_MEMTEST_START       0x00400000      /* memtest works on     */
#define CFG_MEMTEST_END         0x00800000      /* 4 ... 8 MB in DRAM   */

#undef  CFG_CLKS_IN_HZ          /* everything, incl board info, in Hz */

#define CFG_LOAD_ADDR           0x00010000      /* default load address */

#define CFG_HZ                  66000000

#define CFG_BAUDRATE_TABLE      { 9600, 19200, 38400, 57600, 115200 }

#define CFG_MEM_MAP_ITEMS {{"zImage_buffer",  0x00008000, 0x00200000}, \
                           {"scratch_buffer", 0x00600000, 0x01c00000}}
/*
 * Stack sizes
 *
 * The stack sizes are set up in start.S using the settings below
 */
#define CONFIG_STACKSIZE        (128*1024)      /* regular stack */
#ifdef CONFIG_USE_IRQ
#define CONFIG_STACKSIZE_IRQ    (4*1024)        /* IRQ stack */
#define CONFIG_STACKSIZE_FIQ    (4*1024)        /* FIQ stack */
#endif

/***************************************************************
 * Platform/Board specific defines start here.
 ***************************************************************/

/*
 * Hardware drivers
 */

#define CONFIG_PCI_BOOTDELAY

/*
 * select serial console configuration
 */
#define CFG_IXP425_CONSOLE	IXP425_UART2   /* we use the second UART for console */

/*
 * Physical Memory Map
 */
#define CONFIG_NR_DRAM_BANKS    1          /* we have 2 banks of DRAM */
#define PHYS_SDRAM_1            0x00000000 /* SDRAM Bank #1 */
#define PHYS_SDRAM_1_SIZE       0x04000000 /* 64 MB */

#define PHYS_FLASH_1            0x50000000 /* Flash Bank #1 */
#define PHYS_FLASH_2            0x52000000 /* Flash Bank #2 */
#define PHYS_FLASH_SIZE         0x02000000 /* 32 MB */
#define PHYS_FLASH_BANK_SIZE    0x02000000 /* 32 MB Banks */
#define PHYS_FLASH_SECT_SIZE    0x00020000 /* 128 KB sectors (x1) */


#define CFG_DRAM_BASE           0x00000000
#define CFG_DRAM_SIZE           0x08000000

#define CFG_FLASH_BASE          PHYS_FLASH_1
#define CFG_FLASH_BANKS_LIST    {PHYS_FLASH_1, PHYS_FLASH_2}

#define CFG_UNLOCK_WHOLE_AMNESIAC_FLASH_AT_BOOT /* Intel K3 & P30 flash powers up with all blocks locked */

#define CFG_FLASH_USE_BUFFER_WRITE

/*
 * Expansion bus settings
 */
#define CFG_EXP_CS0				0xbcd23c42
#define CFG_EXP_CS1				0xbcd23c42
#define CFG_EXP_CS2				0xbcd23c42
#define CFG_EXP_CS3				0xbcd23c42

/*
 * SDRAM settings
 */
#define CFG_SDR_CONFIG		0xc
#define CFG_SDR_MODE_CONFIG	0x1
#define CFG_SDRAM_REFRESH_CNT 	0x81a

/*
 * GPIO settings
 */

/*
 * FLASH and environment organization
 */
#define CFG_FLASH_CFI
#define CFG_FLASH_CFI_DRIVER	1

/* board will hang if flash is probed 8bits wide */
#define CFG_FLASH_CFI_WIDTH FLASH_CFI_16BIT

#define CFG_MONITOR_BASE	PHYS_FLASH_1
#define CFG_MONITOR_LEN		0x20000

#define CFG_MAX_FLASH_BANKS     2         /* max number of memory banks           */
#define CFG_MAX_FLASH_SECT      (4 + 255) /* max number of sectors on one chip    */

/* timeout values are in ticks */
#define CFG_FLASH_ERASE_TOUT    (25*CFG_HZ) /* Timeout for Flash Erase */
#define CFG_FLASH_WRITE_TOUT    (25*CFG_HZ) /* Timeout for Flash Write */

/* Flash environment locations */
#define	CFG_ENV_IS_IN_FLASH		1
#define CFG_ENV_ADDR            (PHYS_FLASH_1 + 0x20000)        /* Addr of Environment Sector   */
#define CFG_ENV_SIZE            0x20000  /* Total Size of Environment Sector     */
#define CFG_ENV_SECT_SIZE            0x20000  /* Total Size of Environment Sector     */

/*-----------------------------------------------------------------------
 * PCMCIA stuff
 *-----------------------------------------------------------------------
 *
 */
#define CONFIG_I82365
#define	CONFIG_IDE_8xx_PCCARD	1	/* Use IDE with PC Card	Adapter	*/

#define CONFIG_PCMCIA_SLOT_A
/* #define CONFIG_PCMCIA_SLOT_B */

#define CFG_PCMCIA_MEM_ADDR	0x4b000000 /* this is CIS space */
#define CFG_PCMCIA_MEM_SIZE	0x1000

/*-----------------------------------------------------------------------
 * IDE/ATA stuff (Supports IDE harddisk on PCMCIA Adapter)
 *-----------------------------------------------------------------------
 */

#undef	CONFIG_IDE_LED			/* LED	 for ide not supported	*/
#undef	CONFIG_IDE_RESET		/* reset for ide not supported	*/

#define CFG_IDE_MAXBUS		1	/* max. 1 IDE bus		*/
#define CFG_IDE_MAXDEVICE	1	/* max. 1 drive per IDE bus	*/

#define CFG_ATA_IDE0_OFFSET	0x0000

#define CFG_ATA_BASE_ADDR	0x0100

/* Offset for data I/O			*/
#define CFG_ATA_DATA_OFFSET	0x0


/* Offset for normal register accesses	*/
#define CFG_ATA_REG_OFFSET	0x000

/* I2C settings */

#define CONFIG_SOFT_I2C

#define CFG_I2C_SPEED		40000
#define CFG_I2C_SLAVE		0xFE

#define SCL (1 << 5)
#define SDA (1 << 14)

#define I2C_INIT	(*IXP425_GPIO_GPOER &= ~SCL)
#define I2C_ACTIVE	(*IXP425_GPIO_GPOER &= ~SDA)
#define I2C_TRISTATE	(*IXP425_GPIO_GPOER |=  SDA)
#define I2C_READ	((*IXP425_GPIO_GPINR & SDA) != 0)
#define I2C_SDA(bit)	if(bit) *IXP425_GPIO_GPOUTR |=  SDA; \
			else    *IXP425_GPIO_GPOUTR &= ~SDA
#define I2C_SCL(bit)	if(bit) *IXP425_GPIO_GPOUTR |=  SCL; \
			else    *IXP425_GPIO_GPOUTR &= ~SCL
#define I2C_DELAY	udelay(10)

#endif  /* __CONFIG_H */
