/*
 * (C) Copyright 2003
 * Kyle Harris, Nexus Technologies, Inc. kharris@nexus-tech.net
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
 *
 *  Copy from cpu/pxa/mmc.c
 *  Copyright:	Applied Data Systems, Inc. jl.shan@vantrontech.com.cn
 */

#undef DEBUG
#undef MMC_DEBUG
#include <config.h>
#include <common.h>
#include <mmc.h>
#include <asm/errno.h>
#include <asm/arch/hardware.h>
#include <asm/arch/mxc_mmc.h>
#include <part.h>
#include <asm/types.h>

#ifdef CONFIG_MMC
extern int fat_register_device(block_dev_desc_t *dev_desc, int part_no);

static block_dev_desc_t mmc_dev;

block_dev_desc_t * mmc_get_dev(int dev)
{
	return ((block_dev_desc_t *)&mmc_dev);
}

static uchar mmc_buf[MMC_BLOCK_SIZE];
static uchar spec_ver;
static int mmc_ready = 0;
static int wide = 0;

static void start_clock(int wait)
{
	int wait_cnt = 0;

	while(1)
	{
		MMC_STRPCL = STR_STP_CLK_START_CLK;
		if(!wait)
			break;

		wait_cnt = 100;
		while(wait_cnt --)
		{
			if(MMC_STAT & STATUS_CARD_BUS_CLK_RUN) return;
			debug("%s() 0x%x\n", __FUNCTION__, MMC_STAT);
		}
	}//~~while()
}

static void stop_clock(int wait)
{
	int wait_cnt = 0;

	MMC_STAT = MMC_STAT;
	MMC_STRPCL = 0x00;
	while(1) {
		MMC_STRPCL = STR_STP_CLK_STOP_CLK;
		if(!wait)
			break;

		wait_cnt = 100;
		while(wait_cnt --) {
			if(!(MMC_STAT & STATUS_CARD_BUS_CLK_RUN)) return;
			debug("%s() %x\n", __FUNCTION__, MMC_STAT);
			MMC_STRPCL = STR_STP_CLK_STOP_CLK;
			MMC_STAT = MMC_STAT;
			udelay(100);
		}
	}//~~while()
}

static u8 mmc_buf[MMC_BLOCK_SIZE];
static mmc_csd_t mmc_csd;

static u32 * mmc_cmd(u32 cmd, u32 arg, u32 cmdat)
{
	u32 status = 0;
	static u32 a, b, c, resp[4];
	int i;

	debug("%s() cmd(%x) arg(%x) cmddat(%x)\n", __FUNCTION__, cmd, arg, cmdat);
	
	MMC_STAT = MMC_STAT;
	stop_clock(0);
	MMC_CMDAT = cmdat |(wide==4?MMC_CMDAT_4BIT:MMC_CMDAT_1BIT);
	MMC_CMD = cmd;
	MMC_ARG = arg;
	start_clock(1);

	i = 100;
	do {
		udelay(40);	
		status = MMC_STAT;
		if(status & STATUS_TIME_OUT_RESP) {
			debug("time out MMC,status 0x%x\n", status);
			stop_clock(0);
			return NULL;
		}
		if(status & STATUS_END_CMD_RESP) {
			MMC_STAT = STATUS_END_CMD_RESP;
			debug("response ok!\n");
			break;
		}
	}while(i--);

    /* Read response from the card */
	resp[0]= 0x00;
	resp[1]= 0x00;
	resp[2]= 0x00;
	resp[3]= 0x00;
	
	switch (cmdat&0x3) {
		case MMC_CMDAT_R1:  /* r1, r1b, r6 */
			a = MMC_RESFIFO & 0xffff;
			b = MMC_RESFIFO & 0xffff;
			c = MMC_RESFIFO & 0xffff;
			resp[0] = a << 24 | b << 8 | c >> 8;
			break;
		case MMC_CMDAT_R3:  /* r3, r4 */
			a = MMC_RESFIFO & 0xffff;
			b = MMC_RESFIFO & 0xffff;
			c = MMC_RESFIFO & 0xffff;
			resp[0] = a << 24 | b << 8 | c >> 8;
			break;
		case MMC_CMDAT_R2:
			for (i = 0; i < 4; i++) {
				a = MMC_RESFIFO & 0xffff;
				b = MMC_RESFIFO & 0xffff;
				resp[i] = a << 16 | b;
			}
			break;
		default:
			break;
	}

	if(!(cmd & MMC_CMDAT_DATA_EN))
		stop_clock(0);
	debug("resp %x %x %x %x\n", resp[0], resp[1], resp[2], resp[3]);

	return resp;
}

