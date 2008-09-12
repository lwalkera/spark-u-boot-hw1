/*
 * (C) Copyright 2003
 * Kyle Harris, kharris@nexus-tech.net
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
#include <command.h>
#include <mmc.h>

int do_mmc (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int verbose   = 1;
	int bus_width = 0;
	char *s;

	switch (argc) {
	case 1:
		if ((s = getenv("mmcinit_width")) != NULL) {
			bus_width = (int)simple_strtoul(s, NULL, 10);
		} else {
			bus_width = CFG_MMC_MAX_BUS_WIDTH;
		}
		break;
	case 2:
		bus_width = (int)simple_strtoul(argv[1], NULL, 10);
		break;
	default:
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

	if (bus_width < 1) {
		bus_width = 1;
	}
	if (bus_width > CFG_MMC_MAX_BUS_WIDTH) {
		bus_width = CFG_MMC_MAX_BUS_WIDTH;
	}
	switch (bus_width) {
		case 1:
		case 4:
			break;
		default:
			printf ("ERROR: bad bus width: %d\n", bus_width);
			return 1;
	}
	if (mmc_init (verbose, bus_width) != 0) {
		printf ("No MMC card found\n");
		return 1;
	}
	return 0;
}

#define __STRINGIFY_EVAL(e)	__STRINGIFY_EXPR(e)
#define __STRINGIFY_EXPR(s)	#s
U_BOOT_CMD(
	mmcinit,	2,	0,	do_mmc,
	"mmcinit - init mmc/sd card\n",
	"[1, 4]\n"
	"  - Try to configure for a bus of this width.  "
		"Maximum width is " __STRINGIFY_EVAL(CFG_MMC_MAX_BUS_WIDTH)
		".  This is the\n"
	"    default, unless mmcinit_width is set in the environment.\n"
);
#undef __STRINGIFY_EVAL(e)
#undef __STRINGIFY_EXPR(s)
