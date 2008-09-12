/*
 * Copyright 2004-2006 Freescale Semiconductor, Inc. All Rights Reserved.
 */

/*
 * The code contained herein is licensed under the GNU General Public
 * License. You may obtain a copy of the GNU General Public License
 * Version 2 or later at the following locations:
 *
 * http://www.opensource.org/licenses/gpl-license.html
 * http://www.gnu.org/copyleft/gpl.html
 */

#include <common.h>
#include <exports.h>
#include <asm/arch/mx31-regs.h>
#include <asm/arch/mx31_pins.h>
#include <asm/io.h>
#include <asm/arch/iomux.h>

/*!
 * 4 control fields per MUX register
 */
#define MUX_CTL_FIELDS          4

/*!
 * 3 control fields per PAD register
 */
#define PAD_CTL_FIELDS          3

/*!
 * Maximum number of MUX pins
 * Number of pins = (highest iomux reg - lowest iomux reg + 1) * (4 pins/reg)
 */
#define MUX_PIN_NUM_MAX \
        (((u32 *)IOMUXSW_MUX_END - (u32 *)IOMUXSW_MUX_CTL + 1) * MUX_CTL_FIELDS)

/*!
 * Number of pad controls =
 *               (highest pad ctl reg - lowest pad ctl reg + 1) * (3 pins/reg)
 */
#define PAD_CTL_NUM_MAX \
        (((u32 *)IOMUXSW_PAD_END - (u32 *)IOMUXSW_PAD_CTL + 1) * PAD_CTL_FIELDS)

#define PIN_TO_IOMUX_INDEX(pin) ((pin >> MUX_I) & ((1 << (MUX_F - MUX_I)) - 1))
#define PIN_TO_IOMUX_FIELD(pin) ((pin >> MUX_F) & ((1 << (PAD_I - MUX_F)) - 1))

/*!
 * 8 bits for each MUX control field
 */
#define MUX_CTL_BIT_LEN         8

/*!
 * 10 bits for each PAD control field
 */
#define MUX_PAD_BIT_LEN         10


/*!
 * IOMUX register (base) addresses
 */
enum iomux_reg_addr {
	IOMUXGPR = IO_ADDRESS(IOMUXC_BASE_ADDR) + 0x008,	/*!< General purpose */
	IOMUXSW_MUX_CTL = IO_ADDRESS(IOMUXC_BASE_ADDR) + 0x00C,	/*!< MUX control */
	IOMUXSW_MUX_END = IO_ADDRESS(IOMUXC_BASE_ADDR) + 0x150,	/*!< last MUX control register */
	IOMUXSW_PAD_CTL = IO_ADDRESS(IOMUXC_BASE_ADDR) + 0x154,	/*!< Pad control */
	IOMUXSW_PAD_END = IO_ADDRESS(IOMUXC_BASE_ADDR) + 0x308,	/*!< last Pad control register */
	IOMUXINT_OBS1 = IO_ADDRESS(IOMUXC_BASE_ADDR) + 0x000,	/*!< Observe interrupts 1 */
	IOMUXINT_OBS2 = IO_ADDRESS(IOMUXC_BASE_ADDR) + 0x004,	/*!< Observe interrupts 2 */
};

/* len - mask bit length; fld - mask bit field. Example, to have the mask:
 * 0xFF000000, use GET_FIELD_MASK(8, 3). Translate in plain language:
 * "set the 3rd (0-based) 8-bit-long field to all 1's */
#define GET_FIELD_MASK(len, fld)    (((1 << len) - 1) << (len * fld))

/*!
 * This function is used to configure a pin through the IOMUX module.
 * FIXED ME: for backward compatible. Will be static function!
 * @param  pin		a pin number as defined in \b #iomux_pin_name_t
 * @param  out		an output function as defined in \b #iomux_pin_ocfg_t
 * @param  in		an input function as defined in \b #iomux_pin_icfg_t
 *
 * @return 		0 if successful; Non-zero otherwise
 */
int iomux_config_mux(iomux_pin_name_t pin, iomux_pin_ocfg_t out,
		     iomux_pin_icfg_t in)
{
	u32 reg, l;
	u32 mux_index = PIN_TO_IOMUX_INDEX(pin);
	u32 mux_field = PIN_TO_IOMUX_FIELD(pin);
	u32 mux_mask = GET_FIELD_MASK(MUX_CTL_BIT_LEN, mux_field);


	reg = IOMUXSW_MUX_CTL + (mux_index * 4);
	l = readl(reg);
	l = (l & (~mux_mask)) |
	    (((out << 4) | in) << (mux_field * MUX_CTL_BIT_LEN));
	writel(l, reg);
	return 0;
}

