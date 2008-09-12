/*
 * Copyright 2004-2006 Freescale Semiconductor, Inc. All Rights Reserved.
 */

/*
 * The code contained herein is licensed under the GNU General Public
 * License. You may obtain a copy of the GNU General Public License
 * Version 2 or later at the following locations:
 *
 * http://www.opensource.org/licenses/gpl-license.html
 * http://www.gnu.org/copyleft/gpl.html
 */

#ifndef __MXC_MMC_REG_H__
#define __MXC_MMC_REG_H__

#include <asm/arch/hardware.h>
/*!
 * @defgroup MMC_SD MMC/SD Driver
 */

/*
 * MultiMediaCard (MMC) controller
 */
#define MXC_SDHC_REG(x)	__REG(CFG_MXC_SDHC + x)

#define MMC_STRPCL	MXC_SDHC_REG(0x00)  /* Control to start and stop MMC clock */
#define MMC_STAT	MXC_SDHC_REG(0x04)  /* MMC Status Register (read only) */
#define MMC_CLKRT	MXC_SDHC_REG(0x08)  /* MMC clock rate */
#define MMC_CMDAT	MXC_SDHC_REG(0x0c)  /* SPI mode control bits */
#define MMC_RESTO	MXC_SDHC_REG(0x10)  /* Command/response/data sequence control */
#define MMC_RDTO	MXC_SDHC_REG(0x14)  /* Expected response time out */
#define MMC_BLKLEN	MXC_SDHC_REG(0x18)  /* Block length of data transaction */
#define MMC_NOB		MXC_SDHC_REG(0x1c)  /* Number of blocks, for block mode */
#define MMC_REVNO	MXC_SDHC_REG(0x20)  /* Number of blocks, for block mode */
#define MMC_INTCNT	MXC_SDHC_REG(0x24)  /* Partial MMC_TXFIFO FIFO written */
#define MMC_CMD		MXC_SDHC_REG(0x28)  /* Index of current command */
#define MMC_ARG		MXC_SDHC_REG(0x2c)  /* Commmand argument */
#define MMC_RESFIFO		MXC_SDHC_REG(0x34)  /* Response FIFO (read only) */
#define MMC_BUFFACC MXC_SDHC_REG(0x38)   /* Data Buffer Access Reg */
#define MMC_REM_NOB       MXC_SDHC_REG(0x40)    /* Remaining NOB Reg */
#define MMC_REM_BLK_SIZE  MXC_SDHC_REG(0x44)    /* Remaining Block Size Reg */


/*!
 * @file mxc_mmc.h
 *
 * @brief Driver for the Freescale Semiconductor MXC SDHC modules.
 *
 * This file defines offsets and bits of SDHC registers. SDHC is also referred as
 * MMC/SD controller
 *
 * @ingroup MMC_SD
 */

#define DRIVER_NAME                     "MXCMMC"
#define SDHC_MEM_SIZE                   16384
#define SDHC_REV_NO                     0x400
#define READ_TO_VALUE                   0xFFFF


/* Bit definitions for STR_STP_CLK */
#define STR_STP_CLK_IPG_CLK_GATE_DIS    (1<<15)
#define STR_STP_CLK_IPG_PERCLK_GATE_DIS (1<<14)
#define STR_STP_CLK_RESET               (1<<3)
#define STR_STP_CLK_START_CLK           (1<<1)
#define STR_STP_CLK_STOP_CLK            (1<<0)

