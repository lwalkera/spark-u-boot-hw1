/*
 * (C) Copyright 2002
 * Wolfgang Denk, DENX Software Engineering, <wd@denx.de>
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Alex Zuepke <azu@sysgo.de>
 *
 * Copyright (C) 1999 2000 2001 Erik Mouw (J.A.K.Mouw@its.tudelft.nl)
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
#include <watchdog.h>
#include <asm/arch/mx31-regs.h>
#include <linux/ctype.h>

#ifdef CFG_MX31_UART

/*
 * UART Control Register 0 Bit Fields.
 */
#define EUartUCR1_ADEN      (1 << 15)           // Auto dectect interrupt
#define EUartUCR1_ADBR      (1 << 14)           // Auto detect baud rate
#define EUartUCR1_TRDYEN    (1 << 13)           // Transmitter ready interrupt enable
#define EUartUCR1_IDEN      (1 << 12)           // Idle condition interrupt
#define EUartUCR1_RRDYEN    (1 << 9)            // Recv ready interrupt enable
#define EUartUCR1_RDMAEN    (1 << 8)            // Recv ready DMA enable
#define EUartUCR1_IREN      (1 << 7)            // Infrared interface enable
#define EUartUCR1_TXMPTYEN  (1 << 6)            // Transimitter empty interrupt enable
#define EUartUCR1_RTSDEN    (1 << 5)            // RTS delta interrupt enable
#define EUartUCR1_SNDBRK    (1 << 4)            // Send break
#define EUartUCR1_TDMAEN    (1 << 3)            // Transmitter ready DMA enable
#define EUartUCR1_DOZE      (1 << 1)            // Doze
#define EUartUCR1_UARTEN    (1 << 0)            // UART enabled
#define EUartUCR2_ESCI      (1 << 15)           // Escape seq interrupt enable
#define EUartUCR2_IRTS      (1 << 14)           // Ignore RTS pin
#define EUartUCR2_CTSC      (1 << 13)           // CTS pin control
#define EUartUCR2_CTS       (1 << 12)           // Clear to send
#define EUartUCR2_ESCEN     (1 << 11)           // Escape enable
#define EUartUCR2_PREN      (1 << 8)            // Parity enable
#define EUartUCR2_PROE      (1 << 7)            // Parity odd/even
#define EUartUCR2_STPB      (1 << 6)            // Stop
#define EUartUCR2_WS        (1 << 5)            // Word size
#define EUartUCR2_RTSEN     (1 << 4)            // Request to send interrupt enable
#define EUartUCR2_ATEN      (1 << 3)            // Aging timer enable
#define EUartUCR2_TXEN      (1 << 2)            // Transmitter enabled
#define EUartUCR2_RXEN      (1 << 1)            // Receiver enabled
#define EUartUCR2_SRST_     (1 << 0)            // SW reset
#define EUartUCR3_PARERREN  (1 << 12)           // Parity enable
#define EUartUCR3_FRAERREN  (1 << 11)           // Frame error interrupt enable
#define EUartUCR3_ADNIMP    (1 << 7)            // Autobaud detection not improved
#define EUartUCR3_RXDSEN    (1 << 6)            // Receive status interrupt enable
#define EUartUCR3_AIRINTEN  (1 << 5)            // Async IR wake interrupt enable
#define EUartUCR3_AWAKEN    (1 << 4)            // Async wake interrupt enable
#define EUartUCR3_RXDMUXSEL (1 << 2)            // RXD muxed input selected
#define EUartUCR3_INVT      (1 << 1)            // Inverted Infrared transmission
#define EUartUCR3_ACIEN     (1 << 0)            // Autobaud counter interrupt enable
#define EUartUCR4_CTSTL_32  (32 << 10)          // CTS trigger level (32 chars)
#define EUartUCR4_INVR      (1 << 9)            // Inverted infrared reception
#define EUartUCR4_ENIRI     (1 << 8)            // Serial infrared interrupt enable
#define EUartUCR4_WKEN      (1 << 7)            // Wake interrupt enable
#define EUartUCR4_IRSC      (1 << 5)            // IR special case
#define EUartUCR4_LPBYP     (1 << 4)            // Low power bypass
#define EUartUCR4_TCEN      (1 << 3)            // Transmit complete interrupt enable
#define EUartUCR4_BKEN      (1 << 2)            // Break condition interrupt enable
#define EUartUCR4_OREN      (1 << 1)            // Receiver overrun interrupt enable
#define EUartUCR4_DREN      (1 << 0)            // Recv data ready interrupt enable
#define EUartUFCR_RXTL_SHF  0                   // Receiver trigger level shift
#define EUartUFCR_RFDIV_1   (5 << 7)            // Reference freq divider (div 1)
#define EUartUFCR_RFDIV_2   (4 << 7)            // Reference freq divider (div 2)
#define EUartUFCR_RFDIV_3   (3 << 7)            // Reference freq divider (div 3)
#define EUartUFCR_RFDIV_4   (2 << 7)            // Reference freq divider (div 4)
#define EUartUFCR_RFDIV_5   (1 << 7)            // Reference freq divider (div 5)
#define EUartUFCR_RFDIV_6   (0 << 7)            // Reference freq divider (div 6)
#define EUartUFCR_RFDIV_7   (6 << 7)            // Reference freq divider (div 7)
#define EUartUFCR_TXTL_SHF  10                  // Transmitter trigger level shift
#define EUartUSR1_PARITYERR (1 << 15)           // Parity error interrupt flag
#define EUartUSR1_RTSS      (1 << 14)           // RTS pin status
#define EUartUSR1_TRDY      (1 << 13)           // Transmitter ready interrupt/dma flag
#define EUartUSR1_RTSD      (1 << 12)           // RTS delta
#define EUartUSR1_ESCF      (1 << 11)           // Escape seq interrupt flag
#define EUartUSR1_FRAMERR   (1 << 10)           // Frame error interrupt flag
#define EUartUSR1_RRDY      (1 << 9)            // Receiver ready interrupt/dma flag
#define EUartUSR1_AGTIM     (1 << 8)            // Aging timeout interrupt status
#define EUartUSR1_RXDS      (1 << 6)            // Receiver idle interrupt flag
#define EUartUSR1_AIRINT    (1 << 5)            // Async IR wake interrupt flag
#define EUartUSR1_AWAKE     (1 << 4)            // Aysnc wake interrupt flag
#define EUartUSR2_ADET      (1 << 15)           // Auto baud rate detect complete
#define EUartUSR2_TXFE      (1 << 14)           // Transmit buffer FIFO empty
#define EUartUSR2_IDLE      (1 << 12)           // Idle condition
#define EUartUSR2_ACST      (1 << 11)           // Autobaud counter stopped
#define EUartUSR2_IRINT     (1 << 8)            // Serial infrared interrupt flag
#define EUartUSR2_WAKE      (1 << 7)            // Wake
#define EUartUSR2_RTSF      (1 << 4)            // RTS edge interrupt flag
#define EUartUSR2_TXDC      (1 << 3)            // Transmitter complete
#define EUartUSR2_BRCD      (1 << 2)            // Break condition
#define EUartUSR2_ORE       (1 << 1)            // Overrun error
#define EUartUSR2_RDR       (1 << 0)            // Recv data ready
#define EUartUTS_FRCPERR    (1 << 13)           // Force parity error
#define EUartUTS_LOOP       (1 << 12)           // Loop tx and rx
#define EUartUTS_TXEMPTY    (1 << 6)            // TxFIFO empty
#define EUartUTS_RXEMPTY    (1 << 5)            // RxFIFO empty
#define EUartUTS_TXFULL     (1 << 4)            // TxFIFO full
#define EUartUTS_RXFULL     (1 << 3)            // RxFIFO full
#define EUartUTS_SOFTRST    (1 << 0)            // Software reset

