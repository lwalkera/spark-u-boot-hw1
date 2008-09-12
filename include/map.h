/*
 * (C) Copyright 2004
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 * Note: Part of this code has been derived from linux
 *
 */

#define MEM_MAP_ITEM_COUNT 20
#define MEM_MAP_MAX_NAME_LEN 16

#define MEM_MAP_TYPE_FLASH 0
#define MEM_MAP_TYPE_DRAM  1
#define MEM_MAP_TYPE_OTHER 2

#define MEM_MAP_TYPE_NAMES {"Flash", "DRAM", "Other"}

extern char *map_type[];

#define MEM_MAP_SOURCE_INIT 0
#define MEM_MAP_SOURCE_SYSTEM 1
#define MEM_MAP_SOURCE_BOOTARGS 2
#define MEM_MAP_SOURCE_CMD 3

#define MEM_MAP_SOURCE_NAMES {"Initial", "System", "bootargs", "Command"}

extern char *map_source[];

#define MEM_MAP_GUARD_OFF 0
#define MEM_MAP_GUARD_ON 1

#define MEM_MAP_GUARD_NAMES {"Off", "On"};

extern char *map_guard[];

typedef struct mem_map {
  char name[MEM_MAP_MAX_NAME_LEN];
  ulong location;
  ulong size;
  uchar source;
  uchar guard;
} mem_map_t;

extern mem_map_t the_mem_map[MEM_MAP_ITEM_COUNT];

int mem_map_find(char *name, ulong *loc, ulong *size, uchar *prot);
int mem_map_get_addr(char *name, ulong *addr, ulong *size, uchar *prot, ulong base_addr);
int mem_map_add(char *name, ulong loc, ulong size, uchar source, uchar guard);
int mem_map_del(char *name);
void mem_map_del_source(int source);
void mem_map_addr2(char *argv[], ulong base_address, int size, ulong *addr, ulong *dest, ulong *count);
void mem_map_addr(char *argv[], ulong base_address, int size, ulong *addr, ulong *count, int dest_p);
