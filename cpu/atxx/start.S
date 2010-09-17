/*------------------------------------------------------------------------------
* (c) Copyright, Augusta Technology, Inc., 2006-present.
* (c) Copyright, Augusta Technology USA, Inc., 2006-present.
*
* This software, document, web pages, or material (the "Work") is copyrighted
* by its respective copyright owners.  The Work may be confidential and
* proprietary.  The Work may be further protected by one or more patents and
* be protected as a part of a trade secret package.
*
* No part of the Work may be copied, photocopied, reproduced, translated, or
* reduced to any electronic medium or machine-readable form, in whole or in
* part, without prior written consent of the copyright owner. Any other
* reproduction in any form without the permission of the copyright owner is
* prohibited.
*
* All Work are protected by the copyright laws of all relevant jurisdictions,
* including protection under the United States copyright laws, and may not be
* reproduced, distributed, transmitted, displayed, published, or broadcast
* without the prior written permission of the copyright owner.
*
------------------------------------------------------------------------------*/

#include <config.h>
#include <version.h>


#if !defined(CONFIG_ENABLE_MMU) && !defined(CONFIG_SYS_PHY_UBOOT_BASE)
#define CONFIG_SYS_PHY_UBOOT_BASE	CONFIG_SYS_UBOOT_BASE
#endif

/*
 *************************************************************************
 *
 * Jump vector table as in table 3.1 in [1]
 *
 *************************************************************************
 */

.globl _start
_start: b	reset
	ldr	pc, _undefined_instruction
	ldr	pc, _software_interrupt
	ldr	pc, _prefetch_abort
	ldr	pc, _data_abort
	ldr	pc, _not_used
	ldr	pc, _irq
	ldr	pc, _fiq

_undefined_instruction:
	.word undefined_instruction
_software_interrupt:
	.word software_interrupt
_prefetch_abort:
	.word prefetch_abort
_data_abort:
	.word data_abort
_not_used:
	.word not_used
_irq:
	.word irq
_fiq:
	.word fiq
_pad:
	.word 0x12345678 /* now 16*4=64 */

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

/*
 * Below variable is very important because we use MMU in U-Boot.
 * Without it, we cannot run code correctly before MMU is ON.
 * by scsuh.
 */
_TEXT_PHY_BASE:
	.word	CONFIG_SYS_PHY_UBOOT_BASE

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

/*
 * the actual reset code
 */

reset:
	/*
	 * set the cpu to SVC32 mode
	 */
	mrs	r0, cpsr
	bic	r0, r0, #0x3f
	orr	r0, r0, #0xd3
	msr	cpsr, r0

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
	/*
	 * we do sys-critical inits only at reboot,
	 * not when booting from ram!
	 */
cpu_init_crit:
	/*
	 * When booting from NAND - it has definitely been a reset, so, no need
	 * to flush caches and disable the MMU
	 */
	/*
	 * flush v4 I/D caches
	 */
	mov	r0, #0
	mcr	p15, 0, r0, c7, c7, 0	/* flush v3/v4 cache */
	mcr	p15, 0, r0, c8, c7, 0	/* flush v4 TLB */

	/*
	 * disable MMU stuff and caches
	 */
	mrc	p15, 0, r0, c1, c0, 0
	bic	r0, r0, #0x00002300	@ clear bits 13, 9:8 (--V- --RS)
	bic	r0, r0, #0x00000087	@ clear bits 7, 2:0 (B--- -CAM)
	orr	r0, r0, #0x00000002	@ set bit 2 (A) Align
	orr	r0, r0, #0x00001000	@ set bit 12 (I) I-Cache

	/* We presume we're within the first 1024 bytes */
	and	r1, r1, #0x3fc
	ldr	r2, _TEXT_PHY_BASE
	ldr	r3, =0xfff00000
	and	r2, r2, r3
	orr	r2, r2, r1
	b	mmu_disable

	.align 5
	/* Run in a single cache-line */
mmu_disable:
	mcr	p15, 0, r0, c1, c0, 0
	nop
	nop

