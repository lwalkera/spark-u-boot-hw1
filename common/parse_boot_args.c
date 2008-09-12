/*
 * (C) Copyright 2004
 * Robert Whaley, Applied Data Systems, Inc, rwhaley@applieddata.net
 *
 * Originally from the Linux Kernel:
 *
 * Read flash partition table from command line
 *
 * Copyright 2002 SYSGO Real-Time Solutions GmbH
 *
 * The format for the command line is as follows:
 * 
 * mtdparts=<mtddef>[;<mtddef]
 * <mtddef>  := <mtd-id>:<partdef>[,<partdef>]
 * <partdef> := <size>[@offset][<name>][ro]
 * <mtd-id>  := unique id used in mapping driver/device
 * <size>    := standard linux memsize OR "-" to denote all remaining space
 * <name>    := '(' NAME ')'
 * 
 * Examples:
 * 
 * 1 NOR Flash, with 1 single writable partition:
 * edb7312-nor:-
 * 
 * 1 NOR Flash with 2 partitions, 1 NAND with one
 * edb7312-nor:256k(ARMboot)ro,-(root);edb7312-nand:-(home)
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
#include <malloc.h>
#include <flash.h>
#include <map.h>

#if defined(CONFIG_CMD_MAP)

/* error message prefix */
#define PAGE_SIZE (0)
#define KERN_ERR "error: "
#define KERN_WARNING "warning: "
#define ERRP "mtd: "
#define MTD_WRITEABLE 0
#define EINVAL 22

/* debug macro */
#if 0
#define dbg(x) do { printf("DEBUG-CMDLINE-PART: "); printf x; } while(0)
#else
#define dbg(x)
#endif

#define ALIGN(x) (((x) & 0x03) ? (((x) & ~0x03) + 4) : (x))

/* special size referring to all the remaining space in a partition */
#define SIZE_REMAINING 0xffffffff

struct mtd_partition {
	char *name;		/* identifier string */
	u_int32_t size;		/* partition size */
	u_int32_t offset;		/* offset within the master MTD space */
	u_int32_t mask_flags;	/* master MTD flags to mask out for this partition */
	struct mtd_info **mtdp;	/* pointer to store the MTD object */
};

struct cmdline_mtd_partition {
	struct cmdline_mtd_partition *next;
	char *mtd_id;
	int num_parts;
	struct mtd_partition *parts;
};

/* mtdpart_setup() parses into here */
static struct cmdline_mtd_partition *partitions;


/**
 *	memparse - parse a string with mem suffixes into a number
 *	@ptr: Where parse begins
 *	@retptr: (output) Pointer to next char after parse completes
 *
 *	Parses a string into a number.  The number stored at @ptr is
 *	potentially suffixed with %K (for kilobytes, or 1024 bytes),
 *	%M (for megabytes, or 1048576 bytes), or %G (for gigabytes, or
 *	1073741824).  If the number is suffixed with K, M, or G, then
 *	the return value is the number multiplied by one kilobyte, one
 *	megabyte, or one gigabyte, respectively.
 */

unsigned long long memparse (char *ptr, char **retptr)
{
	unsigned long ret = simple_strtoul (ptr, retptr, 0);

	switch (**retptr) {
	case 'G':
	case 'g':
		ret <<= 10;
	case 'M':
	case 'm':
		ret <<= 10;
	case 'K':
	case 'k':
		ret <<= 10;
		(*retptr)++;
	default:
		break;
	}
	return ret;
}

/*
 * Parse one partition definition for an MTD. Since there can be many
 * comma separated partition definitions, this function calls itself 
 * recursively until no more partition definitions are found. Nice side
 * effect: the memory to keep the mtd_partition structs and the names
 * is allocated upon the last definition being found. At that point the
 * syntax has been verified ok.
 */
