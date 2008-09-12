/*
 * (C) Copyright 2004
 * Robert Schwebel, Pengutronix, r.schwebel@pengutronix.de
 *
 * Derived from the i386 variant
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

/* Linux ARM zImage loading */

#include <common.h>
#include <asm/byteorder.h>

/* 
 * There is no way to know the size of a zImage ... *
 * so we assume that 2MB will be enough for now 
 */
#define ZIMAGE_SIZE 0x200000

image_header_t * fake_zimage_header(image_header_t * hdr, void *ptr, int size)
{
	ulong checksum = 0;

	/* check if we really have a ARM zImage magic number */
	if (*((unsigned long *) ((char *)ptr + 0x24)) != 0x016F2818)
		return NULL;

	printf ("## Image is zImage, creating header\n");

	if (size == -1)
		size = ZIMAGE_SIZE;

	memset(hdr, 0, sizeof (image_header_t));

	/* Build new header */
	hdr->ih_magic = htonl(IH_MAGIC);
	hdr->ih_time  = 0;
	hdr->ih_size  = htonl(size);
	hdr->ih_load  = htonl((unsigned long)ptr);
	hdr->ih_ep    = htonl((unsigned long)ptr);
	hdr->ih_dcrc  = htonl(checksum);
	hdr->ih_os    = IH_OS_LINUX;
	hdr->ih_arch  = IH_CPU_ARM;
	hdr->ih_type  = IH_TYPE_KERNEL;
	hdr->ih_comp  = IH_COMP_NONE;

	strncpy((char *) hdr->ih_name, "zImage", IH_NMLEN);

	checksum = crc32(0, (const char *) hdr, sizeof (image_header_t));

	hdr->ih_hcrc = htonl(checksum);

	return hdr;
}
