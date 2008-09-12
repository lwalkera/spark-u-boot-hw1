/*
 *  armboot - Startup Code for MX31 CPU-core
 */

#include <config.h>
#include <version.h>

.globl _start
_start: b	reset
	ldr	pc, _undefined_instruction
	ldr	pc, _software_interrupt
	ldr	pc, _prefetch_abort
	ldr	pc, _data_abort
	ldr	pc, _not_used
	ldr	pc, _irq
	ldr	pc, _fiq

_undefined_instruction: .word undefined_instruction
_software_interrupt:	.word software_interrupt
_prefetch_abort:	.word prefetch_abort
_data_abort:		.word data_abort
_not_used:		.word not_used
_irq:			.word irq
_fiq:			.word fiq
_pad:			.word 0x12345678 /* now 16*4=64 */
.global _end_vect
_end_vect:

	.balignl 16,0xdeadbeef
/*
 *************************************************************************
 *
 * Startup Code (reset vector)
 *
 * do important init only if we don't start from memory!
 * setup Memory and board specific bits prior to relocation.
 * relocate armboot to ram
 * setup stack
 *
 *************************************************************************
 */

_TEXT_BASE:
	.word	TEXT_BASE 

.globl _armboot_start
_armboot_start:
	.word _start

/*
 * These are defined in the board-specific linker script.
 */
.globl _bss_start
_bss_start:
	.word __bss_start

.globl _bss_end
_bss_end:
	.word _end

#ifdef CONFIG_USE_IRQ
/* IRQ stack memory (calculated at run-time) */
.globl IRQ_STACK_START
IRQ_STACK_START:
	.word	0x0badc0de

/* IRQ stack memory (calculated at run-time) */
.globl FIQ_STACK_START
FIQ_STACK_START:
	.word 0x0badc0de