/* Bit definitions for STATUS */
#define STATUS_CARD_INSERTION           (1<<31)
#define STATUS_CARD_REMOVAL             (1<<30)
#define STATUS_YBUF_EMPTY               (1<<29)
#define STATUS_XBUF_EMPTY               (1<<28)
#define STATUS_YBUF_FULL                (1<<27)
#define STATUS_XBUF_FULL                (1<<26)
#define STATUS_BUF_UND_RUN              (1<<25)
#define STATUS_BUF_OVFL                 (1<<24)
#define STATUS_SDIO_INT_ACTIVE          (1<<14)
#define STATUS_END_CMD_RESP             (1<<13)
#define STATUS_WRITE_OP_DONE            (1<<12)
#define STATUS_READ_OP_DONE             (1<<11)
#define STATUS_WR_CRC_ERROR_CODE_MASK   (3<<10)
#define STATUS_CARD_BUS_CLK_RUN         (1<<8)
#define STATUS_BUF_READ_RDY             (1<<7)
#define STATUS_BUF_WRITE_RDY            (1<<6)
#define STATUS_RESP_CRC_ERR             (1<<5)
#define STATUS_READ_CRC_ERR             (1<<3)
#define STATUS_WRITE_CRC_ERR            (1<<2)
#define STATUS_TIME_OUT_RESP            (1<<1)
#define STATUS_TIME_OUT_READ            (1<<0)
#define STATUS_ERR_MASK                 0x3f

/* Clock rate definitions */
#define CLK_RATE_PRESCALER(x)           ((x) & 0xF)
#define CLK_RATE_CLK_DIVIDER(x)         (((x) & 0xF) << 4)

/* Bit definitions for CMD_DAT_CONT */
#define CMD_DAT_CONT_CMD_RESP_LONG_OFF  (1<<12)
#define CMD_DAT_CONT_STOP_READWAIT      (1<<11)
#define CMD_DAT_CONT_START_READWAIT     (1<<10)
#define CMD_DAT_CONT_BUS_WIDTH_1        (0<<8)
#define CMD_DAT_CONT_BUS_WIDTH_4        (2<<8)
#define CMD_DAT_CONT_INIT               (1<<7)
#define CMD_DAT_CONT_WRITE              (1<<4)
#define CMD_DAT_CONT_DATA_ENABLE        (1<<3)
#define CMD_DAT_CONT_RESPONSE_FORMAT_R1 (1)
#define CMD_DAT_CONT_RESPONSE_FORMAT_R2 (2)
#define CMD_DAT_CONT_RESPONSE_FORMAT_R3 (3)
#define CMD_DAT_CONT_RESPONSE_FORMAT_R4 (4)
#define CMD_DAT_CONT_RESPONSE_FORMAT_R5 (5)
#define CMD_DAT_CONT_RESPONSE_FORMAT_R6 (6)

/* Bit definitions for INT_CNTR */
#define INT_CNTR_SDIO_INT_WKP_EN        (1<<18)
#define INT_CNTR_CARD_INSERTION_WKP_EN  (1<<17)
#define INT_CNTR_CARD_REMOVAL_WKP_EN    (1<<16)
#define INT_CNTR_CARD_INSERTION_EN      (1<<15)
#define INT_CNTR_CARD_REMOVAL_EN        (1<<14)
#define INT_CNTR_SDIO_IRQ_EN            (1<<13)
#define INT_CNTR_DAT0_EN                (1<<12)
#define INT_CNTR_BUF_READ_EN            (1<<4)
#define INT_CNTR_BUF_WRITE_EN           (1<<3)
#define INT_CNTR_END_CMD_RES            (1<<2)
#define INT_CNTR_WRITE_OP_DONE          (1<<1)
#define INT_CNTR_READ_OP_DONE           (1<<0)


/* Copy from PXA-250 MMC controller registers */
/* MMC_STRPCL */

#define MMC_STRPCL_STOP_CLK     	(0x0001UL)
#define MMC_STRPCL_START_CLK		(0x0002UL)

/* MMC_STAT */
#define MMC_STAT_END_CMD_RES		(0x0001UL << 13)
#define MMC_STAT_PRG_DONE       	(0x0001UL << 12)
#define MMC_STAT_DATA_TRAN_DONE     	(0x0001UL << 11)
#define MMC_STAT_CLK_EN	 		(0x0001UL << 8)
#define MMC_STAT_RECV_FIFO_FULL     	(0x0001UL << 7)
#define MMC_STAT_XMIT_FIFO_EMPTY    	(0x0001UL << 6)
#define MMC_STAT_RES_CRC_ERROR      	(0x0001UL << 5)
#define MMC_STAT_SPI_READ_ERROR_TOKEN   (0x0001UL << 4)
#define MMC_STAT_CRC_READ_ERROR     	(0x0001UL << 3)
#define MMC_STAT_CRC_WRITE_ERROR    	(0x0001UL << 2)
#define MMC_STAT_TIME_OUT_RESPONSE  	(0x0001UL << 1)
#define MMC_STAT_READ_TIME_OUT      	(0x0001UL)

