/*
 * (C) Copyright 2007
 * Markus Klotzbuecher, DENX Software Engineering <mk@denx.de>
 * Adrian Filipi, Applied Data Systems, Inc. <afilipi@applieddata.net>
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

#if defined(CONFIG_USB_OHCI_NEW) && defined(CFG_USB_OHCI_CPU_INIT)
# if defined(CONFIG_EP93XX)

#include <asm/arch/ep93xx.h>

int usb_cpu_init()
{
	SysconPwrCnt |= SysconPwrCnt_USH_EN;
	return 0;
}

int usb_cpu_stop()
{
	SysconPwrCnt &= ~SysconPwrCnt_USH_EN;
	return 0;
}

int usb_cpu_init_fail()
{
	usb_cpu_stop();
	return 0;
}

#endif /* defined(CONFIG_EP93XX) */
#endif /* defined(CONFIG_USB_OHCI_NEW) && defined(CFG_USB_OHCI_CPU_INIT) */
