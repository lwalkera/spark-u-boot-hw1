/*
 * (C) Copyright 200
 * Robert Whaley, Applied Data Systems, Inc, rwhaley@applieddata.net
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
#include <map.h>

#undef DEBUG

#if defined(CONFIG_CMD_MAP)

#ifdef CFG_MEM_MAP_ITEMS
mem_map_t the_mem_map[MEM_MAP_ITEM_COUNT] = CFG_MEM_MAP_ITEMS;
#else
mem_map_t the_mem_map[MEM_MAP_ITEM_COUNT];
#endif

#if defined(CONFIG_CMD_FLASH)

extern flash_info_t flash_info[];	/* info for FLASH chips */

char *map_type[] = MEM_MAP_TYPE_NAMES;
char *map_source[] = MEM_MAP_SOURCE_NAMES;
char *map_guard[] = MEM_MAP_GUARD_NAMES;

static int is_flash(ulong addr, ulong size)
{
	int bank;
	ulong b_start, b_end;
	flash_info_t *fp = flash_info;

	for (bank=0; bank < CFG_MAX_FLASH_BANKS; bank++, fp++) {

		if (fp->flash_id == FLASH_UNKNOWN) continue;

		b_start = fp->start[0];
		b_end = b_start + fp->size;

		if ((addr >= b_start) && ((addr + size) <= b_end)) return 1;

	}

	return 0;
}

#else

static int is_flash(ulong addr, ulong size)
{
	return 0;
}

#endif

#if defined(CONFIG_PPC) || defined(CONFIG_NIOS) || defined(CONFIG_MIPS)

#warning "untested!"

static int is_dram(ulong addr, ulong size)
{
	return (addr >= bd->bi_memstart) && ((addr + size) <= (bd->bi_memstart + bd->bi_memsize));
}

#elif defined(CONFIG_ARM) || defined(CONFIG_X86)

static int is_dram(ulong addr, ulong size)
{
	DECLARE_GLOBAL_DATA_PTR;

	int i;
	bd_t *bd = gd->bd;

	for (i=0; i<CONFIG_NR_DRAM_BANKS; ++i) {
		if ((addr >= bd->bi_dram[i].start ) && ((addr + size) <= (bd->bi_dram[i].start + bd->bi_dram[i].size)))
			return 1;
	}
	return 0;
}

#else

static int is_dram(ulong addr, ulong size)
{
	return 0;
}
#endif

static int type_id(ulong addr, ulong size)
{
	if (is_flash(addr, size)) return MEM_MAP_TYPE_FLASH;
	if (is_dram(addr, size)) return MEM_MAP_TYPE_DRAM;
	else return MEM_MAP_TYPE_OTHER;
}

static char * type_string(ulong addr, ulong size)
{
	return map_type[type_id(addr, size)];
}

int mem_map_add(char *name, ulong loc, ulong size, uchar source, uchar guard)
{
	int i;

	for (i=0; i<MEM_MAP_ITEM_COUNT; i++) {
		if (the_mem_map[i].size == 0) {
			strncpy(the_mem_map[i].name, name, MEM_MAP_MAX_NAME_LEN);
			the_mem_map[i].location = loc;
			the_mem_map[i].size = size;
			the_mem_map[i].source = source;
			the_mem_map[i].guard = guard;

			return 0;
		}
	}
	return -1;
}

void mem_map_del_source(int source)
{
	int i;

	for (i=0; i<MEM_MAP_ITEM_COUNT; i++) {
		if (the_mem_map[i].size > 0) {
			if (the_mem_map[i].source == source) {
				the_mem_map[i].size = 0;
			}
		}
	}
}

int mem_map_del(char *name)
{
	int i;

	for (i=0; i<MEM_MAP_ITEM_COUNT; i++) {
		if (the_mem_map[i].size > 0) {
			if (!strncmp(the_mem_map[i].name, name, MEM_MAP_MAX_NAME_LEN)) {
				the_mem_map[i].size = 0;
				return 0;
			}
		}
	}
	return -1;
}


int mem_map_find(char *name, ulong *loc, ulong *size, uchar *prot)
{
	int i;

	for (i=0; i<MEM_MAP_ITEM_COUNT; i++) {
		if (the_mem_map[i].size != 0 && !strncmp(name, the_mem_map[i].name, MEM_MAP_MAX_NAME_LEN)) {
			if (loc  != NULL) *loc  = the_mem_map[i].location;
			if (size != NULL) *size = the_mem_map[i].size;
			if (prot != NULL) *prot = the_mem_map[i].guard;
			return 0;
		}
	}
	return 1;
}