int mmc_block_write(ulong dst, uchar *src, int len)
{
	u32 *p = (u32 *) src;
	int i=0;

	if (len == 0) {
		return 0;
	}

	debug("mmc_block_wr dst %lx src %lx len %d\n", dst, (ulong)src, len);

	/* set block len */
	mmc_cmd(MMC_CMD_SET_BLOCKLEN, len, MMC_CMDAT_R1);

	/* send write command */
	MMC_NOB = 1;
	MMC_BLKLEN = len;
	mmc_cmd(MMC_CMD_WRITE_BLOCK, dst, 
			MMC_CMDAT_R1|MMC_CMDAT_WRITE|MMC_CMDAT_DATA_EN);

	for(i=0; i<len; i+=4) {
		while(!(MMC_STAT & (STATUS_BUF_WRITE_RDY | STATUS_WRITE_OP_DONE)))
			;
		MMC_BUFFACC = *p++;
	}
	while(!(MMC_STAT & STATUS_WRITE_OP_DONE))
		;
	stop_clock(0);
	return 0;
}

int mmc_block_read(u8 *dst, u32 src, u32 len)
{
	u32 *p = (u32 *) dst;
	int i=0;
	debug("%s(): %x %x %d\n", __FUNCTION__, dst, src, len);

	mmc_cmd(MMC_CMD_SET_BLOCKLEN, len, MMC_CMDAT_R1);

	MMC_NOB = 1;
	MMC_BLKLEN = len;
	mmc_cmd(MMC_CMD_READ_BLOCK, src, MMC_CMDAT_R1 | MMC_CMDAT_READ | MMC_CMDAT_DATA_EN);
	for(i=0;i<len;i+=4) {
		while (!(MMC_STAT & ((STATUS_BUF_READ_RDY | STATUS_READ_OP_DONE))))
			;
		*p = MMC_BUFFACC;
		p++;
	}

	while (!(MMC_STAT & STATUS_READ_OP_DONE)) ;

	stop_clock(0);
	return 0;
}


int mmc_read(ulong src, uchar *dst, int size)
{
	ulong end, part_start, part_end, part_len, aligned_start, aligned_end;
	ulong mmc_block_size, mmc_block_address;

	debug("%s:src=%x dst=%p size=%d\n", __FUNCTION__, 
			src, dst, size);
	if(!size)
		return 0;

	mmc_block_size = MMC_BLOCK_SIZE;
	mmc_block_address = ~(mmc_block_size -1);

	src -= CFG_MMC_BASE;
	end = src+size;
	part_start = ~mmc_block_address & src;
	part_end = ~mmc_block_address & end;
	aligned_start = mmc_block_address & src;
	aligned_end = mmc_block_address & end;

	/* all block aligned accesses */
	debug("src %lx dst %lx end %lx pstart %lx pend %lx astart %lx aend %lx\n",
	src, (ulong)dst, end, part_start, part_end, aligned_start, aligned_end);
	if (part_start) {
		part_len = mmc_block_size - part_start;
		debug("ps src %lx dst %lx end %lx pstart %lx pend %lx astart %lx aend %lx\n",
		src, (ulong)dst, end, part_start, part_end, aligned_start, aligned_end);
		if ((mmc_block_read(mmc_buf, aligned_start, mmc_block_size)) < 0) {
			return -1;
		}
		memcpy(dst, mmc_buf+part_start, part_len);
		dst += part_len;
		src += part_len;
	}
	debug("src %lx dst %lx end %lx pstart %lx pend %lx astart %lx aend %lx\n",
	src, (ulong)dst, end, part_start, part_end, aligned_start, aligned_end);
	for (; src < aligned_end; src += mmc_block_size, dst += mmc_block_size) {
		debug("al src %lx dst %lx end %lx pstart %lx pend %lx astart %lx aend %lx\n",
		src, (ulong)dst, end, part_start, part_end, aligned_start, aligned_end);
		if ((mmc_block_read((uchar *)(dst), src, mmc_block_size)) < 0) {
			return -1;
		}
	}
	debug("src %lx dst %lx end %lx pstart %lx pend %lx astart %lx aend %lx\n",
	src, (ulong)dst, end, part_start, part_end, aligned_start, aligned_end);
	if (part_end && src < end) {
		debug("pe src %lx dst %lx end %lx pstart %lx pend %lx astart %lx aend %lx\n",
		src, (ulong)dst, end, part_start, part_end, aligned_start, aligned_end);
		if ((mmc_block_read(mmc_buf, aligned_end, mmc_block_size)) < 0) {
			return -1;
		}
		memcpy(dst, mmc_buf, part_end);
	}
	return 0;
}