int iomux_mux_info(iomux_pin_name_t pin, iomux_pin_ocfg_t *out, 
		iomux_pin_icfg_t *in)
{
	u32 reg, l;
	u32 mux_index = PIN_TO_IOMUX_INDEX(pin);
	u32 mux_field = PIN_TO_IOMUX_FIELD(pin);
	u32 mux_mask = GET_FIELD_MASK(MUX_CTL_BIT_LEN, mux_field);


	reg = IOMUXSW_MUX_CTL + (mux_index * 4);
	l = readl(reg);
	l &= mux_mask;
	l >>= (mux_field * MUX_CTL_BIT_LEN);
	*out = (l >> 4);
	*in = (l & 0xF);
	return 0;
}

void mxc_iomux_set_pad(iomux_pin_name_t pin, u32 config)
{
	u32 reg, l;
	u32 pad_index = (pin >> PAD_I) & ((1 << (PAD_F - PAD_I)) - 1);
	u32 pad_field = (pin >> PAD_F) & ((1 << (MUX_IO_I - PAD_F)) - 1);
	u32 pad_mask = GET_FIELD_MASK(MUX_PAD_BIT_LEN, pad_field);

	reg = IOMUXSW_PAD_CTL + (pad_index * 4);
	l = __raw_readl(reg);
	l = (l & (~pad_mask)) | (config << (pad_field * MUX_PAD_BIT_LEN));
	writel(l, reg);
}

#define GPIO_NUM_PIN            32
#define IOMUX_TO_GPIO(pin)  ((((unsigned int)pin >> MUX_IO_P) * GPIO_NUM_PIN) + ((pin >> MUX_IO_I) & ((1 << (MUX_IO_P - MUX_IO_I)) -1)))
#define GPIO_TO_PORT(n)     (n / GPIO_NUM_PIN)
#define GPIO_TO_INDEX(n)    (n % GPIO_NUM_PIN)

#define GPIO_PORT_NUM 3

enum gpio_reg {
	GPIO_DR = 0x00,
	GPIO_GDIR = 0x04,
	GPIO_PSR = 0x08,
	GPIO_ICR1 = 0x0C,
	GPIO_ICR2 = 0x10,
	GPIO_IMR = 0x14,
	GPIO_ISR = 0x18,
};

struct gpio_port {
	u32 num;        /*!< gpio port number */
	u32 base;       /*!< gpio port base VA */
};

static struct gpio_port gpio_port[GPIO_PORT_NUM] = {
	{
		.num = 0,
		.base = IO_ADDRESS(GPIO1_BASE_ADDR),
	},
	{
		.num = 1,
		.base = IO_ADDRESS(GPIO2_BASE_ADDR),
	},
	{
		.num = 2,
		.base = IO_ADDRESS(GPIO3_BASE_ADDR),
	},
};


static inline struct gpio_port *get_gpio_port(u32 gpio)
{
	return &gpio_port[GPIO_TO_PORT(gpio)];
}


static void _set_gpio_direction(struct gpio_port *port, u32 index, int is_input)
{
	u32 reg = port->base + GPIO_GDIR;
	u32 l;

	l = __raw_readl(reg);
	if (is_input)
		l &= ~(1 << index);
	else
		l |= 1 << index;
	writel(l, reg);
}

static int _get_gpio_direction(struct gpio_port *port, u32 index)
{
	u32 reg = port->base + GPIO_GDIR;
	u32 l;

	l = __raw_readl(reg);
	l &= (1 << index);
	return (l?0:1);
}

void mxc_set_gpio_direction(iomux_pin_name_t pin, int is_input)
{
	struct gpio_port *port;
	u32 gpio = IOMUX_TO_GPIO(pin);

	port = get_gpio_port(gpio);
	_set_gpio_direction(port, GPIO_TO_INDEX(gpio), is_input);
}

int mxc_get_gpio_direction(iomux_pin_name_t pin)
{
	struct gpio_port *port;
	u32 gpio = IOMUX_TO_GPIO(pin);

	port = get_gpio_port(gpio);
	return _get_gpio_direction(port, GPIO_TO_INDEX(gpio));
}

static void _set_gpio_dataout(struct gpio_port *port, u32 index, u32 data)
{
	u32 reg = port->base + GPIO_DR;
	u32 l = 0;

	l = (__raw_readl(reg) & (~(1 << index))) | (data << index);
	__raw_writel(l, reg);
}

