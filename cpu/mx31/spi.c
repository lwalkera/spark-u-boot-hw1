/*
 *  spi.c
 *
 *  Author:		jl.shan@vantrontech.com.cn
 *  Created:	May 23, 2007
 *  Copyright:	Applied Data Systems, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * 
 *
 */

#include <common.h>
#include <exports.h>
#include <asm/types.h>
#include <asm/arch/mx31-regs.h>
#include <asm/arch/mx31_pins.h>
#include <asm/io.h>
#include <asm/arch/iomux.h>
#include <asm/arch/spi.h>

#if defined(CONFIG_SPI)

extern u32 get_main_clock(enum main_clocks clk);

typedef enum {
	SPI_STATUS_TRANSFER_COMPLETE = (1 << 8),
	SPI_STATUS_TRANSFER_COMPLETE_0_7 = (1 << 7),
	SPI_STATUS_BIT_COUNT_OVERFLOW = (1 << 7),
	SPI_STATUS_BIT_COUNT_OVERFLOW_0_7 = 0,
	SPI_STATUS_RX_FIFO_OVERFLOW = (1 << 6),
	SPI_STATUS_RX_FIFO_FULL = (1 << 5),
	SPI_STATUS_RX_FIFO_HALF_FULL = (1 << 4),
	SPI_STATUS_RX_FIFO_DATA_READY = (1 << 3),
	SPI_STATUS_TX_FIFO_FULL = (1 << 2),
	SPI_STATUS_TX_FIFO_HALF_EMPTY = (1 << 1),
	SPI_STATUS_TX_FIFO_EMPTY = (1 << 0)
} spi_irqs_t;

typedef struct {
	volatile __u32 rxdata;
	volatile __u32 txdata;
	volatile __u32 conreg;
	volatile __u32 intreg;
	volatile __u32 dmareg;
	volatile __u32 statreg;
	volatile __u32 periodreg;
} spi_mxc_addr;


static spi_mxc_addr * spi_add[3];

static __u32 spi_ipg_clk;

static __u32 spi_set_baudrate(int mod, __u32 baud)
{
	__u32 divisor, shift;

	if (baud <= 0) {
		return -1;
	}
	/* Calculate required divisor (rounded) */
	divisor = (spi_ipg_clk + baud / 2) / baud;

	shift = 0;

	while (divisor > (1 << (MIN_DATA_RATE_SHIFT + shift + 1)) &&
			shift < DATA_RATE_MASK) {
		shift++;
	}

	MODIFY_REGISTER_32((DATA_RATE_MASK << DATA_RATE_SHIFT),
			(shift << DATA_RATE_SHIFT),
			_reg_CSPI(spi_add[mod]->conreg));

	return (spi_ipg_clk >> (MIN_DATA_RATE_SHIFT + shift));
}

static int spi_set_transfer_length(int mod, __u32 transferlength)
{
	__u32 shift, mask;

	/*bit_8-bit_12*/
	shift = 8;
	mask = 0x1f;

	if (((transferlength - 1) > mask) || (transferlength == 0)) {
		return -1;
	}

	MODIFY_REGISTER_32(mask << shift,
			((transferlength - 1) & mask) << shift,
			_reg_CSPI(spi_add[mod]->conreg));
	return 0;
}

int spi_select_ss(int mod, __u8 nb)
{
	int shift, mask;

	shift = 24;
	mask = 0x3;

	MODIFY_REGISTER_32(mask << shift, ((nb & mask) << shift),
			_reg_CSPI(spi_add[mod]->conreg));

	return 0;
}

int spi_set_mode(int mod, spi_mode_t mode)
{
	switch (mode) {
		case SPI_SLAVE:
			/* Configure as a slave */
			MODIFY_REGISTER_32(1 << MODE_MASTER_SLAVE_SHIFT, 0,
					_reg_CSPI(spi_add[mod]->conreg));
			break;

		case SPI_MASTER:
			/* Configure as a master */
			MODIFY_REGISTER_32(0, 1 << MODE_MASTER_SLAVE_SHIFT,
					_reg_CSPI(spi_add[mod]->conreg));
			break;
		default:
			return -1;
	}
	return 0;
}