static struct mtd_partition * newpart(char *s, 
                                      char **retptr,
                                      int *num_parts,
                                      int this_part, 
                                      unsigned char **extra_mem_ptr, 
                                      int extra_mem_size)
{
	struct mtd_partition *parts;
	unsigned long size;
	unsigned long offset = 0;
	char *name;
	int name_len;
	unsigned char *extra_mem;
	char delim;
	unsigned int mask_flags;

	/* fetch the partition size */
	if (*s == '-')
	{	/* assign all remaining space to this partition */
		size = SIZE_REMAINING;
		s++;
	}
	else
	{
		size = memparse(s, &s);
		if (size < PAGE_SIZE)
		{
			printf(KERN_ERR ERRP "partition size too small (%lx)\n", size);
			return 0;
		}
	}

	/* fetch partition name and flags */
	mask_flags = 0; /* this is going to be a regular partition */
	delim = 0;
        /* check for offset */
        if (*s == '@') 
	{
           s++;
           offset = memparse(s, &s);
        }
        /* now look for name */
	if (*s == '(')
	{
		delim = ')';
	}
	if (delim)
	{
		char *p;

	    	name = ++s;
		if ((p = strchr(name, delim)) == 0)
		{
			printf(KERN_ERR ERRP "no closing %c found in partition name\n", delim);
			return 0;
		}
		name_len = p - name;
		s = p + 1;
	}
	else
	{
	    	name = NULL;
		name_len = 13; /* Partition_000 */
	}
   
	/* record name length for memory allocation later */
	extra_mem_size += ALIGN(name_len + 1);

        /* test for options */
        if (strncmp(s, "ro", 2) == 0) 
	{
		mask_flags |= MTD_WRITEABLE;
		s += 2;
        }

	/* test if more partitions are following */
	if (*s == ',')
	{
		if (size == SIZE_REMAINING)
		{
			printf(KERN_ERR ERRP "no partitions allowed after a fill-up partition\n");
			return 0;
		}
		/* more partitions follow, parse them */
		if ((parts = newpart(s + 1, &s, num_parts, 
		                     this_part + 1, &extra_mem, extra_mem_size)) == 0)
		  return 0;
	}
	else
	{	/* this is the last partition: allocate space for all */
		int alloc_size;

		*num_parts = this_part + 1;
		alloc_size = *num_parts * sizeof(struct mtd_partition) +
			     extra_mem_size;

		parts = malloc(alloc_size);

		memset(parts, 0, alloc_size);
		extra_mem = (unsigned char *)(parts + *num_parts);
	}
	/* enter this partition (offset will be calculated later if it is zero at this point) */
	parts[this_part].size = size;
	parts[this_part].offset = offset;
	parts[this_part].mask_flags = mask_flags;
	if (name)
	{
		strncpy(extra_mem, name, name_len);
		extra_mem[name_len] = 0;
	}
	else
	{
		sprintf(extra_mem, "Partition_%03d", this_part);
	}
	parts[this_part].name = extra_mem;
	extra_mem += ALIGN(name_len + 1);

	dbg(("partition %d: name <%s>, offset %x, size %x, mask flags %x\n",
	     this_part, 
	     parts[this_part].name,
	     parts[this_part].offset,
	     parts[this_part].size,
	     parts[this_part].mask_flags));

	/* return (updated) pointer to extra_mem memory */
	if (extra_mem_ptr)
	  *extra_mem_ptr = extra_mem;

	/* return (updated) pointer command line string */
	*retptr = s;

	/* return partition table */
	return parts;
}

/* 
 * Parse the command line. 
 */
static int mtdpart_setup_real(char *s)
{
	for( ; s != NULL; )
	{
		struct cmdline_mtd_partition *this_mtd;
		struct mtd_partition *parts;
	    	int mtd_id_len;
		int num_parts;
		char *p, *mtd_id;

	    	mtd_id = s;
		/* fetch <mtd-id> */
		if (!(p = strchr(s, ':')))
		{
			printf(KERN_ERR ERRP "no mtd-id\n");
			return 0;
		}
		mtd_id_len = p - mtd_id;

		dbg(("parsing <%s>\n", p+1));

		/* 
		 * parse one mtd. have it reserve memory for the
		 * struct cmdline_mtd_partition and the mtd-id string.
		 */
		parts = newpart(p + 1,		/* cmdline */
				&s,		/* out: updated cmdline ptr */
				&num_parts,	/* out: number of parts */
				0,		/* first partition */
				(unsigned char**)&this_mtd, /* out: extra mem */
				mtd_id_len + 1 + sizeof(*this_mtd));

		/* enter results */	    
		this_mtd->parts = parts;
		this_mtd->num_parts = num_parts;
		this_mtd->mtd_id = (char*)(this_mtd + 1);
		strncpy(this_mtd->mtd_id, mtd_id, mtd_id_len);
		this_mtd->mtd_id[mtd_id_len] = 0;

		/* link into chain */
		this_mtd->next = partitions;	    	
		partitions = this_mtd;

		dbg(("mtdid=<%s> num_parts=<%d>\n", 
		     this_mtd->mtd_id, this_mtd->num_parts));
		

		/* EOS - we're done */
		if (*s == 0 || *s == ' ' || *s == '\n' || *s == '\r')
			break;

		/* does another spec follow? */
		if (*s != ';')
		{
			printf(KERN_ERR ERRP "bad character after partition (%c)\n", *s);
			return 0;
		}
		s++;
	}
	return 1;
}