int mem_map_get_addr(char *name, ulong *addr, ulong *size, uchar *prot, ulong base_addr)
{
	char *endp;
	ulong result;

	if (!mem_map_find(name, addr, size, prot)) {
		debug("Using address %#x\n", *addr);
		return 0;
	}
	else {
		result = simple_strtoul(name, &endp, 16) + base_addr;

		/* make sure the hex value was read without error */
		if (endp == name + strlen(name)) {
			*addr = result;
			return 0;
		}
	}

	/* failed to get symbolic or hex memory location */
	return 1;

}

int do_mapadd (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int i;
	uchar guard;

	if (argc < 4 || argc > 5) {
		printf("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

	guard = MEM_MAP_GUARD_OFF;
	if (argc == 5 && !strcmp(argv[4], "guard")) {
		guard = MEM_MAP_GUARD_ON;
	}

	for (i=0; i<MEM_MAP_ITEM_COUNT; i++) {
		if (the_mem_map[i].size == 0) {
			strncpy(the_mem_map[i].name, argv[1], MEM_MAP_MAX_NAME_LEN);
			the_mem_map[i].location = simple_strtoul(argv[2], NULL, 16);
			the_mem_map[i].size = simple_strtoul(argv[3], NULL, 16);
			the_mem_map[i].source = MEM_MAP_SOURCE_CMD;
			the_mem_map[i].guard = guard;
			return 0;
		}
	}

	printf("No space to add map item.\n", argv[1]);
	return 1;
}

int do_mapdel (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int i;

	if (argc != 2) {
		printf("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

	for (i=0; i<MEM_MAP_ITEM_COUNT; i++) {
		if (the_mem_map[i].size > 0) {
			if (!strncmp(the_mem_map[i].name, argv[1], MEM_MAP_MAX_NAME_LEN)) {
				the_mem_map[i].size = 0;
				printf("deleted %s\n", argv[1]);
				return 0;
			}
		}
	}

	printf("Could not find %s to delete\n", argv[1]);
	printf("Usage:\n%s\n", cmdtp->usage);
	return 1;
}

int do_mapinfo (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int i, j;
	int new_j, min_j; 
	ulong size, new_size, min_size;
	ulong loc, new_loc, min_loc;
  

	if (argc != 1) {
		printf("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

	printf(" %-*s %-10s %-10s %-9s %-9s %-9s\n", MEM_MAP_MAX_NAME_LEN,
	       "Name", "Location", "Size", "Type", "Source", "Guard");
	printf(" %-*s %-10s %-10s %-9s %-9s %-9s\n", MEM_MAP_MAX_NAME_LEN,
	       "----", "--------", "----", "----", "------", "-----");

	min_loc = 0;
	min_size = 0xffffffff;
	min_j = 0;

	/* sort by location and size */
	for (i=0; i<MEM_MAP_ITEM_COUNT; i++) {

		new_loc = 0xffffffff;
		new_size = 0;
		new_j = MEM_MAP_ITEM_COUNT;

		for (j=0; j<MEM_MAP_ITEM_COUNT; j++) {

			loc = the_mem_map[j].location;
			size = the_mem_map[j].size;
      
			if ((loc == min_loc && size == min_size && j > min_j) ||
			    (loc == min_loc && size < min_size) ||
			    (loc > min_loc)) {

				if ((loc == new_loc && size == new_size && j < new_j) ||
				    (loc == new_loc && size > new_size) ||
				    (loc < new_loc)) {
					new_loc = loc;
					new_size = size;
					new_j = j;
				}
			}
		}

		min_loc = new_loc;
		min_size = new_size;
		min_j = new_j;

		if (min_size > 0 && min_j != MEM_MAP_ITEM_COUNT) {
			printf(" %-*s %#010x %#010x %-9s %-9s %-9s\n", MEM_MAP_MAX_NAME_LEN,
			       the_mem_map[min_j].name, min_loc, min_size,
			       type_string(min_loc, min_size),
			       map_source[the_mem_map[min_j].source],
			       map_guard[the_mem_map[min_j].guard]);
		}
	}
	return 0;
}

U_BOOT_CMD(
	mapinfo, 1, 1, do_mapinfo,
	"mapinfo - display the memory map information\n",
	"display information about the memory map, this\n"
	"may include memory on flash partitions, DRAM and\n"
	"other locations.\n"
	);

U_BOOT_CMD(
	mapdel, 2, 0, do_mapdel,
	"mapdel - delete a memory map item\n",
	"mapdel <name>\n"
	);

U_BOOT_CMD(
	mapadd, 4, 0, do_mapadd,
	"mapadd - add a memory map item\n",
	"mapadd <name> <address> <size> [guard]\n"
	"     - add memory map item with name address and size\n"
	);

#endif
