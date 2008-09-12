/*
 * Board specific setup info
 * Copy from redboot.
 */

#include <config.h>
#include <version.h>
#include <asm/arch/mx31-regs.h>
#include <asm/arch/mem.h>
#include <asm/arch/clocks.h>
#include "board.h"

#define SDRAM_FULL_PAGE_BIT     0x100
#define SDRAM_FULL_PAGE_MODE    0x37
#define SDRAM_BURST_MODE        0x33

#define SDRAM_COMPARE_CONST1    0x55555555
#define SDRAM_COMPARE_CONST2    0xAAAAAAAA

    /* Clock setup */
    .macro    init_clock
        ldr r0, =IPU_CTRL_BASE_ADDR
        ldr r1, =0x40
        str r1, [r0]

        ldr r0, =CCM_BASE_ADDR

        ldr r2, =0x374B0BE5
        ldr r3, =0x374B0BED
        ldr r4, =0x374B0B6D

        // Make sure to use CKIH
        ldr r1, [r0, #CLKCTL_CCMR]
        bic r1, r1, #0x8            // disable PLL first
        str r1, [r0, #CLKCTL_CCMR]
        str r2, [r0, #CLKCTL_CCMR]  // select CKIH (26MHz) as PLL ref clock
        ldr r1, =0x1000
    1:
        subs r1, r1, #0x1
        bne 1b
        str r3, [r0, #CLKCTL_CCMR]  // enable PLL
        str r4, [r0, #CLKCTL_CCMR]  // switch to PLL (SPLL for FIR)
#if 0
		ldr r1, =PDR0_528_133_66
		add r1, r1, #1
        str r1, [r0, #CLKCTL_PDR0]
		ldr r1, =MPCTL_PARAM_528
        str r1, [r0, #CLKCTL_MPCTL]
#else
		ldr r1, =PDR0_399_133_66
		str r1, [r0, #CLKCTL_PDR0]
		ldr r1, =MPCTL_PARAM_399
		str r1, [r0, #CLKCTL_MPCTL]
/*		str r1, [r0, #CLKCTL_SPCTL] <- linux uses this one for 399 */
#endif
        /* Set UPLL=240MHz, USB=60MHz */
        ldr r1, =0x49FCFE7F
        str r1, [r0, #CLKCTL_PDR1]
        ldr r1, =UPCTL_PARAM_240
        str r1, [r0, #CLKCTL_UPCTL]
        // default CLKO to 1/8 of the ARM core
        ldr r1, =0x000002C6
        str r1, [r0, #CLKCTL_COSR]
    .endm /* init_clock */

    /* CS0 async mode setup */
    .macro init_cs0_async
	/*configure RCR to 0xbfcf*/
        /* Async flash mode */
        ldr r0, =WEIM_CTRL_CS0
        ldr r1, =0x00000f00
        str r1, [r0, #CSCRU]
        ldr r1, =0x21000501
        str r1, [r0, #CSCRL]
        ldr r1, =0x00310000
        str r1, [r0, #CSCRA]
    .endm /* init_cs0_async */

        /* CPLD on CS4 setup */
    .macro init_cs4
        ldr r0, =WEIM_CTRL_CS4
        ldr r1, =0x0000D843
        str r1, [r0, #CSCRU]
        ldr r1, =0x22252521
        str r1, [r0, #CSCRL]
        ldr r1, =0x440A00
        str r1, [r0, #CSCRA]
    .endm /* init_cs4 */

	/*CF or smc1161 on cs5*/
    .macro init_cs5 
        ldr r0, =WEIM_CTRL_CS5
/*        ldr r1, =0x00000900*/
        ldr r1, =0x00001101 /*smsc9116 should set to this value*/
        str r1, [r0, #CSCRU]
		ldr r1, =0x21000501
        str r1, [r0, #CSCRL]
        ldr r1, =0x00210000
        str r1, [r0, #CSCRA]
    .endm /* init_cs5 */

    .macro setup_sdram, name, bus_width, mode, full_page
        /* It sets the "Z" flag in the CPSR at the end of the macro */
        ldr r0, =ESDCTL_BASE
		ldr r1, =0x00
		str r1, [r0] /* Disable CSD0 and CSD1*/
		str r1, [r0, #0x8]
        ldr r2, =SDRAM_BASE_ADDR
        ldr r1, =0x0075E73A
//ldr r1, =0x0075E73B
//ldr r1, =0x006aE72a
        str r1, [r0, #0x4]
        ldr r1, =0x2            // reset
        str r1, [r0, #0x10]
        ldr r1, SDRAM_PARAM1_\mode
        str r1, [r0, #0x10]
        // Hold for more than 200ns
        ldr r1, =0x10000
1:
        subs r1, r1, #0x1
        bne 1b

        ldr r1, =0x92100000/*Precharge command.*/
        str r1, [r0]
        ldr r1, =0x0
        ldr r12, SDRAM_PARAM2_\mode
        str r1, [r12]
        ldr r1, =0xA2100000/*Auto-refresh Command.*/
        str r1, [r0]
        ldr r1, =0x0
        str r1, [r2]
        ldr r1, =0xB2100000/*Load Mode Register command.*/
        str r1, [r0]

        ldr r1, =0x0
        .if \full_page
        strb r1, [r2, #SDRAM_FULL_PAGE_MODE]
        .else
        strb r1, [r2, #SDRAM_BURST_MODE]
        .endif
                
        ldr r1, =0xFF
        ldr r12, =0x81000000
        strb r1, [r12]
#if 1
        ldr r3, =0x82116080 /*Normal Read/Write.*/
#else
		/* enable lpddr power down time*/
        ldr r3, =0x8211649f /*Normal Read/Write.*/
#endif
        ldr r4, SDRAM_PARAM3_\mode
        add r3, r3, r4
        ldr r4, SDRAM_PARAM4_\bus_width
        add r3, r3, r4
        .if \full_page
        add r3, r3, #0x100   /* Force to full page mode */
        .endif

        str r3, [r0]
        ldr r1, =0xDEADBEEF
        str r1, [r2]
        /* Below only for DDR */
        ldr r1, [r0, #0x10]
        ands r1, r1, #0x4
        ldrne r1, =0x0000000C
        strne r1, [r0, #0x10]
        /* Testing if it is truly DDR */
        ldr r1, =SDRAM_COMPARE_CONST1
        ldr r0, =SDRAM_BASE_ADDR
        str r1, [r0]
        ldr r2, =SDRAM_COMPARE_CONST2
        str r2, [r0, #0x4]
        ldr r2, [r0]
        cmp r1, r2
    .endm

    /* To support 133MHz DDR */
    .macro  init_drive_strength
        /*
         * Disable maximum drive strength SDRAM/DDR lines by clearing DSE1 bits
         * in SW_PAD_CTL registers
         */

        // SDCLK
        ldr r1, =(IOMUXC_BASE_ADDR + 0x26C)
        ldr r0, [r1]
        bic r0, r0, #(1 << 12)
        str r0, [r1]
        
        // CAS
        ldr r1, =(IOMUXC_BASE_ADDR + 0x270)
        ldr r0, [r1]
        bic r0, r0, #(1 << 22)
        str r0, [r1]
        
        // RAS
        ldr r1, =(IOMUXC_BASE_ADDR + 0x274)
        ldr r0, [r1]
        bic r0, r0, #(1 << 2)
        str r0, [r1]
        
        // CS2 (CSD0)
        ldr r1, =(IOMUXC_BASE_ADDR + 0x27C)
        ldr r0, [r1]
        bic r0, r0, #(1 << 22)
        str r0, [r1]
        
        // DQM3
        ldr r1, =(IOMUXC_BASE_ADDR + 0x284)
        ldr r0, [r1]
        bic r0, r0, #(1 << 22)
        str r0, [r1]
        
        // DQM2, DQM1, DQM0, SD31-SD0, A25-A0, MA10 (0x288..0x2DC)
        ldr r1, =(IOMUXC_BASE_ADDR + 0x288)
        ldr r2, =22     // (0x2E0 - 0x288) / 4 = 22
pad_loop:
        ldr r0, [r1]
        bic r0, r0, #(1 << 22)
        bic r0, r0, #(1 << 12)
        bic r0, r0, #(1 << 2)
        str r0, [r1]
        add r1, r1, #4
        subs r2, r2, #0x1
        bne pad_loop
    .endm /* init_drive_strength */

SDRAM_PARAM1_DDR:	.word	0x4
SDRAM_PARAM1_SDR:	.word	0x0
SDRAM_PARAM2_DDR:	.word	0x80000F00
SDRAM_PARAM2_SDR:	.word	0x80000400
SDRAM_PARAM3_DDR:       .word   0x00100000
SDRAM_PARAM3_SDR:       .word   0x0
SDRAM_PARAM4_X32:       .word   0x00010000
SDRAM_PARAM4_X16:       .word   0x0

.globl lowlevel_init
lowlevel_init:
	init_cs4 /*CPLD on cs4*/
	init_cs5 /*CF card or smc1161 on CS5*/
	init_cs0_async

	/* If SDRAM has been setup, bypass clock/WEIM setup */
	cmp pc, #SDRAM_BASE_ADDR
	blo init_clock_start
	cmp pc, #(SDRAM_BASE_ADDR + SDRAM_SIZE)
	blo HWInitialise_skip_SDRAM_setup
	init_drive_strength
init_clock_start:
	init_clock
init_sdram_start:
	/* Assuming DDR memory first */
	setup_sdram ddr X32 DDR 0
	beq HWInitialise_skip_SDRAM_setup
	setup_sdram ddr X16 DDR 0
	beq HWInitialise_skip_SDRAM_setup
	setup_sdram sdr X32 SDR 0
	beq HWInitialise_skip_SDRAM_setup
	setup_sdram sdr X16 SDR 0
	beq HWInitialise_skip_SDRAM_setup

	/* Reach hear means memory setup problem. Try to 
	 * increase the HCLK divider */
	ldr r0, =CCM_BASE_ADDR
	ldr r1, [r0, #CLKCTL_PDR0]
	and r2, r1, #0x38
	cmp r2, #0x38
	beq loop_forever
	add r1, r1, #0x8
	str r1, [r0, #CLKCTL_PDR0]
	b init_sdram_start

loop_forever:
	b loop_forever  /* shouldn't get here */

	HWInitialise_skip_SDRAM_setup:
	mov	pc,	lr
