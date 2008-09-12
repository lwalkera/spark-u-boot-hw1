/* vim: set ts=8 sw=8 noet:
 *
 * Cirrus Logic EP93xx ethernet MAC / MII driver.
 *
 * Copyright (C) 2004, 2005
 * Cory T. Tusar, Videon Central, Inc., <ctusar@videon-central.com>
 *
 * Based on the original eth.[ch] Cirrus Logic EP93xx Rev D. Ethernet Driver,
 * which is
 *
 * (C) Copyright 2002 2003
 * Adam Bezanson, Network Audio Technologies, Inc.
 * <bezanson@netaudiotech.com>
 *
 * See file CREDITS for list of people who contributed to this project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#include <command.h>
#include <common.h>
#include <asm/arch/ep93xx.h>
#include <malloc.h>
#include <miiphy.h>
#include <net.h>
#include <linux/types.h>


/**
 * Send an error message to the terminal.
 */
#define ERROR(x) \
do { \
	char *__foo = strrchr(__FILE__, '/'); \
	\
	printf("%s: %d: %s(): ", (__foo == NULL ? __FILE__ : (__foo + 1)), \
			__LINE__, __FUNCTION__); \
	printf x; printf("\n"); \
} while (0);


/**
 * Send a trace message to the terminal.
 */
#if 0
#define TRACE(x) \
do { \
	char *__foo = strrchr(__FILE__, '/'); \
	\
	printf("%s: %d: %s(): ", (__foo == NULL ? __FILE__ : (__foo + 1)), \
		__LINE__, __FUNCTION__); \
	printf x; printf("\n"); \
} while (0);

#else
#define TRACE(x)
#endif


/* -----------------------------------------------------------------------------
 * EP93xx ethernet MAC functionality
 */
#if defined(CONFIG_DRIVER_EP93XX_MAC)

/**
 * #define this to dump device status and queue info during initialization and
 * following errors.
 */
#undef EP93XX_MAC_DEBUG

/**
 * Number of descriptor and status entries in our RX queues.
 */
#define NUMRXDESC		(8)

/**
 * Number of descriptor and status entries in our TX queues.
 */
#define NUMTXDESC		(1)

/**
 * 944 = (1024 - 64) - 16, Fifo size - Minframesize - 16 (Chip FACT)
 */
#define TXSTARTMAX		(944)


/**
 * Ethernet MAC interface return values for public functions.
 */
enum eth_status {
	ETH_STATUS_SUCCESS = 0,
	ETH_STATUS_FAILURE = 1,
};


/**
 * Receive descriptor queue entry
 */
struct rx_descriptor_t {
	union {
		uint32_t word1;

		struct {
			unsigned buffer_address	: 32;
		};
	};

	union {
		uint32_t word2;

		struct {
			unsigned buffer_length	: 16;
			unsigned buffer_index	: 15;
			unsigned not_sof	:  1;
		};
	};
} __attribute__((packed));

typedef struct rx_descriptor_t rx_descriptor_t;


/**
 * Receive status queue entry
 */
struct rx_status_t {
	union {
		uint32_t word1;

		struct {
			unsigned		:  8;
			unsigned hti		:  6;
			unsigned		:  1;
			unsigned crci		:  1;
			unsigned crce		:  1;
			unsigned edata		:  1;
			unsigned runt		:  1;
			unsigned fe		:  1;
			unsigned oe		:  1;
			unsigned rx_err		:  1;
			unsigned am		:  2;
			unsigned		:  4;
			unsigned eob		:  1;
			unsigned eof		:  1;
			unsigned rwe		:  1;
			unsigned rfp		:  1;
		};
	};

	union {
		uint32_t word2;

		struct {
			unsigned frame_length	: 16;
			unsigned buffer_index	: 15;
			unsigned rfp		:  1;
		};
	};
} __attribute__((packed));

typedef struct rx_status_t rx_status_t;


/**
 * Transmit descriptor queue entry
 */
struct tx_descriptor_t
{
	union {
		uint32_t word1;