#endif

    /* AIPS setup - Only setup MPROTx registers. The PACR default values are good.*/
    .macro init_aips
        /*
         * Set all MPROTx to be non-bufferable, trusted for R/W,
         * not forced to user-mode.
         */
        ldr r0, =AIPS1_CTRL_BASE_ADDR
        ldr r1, =0x77777777
        str r1, [r0, #0x00]
        str r1, [r0, #0x04]
        ldr r0, =AIPS2_CTRL_BASE_ADDR
        str r1, [r0, #0x00]
        str r1, [r0, #0x04]

        /*
         * Clear the on and off peripheral modules Supervisor Protect bit
         * for SDMA to access them. Did not change the AIPS control registers
         * (offset 0x20) access type
         */
        ldr r0, =AIPS1_CTRL_BASE_ADDR
        ldr r1, =0x0
        str r1, [r0, #0x40]
        str r1, [r0, #0x44]
        str r1, [r0, #0x48]
        str r1, [r0, #0x4C]
        ldr r1, [r0, #0x50]
        and r1, r1, #0x00FFFFFF
        str r1, [r0, #0x50]

        ldr r0, =AIPS2_CTRL_BASE_ADDR
        ldr r1, =0x0
        str r1, [r0, #0x40]
        str r1, [r0, #0x44]
        str r1, [r0, #0x48]
        str r1, [r0, #0x4C]
        ldr r1, [r0, #0x50]
        and r1, r1, #0x00FFFFFF
        str r1, [r0, #0x50]
    .endm /* init_aips */

    /* MAX (Multi-Layer AHB Crossbar Switch) setup */
    .macro init_max        
        ldr r0, =MAX_BASE_ADDR
        /* MPR - priority is M4 > M2 > M3 > M5 > M0 > M1 */
        ldr r1, =0x00302154
        str r1, [r0, #0x000]        /* for S0 */
        str r1, [r0, #0x100]        /* for S1 */
        str r1, [r0, #0x200]        /* for S2 */
        str r1, [r0, #0x300]        /* for S3 */
        str r1, [r0, #0x400]        /* for S4 */
        /* SGPCR - always park on last master */
        ldr r1, =0x10
        str r1, [r0, #0x010]        /* for S0 */
        str r1, [r0, #0x110]        /* for S1 */
        str r1, [r0, #0x210]        /* for S2 */
        str r1, [r0, #0x310]        /* for S3 */
        str r1, [r0, #0x410]        /* for S4 */
        /* MGPCR - restore default values */
        ldr r1, =0x0
        str r1, [r0, #0x800]        /* for M0 */
        str r1, [r0, #0x900]        /* for M1 */
        str r1, [r0, #0xA00]        /* for M2 */
        str r1, [r0, #0xB00]        /* for M3 */
        str r1, [r0, #0xC00]        /* for M4 */
        str r1, [r0, #0xD00]        /* for M5 */
    .endm /* init_max */

        /* M3IF setup */
    .macro init_m3if
        /* Configure M3IF registers */
        ldr r1, =M3IF_BASE
        /*
        * M3IF Control Register (M3IFCTL)
        * MRRP[0] = L2CC0 not on priority list (0 << 0)        = 0x00000000
        * MRRP[1] = L2CC1 not on priority list (0 << 0)        = 0x00000000
        * MRRP[2] = MBX not on priority list (0 << 0)        = 0x00000000
        * MRRP[3] = MAX1 not on priority list (0 << 0)        = 0x00000000
        * MRRP[4] = SDMA not on priority list (0 << 0)        = 0x00000000
        * MRRP[5] = MPEG4 not on priority list (0 << 0)       = 0x00000000
        * MRRP[6] = IPU1 on priority list (1 << 6)             = 0x00000040
        * MRRP[7] = IPU2 not on priority list (0 << 0)   = 0x00000000
        *                                                       ------------
        *                                                       0x00000040
        */
        ldr r0, =0x00000040
        str r0, [r1]  /* M3IF control reg */
    .endm /* init_m3if */
/*
 *************************************************************************
 *
 * CPU_init_critical registers
 *
 * setup important registers
 * setup memory timing
 *
 *************************************************************************
 */
.macro cpu_init_crit
	mrs r0,cpsr         /* set the cpu to SVC32 mode        */
	bic r0,r0,#0x1f     /* (superviser mode, M=10011)       */
	orr r0,r0,#0xd3		/* Disable IRQ and FIQ*/
	msr cpsr,r0
	/*
	 * disable MMU 
	 */
	mrc p15, 0, r0, c1, c0, 0
	ldr r1, =0x00001007
	bic r0, r0, r1 @ clear bits 12,2:0 (ICAM)
	mcr p15, 0, r0, c1, c0, 0

	/* Also setup the Peripheral Port Remap register inside the core */
	ldr r0, =0x40000015        /* start from AIPS 1GB region */
	mcr p15, 0, r0, c15, c2, 4

	/*** L2 Cache setup/invalidation/disable ***/
	/* Disable L2 cache first */
	ldr r0, =L2CC_BASE_ADDR
	ldr r2, [r0, #L2_CACHE_CTL_REG]
	bic r2, r2, #0x1
	str r2, [r0, #L2_CACHE_CTL_REG]
	/*
	 * Configure L2 Cache:
	 * - 128k size(16k way)
	 * - 8-way associativity
	 * - 0 ws TAG/VALID/DIRTY
	 * - 4 ws DATA R/W
	 */

	ldr r1, [r0, #L2_CACHE_AUX_CTL_REG]
	and r1, r1, #0xFE000000
	ldr r2, =0x00030024
	orr r1, r1, r2
	str r1, [r0, #L2_CACHE_AUX_CTL_REG]

	/* Invalidate L2 */
	ldr r1, =0x000000FF
	str r1, [r0, #L2_CACHE_INV_WAY_REG]
L2_loop:
	/* Poll Invalidate By Way register */
	ldr r2, [r0, #L2_CACHE_INV_WAY_REG]
	cmp r2, #0
bne L2_loop

	/*** End of L2 operations ***/
	init_aips
	init_max
	init_m3if

	/*
	 * End of ARM1136 init
	 */
	bl	lowlevel_init	/* go setup pll,mux,memory */
.endm

/*
 * the actual reset code
 */
reset:

#ifndef CONFIG_SKIP_LOWLEVEL_INIT
	cpu_init_crit
#endif

#ifndef CONFIG_SKIP_RELOCATE_UBOOT

relocate:				/* relocate U-Boot to RAM	    */
	adr	r0, _start		/* r0 <- current position of code   */
	ldr	r1, _TEXT_BASE		/* test if we run from flash or RAM */
	cmp	r0, r1			/* don't reloc during debug	    */
	beq	stack_setup

	ldr	r2, _armboot_start
	ldr	r3, _bss_start
	sub	r2, r3, r2		/* r2 <- size of armboot	    */
	add	r2, r0, r2		/* r2 <- source end address	    */
copy_loop:
	ldmia	r0!, {r3-r10}		/* copy from source address [r0]    */
	stmia	r1!, {r3-r10}		/* copy to   target address [r1]    */
	cmp	r0, r2			/* until source end addreee [r2]    */
	ble	copy_loop
#endif	/* CONFIG_SKIP_RELOCATE_UBOOT */

	/* Set up the stack						    */
stack_setup:
	ldr	r0, _TEXT_BASE		/* upper 128 KiB: relocated uboot   */
	sub	r0, r0, #CFG_MALLOC_LEN /* malloc area			    */
	sub	r0, r0, #CFG_GBL_DATA_SIZE /* bdinfo			    */
#ifdef CONFIG_USE_IRQ
	sub	r0, r0, #(CONFIG_STACKSIZE_IRQ+CONFIG_STACKSIZE_FIQ)
#endif
	sub	sp, r0, #12		/* leave 3 words for abort-stack    */

clear_bss:
	ldr	r0, _bss_start		/* find start of bss segment	    */
	ldr	r1, _bss_end		/* stop here			    */
	mov	r2, #0x00000000		/* clear			    */

clbss_l:str	r2, [r0]		/* clear loop...		    */
	add	r0, r0, #4
	cmp	r0, r1
	bne	clbss_l

	ldr r0, =u_boot_addr
	str pc, [r0]
	ldr	pc, _start_armboot

.globl u_boot_addr
u_boot_addr:
	.word CS0_BASE_ADDR

_start_armboot: .word start_armboot

	/*
	 *************************************************************************
	 *
	 * Interrupt handling
	 *
	 *************************************************************************
	 */
	@
	@ IRQ stack frame.
	@
#define S_FRAME_SIZE	72

#define S_OLD_R0	68
#define S_PSR		64
#define S_PC		60
#define S_LR		56
#define S_SP		52

#define S_IP		48
#define S_FP		44
#define S_R10		40
#define S_R9		36
#define S_R8		32
#define S_R7		28
#define S_R6		24
#define S_R5		20
#define S_R4		16
#define S_R3		12
#define S_R2		8
#define S_R1		4
#define S_R0		0

#define MODE_SVC 0x13
#define I_BIT	 0x80

/*
 * use bad_save_user_regs for abort/prefetch/undef/swi ...
 * use irq_save_user_regs / irq_restore_user_regs for IRQ/FIQ handling
 */

	.macro	bad_save_user_regs
	sub	sp, sp, #S_FRAME_SIZE		@ carve out a frame on current user stack
	stmia	sp, {r0 - r12}			@ Save user registers (now in svc mode) r0-r12

	ldr	r2, _armboot_start
	sub	r2, r2, #(CFG_MALLOC_LEN)
	sub	r2, r2, #(CFG_GBL_DATA_SIZE+8)	@ set base 2 words into abort stack
	ldmia	r2, {r2 - r3}			@ get values for "aborted" pc and cpsr (into parm regs)
	add	r0, sp, #S_FRAME_SIZE		@ grab pointer to old stack

	add	r5, sp, #S_SP
	mov	r1, lr
	stmia	r5, {r0 - r3}			@ save sp_SVC, lr_SVC, pc, cpsr
	mov	r0, sp				@ save current stack into r0 (param register)
	.endm

	.macro	irq_save_user_regs
	sub	sp, sp, #S_FRAME_SIZE
	stmia	sp, {r0 - r12}			@ Calling r0-r12
	add	r8, sp, #S_PC			@ !!!! R8 NEEDS to be saved !!!! a reserved stack spot would be good.
	stmdb	r8, {sp, lr}^			@ Calling SP, LR
	str	lr, [r8, #0]			@ Save calling PC
	mrs	r6, spsr
	str	r6, [r8, #4]			@ Save CPSR
	str	r0, [r8, #8]			@ Save OLD_R0
	mov	r0, sp
	.endm

	.macro	irq_restore_user_regs
	ldmia	sp, {r0 - lr}^			@ Calling r0 - lr
	mov	r0, r0
	ldr	lr, [sp, #S_PC]			@ Get PC
	add	sp, sp, #S_FRAME_SIZE
	subs	pc, lr, #4			@ return & move spsr_svc into cpsr
	.endm

	.macro get_bad_stack
	ldr	r13, _armboot_start		@ setup our mode stack (enter in banked mode)
	sub	r13, r13, #(CFG_MALLOC_LEN)	@ move past malloc pool
	sub	r13, r13, #(CFG_GBL_DATA_SIZE+8) @ move to reserved a couple spots for abort stack

	str	lr, [r13]			@ save caller lr in position 0 of saved stack
	mrs	lr, spsr			@ get the spsr
	str	lr, [r13, #4]			@ save spsr in position 1 of saved stack

	mov	r13, #MODE_SVC			@ prepare SVC-Mode
	@ msr	spsr_c, r13
	msr	spsr, r13			@ switch modes, make sure moves will execute
	mov	lr, pc				@ capture return pc
	movs	pc, lr				@ jump to next instruction & switch modes.
	.endm

	.macro get_bad_stack_swi
	sub	r13, r13, #4			@ space on current stack for scratch reg.
	str	r0, [r13]			@ save R0's value.
	ldr	r0, _armboot_start		@ get data regions start
	sub	r0, r0, #(CFG_MALLOC_LEN)	@ move past malloc pool
	sub	r0, r0, #(CFG_GBL_DATA_SIZE+8)	@ move past gbl and a couple spots for abort stack
	str	lr, [r0]			@ save caller lr in position 0 of saved stack
	mrs	r0, spsr			@ get the spsr
	str	lr, [r0, #4]			@ save spsr in position 1 of saved stack
	ldr	r0, [r13]			@ restore r0
	add	r13, r13, #4			@ pop stack entry
	.endm

	.macro get_irq_stack			@ setup IRQ stack
	ldr	sp, IRQ_STACK_START
	.endm

	.macro get_fiq_stack			@ setup FIQ stack
	ldr	sp, FIQ_STACK_START
	.endm

/*
 * exception handlers
 */
	.align	5
undefined_instruction:
	get_bad_stack
	bad_save_user_regs
	bl	do_undefined_instruction

	.align	5
software_interrupt:
	get_bad_stack_swi
	bad_save_user_regs
	bl	do_software_interrupt

	.align	5
prefetch_abort:
	get_bad_stack
	bad_save_user_regs
	bl	do_prefetch_abort

	.align	5
data_abort:
	get_bad_stack
	bad_save_user_regs
	bl	do_data_abort

	.align	5
not_used:
	get_bad_stack
	bad_save_user_regs
	bl	do_not_used

#ifdef CONFIG_USE_IRQ

	.align	5
irq:
	get_irq_stack
	irq_save_user_regs
	bl	do_irq
	irq_restore_user_regs

	.align	5
fiq:
	get_fiq_stack
	/* someone ought to write a more effiction fiq_save_user_regs */
	irq_save_user_regs
	bl	do_fiq
	irq_restore_user_regs

#else

	.align	5
irq:
	get_bad_stack
	bad_save_user_regs
	bl	do_irq

	.align	5
fiq:
	get_bad_stack
	bad_save_user_regs
	bl	do_fiq

#endif
	.align 5
.global arm1136_cache_flush
arm1136_cache_flush:
	mcr	p15, 0, r1, c7, c5, 0	@ invalidate I cache
	mov	pc, lr			@ back to caller

	.align	5
.globl reset_cpu
reset_cpu:
 /* generate reset signal.*/
	ldr r0,=WDOG_WCR_REG
	ldrh r1,[r0]
	bic r1, r1, #WDOG_SRS
	strh r1, [r0]
_loop_forever:
	b	_loop_forever