int mmc_write(uchar *src, ulong dst, int size)
{
	ulong end, part_start, part_end, part_len, aligned_start, aligned_end;
	ulong mmc_block_size, mmc_block_address;

	if (size == 0) {
		return 0;
	}

	mmc_block_size = MMC_BLOCK_SIZE;
	mmc_block_address = ~(mmc_block_size - 1);

	dst -= CFG_MMC_BASE;
	end = dst + size;
	part_start = ~mmc_block_address & dst;
	part_end = ~mmc_block_address & end;
	aligned_start = mmc_block_address & dst;
	aligned_end = mmc_block_address & end;

	/* all block aligned accesses */
	debug("src %lx dst %lx end %lx pstart %lx pend %lx astart %lx aend %lx\n",
	src, (ulong)dst, end, part_start, part_end, aligned_start, aligned_end);
	if (part_start) {
		part_len = mmc_block_size - part_start;
		debug("ps src %lx dst %lx end %lx pstart %lx pend %lx astart %lx aend %lx\n",
		(ulong)src, dst, end, part_start, part_end, aligned_start, aligned_end);
		if ((mmc_block_read(mmc_buf, aligned_start, mmc_block_size)) < 0) {
			return -1;
		}
		memcpy(mmc_buf+part_start, src, part_len);
		if ((mmc_block_write(aligned_start, mmc_buf, mmc_block_size)) < 0) {
			return -1;
		}
		dst += part_len;
		src += part_len;
	}
	debug("src %lx dst %lx end %lx pstart %lx pend %lx astart %lx aend %lx\n",
	src, (ulong)dst, end, part_start, part_end, aligned_start, aligned_end);
	for (; dst < aligned_end; src += mmc_block_size, dst += mmc_block_size) {
		debug("al src %lx dst %lx end %lx pstart %lx pend %lx astart %lx aend %lx\n",
		src, (ulong)dst, end, part_start, part_end, aligned_start, aligned_end);
		if ((mmc_block_write(dst, (uchar *)src, mmc_block_size)) < 0) {
			return -1;
		}
	}
	debug("src %lx dst %lx end %lx pstart %lx pend %lx astart %lx aend %lx\n",
	src, (ulong)dst, end, part_start, part_end, aligned_start, aligned_end);
	if (part_end && dst < end) {
		debug("pe src %lx dst %lx end %lx pstart %lx pend %lx astart %lx aend %lx\n",
		src, (ulong)dst, end, part_start, part_end, aligned_start, aligned_end);
		if ((mmc_block_read(mmc_buf, aligned_end, mmc_block_size)) < 0) {
			return -1;
		}
		memcpy(mmc_buf, src, part_end);
		if ((mmc_block_write(aligned_end, mmc_buf, mmc_block_size)) < 0) {
			return -1;
		}
	}
	return 0;
}

