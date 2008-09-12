/*
 *  adsturboxbcf.c
 *
 *  Author:	
 *  Created:	Sep 20, 2006
 *  Copyright:	Applied Data Systems, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * 
 *
 */
 
#include <common.h>
#include <config.h>
#include <ata.h>
#include <asm/io.h>
#include <asm/arch/mx31-regs.h>
#include "board.h"

#ifdef CONFIG_ATA_16BIT_MEMORY

extern ulong ide_bus_offset[] ;


#define	ATA_CURR_BASE(dev)	(CFG_ATA_BASE_ADDR+ide_bus_offset[IDE_BUS(dev)])

static struct CompactFlash
{
	char sec_cnt;
	char sec_num;
	char cyl_low;
	char cyl_high;
	char hd;
	char cmd;
} ata_cmd_buf[CFG_IDE_MAXBUS];

void ide_outb(int dev, int port,  uchar value)
{
	u16 val = value;
	uchar * cmd;
	uchar index = port & 0xF;
	int i;

	cmd = (uchar *) &ata_cmd_buf[dev];
	
	switch(port)
	{
		case ATA_SECT_CNT ... ATA_CYL_HIGH:
			cmd[index-2] = value;
			break;
		case ATA_DEV_HD/*6*/:
			cmd[index-2] = value;
			if(cmd[0] == 0x00)
			{
				val |= 0x1000;
				writew(val, ATA_CURR_BASE(dev) + port);
			}
			break;
		case ATA_COMMAND/*7*/:
			//val = (value<<8)|0xe0;
			cmd[index-2] = value;
			for(i=0; i<6; i+=2)
			{
				val = cmd[i] | (cmd[i+1]<<8);
				writew(val, ATA_CURR_BASE(dev) + i+ATA_SECT_CNT);
				cmd[0] = 0x00;
			}
			return;
		default:
			break;
	}
}

unsigned char ide_inb(int dev, int port)
{
	uchar val;
	val = readb(ATA_CURR_BASE(dev)+port);
	return (val);
}
#endif

#if 0
void outb(uchar val, __u32 addr)
{
	writeb(val, addr);
}

uchar inb(__u32 addr)
{
	return readb(addr);
}

void insw (u32 port, ushort *addr, int count)
{

	for(; count; count --) 
	{
		* addr = readw(port);;
		addr ++;
	}
}

void outsw (u32 port, u16 *addr, int count)
{
	
	for(; count; count --) {
		writew(*addr, port);
		addr ++ ;
	}
}
#endif

void pcmcia_uninit (void)
{
	ADSBITSYG5_PCMCIA &= ~ADSBITSYG5_PCMCIA_5V; /*5v off*/
	ADSBITSYG5_PCMCIA &= ~ADSBITSYG5_PCMCIA_33V;/*3.3v off*/
}

#include <asm/arch/iomux.h>
int	pcmcia_init (void)
{
	ADSBITSYG5_PCMCIA &= ~ADSBITSYG5_PCMCIA_5V;
	ADSBITSYG5_PCMCIA |= ADSBITSYG5_PCMCIA_33V;/*Power 3.3v*/
	udelay(1000);
	return 0;
}

void ide_power (int on)
{
	ADSBITSYG5_PCMCIA &= ~(ADSBITSYG5_CF_5V | ADSBITSYG5_CF_33V);
	if(on)
	{
		udelay(1000);
		ADSBITSYG5_PCMCIA |= ADSBITSYG5_CF_33V;/*Power 3.3v*/
		udelay(1000);
	}
}

int ide_preinit (void)
{
	ide_power(1);
	return 0;
}

void ide_set_reset(int idereset)
{
	if(idereset)
		ADSBITSYG5_PCMCIA |= ADSBITSYG5_RST_CF;/* assert Reset*/
	else
		ADSBITSYG5_PCMCIA &= ~ADSBITSYG5_RST_CF;/* de-assert Reset*/
}
