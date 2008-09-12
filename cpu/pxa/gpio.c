/*
 * (C) Copyright 2003
 * Kyle Harris, Nexus Technologies, Inc. kharris@nexus-tech.net
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

#include <config.h>
#include <common.h>
#include <command.h>
#include <asm/arch/pxa-regs.h>

extern void pxa_gpio_mode(int gpio_mode);

void pxa_gpio_mode(int gpio_mode)
{
	int gpio = gpio_mode & GPIO_MD_MASK_NR;
	int fn = (gpio_mode & GPIO_MD_MASK_FN) >> 8;
	int gafr;

	if (gpio_mode & GPIO_DFLT_LOW)
		GPCR(gpio) = GPIO_bit(gpio);
	else if (gpio_mode & GPIO_DFLT_HIGH)
		GPSR(gpio) = GPIO_bit(gpio);
	if (gpio_mode & GPIO_MD_MASK_DIR)
		GPDR(gpio) |= GPIO_bit(gpio);
	else
		GPDR(gpio) &= ~GPIO_bit(gpio);
	gafr = GAFR(gpio) & ~(0x3 << (((gpio) & 0xf)*2));
	GAFR(gpio) = gafr |  (fn  << (((gpio) & 0xf)*2));
}

int gpio_dir (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int dir, gpio;

	if (argc > 3 || argc <2) {
		printf ("Usage: gpiodir cpupin <dir>\n");
		return 1;
	}

	gpio = simple_strtoul(argv[1], NULL, 10);
	if(argc >2)
	{
		dir = simple_strtoul(argv[2], NULL, 10);
		pxa_gpio_mode(gpio|(dir?GPIO_OUT:GPIO_IN));
		printf("Set GPIO(%d) as [%s] PIN\n", gpio, (dir?"OUTPUT":"INPUT"));
	}
	else
	{
		dir = GPDR(gpio)&GPIO_bit(gpio);
		printf("GPIO(%d) as [%s] PIN\n", gpio, (dir?"OUTPUT":"INPUT"));
	}

	return 0;
}

int gpio_value (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int value, gpio;

	if (argc < 2 || argc >3) {
		printf ("Usage: gpio cpupin <value>\n");
		return 1;
	}

	gpio = simple_strtoul(argv[1], NULL, 10);
	if(argc >2 )
	{
		value = simple_strtoul(argv[2], NULL, 10);
		value?(GPSR(gpio)=GPIO_bit(gpio)):(GPCR(gpio)=GPIO_bit(gpio));
		printf("Set GPIO(%d)=%d\n", gpio, (value?1:0));
	}
	else
		printf("GPIO(%d)=%#x\n", gpio, (GPLR(gpio) & GPIO_bit(gpio))>>(gpio & 0x1f));

	return 0;
}

int set_inputpin_func (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int pin, func;

	if (argc != 3) {
		printf ("Usage: inputpin cpupin alterfunc \n");
		return 1;
	}

	pin = simple_strtoul(argv[1], NULL, 10);
	func = simple_strtoul(argv[2], NULL, 10);
	if(func > 3 )
	{
		printf("alterfunc should be 0-3\n");
		return 1;
	}
	func <<= 8;
	printf("Set input(%d) %#x\n", pin, func);

	pxa_gpio_mode(pin|func);
	return 0;
}

int set_outputpin_func (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int pin, func;

	if (argc != 3) {
		printf ("Usage: outputpin cpupin alterfunc \n");
		return 1;
	}

	pin = simple_strtoul(argv[1], NULL, 10);
	func = simple_strtoul(argv[2], NULL, 10);
	if(func > 3 )
	{
		printf("alterfunc should be 0-3\n");
		return 1;
	}
	func = (func << 8)|0x80;
	printf("Set input(%d) %#x\n", pin, func);

	pxa_gpio_mode(pin|func);
	return 0;
}

int get_pin_info (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int pin, func;
	char * dir;

	if(argc!=2)
	{
		printf("Usage: pin <cpupin>\n");
		return 1;
	}
	pin = simple_strtoul(argv[1], NULL, 10);
	dir = ((GPDR(pin)&GPIO_bit(pin))?"OUTPUT":"INPUT");
	func = GAFR(pin)&(0x3 << (((pin) & 0xf)*2));
	func >>=(((pin) & 0xf)*2);
	printf("PIN(%d) as [%s] alter[%#x].\n", pin, dir, func);
	return 0;
}

U_BOOT_CMD(
	pin,	2,	0,	get_pin_info,
	"pin - get gpio pin info.\n",
	NULL
);

U_BOOT_CMD(
	inputpin,	3,	0,	set_inputpin_func,
	"inputpin - select gpio input alter function\n",
	NULL
);

U_BOOT_CMD(
	outputpin,	3,	0,	set_outputpin_func,
	"outputpin - select gpio output alter function\n",
	NULL
);

U_BOOT_CMD(
	gpio,	3,	0,	gpio_value,
	"gpio - set/get gpio value\n",
	NULL
);

U_BOOT_CMD(
	gpiodir,	3,	0,	gpio_dir,
	"gpiodir - set gpio dir\n",
	NULL
);

