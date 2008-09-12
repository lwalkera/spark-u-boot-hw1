/*
 * (C) Copyright 2005
 * Robert Whaley, Applied Data Systems, Inc. rwhaley@applieddata.net
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
#include "adssphere.h"
#include <asm/arch/ep93xx.h>

#define PCMCIA_WRPROT 0x01
#define PCMCIA_DETECT 0x06
#define PCMCIA_BVD1   0x08
#define PCMCIA_BVD2   0x10
#define PCMCIA_VS_3V  0x20
#define PCMCIA_READY  0x40
#define PCMCIA_VS_XV  0x80

void pcmcia_power_on(void)
{
	ulong cr0;

	/* note this is for the ADS Sphere board - 
	 * PCMCIA power works differently on the EDB9315 */
	cr0 = *((ulong *)ADSSPHERE_CR0);

	if (!(PFDR & PCMCIA_VS_3V)) {
		cr0 |= ADSSPHERE_CR0_CF_33;
		cr0 &= ~ADSSPHERE_CR0_CF_50;
	} else {
		cr0 |= ADSSPHERE_CR0_CF_50;
		cr0 &= ~ADSSPHERE_CR0_CF_33;
	}
  
	*((ulong *)ADSSPHERE_CR0) = cr0;

}

void pcmcia_power_off(void)
{
	ulong cr0;

	cr0 = *((ulong *)ADSSPHERE_CR0);

	cr0 &= ~(ADSSPHERE_CR0_CF_50 | ADSSPHERE_CR0_CF_33);

	*((ulong *)ADSSPHERE_CR0) = cr0;

}
