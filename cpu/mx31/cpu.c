/*
 * (C) Copyright 2004 Texas Insturments
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * (C) Copyright 2002
 * Gary Jennejohn, DENX Software Engineering, <gj@denx.de>
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

/*
 * CPU specific code
 */

#include <common.h>
#include <command.h>
#include <asm/io.h>

extern void serial_stop (void);
int setup_clock(u32 core, u32 ahb, u32 ipg);

unsigned int g_clock_src;
#define ERR_WRONG_CLK   -1
#define ERR_NO_MFI      -2
#define ERR_NO_MFN      -3
#define ERR_NO_PD       -4
#define ERR_NO_PRESC    -5
#define ERR_NO_AHB_DIV  -6

/* See also ARM Ref. Man. */
#define C1_MMU		(1<<0)		/* mmu off/on */
#define C1_ALIGN	(1<<1)		/* alignment faults off/on */
#define C1_DC		(1<<2)		/* dcache off/on */
#define C1_WB		(1<<3)		/* merging write buffer on/off */
#define C1_BIG_ENDIAN	(1<<7)	/* big endian off/on */
#define C1_SYS_PROT	(1<<8)		/* system protection */
#define C1_ROM_PROT	(1<<9)		/* ROM protection */
#define C1_IC		(1<<12)		/* icache off/on */
#define C1_HIGH_VECTORS	(1<<13)	/* location of vectors: low/high addresses */
#define RESERVED_1	(0xf << 3)	/* must be 111b for R/W */

extern int pmic_read_reg(int num_reg, unsigned int *reg_value);
extern int pmic_write_reg(int num_reg, unsigned int *reg_value);
extern int pmic_init(void);
extern int interrupt_init (void);

int cpu_init (void)
{
	/*
	 * setup up stacks if necessary
	 */
	unsigned int val;
#ifdef CONFIG_USE_IRQ
	DECLARE_GLOBAL_DATA_PTR;

	IRQ_STACK_START = _armboot_start - CFG_MALLOC_LEN - CFG_GBL_DATA_SIZE - 4;
	FIQ_STACK_START = IRQ_STACK_START - CONFIG_STACKSIZE_IRQ;
#endif
	AVIC_NIMASK = 0x1f; /*Mask all interrupts.*/

	AVIC_INTTYPEH = 0x00;
	AVIC_INTTYPEL = 0x00; /*Make all interrupts do IRQ and not FIQ.*/

	val = readl(CCM_BASE_ADDR + CLKCTL_CCMR);
	if ((val & 0x6) == 0x4) {
		g_clock_src = FREQ_26MHZ;
	} else if ((val & 0x6) == 0x2) {
		g_clock_src = FREQ_32768HZ;
	}
	interrupt_init();
#if defined(CONFIG_PMIC)
	pmic_init();
#endif
#ifdef CFG_CPU_SPEED
	/* Set 13783 switcher output voltage.*/
	/*default value is 0x18: 0xc30c*/
	pmic_read_reg(0x18, &val); /* Switcher 0 */
#if 0
	val  &= (~0xFC);

	if(CFG_CPU_SPEED > 600)
		val |= 0x18;
	else if(CFG_CPU_SPEED > 500)
		val |= 0x14;
	else if(CFG_CPU_SPEED > 400)
		val |= 0x10;
#else
	val  &= (~0xfff);

	/* using same voltage for both DVSx states */
	if (CFG_CPU_SPEED > 400)
		val |= 27 << 6 | 27; // 1.575
	else
		val |= 22 << 6 | 22; // 1.45
#endif
	pmic_write_reg(0x18, &val); /* Switcher 0 */

	setup_clock(CFG_CPU_SPEED, 0, 0);
#endif
	return 0;
}

static inline void l2cache_disable(void )
{
	__REG(L2CC_BASE_ADDR+0x100) = __REG(L2CC_BASE_ADDR+0x100) & ~0x1;
}

int cleanup_before_linux (void)
{
	/*
	 * this function is called just before we call linux
	 * it prepares the processor for linux
	 *
	 * we turn off caches etc ...
	 */
	disable_interrupts ();
	l2cache_disable();

	return(0);
}

void do_reset (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	disable_interrupts ();
	reset_cpu(0);
}