		struct {
			unsigned buffer_address	: 32;
		};
	};

	union {
		uint32_t word2;

		struct {
			unsigned buffer_length	: 12;
			unsigned		:  3;
			unsigned abort_frame	:  1;
			unsigned buffer_index	: 15;
			unsigned eof		:  1;
		};
	};
} __attribute__((packed));

typedef struct tx_descriptor_t tx_descriptor_t;


/**
 * Transmit status queue entry
 */
struct tx_status_t {
	union {
		uint32_t word1;

		struct {
			unsigned tbi		: 15;
			unsigned		:  1;
			unsigned ncoll		:  5;
			unsigned		:  3;
			unsigned ecoll		:  1;
			unsigned txu		:  1;
			unsigned ow		:  1;
			unsigned		:  1;
			unsigned lcrs		:  1;
			unsigned fa		:  1;
			unsigned txwe		:  1;
			unsigned txfp		:  1;
		};
	};
} __attribute__((packed));

typedef struct tx_status_t tx_status_t;


/**
 * Transmit descriptor queue
 */
struct tx_descriptor_queue_t {
	tx_descriptor_t *base;
	tx_descriptor_t *current;
	tx_descriptor_t *end;
};

typedef struct tx_descriptor_queue_t tx_descriptor_queue_t;


/**
 * Transmit status queue
 */
struct tx_status_queue_t {
	tx_status_t *base;
	tx_status_t *current;
	tx_status_t *end;
};

typedef struct tx_status_queue_t tx_status_queue_t;


/**
 * Receive descriptor queue
 */
struct rx_descriptor_queue_t {
	rx_descriptor_t *base;
	rx_descriptor_t *current;
	rx_descriptor_t *end;
};

typedef struct rx_descriptor_queue_t rx_descriptor_queue_t;


/**
 * Receive status queue
 */
struct rx_status_queue_t {
	rx_status_t *base;
	rx_status_t *current;
	rx_status_t *end;
};

typedef struct rx_status_queue_t rx_status_queue_t;


/**
 * EP93xx MAC private data structure
 */
struct ep93xx_mac {
	int			is_initialized;

	rx_descriptor_queue_t	rx_dq;
	rx_status_queue_t	rx_sq;
	void *			rx_buffer[NUMRXDESC];

	tx_descriptor_queue_t	tx_dq;
	tx_status_queue_t	tx_sq;
};


/* ep93xx_miiphy ops forward declarations */
static int ep93xx_miiphy_read(char * const dev, unsigned char const addr,
		unsigned char const reg, unsigned short * const value);
static int ep93xx_miiphy_write(char * const dev, unsigned char const addr,
		unsigned char const reg, unsigned short const value);


/* Reserve memory for the MAC's private data */
static struct ep93xx_mac dev = { 0 };


/**
 * Dump ep93xx_mac values to the terminal.
 */
extern inline void dump_dev(void)
{
#if defined(EP93XX_MAC_DEBUG)
	int i;

	printf("\ndump_dev()\n");
	printf("  is_initialized     %02X\n", dev.is_initialized);
	printf("  rx_dq.base         %08X\n", dev.rx_dq.base);
	printf("  rx_dq.current      %08X\n", dev.rx_dq.current);
	printf("  rx_dq.end          %08X\n", dev.rx_dq.end);
	printf("  rx_sq.base         %08X\n", dev.rx_sq.base);
	printf("  rx_sq.current      %08X\n", dev.rx_sq.current);
	printf("  rx_sq.end          %08X\n", dev.rx_sq.end);

	for (i = 0; i < NUMRXDESC; i++) {
		printf("  rx_buffer[%2.d]      %08X\n", i, dev.rx_buffer[i]);
	}

	printf("  tx_dq.base         %08X\n", dev.tx_dq.base);
	printf("  tx_dq.current      %08X\n", dev.tx_dq.current);
	printf("  tx_dq.end          %08X\n", dev.tx_dq.end);
	printf("  tx_sq.base         %08X\n", dev.tx_sq.base);
	printf("  tx_sq.current      %08X\n", dev.tx_sq.current);
	printf("  tx_sq.end          %08X\n", dev.tx_sq.end);
#endif  /* defined(EP93XX_MAC_DEBUG) */
}