int spi_set_ss_polarity(int mod, spi_ss_polarity_t polarity)
{
	switch (polarity) {
		case SPI_SS_ACTIVE_LOW:
			/* Slave Select active on low level */
			MODIFY_REGISTER_32(1 << SS_POLARITY_SHIFT, 0,
					_reg_CSPI(spi_add[mod]->conreg));
			break;

		case SPI_SS_ACTIVE_HIGH:
			/* Slave Select active on high level */
			MODIFY_REGISTER_32(0, 1 << SS_POLARITY_SHIFT,
					_reg_CSPI(spi_add[mod]->conreg));
			break;
		default:
			return -1;
	}
	return 0;
}

int spi_set_ss_waveform(int mod, spi_ss_waveform_t waveform)
{
	switch (waveform) {
		case SPI_LOW_BTN_BURST:
			/* Slave Select doesn't change between bursts */
			MODIFY_REGISTER_32(1 << SS_WAVEFORM_SHIFT, 0,
					_reg_CSPI(spi_add[mod]->conreg));
			break;

		case SPI_PULSE_BTN_BURST:
			/* Slave Select not asserted between bursts */
			MODIFY_REGISTER_32(0, 1 << SS_WAVEFORM_SHIFT,
					_reg_CSPI(spi_add[mod]->conreg));
			break;
		default:
			return -1;
	}
	return 0;
}

int spi_set_phase(int mod, spi_phase_t phase)
{
	switch (phase) {
		case SPI_PHASE_0:
			/* Phase 0 mode */
			MODIFY_REGISTER_32(1 << PHASE_SHIFT, 0,
					_reg_CSPI(spi_add[mod]->conreg));
			break;

		case SPI_PHASE_1:
			/* Phase 1 mode */
			MODIFY_REGISTER_32(0, 1 << PHASE_SHIFT,
					_reg_CSPI(spi_add[mod]->conreg));
			break;
		default:
			return -1;
	}
	return 0;
}

int spi_set_polarity(int mod, spi_polarity_t polarity)
{
	switch (polarity) {
		case SPI_POLARITY_ACTIVE_HIGH:
			/* Polarity active at high level */
			MODIFY_REGISTER_32(1 << POLARITY_SHIFT, 0,
					_reg_CSPI(spi_add[mod]->conreg));
			break;

		case SPI_POLARITY_ACTIVE_LOW:
			/* Polarity active at low level */
			MODIFY_REGISTER_32(0, 1 << POLARITY_SHIFT,
					_reg_CSPI(spi_add[mod]->conreg));
			break;
		default:
			return -1;
	}
	return 0;
}

static void spi_enable(int mod)
{
	MODIFY_REGISTER_32(0, 1 << STATE_ENABLE_DISABLE_SHIFT,
			_reg_CSPI(spi_add[mod]->conreg));
}

extern void gpio_spi_active(int);

int spi_hard_config(spi_config * config)
{
	int error = 0;
	int mod = config->module_number;

	spi_add[0] = (spi_mxc_addr *)IO_ADDRESS(CSPI1_BASE_ADDR);
	spi_add[1] = (spi_mxc_addr *)IO_ADDRESS(CSPI2_BASE_ADDR);
	spi_add[2] = (spi_mxc_addr *)IO_ADDRESS(CSPI3_BASE_ADDR);

	gpio_spi_active(mod);
	spi_ipg_clk = get_main_clock(IPG_CLK);
	error = spi_set_baudrate(mod, config->bit_rate);
	if (error < 0) {
		return error;
	}
	spi_set_transfer_length(mod, config->bit_count);
	error = spi_select_ss(mod, config->ss_asserted);

	if (error < 0) {
		return error;
	}

	if (config->master_mode == true) {
		spi_set_mode(mod, SPI_MASTER);
	} else {
		spi_set_mode(mod, SPI_SLAVE);
	}

	if (config->active_high_ss_polarity == true) {
		spi_set_ss_polarity(mod, SPI_SS_ACTIVE_HIGH);
	} else {
		spi_set_ss_polarity(mod, SPI_SS_ACTIVE_LOW);
	}

	if (config->ss_low_between_bursts == true) {
		spi_set_ss_waveform(mod, SPI_LOW_BTN_BURST);
	} else {
		spi_set_ss_waveform(mod, SPI_PULSE_BTN_BURST);
	}

	if (config->phase == true) {
		spi_set_phase(mod, SPI_PHASE_1);
	} else {
		spi_set_phase(mod, SPI_PHASE_0);
	}

	if (config->active_high_polarity == true) {
		spi_set_polarity(mod, SPI_POLARITY_ACTIVE_HIGH);
	} else {
		spi_set_polarity(mod, SPI_POLARITY_ACTIVE_LOW);
	}
	spi_enable(mod);
	return 0;
}