/*!
 * This is to calculate various parameters based on reference clock and 
 * targeted clock based on the equation:
 *      t_clk = 2*ref_freq*(mfi + mfn/(mfd+1))/(pd+1)
 * This calculation is based on a fixed MFD value for simplicity.
 *
 * @param ref       reference clock freq
 * @param target    targeted clock in HZ
 * @param p_pd      calculated pd value (pd value from register + 1) upon return
 * @param p_mfi     calculated actual mfi value upon return
 * @param p_mfn     calculated actual mfn value upon return
 * @param p_mfd     fixed mfd value (mfd value from register + 1) upon return
 *
 * @return          0 if successful; non-zero otherwise.
 */
int calc_pll_params(u32 ref, u32 target, u32 *p_pd, 
                    u32 *p_mfi, u32 *p_mfn, u32 *p_mfd)
{
    u64 pd, mfi, mfn, n_target = (u64)target, n_ref = (u64)ref;

    // Make sure targeted freq is in the valid range. Otherwise the 
    // following calculation might be wrong!!!
    if (target < PLL_FREQ_MIN || target > PLL_FREQ_MAX) {
        return ERR_WRONG_CLK;
    }
    // Use n_target and n_ref to avoid overflow
    for (pd = 1; pd <= PLL_PD_MAX; pd++) {
        mfi = (n_target * pd) / (2 * n_ref);
        if (mfi > PLL_MFI_MAX) {
            return ERR_NO_MFI;
        } else if (mfi < 5) {
            continue;
        }
        break;
    }
    // Now got pd and mfi already
    mfn = (((n_target * pd) / 2 - n_ref * mfi) * PLL_MFD_FIXED) / n_ref;
    // Check mfn within limit and mfn < denominator
    if (mfn > PLL_MFN_MAX || mfn >= PLL_MFD_FIXED) {
        return ERR_NO_MFN;
    }

    if (pd > PLL_PD_MAX) {
        return ERR_NO_PD;
    }
    *p_pd = (u32)pd;
    *p_mfi = (u32)mfi;
    *p_mfn = (u32)mfn;
    *p_mfd = PLL_MFD_FIXED;
    return 0;
}

/*!
 * This function assumes the expected core clock has to be changed by
 * modifying the PLL. This is NOT true always but for most of the times,
 * it is. So it assumes the PLL output freq is the same as the expected 
 * core clock (presc=1) unless the core clock is less than PLL_FREQ_MIN.
 * In the latter case, it will try to increase the presc value until 
 * (presc*core_clk) is greater than PLL_FREQ_MIN. It then makes call to
 * calc_pll_params() and obtains the values of PD, MFI,MFN, MFD based
 * on the targeted PLL and reference input clock to the PLL. Lastly, 
 * it sets the register based on these values along with the dividers.
 * Note 1) There is no value checking for the passed-in divider values
 *         so the caller has to make sure those values are sensible.
 *      2) Also adjust the NFC divider such that the NFC clock doesn't
 *         exceed NFC_CLK_MAX.
 *      3) IPU HSP clock is independent of AHB clock. Even it can go up to
 *         177MHz for higher voltage, this function fixes the max to 133MHz.
 *      4) This function should not have allowed printf() calls since
 *         the serial driver has been stoped. But leave then here to allow
 *         easy debugging by NOT calling the serial_stop().
 * 
 * @param ref       pll input reference clock (32KHz or 26MHz)
 * @param core_clk  core clock in Hz
 * @param ahb_div   ahb divider to divide the core clock to get ahb clock 
 *                  (ahb_div - 1) needs to be set in the register
 * @param ipg_div   ipg divider to divide the ahb clock to get ipg clock
 *                  (ipg_div - 1) needs to be set in the register
 # @return          0 if successful; non-zero otherwise
 */
