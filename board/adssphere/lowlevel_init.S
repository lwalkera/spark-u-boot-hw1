/*
 * Memory Setup stuff - taken from Cirrus Logic EP9312 spec Revision 0.14
 *
 * Copyright (C) 2002 2003  Adam Bezanson, Network Audio Technologies, Inc.
 *	     (bezanson@netaudiotech.com)
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
 */



#include <config.h>
#include <version.h>
#include <asm/arch/ep93xx.h>

.globl lowlevel_init
lowlevel_init:

	/* Make sure caches are off */
	ldr	r1,=0
	mcr	p15,0,r1,c1,c0,0
	nop
	nop
	nop
	nop

	/* Go back to running from the external clock. */
	ldr	r1, =EP9312_CLKSET1
	mov	r0, #0
	str	r0, [r1]
	nop
	nop
	nop
	nop
	nop
	    
	/* Turn on LEDs */
	ldr	r1, =EP9312_LED_DATA
	ldr	r0, [r1]

	 /* Don't use RED LED!	see AN258 - keep it low */
	orr	r0, r0, #(EP9312_LED_GREEN_ON)
	bic	r0, r0, #(EP9312_LED_RED_ON)
	str	r0, [r1]

	/* setup the static CS memory timings */
	ldr	r1, =SMC_BASE
#ifdef CFG_SMCBCR0_VAL
	ldr	r0, =CFG_SMCBCR0_VAL
	str	r0, [r1, #0x00]
#endif		

#ifdef CFG_SMCBCR1_VAL
	ldr	r0, =CFG_SMCBCR1_VAL
	str	r0, [r1, #0x04]
#endif		

#ifdef CFG_SMCBCR2_VAL
	ldr	r0, =CFG_SMCBCR2_VAL
	str	r0, [r1, #0x08]
#endif		

#ifdef CFG_SMCBCR3_VAL
	ldr	r0, =CFG_SMCBCR3_VAL
	str	r0, [r1, #0x0C]
#endif		

#ifdef CFG_SMCBCR6_VAL
	ldr	r0, =CFG_SMCBCR6_VAL
	str	r0, [r1, #0x18]
#endif		

#ifdef CFG_SMCBCR7_VAL
	ldr	r0, =CFG_SMCBCR7_VAL
	str	r0, [r1, #0x1c]
#endif		

	/* Turn off auto-precharge. */
	ldr	r1, =SDRAM_BASE
	ldr	r0, [r1,#CFG3]
	bic	r0, r0, #0x01000000
	str	r0, [r1,#CFG3]

	/* Turn off the Green LED and leave the Red LED on. */
	ldr	r1, =EP9312_LED_DATA
	ldr	r0, [r1]
	bic	r0, r0, #(EP9312_LED_GREEN_ON)
	str	r0, [r1]


	/* Need to make sure that SDRAM is configured correctly before */
	/* coping the code into it.  See Table 13-3 in the EP9315 User's Guide */

	/* Step 1 */
	ldr	r2, =200000
1:	subs	r2, r2, #1
	bne	1b

	/* Step 2 */
	ldr	r1, =SDRAM_BASE
	ldr	r0, =0x00210028
	str	r0, [r1, #CFG3]

	/* Step 3 */
	ldr	r0, =0x80000003
	str	r0, [r1, #GICONFIG]

	/* Step 4 */
	ldr	r2, =200000
2:	subs	r2, r2, #1
	bne	2b

	/* Step 5 */
	ldr	r0, =0x80000001
	str	r0, [r1, #GICONFIG]

	/* Step 6 */
	mov	r0, #10
	str	r0, [r1, #REFRSHTIMR]

	/* Step 7 */
	mov	r2, #80
3:	subs	r2, r2, #1
	bne	3b

	/* Step 8 */
	ldr	r0, =0x00000200
	str	r0, [r1, #REFRSHTIMR]

	/* Step 9 */
	ldr	r0, =0x80000002
	str	r0, [r1, #GICONFIG]

	/* Step 10 - Two 8mbit x 16 bit wide chips */
	ldr	r2, =0x00008800
	ldr	r0, [r2]

	/* Step 11 */
	ldr	r0, =0x80000000
	str	r0, [r1, #GICONFIG]

	nop
	nop
	nop
	nop

	/* Turn on Green LED  */
	ldr	r1, =EP9312_LED_DATA
	ldr	r0, [r1]
	eor	r0, r0, #EP9312_LED_GREEN_ON
	strb	r0,[r1]					    

	/* Errata GICONFIG precharge doesn't always work - so read from each bank (or row?) */

#define STRIDE 512	
	ldr	r2, =PHYS_SDRAM_1
	ldr	r1, =PHYS_SDRAM_SIZE_1-STRIDE
10:	ldr	r0, [r2]
	add	r2, r2, #STRIDE
	subs	r1, r1, #STRIDE
	bne	10b

	ldr	r2, =PHYS_SDRAM_2
	ldr	r1, =PHYS_SDRAM_SIZE_2-STRIDE
20:	ldr	r0, [r2]
	add	r2, r2, #STRIDE
	subs	r1, r1, #STRIDE
	bne	20b

	mov pc,lr

