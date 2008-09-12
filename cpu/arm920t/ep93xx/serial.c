/*
 * (C) Copyright 2002
 * Gary Jennejohn, DENX Software Engineering, <gj@denx.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <common.h>
#include <asm/arch/ep93xx.h>


void serial_setbrg (void)
{
	DECLARE_GLOBAL_DATA_PTR;

	int i;

	/* set UARTCLK to 14Mhz */
	SysconPwrCnt |= EP9312_PWRCNT_UARTBAUD;
	SysconDEVCFG = 0x40000; /* enable UART1 */

	rUART1_LCR_L = ((14745600 / (16 * gd->baudrate)) - 1);
	rUART1_LCR_M = 0x0;
	rUART1_LCR_H = 0x60; /* FIFO Disable, 8 bits, 1 stop, no parity */
	rUART1_CR = 0x31; /* UART Enable, TX/RX int enable, disable timeout int */

	for (i = 0; i < 100; i++);
}

/*
 * Initialise the serial port with the given baudrate. The settings
 * are always 8 data bits, no parity, 1 stop bit, no start bits.
 *
 */
int serial_init (void)
{
	serial_setbrg ();

	return (0);
}

/*
 * Read a single byte from the serial port. Returns 1 on success, 0
 * otherwise. When the function is succesfull, the character read is
 * written into its argument c.
 */
int serial_getc (void)
{

  while (!(rUART1_IIR & 0x2));
  return rUART1_DR & 0xff;
}

#ifdef CONFIG_HWFLOW
static int hwflow = 0; /* turned off by default */
int hwflow_onoff(int on)
{
	switch(on) {
	case 0:
	default:
		break; /* return current */
	case 1:
		hwflow = 1; /* turn on */
		break;
	case -1:
		hwflow = 0; /* turn off */
		break;
	}
	return hwflow;
}
#endif

#ifdef CONFIG_MODEM_SUPPORT
static int be_quiet = 0;
void disable_putc(void)
{
	be_quiet = 1;
}

void enable_putc(void)
{
	be_quiet = 0;
}
#endif


/*
 * Output a single byte to the serial port.
 */
void serial_putc (const char c)
{
    /* wait for room in the tx FIFO */
	while(rUART1_FR & 0x20);
	rUART1_DR = c;

	/* If \n, also do \r */
	if (c == '\n')
		serial_putc ('\r');
}


/*
 * Test whether a character is in the RX buffer
 */
int serial_tstc (void)
{
  	return (!(rUART1_FR & 0x10));
}

void
serial_puts (const char *s)
{
	while (*s) {
		serial_putc (*s++);
	}
}
