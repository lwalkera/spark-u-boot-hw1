/*
 * (C) Copyright 2006
 * Stefan Roese, DENX Software Engineering, sr@denx.de.
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Alex Zuepke <azu@sysgo.de>
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
#include <asm/arch/ixp425.h>

#ifndef CONFIG_USE_IRQ
ulong get_timer (ulong base)
{
       return get_timer_masked () - base;
}

static void ixp425_udelay(ulong count)
{
	/*
	 * This function has a max usec, but since it is called from udelay
	 * we should not have to worry... be happy
	 */
	unsigned long usecs = CFG_HZ/1000000L & ~IXP425_OST_RELOAD_MASK;

	if (count > 1) usecs *= count;

	*IXP425_OSST = IXP425_OSST_TIMER_1_PEND;
	usecs |= IXP425_OST_ONE_SHOT | IXP425_OST_ENABLE;
	*IXP425_OSRT1 = usecs;
	while (!(*IXP425_OSST & IXP425_OSST_TIMER_1_PEND));
}

void udelay (unsigned long usec)
{
	while (usec > 10) {ixp425_udelay(10); usec -= 10;}
	while (usec--) ixp425_udelay(1);
}

static ulong reload_constant = 0xfffffff0;
#if 0
void reset_timer_masked (void)
{
#if 0
	ulong reload = reload_constant | IXP425_OST_ONE_SHOT | IXP425_OST_ENABLE;

	*IXP425_OSST = IXP425_OSST_TIMER_1_PEND;
	*IXP425_OSRT1 = reload;
#else
	*IXP425_OSTS = 0;
#endif
}
#endif
ulong get_timer_masked (void)
{
#if 0
	/*
	 * Note that it is possible for this to wrap!
	 * In this case we return max.
	 */
	ulong current = *IXP425_OST1;
	if (*IXP425_OSST & IXP425_OSST_TIMER_1_PEND)
	{
		return reload_constant;
	}
	return (reload_constant - current);
#else
	return *IXP425_OSTS;
#endif
}

/*
 * This function is derived from PowerPC code (read timebase as long long).
 * On IXP it returns the timer value and tries to account for wraps.
 */
unsigned long long get_ticks(void)
{
	static long tick_lowbits = 0;
	static long tick_highbits = 0;
	unsigned long new_tick;

	new_tick = get_timer(0);

	if (tick_lowbits > new_tick) {
		tick_highbits++;
	}

	tick_lowbits = new_tick;
	       
	return ((unsigned long long) tick_highbits) << 32 | tick_lowbits;
}

/*
 * This function is derived from PowerPC code (timebase clock frequency).
 * On ARM it returns the number of timer ticks per second.
 */
ulong get_tbclk (void)
{
	ulong tbclk;

	tbclk = CFG_HZ;
	return tbclk;
}
#endif /* #ifndef CONFIG_USE_IRQ */