/**
 * Dump all RX descriptor queue entries to the terminal.
 */
extern inline void dump_rx_descriptor_queue(void)
{
#if defined(EP93XX_MAC_DEBUG)
	int i;

	printf("\ndump_rx_descriptor_queue()\n");
	printf("  descriptor address     word1           word2\n");
	for (i = 0; i < NUMRXDESC; i++) {
		printf("  [ %08X ]           %08X        %08X\n",
				(dev.rx_dq.base + i),
				(dev.rx_dq.base + i)->word1,
				(dev.rx_dq.base + i)->word2);
	}
#endif  /* defined(EP93XX_MAC_DEBUG) */
}


/**
 * Dump all RX status queue entries to the terminal.
 */
extern inline void dump_rx_status_queue(void)
{
#if defined(EP93XX_MAC_DEBUG)
	int i;

	printf("\ndump_rx_status_queue()\n");
	printf("  descriptor address     word1           word2\n");
	for (i = 0; i < NUMRXDESC; i++) {
		printf("  [ %08X ]           %08X        %08X\n",
				(dev.rx_sq.base + i),
				(dev.rx_sq.base + i)->word1,
				(dev.rx_sq.base + i)->word2);
	}
#endif  /* defined(EP93XX_MAC_DEBUG) */
}


/**
 * Dump all TX descriptor queue entries to the terminal.
 */
extern inline void dump_tx_descriptor_queue(void)
{
#if defined(EP93XX_MAC_DEBUG)
	int i;

	printf("\ndump_tx_descriptor_queue()\n");
	printf("  descriptor address     word1           word2\n");
	for (i = 0; i < NUMTXDESC; i++) {
		printf("  [ %08X ]           %08X        %08X\n",
				(dev.tx_dq.base + i),
				(dev.tx_dq.base + i)->word1,
				(dev.tx_dq.base + i)->word2);
	}
#endif  /* defined(EP93XX_MAC_DEBUG) */
}


/**
 * Dump all TX status queue entries to the terminal.
 */
extern inline void dump_tx_status_queue(void)
{
#if defined(EP93XX_MAC_DEBUG)
	int i;

	printf("\ndump_tx_status_queue()\n");
	printf("  descriptor address     word1\n");
	for (i = 0; i < NUMTXDESC; i++) {
		printf("  [ %08X ]           %08X\n",
				(dev.rx_sq.base + i),
				(dev.rx_sq.base + i)->word1);
	}
#endif  /* defined(EP93XX_MAC_DEBUG) */
}


/**
 * Reset the EP93xx MAC by twiddling the soft reset bit and spinning until
 * it's cleared.
 */
static void ep93xx_mac_reset(void)
{
	TRACE(("+ep93xx_mac_reset"));

	OpReg_SelfCTL |= SelfCTL_RESET;
	while (OpReg_SelfCTL & SelfCTL_RESET) {
		/* nop */
	}

	TRACE(("-ep93xx_mac_reset"));
}


/**
 * Halt EP93xx MAC transmit and receive by clearing the TxCTL and RxCTL
 * registers.
 */
extern void eth_halt(void)
{
	TRACE(("+eth_halt"));

	OpReg_RxCTL = 0x00000000;
	OpReg_TxCTL = 0x00000000;

	TRACE(("-eth_halt"));
}


/**
 * Initialize the EP93xx MAC.  The MAC hardware is reset.  Buffers are
 * allocated, if necessary, for the TX and RX descriptor and status queues,
 * as well as for received packets.  The EP93XX MAC hardware is initialized.
 * Transmit and receive operations are enabled.
 */