static inline __u32 spi_get_base_address(int spi_id)
{
	static __u32 cspi[3] = {
		IO_ADDRESS(CSPI1_BASE_ADDR),
		IO_ADDRESS(CSPI2_BASE_ADDR),
		IO_ADDRESS(CSPI3_BASE_ADDR),
	};

	return cspi[spi_id%3];
}

static void spi_init_exchange(int mod)
{
	_reg_CSPI(spi_add[mod]->conreg) |= (1 << EXCHANGE_BIT_SHIFT);
}

static inline int spi_rx_fifo_data_ready(int mod)
{
	int result;
	result = _reg_CSPI(spi_add[mod]->statreg) &
		SPI_STATUS_RX_FIFO_DATA_READY;
	return result;
}

static __u32 spi_get_rx_data(int mod, __u8 *ptr, int size)
{
	__u32 length, remainder;
	__u32 base_addr;
	__u8 *p = NULL;
	__u32 val, total;

	total = 0;
	p = ptr;
	remainder = (size % 4);
	length = size - remainder;

	base_addr = spi_get_base_address(mod);

	/* Get into this loop only if SPI received 4 or more bytes */
	while (length > 0) {
		val = 0;

		/* Read top of RX FIFO to get next value */
		val = __raw_readl(base_addr + OFFSET_CSPI_RXDATAREG);

		/* Copy bytes to our buffer */
		*(p + 0) = (__u8)((val & 0xff000000) >> 24);
		*(p + 1) = (__u8)((val & 0x00ff0000) >> 16);
		*(p + 2) = (__u8)((val & 0x0000ff00) >> 8);
		*(p + 3) = (__u8)((val & 0x000000ff) >> 0);

		length -= 4;
		total += 4;
		p += 4;

		if (spi_rx_fifo_data_ready(mod) == 0) {
			break;
		}
	}

	/* copy now remainder bytes to our buffer */
	if (remainder > 0) {
		int shift = 24;
		int i = 0;

		val = __raw_readl(base_addr + OFFSET_CSPI_RXDATAREG);
		while (i < remainder) {
			*(p + i) =
				(__u32)((val & (0xff << shift)) >> shift);
			shift -= 8;
			i++;
		}

		total += remainder;
	}

	return total;

}

static void spi_put_tx_data(int mod, __u8 *buffer, int size)
{
	__u32 length, remainder;
	__u32 base_addr;
	__u8 *p = NULL;
	__u32 val;

	p = buffer;
	remainder = (size % 4);
	length = size - remainder;

	base_addr = spi_get_base_address(mod);

	while (length > 0) {
		val = 0;
		val =
			(*(p + 0) << 24) | (*(p + 1) << 16) | (*(p + 2) << 8) | *(p
																	  +
																	  3);
		__raw_writel(val, base_addr + OFFSET_CSPI_TXDATAREG);
		length -= 4;
		p += 4;
	}

	if (remainder > 0) {
		int shift = 24;
		int i = 0;

		val = 0;
		while (i < remainder) {
			val |= (__u32)(*(p + i) << shift);
			shift -= 8;
			i++;
		}

		__raw_writel(val, base_addr + OFFSET_CSPI_TXDATAREG);
	}

	spi_init_exchange(mod);
}

/*!
 * This function sends a SPI frame.
 *
 * @param        buffer		The buffer to send, this buffer must
 *				be allocated with 'nb' bytes size by user
 * @param        bytes          The number of bytes to send, can not be more than 32 bytes.
 * @param        config  The handle to identify the SPI device.
 *
 * @return       		This function returns the number of bytes sent
 *				minus the number of received bytes, -1 in case of error.
 */
ssize_t spi_send_frame(__u8 *buffer, __u32 bytes,
		       spi_config * config)
{
	__u32 reg;
	int mod;
	int result;

	if ((buffer == NULL) || (config == NULL)) {
		return -1;
	}
	result = -1;

	if ((bytes * 8) > (config->bit_count * 8)) {
		goto error_out;
	}

	mod = config->module_number;

	spi_put_tx_data(mod, buffer, bytes);

	do {
		reg = __raw_readl(IO_ADDRESS(&spi_add[mod]->conreg));
	} while (reg & MXC_CSPICONREG_XCH);

	result = 0;
	do {
		result += spi_get_rx_data(mod, buffer, bytes);
		buffer += result;
	} while (bytes > result);

	result = bytes - result;

error_out:
	return result;
}

#endif/*CONFIG_SPI*/