int configure_clock(u32 ref, u32 core_clk, u32 ahb_div, u32 ipg_div)
{
    u32 pll, presc = 1, pd, mfi, mfn, mfd, brmo = 0, mpctl0, hsp_div;
    u32 pdr0, nfc_div;
    int ret, i;

    // assume pll default to core clock first
    pll = core_clk;
    // when core_clk >= PLL_FREQ_MIN, the presc can be 1.
    // Otherwise, need to calculate presc value below and adjust the targeted pll
    if (core_clk < PLL_FREQ_MIN) {
        for (presc = 1; presc <= PRESC_MAX; presc++) {
            if ((core_clk * presc) > PLL_FREQ_MIN) {
                break;
            }
        }
        if (presc == (PRESC_MAX + 1)) {
            return ERR_NO_PRESC;
        }
        pll = core_clk * presc;
    }
    // get hsp_div
    for (hsp_div = 1; hsp_div <= HSP_PODF_MAX; hsp_div++) {
        if ((pll / hsp_div) <= HSP_CLK_MAX) {
            break;
        }
    }
    if (hsp_div == (HSP_PODF_MAX + 1)) {
        return ERR_NO_PRESC;
    }
    
    // get nfc_div - make sure optimal NFC clock but less than NFC_CLK_MAX
    for (nfc_div = 1; nfc_div <= NFC_PODF_MAX; nfc_div++) {
        if ((pll / (ahb_div * nfc_div)) <= NFC_CLK_MAX) {
            break;
        }
    }

    // pll is now the targeted pll output. Use it along with ref input clock
    // to get pd, mfi, mfn, mfd
    if ((ret = calc_pll_params(ref, pll, &pd, &mfi, &mfn, &mfd)) != 0) {
        return ret;
    }
#ifdef CMD_CLOCK_DEBUG
    printf("ref=%d, pll=%d, pd=%d, mfi=%d,mfn=%d, mfd=%d\n", 
                ref, pll, pd, mfi, mfn, mfd);
#endif

    // blindly increase divider first to avoid too fast ahbclk and ipgclk
    // in case the core clock increases too much
    pdr0 = readl(CCM_BASE_ADDR + CLKCTL_PDR0);
    pdr0 &= ~0x000000FF;
    // increase the dividers. should work even when core clock is 832 (26*2*16)MHz
    // which is unlikely true.
    pdr0 |= (1 << 6) | (6 << 3) | (0 << 0);
    writel(pdr0, CCM_BASE_ADDR + CLKCTL_PDR0);
    // calculate new pdr0
    pdr0 &= ~0x00003FFF;
    pdr0 |= ((hsp_div - 1) << 11) | ((nfc_div - 1) << 8) | ((ipg_div - 1) << 6) | 
            ((ahb_div - 1) << 3) | ((presc - 1) << 0);

    // update PLL register
    if ((mfd >= (10 * mfn)) || ((10 * mfn) >= (9 * mfd)))
        brmo = 1;

    mpctl0 = readl(CCM_BASE_ADDR + CLKCTL_MPCTL);
    mpctl0 = (mpctl0 & 0x4000C000)  |
             (brmo << 31)           |
             ((pd - 1) << 26)       |
             ((mfd - 1) << 16)      |
             (mfi << 10)            |
             mfn;
    writel(mpctl0, CCM_BASE_ADDR + CLKCTL_MPCTL);
    writel(pdr0, CCM_BASE_ADDR + CLKCTL_PDR0);
    // add some delay for new values to take effect
    for (i = 0; i < 10000; i++);

    return 0;
}

/*!
 * This function returns the PLL output value in Hz based on pll.
 */
u32 pll_clock(enum plls pll)
{
    u64 mfi, mfn, mfd, pdf, ref_clk, pll_out, sign;
    u64 reg = readl(pll);

    pdf = (reg >> 26) & 0xF;
    mfd = (reg >> 16) & 0x3FF;
    mfi = (reg >> 10) & 0xF;
    mfi = (mfi <= 5) ? 5: mfi;
    mfn = reg & 0x3FF;
    sign = (mfn < 512) ? 0: 1;
    mfn = (mfn < 512) ? mfn: (1024 - mfn);

    ref_clk = g_clock_src;

    if (sign == 0) {
        pll_out = (2 * ref_clk * mfi + ((2 * ref_clk * mfn) / (mfd + 1))) /
                  (pdf + 1);
    } else {
        pll_out = (2 * ref_clk * mfi - ((2 * ref_clk * mfn) / (mfd + 1))) /
                  (pdf + 1);
    }

    return (u32)pll_out;
}

/*!
 * This function returns the main clock value in Hz.
 */