extern int eth_init(bd_t * const bd)
{
	int ret = ETH_STATUS_FAILURE;
	int i;

	TRACE(("+eth_init"));

	/* Parameter check */
	if (bd == NULL) {
		ERROR(("NULL bd"));
		goto eth_init_failed_0;
	}

	/* Reset the MAC */
	ep93xx_mac_reset();

	/* Allocate space for the queues and RX packet buffers if we're not
	 * already initialized */
	if (!dev.is_initialized) {
		if ((dev.tx_dq.base = calloc(NUMTXDESC,
					sizeof(tx_descriptor_t))) == NULL) {
			ERROR(("calloc() failed"));
			goto eth_init_failed_0;
		}

		if ((dev.tx_sq.base = calloc(NUMTXDESC,
					sizeof(tx_status_t))) == NULL) {
			ERROR(("calloc() failed"));
			goto eth_init_failed_1;
		}

		if ((dev.rx_dq.base = calloc(NUMRXDESC,
					sizeof(rx_descriptor_t))) == NULL) {
			ERROR(("calloc() failed"));
			goto eth_init_failed_2;
		}

		if ((dev.rx_sq.base = calloc(NUMRXDESC,
					sizeof(rx_status_t))) == NULL) {
			ERROR(("calloc() failed"));
			goto eth_init_failed_3;
		}

		for (i = 0; i < NUMRXDESC; i++) {
			dev.rx_buffer[i] = NULL;
		}

		for (i = 0; i < NUMRXDESC; i++) {
			if ((dev.rx_buffer[i] = calloc(1, PKTSIZE)) == NULL) {
				ERROR(("calloc() failed"));
				goto eth_init_failed_4;
			}
		}

		/* Set is_initialized flag so we don't go through allocation
		 * portion of init again. */
		dev.is_initialized = 1;
	}

	/* Reset the descriptor queues' current and end address values */
	dev.tx_dq.current = dev.tx_dq.base;
	dev.tx_dq.end = (dev.tx_dq.base + NUMTXDESC);

	dev.tx_sq.current = dev.tx_sq.base;
	dev.tx_sq.end = (dev.tx_sq.base + NUMTXDESC);

	dev.rx_dq.current = dev.rx_dq.base;
	dev.rx_dq.end = (dev.rx_dq.base + NUMRXDESC);

	dev.rx_sq.current = dev.rx_sq.base;
	dev.rx_sq.end = (dev.rx_sq.base + NUMRXDESC);

	/* Set the transmit descriptor and status queues' base address,
	 * current address, and length registers.  Set the maximum frame
	 * length and threshold.  Enable the transmit descriptor processor. */
	OpReg_TxDBA = (uint32_t)dev.tx_dq.base;
	OpReg_TxDCA = (uint32_t)dev.tx_dq.base;
	OpReg_TxDBL = (sizeof(tx_descriptor_t) * NUMTXDESC);

	OpReg_TxSBA = (uint32_t)dev.tx_sq.base;
	OpReg_TxSCA = (uint32_t)dev.tx_sq.base;
	OpReg_TxSBL = (sizeof(tx_status_t) * NUMTXDESC);

	OpReg_MaxFL = (TXSTARTMAX << 16) | (PKTSIZE << 0);
	OpReg_BMCTL = BMCTL_TxEn;

	/* Set the receive descriptor and status queues' base address,
	 * current address, and length registers.  Enable the receive
	 * descriptor processor. */
	OpReg_RxDBA = (uint32_t)dev.rx_dq.base;
	OpReg_RxDCA = (uint32_t)dev.rx_dq.base;
	OpReg_RxDBL = (sizeof(rx_descriptor_t) * NUMRXDESC);

	OpReg_RxSBA = (uint32_t)dev.rx_sq.base;
	OpReg_RxSCA = (uint32_t)dev.rx_sq.base;
	OpReg_RxSBL = (sizeof(rx_status_t) * NUMRXDESC);

	OpReg_BMCTL = BMCTL_RxEn;

	/* Wait until the receive descriptor processor is active */
	while (!(OpReg_BMSts & BMSts_RxAct)) {
		/* nop */
	}

	/* Initialize the RX descriptor queue.  Clear the TX descriptor queue.
	 * Clear the RX and TX status queues.  Enqueue the RX descriptor and
	 * status entries to the MAC. */
	for (i = 0; i < NUMRXDESC; i++) {
		(dev.rx_dq.base + i)->buffer_address =
			(uint32_t)dev.rx_buffer[i];
		(dev.rx_dq.base + i)->buffer_length = PKTSIZE;
		(dev.rx_dq.base + i)->buffer_index = 0;
	}

	memset(dev.tx_dq.base, 0, (sizeof(tx_descriptor_t) * NUMTXDESC));
	memset(dev.rx_sq.base, 0, (sizeof(rx_status_t) * NUMRXDESC));
	memset(dev.tx_sq.base, 0, (sizeof(tx_status_t) * NUMTXDESC));

	OpReg_RxDEQ = NUMRXDESC;
	OpReg_RxSEQ = NUMRXDESC;

	/* Set the primary MAC address */
	OpReg_AFP = AFP_IAPrimary;
	OpReg_IndAd = (bd->bi_enetaddr[0] |
			(bd->bi_enetaddr[1] << 8) |
			(bd->bi_enetaddr[2] << 16) |
			(bd->bi_enetaddr[3] << 24));
	OpReg_IndAd1 = (bd->bi_enetaddr[4] | (bd->bi_enetaddr[5] << 8));

	/* Turn on RX and TX */
	OpReg_RxCTL = (RxCTL_IndividualAccept0 | RxCTL_BroadcastA |
			RxCTL_SerRxON);
	OpReg_TxCTL = TxCTL_SerTxON;

	/* Dump data structures if we're debugging */
	dump_dev();
	dump_rx_descriptor_queue();
	dump_rx_status_queue();
	dump_tx_descriptor_queue();
	dump_tx_status_queue();

#if defined(CONFIG_MII)
	miiphy_register("ep93xx_eth0", ep93xx_miiphy_read, ep93xx_miiphy_write);
#endif

	/* Done! */
	ret = ETH_STATUS_SUCCESS;
	goto eth_init_done;

eth_init_failed_4:
	for (i = 0; i < NUMRXDESC; i++) {
		if (dev.rx_buffer[i] != NULL) {
			free(dev.rx_buffer[i]);
		}
	}

	free(dev.rx_sq.base);
	/* Fall through */

eth_init_failed_3:
	free(dev.rx_dq.base);
	/* Fall through */

eth_init_failed_2:
	free(dev.tx_sq.base);
	/* Fall through */

eth_init_failed_1:
	free(dev.tx_dq.base);
	/* Fall through */

eth_init_failed_0:
eth_init_done:
	TRACE(("-eth_init %d", ret));
	return(ret);
}


