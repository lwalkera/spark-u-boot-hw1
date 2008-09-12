/*
 * (C) Copyright 2008
 * Laine Walker-Avina, PASCO scientific, lwalkera@pasco.com
 *
 * (C) Copyright 2002
 * Kyle Harris, Nexus Technologies, Inc. kharris@nexus-tech.net
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
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

#if defined(CONFIG_USB_OHCI_NEW)

#include <asm/arch/pxa-regs.h>
#include <usb.h>
#include <../drivers/usb/usb_ohci.h>

int usb_board_init(void)
{
	UHCHR = (UHCHR | UHCHR_PCPL) & ~(UHCHR_SSEP1 | UHCHR_SSEP2 | UHCHR_SSE);;

	return 0;
}

int usb_board_init_fail(void)
{
	return 0;
}

int usb_board_stop(void)
{
	return 0;
}

#endif