u32 get_main_clock(enum main_clocks clk)
{
    u32 mcu_podf, max_pdf, ipg_pdf, nfc_pdf, hsp_podf, clk_sel;
    u32 pll, ret_val = 0, usb_prdf, usb_podf, pdf;
    u32 reg = readl(CCM_BASE_ADDR + CLKCTL_PDR0);
    u32 reg1 = readl(CCM_BASE_ADDR + CLKCTL_PDR1);
    u32 ccmr = readl(CCM_BASE_ADDR + CLKCTL_CCMR);
    u32 mpdr0 = readl(CCM_BASE_ADDR + CLKCTL_PDR0);

    switch (clk) {
    case CPU_CLK:
        mcu_podf = reg & 0x7;
        pll = pll_clock(MCU_PLL);
        ret_val = pll / (mcu_podf + 1);
        break;
    case AHB_CLK:
        max_pdf = (reg >> 3) & 0x7;
        pll = pll_clock(MCU_PLL);
        ret_val = pll / (max_pdf + 1);
        break;
    case HSP_CLK:
        hsp_podf = (reg >> 11) & 0x7;
        pll = pll_clock(MCU_PLL);
        ret_val = pll / (hsp_podf + 1);
        break;
    case MBX_CLK:
        max_pdf = (reg >> 3) & 0x7;
        pll = pll_clock(MCU_PLL);
        ret_val = (pll / (max_pdf + 1)) / 2;
        break;
    case IPG_CLK:
        max_pdf = (reg >> 3) & 0x7;
        ipg_pdf = (reg >> 6) & 0x3;
        pll = pll_clock(MCU_PLL);
        ret_val = pll / ((max_pdf + 1) * (ipg_pdf + 1));
        break;
    case IPG_PER_CLK:
        clk_sel = ccmr & (1 << 24);
        pdf = (mpdr0 >> 16) & 0x1F;
        if (clk_sel != 0) {
            // get the ipg_clk
            max_pdf = (reg >> 3) & 0x7;
            ipg_pdf = (reg >> 6) & 0x3;
            pll = pll_clock(MCU_PLL);
            ret_val = pll / ((max_pdf + 1) * (ipg_pdf + 1));
        } else {
            ret_val = pll_clock(USB_PLL) / (pdf + 1);
        }
        break;
    case NFC_CLK:
        nfc_pdf = (reg >> 8) & 0x7;
        max_pdf = (reg >> 3) & 0x7;
        pll = pll_clock(MCU_PLL);
        /* AHB/nfc_pdf */
        ret_val = pll / ((max_pdf + 1) * (nfc_pdf + 1));
        break;
    case USB_CLK:
        usb_prdf = reg1 >> 30;
        usb_podf = (reg1 >> 27) & 0x7;
        pll = pll_clock(USB_PLL);
        ret_val = pll / ((usb_prdf + 1) * (usb_podf + 1));
        break;
    default:
        break;
    }

    return ret_val;
}

/*!
 * This function returns the peripheral clock value in Hz.
 */