#define MMC_STAT_ERRORS (MMC_STAT_RES_CRC_ERROR|MMC_STAT_SPI_READ_ERROR_TOKEN\
	|MMC_STAT_CRC_READ_ERROR|MMC_STAT_TIME_OUT_RESPONSE\
	|MMC_STAT_READ_TIME_OUT|MMC_STAT_CRC_WRITE_ERROR)

/* MMC_CMD */
#define MMC_CMD_RESET			0
#define MMC_CMD_SEND_OP_COND		1
#define MMC_CMD_ALL_SEND_CID 		2
#define MMC_CMD_SET_RCA			3
#define MMC_CMD_SELECT_CARD		7
#define MMC_CMD_SEND_CSD 		9
#define MMC_CMD_SEND_CID 		10
#define MMC_CMD_SEND_STATUS		13
#define MMC_CMD_SET_BLOCKLEN		16
#define MMC_CMD_READ_BLOCK		17
#define MMC_CMD_RD_BLK_MULTI		18
#define MMC_CMD_WRITE_BLOCK		24
#define MMC_CMD_APP_CMD			55
#define SD_CMD_APP_SET_BUS_WIDTH	6
#define SD_CMD_APP_OP_COND		41

#ifndef CFG_MMC_MAX_BUS_WIDTH	
#define CFG_MMC_MAX_BUS_WIDTH		4	/* in bits */
#endif

/* MMC_CMDAT */
#define MMC_CMDAT_MMC_DMA_EN		(0x0001UL << 7)
#define MMC_CMDAT_1BIT	  		(0x0000UL << 8)
#define MMC_CMDAT_4BIT	  		(0x0002UL << 8)
#define MMC_CMDAT_INIT	  		(0x0001UL << 7)
#define MMC_CMDAT_WRITE	 		(0x0001UL << 4)
#define MMC_CMDAT_READ	  		(0x0000UL << 4)
#define MMC_CMDAT_DATA_EN       	(0x0001UL << 3)
#define MMC_CMDAT_R1	    		(0x0001UL)
#define MMC_CMDAT_R2	    		(0x0002UL)
#define MMC_CMDAT_R3	    		(0x0003UL)

/* MMC_RESTO */
#define MMC_RES_TO_MAX	  		(0x007fUL) /* [6:0] */

/* MMC_RDTO */
#define MMC_READ_TO_MAX	 		(0x0ffffUL) /* [15:0] */

/* MMC_BLKLEN */
#define MMC_BLK_LEN_MAX	 		(0x03ffUL) /* [9:0] */

/* MMC_PRTBUF */
#define MMC_PRTBUF_BUF_PART_FULL       	(0x01UL)
#define MMC_PRTBUF_BUF_FULL		(0x00UL    )

/* MMC_I_MASK */
#define MMC_I_MASK_TXFIFO_WR_REQ	(0x01UL << 6)
#define MMC_I_MASK_RXFIFO_RD_REQ	(0x01UL << 5)
#define MMC_I_MASK_CLK_IS_OFF	   	(0x01UL << 4)
#define MMC_I_MASK_STOP_CMD	 	(0x01UL << 3)
#define MMC_I_MASK_END_CMD_RES	  	(0x01UL << 2)
#define MMC_I_MASK_PRG_DONE	 	(0x01UL << 1)
#define MMC_I_MASK_DATA_TRAN_DONE       (0x01UL)
#define MMC_I_MASK_ALL	      		(0x07fUL)