unsigned long mmc_bread(int dev_num, unsigned long blknr, lbaint_t blkcnt, void *dst)
{
	u32 src = blknr * MMC_BLOCK_SIZE + CFG_MMC_BASE;
	mmc_read(src, (uchar *)dst, blkcnt * MMC_BLOCK_SIZE);
	return blkcnt;
}

static void mmc_softreset(void)
{
	int i;

	/* disable interrupt*/
	MMC_STRPCL = 0x08;
	udelay(1000);
	MMC_STRPCL = 0x00;
	MMC_INTCNT = 0x00;

	MMC_STRPCL = 0x8;
	MMC_STRPCL = 0x9;
	for(i=0;i<8;i++)
		MMC_STRPCL = 0x1;

	MMC_CLKRT = 0x3f;
	MMC_CMDAT = 0x00;

	MMC_RESTO = 0xff;
	MMC_BLKLEN = 512;

	MMC_NOB = 1;

	MMC_RDTO = READ_TO_VALUE;
	/* MMC_INTCNT = INT_CNTR_END_CMD_RES; */

	/* set clock rate */
	MMC_CLKRT = 0x3f; 
	/* clean status.*/
	MMC_STAT = MMC_STAT;
}

#define UNSTUFF_BITS(resp,start,size)					\
	({								\
		const int __size = size;				\
		const uint32_t __mask = (__size < 32 ? 1 << __size : 0) - 1;	\
		const int32_t __off = 3 - ((start) / 32);			\
		const int32_t __shft = (start) & 31;			\
		uint32_t __res;						\
									\
		__res = resp[__off] >> __shft;				\
		if (__size + __shft > 32)				\
			__res |= resp[__off-1] << ((32 - __shft) % 32);	\
		__res & __mask;						\
	})

/*
 * Given the decoded CSD structure, decode the raw CID to our CID structure.
 */
static void mmc_decode_cid(uint32_t *resp)
{
	if (IF_TYPE_SD == mmc_dev.if_type) {
		/*
		 * SD doesn't currently have a version field so we will
		 * have to assume we can parse this.
		 */
		sprintf((char *) mmc_dev.vendor, "Man %02x OEM %c%c \"%c%c%c%c%c\" Date %02u/%04u",
						UNSTUFF_BITS(resp, 120, 8),
						UNSTUFF_BITS(resp, 112, 8),
						UNSTUFF_BITS(resp, 104, 8),
						UNSTUFF_BITS(resp, 96, 8),
						UNSTUFF_BITS(resp, 88, 8),
						UNSTUFF_BITS(resp, 80, 8),
						UNSTUFF_BITS(resp, 72, 8),
						UNSTUFF_BITS(resp, 64, 8),
						UNSTUFF_BITS(resp, 8, 4),
						UNSTUFF_BITS(resp, 12, 8) + 2000);
		sprintf((char *) mmc_dev.revision, "%d.%d",
						UNSTUFF_BITS(resp, 60, 4),
						UNSTUFF_BITS(resp, 56, 4));
		sprintf((char *) mmc_dev.product, "%u",
						UNSTUFF_BITS(resp, 24, 32));
	} else {
		/*
		 * The selection of the format here is based upon published
		 * specs from sandisk and from what people have reported.
		 */
		switch (spec_ver) {
		case 0: /* MMC v1.0 - v1.2 */
		case 1: /* MMC v1.4 */
			sprintf((char *) mmc_dev.vendor, "Man %02x%02x%02x \"%c%c%c%c%c%c%c\" Date %02u/%04u",
						UNSTUFF_BITS(resp, 120, 8),
						UNSTUFF_BITS(resp, 112, 8),
						UNSTUFF_BITS(resp, 104, 8),
						UNSTUFF_BITS(resp, 96, 8),
						UNSTUFF_BITS(resp, 88, 8),
						UNSTUFF_BITS(resp, 80, 8),
						UNSTUFF_BITS(resp, 72, 8),
						UNSTUFF_BITS(resp, 64, 8),
						UNSTUFF_BITS(resp, 56, 8),
						UNSTUFF_BITS(resp, 48, 8),
						UNSTUFF_BITS(resp, 12, 4),
						UNSTUFF_BITS(resp, 8, 4) + 1997);
			sprintf((char *) mmc_dev.revision, "%d.%d",
						UNSTUFF_BITS(resp, 44, 4),
						UNSTUFF_BITS(resp, 40, 4));
			sprintf((char *) mmc_dev.product, "%u",
						UNSTUFF_BITS(resp, 16, 24));
			break;

		case 2: /* MMC v2.0 - v2.2 */
		case 3: /* MMC v3.1 - v3.3 */
		case 4: /* MMC v4 */
			sprintf((char *) mmc_dev.vendor, "Man %02x OEM %04x \"%c%c%c%c%c%c\" Date %02u/%04u",
						UNSTUFF_BITS(resp, 120, 8),
						UNSTUFF_BITS(resp, 104, 16),
						UNSTUFF_BITS(resp, 96, 8),
						UNSTUFF_BITS(resp, 88, 8),
						UNSTUFF_BITS(resp, 80, 8),
						UNSTUFF_BITS(resp, 72, 8),
						UNSTUFF_BITS(resp, 64, 8),
						UNSTUFF_BITS(resp, 56, 8),
						UNSTUFF_BITS(resp, 12, 4),
						UNSTUFF_BITS(resp, 8, 4) + 1997);
			sprintf((char *) mmc_dev.product, "%u",
						UNSTUFF_BITS(resp, 16, 32));
			sprintf((char *) mmc_dev.revision, "N/A");
			break;

		default:
			printf("MMC card has unknown MMCA version %d\n",
						spec_ver);
			break;
		}
	}
	printf("%s card.\nVendor: %s\nProduct: %s\nRevision: %s\n",(IF_TYPE_SD == mmc_dev.if_type)?"SD":"MMC", mmc_dev.vendor, mmc_dev.product, mmc_dev.revision);
}