#ifdef CONFIG_ENABLE_MMU
enable_mmu:
	/* enable domain access */
	ldr	r5, =0x0000ffff
	mcr	p15, 0, r5, c3, c0, 0	/* load domain access register */

	/* Set the TTB register */
	ldr	r0, _mmu_table_base
	ldr	r1, =CONFIG_SYS_PHY_UBOOT_BASE
	ldr	r2, =0xfff00000
	bic	r0, r0, r2
	orr	r1, r0, r1
	mcr	p15, 0, r1, c2, c0, 0

	/* Enable the MMU */
	mrc	p15, 0, r0, c1, c0, 0
	orr	r0, r0, #1		/* Set CR_M to enable MMU */

	/* Prepare to enable the MMU */
	adr	r1, skip_hw_init
	and	r1, r1, #0x3fc
	ldr	r2, _TEXT_BASE
	ldr	r3, =0xfff00000
	and	r2, r2, r3
	orr	r2, r2, r1
	b	mmu_enable

	.align 5
	/* Run in a single cache-line */
mmu_enable:

	mcr	p15, 0, r0, c1, c0, 0
	nop
	nop
	mov	pc, r2
#endif

skip_hw_init:
	/* Set up the stack						    */
stack_setup:
#ifdef CONFIG_UNCONTINUOUS_MEM
	ldr	r0, =CONFIG_SYS_MALLOC_END		/* upper 128 KB: relocated uboot   */
	sub	r0, r0, #CONFIG_SYS_MALLOC_LEN	/* malloc area */
	sub	r0, r0, #CONFIG_SYS_GBL_DATA_SIZE /* bdinfo */
#ifdef CONFIG_IRQ
	sub	r0, r0, #CONFIG_STACKSIZE_IRQ /* stack irq */
	sub	r0, r0, #CONFIG_STACKSIZE_FIQ /* stack fiq */
#endif
	sub	sp, r0, #12		/* leave 3 words for abort-stack */

#else
#ifdef CONFIG_MEMORY_UPPER_CODE
	ldr	sp, =(CONFIG_SYS_UBOOT_BASE + CONFIG_SYS_UBOOT_SIZE - 0xc)
#else
	ldr	r0, _TEXT_BASE		/* upper 128 KiB: relocated uboot   */
	sub	r0, r0, #CONFIG_SYS_MALLOC_LEN	/* malloc area                      */
	sub	r0, r0, #CONFIG_SYS_GBL_DATA_SIZE /* bdinfo                        */
	sub	sp, r0, #12		/* leave 3 words for abort-stack    */

#endif
#endif

clear_bss:
	ldr	r0, _bss_start		/* find start of bss segment        */
	ldr	r1, _bss_end		/* stop here                        */
	mov 	r2, #0			/* clear                            */

clbss_l:
	str	r2, [r0]		/* clear loop...                    */
	add	r0, r0, #4
	cmp	r0, r1
	ble	clbss_l

	ldr	pc, _start_armboot

_start_armboot:
	.word start_armboot

#ifdef CONFIG_ENABLE_MMU
_mmu_table_base:
	.word mmu_table
#endif

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
 */

	.macro	bad_save_user_regs
	/* carve out a frame on current user stack */
	sub	sp, sp, #S_FRAME_SIZE
	/* Save user registers (now in svc mode) r0-r12 */
	stmia	sp, {r0 - r12}
#ifdef CONFIG_UNCONTINUOUS_MEM
	ldr	r2, =CONFIG_SYS_MALLOC_END
#else
	ldr	r2, _armboot_start
#endif
	sub	r2, r2, #(CONFIG_SYS_MALLOC_LEN)
	/* set base 2 words into abort stack */
	sub	r2, r2, #(CONFIG_SYS_GBL_DATA_SIZE + 8)
	/* get values for "aborted" pc and cpsr (into parm regs) */
	ldmia	r2, {r2 - r3}
	/* grab pointer to old stack */
	add	r0, sp, #S_FRAME_SIZE

	add	r5, sp, #S_SP
	mov	r1, lr
	/* save sp_SVC, lr_SVC, pc, cpsr */
	stmia	r5, {r0 - r3}
	/* save current stack into r0 (param register) */
	mov	r0, sp
	.endm

	.macro get_bad_stack
	/* setup our mode stack (enter in banked mode) */