/* MMC_I_REG */
#define MMC_I_REG_TXFIFO_WR_REQ     	(0x01UL << 6)
#define MMC_I_REG_RXFIFO_RD_REQ     	(0x01UL << 5)
#define MMC_I_REG_CLK_IS_OFF		(0x01UL << 4)
#define MMC_I_REG_STOP_CMD      	(0x01UL << 3)
#define MMC_I_REG_END_CMD_RES       	(0x01UL << 2)
#define MMC_I_REG_PRG_DONE      	(0x01UL << 1)
#define MMC_I_REG_DATA_TRAN_DONE    	(0x01UL)
#define MMC_I_REG_ALL	   		(0x007fUL)

/* MMC_CMD */
#define MMC_CMD_INDEX_MAX       	(0x006fUL)  /* [5:0] */
#define CMD(x)  (x)

#define MMC_DEFAULT_RCA			1

#define MMC_BLOCK_SIZE			512
#define MMC_CMD_RESET			0
#define MMC_CMD_SEND_OP_COND		1
#define MMC_CMD_ALL_SEND_CID 		2
#define MMC_CMD_SET_RCA			3
#define MMC_CMD_SEND_CSD 		9
#define MMC_CMD_SEND_CID 		10
#define MMC_CMD_SEND_STATUS		13
#define MMC_CMD_SET_BLOCKLEN		16
#define MMC_CMD_READ_BLOCK		17
#define MMC_CMD_RD_BLK_MULTI		18
#define MMC_CMD_WRITE_BLOCK		24

#define MMC_MAX_BLOCK_SIZE		512

#define MMC_R1_IDLE_STATE		0x01
#define MMC_R1_ERASE_STATE		0x02
#define MMC_R1_ILLEGAL_CMD		0x04
#define MMC_R1_COM_CRC_ERR		0x08
#define MMC_R1_ERASE_SEQ_ERR		0x01
#define MMC_R1_ADDR_ERR			0x02
#define MMC_R1_PARAM_ERR		0x04

#define MMC_R1B_WP_ERASE_SKIP		0x0002
#define MMC_R1B_ERR			0x0004
#define MMC_R1B_CC_ERR			0x0008
#define MMC_R1B_CARD_ECC_ERR		0x0010
#define MMC_R1B_WP_VIOLATION		0x0020
#define MMC_R1B_ERASE_PARAM		0x0040
#define MMC_R1B_OOR			0x0080
#define MMC_R1B_IDLE_STATE		0x0100
#define MMC_R1B_ERASE_RESET		0x0200
#define MMC_R1B_ILLEGAL_CMD		0x0400
#define MMC_R1B_COM_CRC_ERR		0x0800
#define MMC_R1B_ERASE_SEQ_ERR		0x1000
#define MMC_R1B_ADDR_ERR		0x2000
#define MMC_R1B_PARAM_ERR		0x4000

typedef struct mmc_cid
{
/* FIXME: BYTE_ORDER */
   uchar year:4,
   month:4;
   uchar sn[3];
   uchar fwrev:4,
   hwrev:4;
   uchar name[6];
   uchar id[3];
} mmc_cid_t;

typedef struct mmc_csd
{
	uchar	ecc:2,
		file_format:2,
		tmp_write_protect:1,
		perm_write_protect:1,
		copy:1,
		file_format_grp:1;
	uint64_t content_prot_app:1,
		rsvd3:4,
		write_bl_partial:1,
		write_bl_len:4,
		r2w_factor:3,
		default_ecc:2,
		wp_grp_enable:1,
		wp_grp_size:5,
		erase_grp_mult:5,
		erase_grp_size:5,
		c_size_mult1:3,
		vdd_w_curr_max:3,
		vdd_w_curr_min:3,
		vdd_r_curr_max:3,
		vdd_r_curr_min:3,
		c_size:12,
		rsvd2:2,
		dsr_imp:1,
		read_blk_misalign:1,
		write_blk_misalign:1,
		read_bl_partial:1;

	ushort	read_bl_len:4,
		ccc:12;
	uchar	tran_speed;
	uchar	nsac;
	uchar	taac;
	uchar	rsvd1:2,
  		spec_vers:4,
		csd_structure:2;
} mmc_csd_t;

#endif				/* __MXC_MMC_REG_H__ */
