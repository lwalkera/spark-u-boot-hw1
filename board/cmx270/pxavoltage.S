/*
 * (C) Copyright 2008
 * Laine Walker-Avina, PASCO scientific, lwalkera@pasco.com
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
#include <asm/arch/pxa-regs.h>

.global	initPXAvoltage

@ This routine clobbers r0,r1,r4

/* --------------------- */

#define ISL6271A_ID 0x0c
#define ISL6271A_1_00V 0x413 /*104 MHz*/
#define ISL6271A_1_25V 0x418 /*208 MHz*/
#define ISL6271A_1_35V 0x41a /*312 MHz*/
#define ISL6271A_1_55V 0x41e /*520 MHz*/

initPXAvoltage:
	mov r4, lr
	
	/*Change power I2C pins away from GPIO*/
	mov r0, #0x40
	ldr r1, =PCFR
	str r0, [r1] 

	/*set I2c addr of power chip*/
	mov r0, #ISL6271A_ID
	ldr r1, =PVCR
	str r0, [r1] 

	/*figure out what freq we're running at*/
	ldr r1, =CCCR
	ldr r0, [r1]

	ldr r1, =0x2000190 /*320 MHz*/
	cmp r1, r0
	bne try_next_1
	ldr r1, =ISL6271A_1_35V
	b set_voltage

try_next_1:
	ldr r1, =0x2000290 /*520 MHz*/
	cmp r1, r0
	bne try_next_2
	ldr r1, =ISL6271A_1_55V
	b set_voltage

try_next_2:
	ldr r1, =0x2000108 /*104 MHz*/
	cmp r1, r0
	bne try_next_3
	ldr r1, =ISL6271A_1_00V
	b set_voltage

try_next_3:
	ldr r1, =0x2000110 /*208 MHz*/
	cmp r1, r0
	bne try_next_4
	ldr r1, =ISL6271A_1_25V
	b set_voltage

try_next_4:
	ldr r1, =0x2000210 /*416 MHz*/
	cmp r1, r0
	bne try_next_3
	ldr r1, =ISL6271A_1_55V

set_voltage:
	/*set r1 to cmd reg*/
	ldr r0, =PCMD0
	str r1, [r0]
	nop
	nop
	b EnterVoltageChange__asm

@ Fix for E28: POWER MANAGER: Core hangs during voltage change when there are outstanding transactions on the bus
.global EnterVoltageChange__asm
@ align code to fit in one page
.align 7 
EnterVoltageChange__asm:
	ldr   r0, =0x41300000       @ APB register read and compare
	ldr   r0, [r0]              @ fence for pending slow apb reads
	cmp   r0, #0

	mov   r0, #8                @ VC bit for PWRMODE
	movs  r1, #1                @ don't execute mcr on 1st pass
	mov   r2, #0x0A000000       @ uncacheable memory to force memory read
@ CHANGE THIS ADDRESS for your environment
@ MUST be mapped into pagetable prior to
@ this function call

retry:
	ldreq r3, [r2]              @ only stall on the 2nd pass
	cmpeq r3, #0                @ compare causes fence on memory transfers
	cmp   r1, #0                @ is this the 2nd pass?
	mcreq p14, 0, r0, c7, c0, 0 @ write to PWRMODE on 2nd pass only

@ Read the VC bit until it is 0, indicates that the VoltageChange is done.
@ On the first pass, we never set the VC bit, so it will be clear already.
VoltageChange_loop:
	mrc    p14, 0, r3, c7, c0, 0
	tst    r3, #0x8
	bne    VoltageChange_loop

	subs   r1, r1, #1           @ update conditional execution counter
	beq    retry

	mov pc, r4