/**
 * Copy a frame of data from the MAC into the protocol layer for further
 * processing.
 *
 * TODO: Enhance this to deal with as many packets as are available at
 * the MAC at one time? */
extern int eth_rx(void)
{
	int ret = ETH_STATUS_FAILURE;
	int nbytes_frame = 0;
	int nbytes_fragment = 0;
	void *dest = (void *)NetRxPackets[0];

	TRACE(("+eth_rx"));

	while (1) {
		if (dev.rx_sq.current->rfp && dev.rx_sq.current->rwe) {
			/* We have a good frame.  Extract the frame's length
			 * from the current rx_status_queue entry, and copy
			 * the frame's data into NetRxPackets[] of the
			 * protocol stack.  We track the total number of
			 * bytes in the frame (nbytes_frame) which will be
			 * used when we pass the data off to the protocol
			 * layer via NetReceive(). */
			nbytes_fragment = dev.rx_sq.current->frame_length;
			nbytes_frame += nbytes_fragment;

			memcpy(dest, (void *)dev.rx_dq.current->buffer_address,
					nbytes_fragment);
			dest += nbytes_fragment;

			/* Clear the associated status queue entry, and
			 * increment our current pointers to the next RX
			 * descriptor and status queue entries (making sure
			 * we wrap properly). */
			memset(dev.rx_sq.current, 0, sizeof(rx_status_t));

			dev.rx_sq.current++;
			if (dev.rx_sq.current >= dev.rx_sq.end)
				dev.rx_sq.current = dev.rx_sq.base;

			dev.rx_dq.current++;
			if (dev.rx_dq.current >= dev.rx_dq.end) {
				dev.rx_dq.current = dev.rx_dq.base;
			}

			/* Finally, return the RX descriptor and status entries
			 * back to the MAC engine, and loop again, checking for
			 * more descriptors to process. */
			OpReg_RxDEQ = 1;
			OpReg_RxSEQ = 1;

		} else if (!dev.rx_sq.current->rfp && !dev.rx_sq.current->rwe) {
			/* We've no further queued descriptors.  If data have
			 * been copied into the protocol layer, issue a
			 * NetReceive() call to pass the data to the protocol
			 * stack for further processing, and return
			 * successfully. */
			if (nbytes_frame > 0) {
				NetReceive(NetRxPackets[0], nbytes_frame);
				TRACE(("reporting %d bytes (last: %d)...\n",
						nbytes_frame, nbytes_fragment));
			}

			ret = ETH_STATUS_SUCCESS;
			break;

		} else {
			/* Do we have an erroneous packet? */
			ERROR(("packet rx error, status %08X %08X",
					dev.rx_sq.current->word1,
					dev.rx_sq.current->word2));
			dump_rx_descriptor_queue();
			dump_rx_status_queue();

			/* TODO: Add better error handling? */
			break;
		}
	}

	TRACE(("-eth_rx %d", ret));
	return(ret);
}