#define EUartUFCR_RFDIV                        EUartUFCR_RFDIV_2

extern u32 get_peri_clock(enum peri_clocks clk);

#if 1
#if (EUartUFCR_RFDIV==EUartUFCR_RFDIV_2)
#define MXC_UART_REFFREQ                        (get_peri_clock(UART1_BAUD) / 2)
#endif

#if (EUartUFCR_RFDIV==EUartUFCR_RFDIV_4)
#define MXC_UART_REFFREQ                        (get_peri_clock(UART1_BAUD) / 4)
#endif
#else
#define MXC_UART_REFFREQ                        33300000
#endif

#ifndef CONFIG_SERIAL
#define CONFIG_SERIAL	1
#endif

struct mx31_serial {
	volatile u32 urxd[16];
	volatile u32 utxd[16];
	volatile u32 ucr1;
	volatile u32 ucr2;
	volatile u32 ucr3;
	volatile u32 ucr4;
	volatile u32 ufcr;
	volatile u32 usr1;
	volatile u32 usr2;
	volatile u32 uesc;
	volatile u32 utim;
	volatile u32 ubir;
	volatile u32 ubmr;
	volatile u32 ubrc;
	volatile u32 onems;
	volatile u32 uts;
};
static volatile struct mx31_serial* serial_base = NULL;

void serial_setbrg (void)
{
	serial_init();
}


/*board special function.*/
extern void uart_gpio_init(int port);

static volatile struct mx31_serial * mx31uarts[] =
{
	(volatile struct mx31_serial*)UART1_BASE_ADDR,
	(volatile struct mx31_serial*)UART2_BASE_ADDR,
	(volatile struct mx31_serial*)UART3_BASE_ADDR,
	(volatile struct mx31_serial*)UART4_BASE_ADDR,
	(volatile struct mx31_serial*)UART5_BASE_ADDR,
};

#define disable_uart(x) (x)->ucr1 &= ~EUartUCR1_UARTEN
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