u32 get_peri_clock(enum peri_clocks clk)
{
    u32 ret_val = 0, pdf, pre_pdf, clk_sel;
    u32 ccmr = readl(CCM_BASE_ADDR + CLKCTL_CCMR);
    u32 mpdr0 = readl(CCM_BASE_ADDR + CLKCTL_PDR0);
    u32 mpdr1 = readl(CCM_BASE_ADDR + CLKCTL_PDR1);
    u32 mpdr2 = readl(CCM_BASE_ADDR + CLKCTL_PDR2);

    switch (clk) {
    case UART1_BAUD:
    case UART2_BAUD:
    case UART3_BAUD:
    case UART4_BAUD:
    case UART5_BAUD:
    case SIM_BAUD:
        clk_sel = ccmr & (1 << 24);
        pdf = (mpdr0 >> 16) & 0x1F;
        ret_val = (clk_sel != 0) ? get_main_clock(IPG_CLK) : 
                  pll_clock(USB_PLL) / (pdf + 1);
        break;
    case SSI1_BAUD:
        pre_pdf = (mpdr1 >> 6) & 0x7;
        pdf = mpdr1 & 0x3F;
        clk_sel = (ccmr >> 18) & 3;
        if (clk_sel == 0) {
            ret_val = pll_clock(MCU_PLL) / ((pre_pdf + 1) * (pdf + 1));
        } else if (clk_sel == 0x1) {
            ret_val = pll_clock(USB_PLL) / ((pre_pdf + 1) * (pdf + 1));
        } else if (clk_sel == 0x2) {
            ret_val = pll_clock(SER_PLL) / ((pre_pdf + 1) * (pdf + 1));
        } else {
            ret_val = 0;
        }
        break;
    case SSI2_BAUD:
        pre_pdf = (mpdr1 >> 15) & 0x7;
        pdf = (mpdr1 >> 9) & 0x3F;
        clk_sel = (ccmr >> 21) & 3;
        if (clk_sel == 0) {
            ret_val = pll_clock(MCU_PLL) / ((pre_pdf + 1) * (pdf + 1));
        } else if (clk_sel == 0x1) {
            ret_val = pll_clock(USB_PLL) / ((pre_pdf + 1) * (pdf + 1));
        } else if (clk_sel == 0x2) {
            ret_val = pll_clock(SER_PLL) / ((pre_pdf + 1) * (pdf + 1));
        } else {
            ret_val = 0;
        }
        break;
    case CSI_BAUD:
        clk_sel = ccmr & (1 << 25);
        pdf = (mpdr0 >> 23) & 0x1FF;
        ret_val = (clk_sel != 0) ? (pll_clock(SER_PLL) / (pdf + 1)) : 
                  (pll_clock(USB_PLL) / (pdf + 1));
        break;
    case FIRI_BAUD:
        pre_pdf = (mpdr1 >> 24) & 0x7;
        pdf = (mpdr1 >> 18) & 0x3F;
        clk_sel = (ccmr >> 11) & 3;
        if (clk_sel == 0) {
            ret_val = pll_clock(MCU_PLL) / ((pre_pdf + 1) * (pdf + 1));
        } else if (clk_sel == 0x1) {
            ret_val = pll_clock(USB_PLL) / ((pre_pdf + 1) * (pdf + 1));
        } else if (clk_sel == 0x2) {
            ret_val = pll_clock(SER_PLL) / ((pre_pdf + 1) * (pdf + 1));
        } else {
            ret_val = 0;
        }
        break;
    case MSTICK1_CLK:
        pdf = mpdr2 & 0x3F;
        ret_val = pll_clock(USB_PLL) / (pdf + 1);
        break;
    case MSTICK2_CLK:
        pdf = (mpdr2 >> 7) & 0x3F;
        ret_val = pll_clock(USB_PLL) / (pdf + 1);
        break;
    default:
        break;
    }

    return ret_val;
}

int setup_clock(u32 core, u32 ahb, u32 ipg)
{
    u32 core_clk, ipg_div, ahb_div, ahb_clk, ipg_clk;
    int ret;

    core_clk = core * SZ_DEC_1M;
    ahb_div = ahb;  // actual register field + 1
    ipg_div = ipg;  // actual register field + 1

    if (core_clk < (PLL_FREQ_MIN / PRESC_MAX) || core_clk > PLL_FREQ_MAX) {
        return -1;
    }

    // find the ahb divider  
    if (ahb_div > AHB_DIV_MAX) {
        return -1;
    }
    if (ahb_div == 0) {
        // no HCLK divider specified
        for (ahb_div = 1; ; ahb_div++) {
            if ((core_clk / ahb_div) <= AHB_CLK_MAX) {
                break;
            }
        }
    }
    if (ahb_div > AHB_DIV_MAX || (core_clk / ahb_div) > AHB_CLK_MAX) {
        return -1;
    }

    // find the ipg divider
    ahb_clk = core_clk / ahb_div;
    if (ipg_div > IPG_DIV_MAX) {
        return -1;
    }
    if (ipg_div == 0) {
        ipg_div++;          // At least =1
        if (ahb_clk > IPG_CLK_MAX)
            ipg_div++;      // Make it =2
    }
    if (ipg_div > IPG_DIV_MAX || (ahb_clk / ipg_div) > IPG_CLK_MAX) {
        return -1;
    }
    ipg_clk = ahb_clk / ipg_div;


    // stop the serial to be ready to adjust the clock
    udelay(100000);
	serial_stop ();
    // adjust the clock
    ret = configure_clock(PLL_REF_CLK, core_clk, ahb_div, ipg_div);
     // restart the serial driver
    serial_init();
    udelay(100000);
	return 0;
}

int raise(int sig)
{
	return 0;
}