/*
 * Main function to be called from the MTD mapping driver/device to
 * obtain the partitioning information. At this point the command line
 * arguments will actually be parsed and turned to struct mtd_partition
 * information.
 */
static int parse_mtd_partitions(int size, 
				    struct mtd_partition **pparts,
				    int mtd_id_size, 
				    char * cmdline)
{
	char mtd_id[mtd_id_size + 1];
	unsigned long offset;
	int i;
	struct cmdline_mtd_partition *part;

	strncpy(mtd_id, cmdline, mtd_id_size);
	mtd_id[mtd_id_size] = '\0';

	/* parse command line */
	mtdpart_setup_real(cmdline);

	for(part = partitions; part; part = part->next)
	{
		if (!strcmp(part->mtd_id, mtd_id))
		{
			for(i = 0, offset = 0; i < part->num_parts; i++)
			{
				if (!part->parts[i].offset)
				  part->parts[i].offset = offset;
				else
				  offset = part->parts[i].offset;
				if (part->parts[i].size == SIZE_REMAINING)
				  part->parts[i].size = size - offset;
				if (offset + part->parts[i].size > size)
				{
					printf(KERN_WARNING ERRP 
					       "%s: partitioning exceeds flash size, truncating\n",
					       mtd_id);
					part->parts[i].size = size - offset;
					part->num_parts = i;
				}
				offset += part->parts[i].size;
			}
			*pparts = part->parts;
			return part->num_parts;
		}
	}
	return -EINVAL;
}

void mem_map_parse_bootargs(char *bootargs)
{
	struct mtd_partition *parsed_parts = NULL;
	char *next,*quote,*lineorig, *line;
	int linesize;
	int i;

	if (!bootargs || !*bootargs)
		return;

	/* this is necessary because line will be altered! */
	linesize = strlen(bootargs) + 1;
	line = lineorig = malloc(linesize);

	if (!line) {
		printf("%s: malloc failed\n", __FUNCTION__);
		return;
	}

	strncpy(line, bootargs, linesize);

	/* delete any earlier map info from bootargs! */
	mem_map_del_source(MEM_MAP_SOURCE_BOOTARGS);

	next = line;
	while ((line = next) != NULL) {
                quote = strchr(line,'"');
                next = strchr(line, ' ');
                while (next != NULL && quote != NULL && quote < next) {
                        /* we found a left quote before the next blank
                         * now we have to find the matching right quote
                         */
                        next = strchr(quote+1, '"');
                        if (next != NULL) {
                                quote = strchr(next+1, '"');
                                next = strchr(next+1, ' ');
                        }
                }
                if (next != NULL)
                        *next++ = 0;

		/* for now the only args we care about are the mtd partitions and the initrd location */
		if (strncmp(line, "mtdparts=", sizeof("mtdparts=") - 1) == 0) {

			int mtd_id_size;
			int flash_size = 0;
			int count;
			extern flash_info_t flash_info[];

			for (i=0; i<CFG_MAX_FLASH_BANKS; i++) {
				flash_size += flash_info[0].size;
			}

			line += sizeof("mtdparts=") - 1;

			mtd_id_size = strchr(line, ':') - line;

			count = parse_mtd_partitions(flash_size, &parsed_parts, mtd_id_size, line);

			for (i=0; i<count; i++) {

				if (mem_map_add(parsed_parts[i].name,
						parsed_parts[i].offset + flash_info[0].start[0],
						parsed_parts[i].size, MEM_MAP_SOURCE_BOOTARGS,
						MEM_MAP_GUARD_OFF) != 0) {
					printf("warning could not map %s\n", parsed_parts[i].name);
				}
			}

			continue;
		}
		
		if (strncmp(line, "initrd=", sizeof("initrd=") - 1) == 0) {

			ulong initrd_size, initrd_start;

			line += sizeof("initrd=") - 1;

			initrd_start = memparse(line, &line);

			if (*line == ',') {
				initrd_size = memparse(line + 1, &line);

				if (mem_map_add("initrd", initrd_start, initrd_size,
						MEM_MAP_SOURCE_BOOTARGS, MEM_MAP_GUARD_OFF) != 0) {
					printf("warning could not map %s\n", "initrd");
				}
			}

			continue;
		}

	}

	/* free the malloc-ed space */
	if (parsed_parts) {
		free(parsed_parts);
	}

	free(lineorig);
}

#endif