/**
 * Send a block of data via ethernet.
 *
 * TODO: Enhance this to deal with as much data as are available at one time? */
extern int eth_send(volatile void * const packet, int const length)
{
	int ret = ETH_STATUS_FAILURE;

	TRACE(("+eth_send"));

	/* Parameter check */
	if (packet == NULL) {
		ERROR(("NULL packet"));
		goto eth_send_failed_0;
	}

	/* Initialize the TX descriptor queue with the new packet's info.
	 * Clear the associated status queue entry.  Enqueue the packet
	 * to the MAC for transmission. */
	dev.tx_dq.current->buffer_address = (uint32_t)packet;
	dev.tx_dq.current->buffer_length = length;
	dev.tx_dq.current->buffer_index = 0;
	dev.tx_dq.current->eof = 1;

	dev.tx_sq.current->word1 = 0;

	OpReg_TxDEQ = 1;

	/* Wait for TX to complete, and check status entry for errors. */
	while (!(OpReg_IntStsC & IntSts_TxStsQ)) {
		/* nop */
	}

	if (!dev.tx_sq.current->txfp || !dev.tx_sq.current->txwe) {
		ERROR(("packet tx error, status %08X",
				dev.tx_sq.current->word1));
		dump_tx_descriptor_queue();
		dump_tx_status_queue();

		/* TODO: Add better error handling? */
		goto eth_send_failed_0;
	}

	ret = ETH_STATUS_SUCCESS;
	/* Fall through */

eth_send_failed_0:
	TRACE(("-eth_send %d", ret));
	return(ret);
}
#endif  /* defined(CONFIG_DRIVER_EP93XX_MAC) */


/* -----------------------------------------------------------------------------
 * EP93xx ethernet MII functionality.
 */
#if defined(CONFIG_MII)

/**
 * Maximum MII address we support
 */
#define MII_ADDRESS_MAX			(31)

/**
 * Maximum MII register address we support
 */
#define MII_REGISTER_MAX		(31)


/**
 * Ethernet MII interface return values for public functions.
 */
enum mii_status {
	MII_STATUS_SUCCESS = 0,
	MII_STATUS_FAILURE = 1,
};


/**
 * Read a 16-bit value from an MII register.
 */