void mxc_set_gpio_dataout(iomux_pin_name_t pin, u32 data)
{
	struct gpio_port *port;
	u32 gpio = IOMUX_TO_GPIO(pin);

	port = get_gpio_port(gpio);
	_set_gpio_dataout(port, GPIO_TO_INDEX(gpio), (data == 0) ? 0 : 1);
}

int mxc_get_gpio_dataout(iomux_pin_name_t pin)
{
	struct gpio_port *port;
	u32 gpio = IOMUX_TO_GPIO(pin);
	u32 reg, l;

	port = get_gpio_port(gpio);

	reg = port->base + GPIO_DR;
	l = __raw_readl(reg) & (1 << GPIO_TO_INDEX(gpio));
	return (l?1:0);
}

int mxc_get_gpio_datain(iomux_pin_name_t pin)
{
	struct gpio_port *port;
	u32 gpio = IOMUX_TO_GPIO(pin);

	port = get_gpio_port(gpio);

	return (__raw_readl(port->base + GPIO_DR) >> GPIO_TO_INDEX(gpio)) & 1;
}


void gpio_pcmcia_active(void)
{
//	__raw_writel(0x082689a2, 0x43fac244);
    iomux_config_mux(MX31_PIN_SDBA0, OUTPUTCONFIG_FUNC, INPUTCONFIG_FUNC);
    iomux_config_mux(MX31_PIN_SDBA1, OUTPUTCONFIG_FUNC, INPUTCONFIG_FUNC);

    iomux_config_mux(MX31_PIN_LBA, OUTPUTCONFIG_FUNC, INPUTCONFIG_FUNC);
    iomux_config_mux(MX31_PIN_RW, OUTPUTCONFIG_FUNC, INPUTCONFIG_FUNC);
    iomux_config_mux(MX31_PIN_EB0, OUTPUTCONFIG_FUNC, INPUTCONFIG_FUNC);
    iomux_config_mux(MX31_PIN_EB1, OUTPUTCONFIG_FUNC, INPUTCONFIG_FUNC);
    iomux_config_mux(MX31_PIN_OE, OUTPUTCONFIG_FUNC, INPUTCONFIG_FUNC);

    iomux_config_mux(MX31_PIN_IOIS16, OUTPUTCONFIG_FUNC, INPUTCONFIG_FUNC);
    iomux_config_mux(MX31_PIN_PC_BVD1, OUTPUTCONFIG_FUNC, INPUTCONFIG_FUNC);
    iomux_config_mux(MX31_PIN_PC_BVD2, OUTPUTCONFIG_FUNC, INPUTCONFIG_FUNC);
    iomux_config_mux(MX31_PIN_PC_CD1_B, OUTPUTCONFIG_FUNC, INPUTCONFIG_FUNC);
    iomux_config_mux(MX31_PIN_PC_CD2_B, OUTPUTCONFIG_FUNC, INPUTCONFIG_FUNC);
    iomux_config_mux(MX31_PIN_PC_POE, OUTPUTCONFIG_FUNC, INPUTCONFIG_FUNC);
    iomux_config_mux(MX31_PIN_PC_PWRON, OUTPUTCONFIG_FUNC, INPUTCONFIG_FUNC);
    iomux_config_mux(MX31_PIN_PC_READY, OUTPUTCONFIG_FUNC, INPUTCONFIG_FUNC);
    iomux_config_mux(MX31_PIN_PC_RST, OUTPUTCONFIG_FUNC, INPUTCONFIG_FUNC);
    iomux_config_mux(MX31_PIN_PC_RW_B, OUTPUTCONFIG_FUNC, INPUTCONFIG_FUNC);
    iomux_config_mux(MX31_PIN_PC_VS1, OUTPUTCONFIG_FUNC, INPUTCONFIG_FUNC);
    iomux_config_mux(MX31_PIN_PC_VS2, OUTPUTCONFIG_FUNC, INPUTCONFIG_FUNC);
    iomux_config_mux(MX31_PIN_PC_WAIT_B, OUTPUTCONFIG_FUNC, INPUTCONFIG_FUNC);

//	mxc_iomux_set_pad(MX31_PIN_PC_CD1_B, PAD_CTL_PKE_NONE);
}

void mxc_iomux_set_gpr(iomux_gp_func_t gp, int en)
{
	u32 l;

	l = __raw_readl(IOMUXGPR);
	if (en) {
		l |= gp;
	} else {
		l &= ~gp;
	}
	__raw_writel(l, IOMUXGPR);
}