/*
 * Given a 128-bit response, decode to our card CSD structure.
 */
static void mmc_decode_csd(uint32_t *resp)
{
	unsigned int mult, csd_struct;

	if (IF_TYPE_SD == mmc_dev.if_type) {
		csd_struct = UNSTUFF_BITS(resp, 126, 2);
		if (csd_struct != 0) {
			printf("SD: unrecognised CSD structure version %d\n",
						csd_struct);
			return;
		}
	} else {
		/*
		 * We only understand CSD structure v1.1 and v1.2.
		 * v1.2 has extra information in bits 15, 11 and 10.
		 */
		csd_struct = UNSTUFF_BITS(resp, 126, 2);
		if (csd_struct != 1 && csd_struct != 2) {
			printf("MMC: unrecognised CSD structure version %d\n",
						csd_struct);
			return;
		}

		spec_ver = UNSTUFF_BITS(resp, 122, 4);
		mmc_dev.if_type = IF_TYPE_MMC;
	}

	mult = 1 << (UNSTUFF_BITS(resp,47,3) + 2);
	mmc_dev.lba = (1 + UNSTUFF_BITS(resp, 62, 12)) * mult;
	mmc_dev.blksz = 1 << UNSTUFF_BITS(resp, 80, 4);

	/* FIXME: The following just makes assumes that's the partition type -- should really read it */
	mmc_dev.part_type = PART_TYPE_DOS;
	mmc_dev.dev = 0;
	mmc_dev.lun = 0;
	mmc_dev.type = DEV_TYPE_HARDDISK;
	mmc_dev.removable = 0;
	mmc_dev.block_read = mmc_bread;

	printf("Detected: %u blocks of %u bytes (%uMB) ", mmc_dev.lba, mmc_dev.blksz, mmc_dev.lba*mmc_dev.blksz/(1024*1024));
}