static int ep93xx_miiphy_read(char * const dev, unsigned char const addr,
		unsigned char const reg, unsigned short * const value)
{
	int ret = MII_STATUS_FAILURE;
	uint32_t self_ctl;

	TRACE(("+ep93xx_miiphy_read"));

	/* Parameter checks */
	if (dev == NULL) {
		ERROR(("NULL dev"));
		goto ep93xx_miiphy_read_failed_0;
	}

	if (addr > MII_ADDRESS_MAX) {
		ERROR(("invalid addr, 0x%02X", addr));
		goto ep93xx_miiphy_read_failed_0;
	}

	if (reg > MII_REGISTER_MAX) {
		ERROR(("invalid reg, 0x%02X", reg));
		goto ep93xx_miiphy_read_failed_0;
	}

	if (value == NULL) {
		ERROR(("NULL value"));
		goto ep93xx_miiphy_read_failed_0;
	}

	/* Save the current SelfCTL register value.  Set MAC to suppress
	 * preamble bits.  Wait for any previous MII command to complete
	 * before issuing the new command. */
	self_ctl = OpReg_SelfCTL;
#if defined(CONFIG_MII_SUPPRESS_PREAMBLE)
	OpReg_SelfCTL = (self_ctl & ~(1 << 8));
#endif  /* defined(CONFIG_MII_SUPPRESS_PREAMBLE) */

	while (OpReg_MIISts & MIISts_Busy) {
		/* nop */
	}

	/* Issue the MII 'read' command.  Wait for the command to complete.
	 * Read the MII data value. */
	OpReg_MIICmd = (MIICmd_Opcode_Read | ((uint32_t)addr << 5) |
			(uint32_t)reg);
	while (OpReg_MIISts & MIISts_Busy) {
		/* nop */
	}

	*value = (unsigned short)OpReg_MIIData;

	/* Restore the saved SelfCTL value and return. */
	OpReg_SelfCTL = self_ctl;

	ret = MII_STATUS_SUCCESS;
	/* Fall through */

ep93xx_miiphy_read_failed_0:
	TRACE(("-ep93xx_miiphy_read"));
	return(ret);
}


/**
 * Write a 16-bit value to an MII register.
 */
static int ep93xx_miiphy_write(char * const dev, unsigned char const addr,
		unsigned char const reg, unsigned short const value)
{
	int ret = MII_STATUS_FAILURE;
	uint32_t self_ctl;

	TRACE(("+ep93xx_miiphy_write"));

	/* Parameter checks */
	if (dev == NULL) {
		ERROR(("NULL dev"));
		goto ep93xx_miiphy_write_failed_0;
	}

	if (addr > MII_ADDRESS_MAX) {
		ERROR(("invalid addr, 0x%02X", addr));
		goto ep93xx_miiphy_write_failed_0;
	}

	if (reg > MII_REGISTER_MAX) {
		ERROR(("invalid reg, 0x%02X", reg));
		goto ep93xx_miiphy_write_failed_0;
	}

	/* Save the current SelfCTL register value.  Set MAC to suppress
	 * preamble bits.  Wait for any previous MII command to complete
	 * before issuing the new command. */
	self_ctl = OpReg_SelfCTL;
#if defined(CONFIG_MII_SUPPRESS_PREAMBLE)
	OpReg_SelfCTL = (self_ctl & ~(1 << 8));
#endif  /* defined(CONFIG_MII_SUPPRESS_PREAMBLE) */

	while (OpReg_MIISts & MIISts_Busy) {
		/* nop */
	}

	/* Issue the MII 'write' command.  Wait for the command to complete. */
	OpReg_MIIData = (uint32_t)value;
	OpReg_MIICmd = (MIICmd_Opcode_Write | ((uint32_t)addr << 5) |
			(uint32_t)reg);
	while (OpReg_MIISts & MIISts_Busy) {
		/* nop */
	}

	/* Restore the saved SelfCTL value and return. */
	OpReg_SelfCTL = self_ctl;

	ret = MII_STATUS_SUCCESS;
	/* Fall through */

ep93xx_miiphy_write_failed_0:
	TRACE(("-ep93xx_miiphy_write"));
	return(ret);
}
#endif  /* defined(CONFIG_MII) */