int serial_init (void)
{
	DECLARE_GLOBAL_DATA_PTR;
	char *p;
	int port = CONFIG_SERIAL;
	int i;

	for(i=0; i< ARRAY_SIZE(mx31uarts); i++)
		disable_uart(mx31uarts[i]); /*disable all uarts first.*/

	p = getenv ("console");
	if(p)
	{
		while( *p && !isdigit(*p))
			p++;
		port = (*p) ? (int) simple_strtoul (p, NULL, 10)+1
			: CONFIG_SERIAL;
	}

	uart_gpio_init(port);

	switch(port)
	{
		case 2:
		case 3:
		case 4:
		case 5:
			serial_base = mx31uarts[port - 1];
			break;
		default:
			serial_base = mx31uarts[0];
	}
	while (!(serial_base->uts & EUartUTS_TXEMPTY));

	/* Disable UART */
	serial_base->ucr1 &= ~EUartUCR1_UARTEN;
	disable_uart(serial_base);

	/* Set to default POR state */
	serial_base->ucr1 = 0x00000000;
	serial_base->ucr2 = 0x00000000;

	while (!(serial_base->ucr2 & EUartUCR2_SRST_));

	serial_base->ucr3 = 0x00000704;
	serial_base->ucr4 = 0x00008000;
	serial_base->ufcr = 0x00000801;
	serial_base->uesc = 0x0000002B;
	serial_base->utim = 0x00000000;
	serial_base->ubir = 0x00000000;
	serial_base->ubmr = 0x00000000;
	serial_base->onems = 0x00000000;
	serial_base->uts  = 0x00000000;

	/* Configure FIFOs */
	serial_base->ufcr = (1 << EUartUFCR_RXTL_SHF) | EUartUFCR_RFDIV
		| (2 << EUartUFCR_TXTL_SHF);

	/* Setup One MS timer */
	serial_base->onems  = (MXC_UART_REFFREQ / 1000);

	/* Set to 8N1 */
	serial_base->ucr2 &= ~EUartUCR2_PREN;
	serial_base->ucr2 |= EUartUCR2_WS;
	serial_base->ucr2 &= ~EUartUCR2_STPB;

	/* Ignore RTS */
	serial_base->ucr2 |= EUartUCR2_IRTS;

	/* Enable UART */
	serial_base->ucr1 |= EUartUCR1_UARTEN;

	/* Enable FIFOs */
	serial_base->ucr2 |= EUartUCR2_SRST_ | EUartUCR2_RXEN | EUartUCR2_TXEN;

	/* Clear status flags */
	serial_base->usr2 |= EUartUSR2_ADET  |
		EUartUSR2_IDLE  |
		EUartUSR2_IRINT |
		EUartUSR2_WAKE  |
		EUartUSR2_RTSF  |
		EUartUSR2_BRCD  |
		EUartUSR2_ORE   |
		EUartUSR2_RDR;

	/* Clear status flags */
	serial_base->usr1 |= EUartUSR1_PARITYERR |
		EUartUSR1_RTSD      |
		EUartUSR1_ESCF      |
		EUartUSR1_FRAMERR   |
		EUartUSR1_AIRINT    |
		EUartUSR1_AWAKE;


	/* Set the numerator value minus one of the BRM ratio */
	serial_base->ubir = (gd->baudrate / 100) - 1;

	/* Set the denominator value minus one of the BRM ratio    */
	serial_base->ubmr = ((MXC_UART_REFFREQ / 1600) - 1);

	return (0);
}

void serial_stop (void)
{
	if(!serial_base)
		return ;
	/* Wait for UART to finish transmitting */
	while (!(serial_base->uts & EUartUTS_TXEMPTY));

	/* Disable UART */
	serial_base->ucr1 &= ~EUartUCR1_UARTEN;
}

#define UART_UTXD			0x40
#define UART_UTS			0xb4

/*
 * Output a single byte to the serial port.
 */
void serial_putc (const char c)
{
	if(!serial_base)
	{
		while(__REG(UART1_BASE_ADDR + UART_UTS) & EUartUTS_TXFULL)
			;
		__REG(UART1_BASE_ADDR + UART_UTXD) = c;
	}
	else
	{
		while(serial_base->uts & EUartUTS_TXFULL)
			;
		serial_base->utxd[0] = c;
	}

	/* If \n, also do \r */
	if (c == '\n')
		serial_putc ('\r');
}

/*
 * Read a single byte from the serial port. Returns 1 on success, 0
 * otherwise. When the function is succesfull, the character read is
 * written into its argument c.
 */
int serial_tstc (void)
{
	if(!serial_base)
	{
		return !(__REG(UART1_BASE_ADDR + UART_UTS) & EUartUTS_RXEMPTY);
	}
	return !(serial_base->uts & EUartUTS_RXEMPTY);
}

/*
 * Read a single byte from the serial port. Returns 1 on success, 0
 * otherwise. When the function is succesfull, the character read is
 * written into its argument c.
 */
int serial_getc (void)
{
	while(!serial_tstc())
		;
	if(!serial_base)
		return (char ) (__REG(UART1_BASE_ADDR) & 0xff);
	return (char) serial_base->urxd[0] & 0xff;
}

void
serial_puts (const char *s)
{
	while (*s) {
		serial_putc (*s++);
	}
}

#endif