int mmc_init(int verbose, int bus_width)
{
 	int retries, rc = -ENODEV;
	u32 *resp;
	uint32_t cid_resp[4];
	uint16_t rca = MMC_DEFAULT_RCA;

	if(bus_width == 4)
		wide = 4;
	else 
		wide = 1;

	mmc_dev.if_type = IF_TYPE_UNKNOWN;
	mmc_csd.c_size = 0;
	mmc_board_init(CFG_MXC_SDHC);
	mmc_softreset();
	udelay(1000);

	retries = 100;
	mmc_cmd(MMC_CMD_RESET, 0, MMC_CMDAT_INIT);
	retries = 10;
	while (retries--) {
 		udelay(50000);
		resp = mmc_cmd(MMC_CMD_APP_CMD, 0, MMC_CMDAT_R1);
		if (!(resp[0] & 0x00000020)) {
			debug("Card does not support APP_CMD\n");
			break;
		}
		resp = mmc_cmd(SD_CMD_APP_OP_COND, 0x00ff8000, MMC_CMDAT_R3|(retries<2?0:MMC_CMDAT_INIT));
		if (resp[0] & 0x80000000) {
			mmc_dev.if_type = IF_TYPE_SD;
			printf("Detected SD card\n");
			break;
		}
 	}

	if (retries <= 0 || !(IF_TYPE_SD == mmc_dev.if_type)) {
		debug("Failed to detect SD Card, trying MMC\n");
		resp = mmc_cmd(MMC_CMD_SEND_OP_COND, 0x00ff8000, MMC_CMDAT_R3);
		
		retries = 100;
		while(retries-- && resp && !(resp[0] & 0x80000000)) {
			udelay(2000);
			resp = mmc_cmd(MMC_CMD_SEND_OP_COND, 0x00ff8000, MMC_CMDAT_R3);
		}
	}

	/* try to get card id */
	resp = mmc_cmd(MMC_CMD_ALL_SEND_CID, 0, MMC_CMDAT_R2 );
	if (resp) {
		memcpy(cid_resp, resp, sizeof(cid_resp));

		/* MMC exists, get CSD too */
		resp = mmc_cmd(MMC_CMD_SET_RCA, rca<<16, MMC_CMDAT_R1);
		if (IF_TYPE_SD == mmc_dev.if_type) {
			rca = ((resp[0] & 0xffff0000) >> 16);
		}
		resp = mmc_cmd(MMC_CMD_SEND_CSD, rca<<16, MMC_CMDAT_R2);
		if (resp) {
			mmc_decode_csd(resp);
			rc = 0;
			mmc_ready = 1;
		}
		mmc_decode_cid(cid_resp);
	}

	if(mmc_dev.if_type == IF_TYPE_UNKNOWN) 
		return -1;

	MMC_CLKRT = 0x2;/* Set clock rate*/

	resp = mmc_cmd(MMC_CMD_SELECT_CARD, rca<<16, MMC_CMDAT_R1);
#if 0
	/* fill in device description */
	mmc_dev.part_type = PART_TYPE_DOS;
	mmc_dev.dev = 0;
	mmc_dev.lun = 0;
	mmc_dev.type = 0;
	/* FIXME fill in the correct size (is set to 32MByte) */
	mmc_dev.blksz = 512;
	mmc_dev.lba = 0x10000;
	mmc_dev.removable = 0;
	mmc_dev.block_read = mmc_bread;
#endif
	printf("Bus Width: %d-bit\n", wide);
	fat_register_device(&mmc_dev,1); /* partitions start counting with 1 */
	return rc;
}

int mmc2info(ulong addr)
{
	if (addr >= CFG_MMC_BASE && addr < 
			CFG_MMC_BASE + (mmc_dev.lba * mmc_dev.blksz)) {
		return 1;
	}
	return 0;
}

#if 0

#include <command.h>

int do_mmcmd (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	u32 cmd, arg, cmdat;

	cmd = simple_strtoul (argv[1], NULL, 16);
	arg = simple_strtoul (argv[2], NULL, 10);
	cmdat = simple_strtoul (argv[3], NULL, 10);
	mmc_cmd(cmd, arg, cmdat);
	return 0;
}

U_BOOT_CMD(
		mmcmd, 4, 0, do_mmcmd, 
		"mmcmd - read from mmc card\n",
		"mmcmd <cmd> <arg> <cmdat>\n"
		);
#endif

#endif	/* CONFIG_MMC */