#ifdef CONFIG_UNCONTINUOUS_MEM
	ldr	r13, =CONFIG_SYS_MALLOC_END
#else
	ldr	r13, _armboot_start
#endif
	/* move past malloc pool */
	sub	r13, r13, #(CONFIG_SYS_MALLOC_LEN)
	/* move to reserved a couple spots for abort stack */
	sub	r13, r13, #(CONFIG_SYS_GBL_DATA_SIZE + 8)

	/* save caller lr in position 0 of saved stack */
	str	lr, [r13]
	/* get the spsr */
	mrs	lr, spsr
	/* save spsr in position 1 of saved stack */
	str	lr, [r13, #4]

	/* prepare SVC-Mode */
	mov	r13, #MODE_SVC
	@ msr	spsr_c, r13
	/* switch modes, make sure moves will execute */
	msr	spsr, r13
	/* capture return pc */
	mov	lr, pc
	/* jump to next instruction & switch modes. */
	movs	pc, lr
	.endm

	.macro get_bad_stack_swi
	/* space on current stack for scratch reg. */
	sub	r13, r13, #4
	/* save R0's value. */
	str	r0, [r13]
	/* get data regions start */
	ldr	r0, _armboot_start
	/* move past malloc pool */
	sub	r0, r0, #(CONFIG_SYS_MALLOC_LEN)
	/* move past gbl and a couple spots for abort stack */
	sub	r0, r0, #(CONFIG_SYS_GBL_DATA_SIZE + 8)
	/* save caller lr in position 0 of saved stack */
	str	lr, [r0]
	/* get the spsr */
	mrs	r0, spsr
	/* save spsr in position 1 of saved stack */
	str	lr, [r0, #4]
	/* restore r0 */
	ldr	r0, [r13]
	/* pop stack entry */
	add	r13, r13, #4
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

.globl __mmu_cache_on

__mmu_cache_on:
	mov	r12, lr
	mov r3, r0
	mov	r0, #0
	mcr	p15, 0, r0, c7, c10, 4	@ drain write buffer
	mcr	p15, 0, r0, c8, c7, 0	@ flush I,D TLBs
	mrc	p15, 0, r0, c1, c0, 0	@ read control reg
	orr	r0, r0, #0x5000 	@ I-cache enable, RR cache replacement
	orr	r0, r0, #0x0030
	bl	__common_mmu_cache_on
	mov	r0, #0
	mcr	p15, 0, r0, c8, c7, 0	@ flush I,D TLBs
	mov	pc, r12

__common_mmu_cache_on:
	orr	r0, r0, #0x000d 	@ Write buffer, mmu
	mov	r1, #-1			@ 0xffffffff
	mcr	p15, 0, r3, c2, c0, 0	@ load page table pointer
	mcr	p15, 0, r1, c3, c0, 0	@ load domain access control
	b	1f		
	.align	5			@ cache line aligned
1:
	mcr	p15, 0, r0, c1, c0, 0	@ load control register
	mrc	p15, 0, r0, c1, c0, 0	@ and read it back to
	sub	pc, lr, r0, lsr #32	@ properly flush pipeline

.globl __mmu_cache_off
__mmu_cache_off:
	mov r2, lr
	bl __clean_cache
	mrc	p15, 0, r0, c1, c0, 0
	bic	r0, r0, #0x000d
	mcr	p15, 0, r0, c1, c0, 0	@ turn MMU and cache off
	mov	r0, #0
	mcr	p15, 0, r0, c7, c7, 0	@ invalidate whole cache v4
	mcr	p15, 0, r0, c8, c7, 0	@ invalidate whole TLB v4
	mov	pc, r2

.globl __clean_cache
__clean_cache:
	mcr	p15, 0, r0, c7, c10, 0	@ clean entire Data cache
	mov	r0, #0
	mcr	p15, 0, r0, c7, c10, 4	@ drain write buffer
	mov	pc, lr

.globl __flush_cache
__flush_cache:
	mcr	p15, 0, r0, c7, c14, 0	@ clean and invalidate entire Data cache
	mov	r0, #0
	mcr	p15, 0, r0, c7, c10, 4	@ drain write buffer
	mov	pc, lr

