/*
 *  armboot - Startup Code for ARM920 CPU-core
 *
 *  Copyright (c) 2001	Marius Gr鰃er <mag@sysgo.de>
 *  Copyright (c) 2002	Alex Z黳ke <azu@sysgo.de>
 *  Copyright (c) 2002	Gary Jennejohn <gj@denx.de>
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


#include <config.h>
#include <version.h>


/*
 *************************************************************************
 *
 * Jump vector table as in table 3.1 in [1]
 *
 *************************************************************************
 */
#define GSTATUS2       (0x560000B4)
#define GSTATUS3       (0x560000B8)
#define GSTATUS4       (0x560000BC)

#define REFRESH        (0x48000024)
#define MISCCR         (0x56000080)

#define LOCKTIME	0x4C000000	/* R/W, PLL lock time count register */
#define MPLLCON		0x4C000004	/* R/W, MPLL configuration register */
#define UPLLCON		0x4C000008	/* R/W, UPLL configuration register */
#define CLKCON		0x4C00000C	/* R/W, Clock generator control reg. */
#define CLKSLOW		0x4C000010	/* R/W, Slow clock control register */
#define CLKDIVN		0x4C000014	/* R/W, Clock divider control */


.globl _start
_start:	b       reset
	ldr	pc, _undefined_instruction
	ldr	pc, _software_interrupt
	ldr	pc, _prefetch_abort
	ldr	pc, _data_abort
	ldr	pc, _not_used
	ldr	pc, _irq
	ldr	pc, _fiq

_undefined_instruction:	.word undefined_instruction
_software_interrupt:	.word software_interrupt
_prefetch_abort:	.word prefetch_abort
_data_abort:		.word data_abort
_not_used:		.word not_used
_irq:			.word irq
_fiq:			.word fiq

	.balignl 16,0xdeadbeef


/*
 *************************************************************************
 *
 * Startup Code (reset vector)
 *
 * do important init only if we don't start from memory!
 * relocate armboot to ram
 * setup stack
 * jump to second stage
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

.globl FREE_RAM_END
FREE_RAM_END:
	.word	0x0badc0de

.globl FREE_RAM_SIZE
FREE_RAM_SIZE:
	.word	0x0badc0de

.globl PreLoadedONRAM
PreLoadedONRAM:
	.word	0

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


/*
 * the actual reset code
 */

reset:
	/*
	 * set the cpu to SVC32 mode
	 */
	mrs	r0,cpsr
	bic	r0,r0,#0x1f
	orr	r0,r0,#0xd3
	msr	cpsr,r0

/* turn off the watchdog */
#if defined(CONFIG_S3C2400)
# define pWTCON		0x15300000
# define INTMSK		0x14400008	/* Interupt-Controller base addresses */
# define CLKDIVN	0x14800014	/* clock divisor register */
#elif defined(CONFIG_S3C2410)
# define pWTCON		0x53000000
# define INTMOD     0X4A000004
# define INTMSK		0x4A000008	/* Interupt-Controller base addresses */
# define INTSUBMSK	0x4A00001C
# define CLKDIVN	0x4C000014	/* clock divisor register */
#endif

#if defined(CONFIG_S3C2400) || defined(CONFIG_S3C2410)
	ldr     r0, =pWTCON
	mov     r1, #0x0
	str     r1, [r0]

	/*
	 * mask all IRQs by setting all bits in the INTMR - default
	 */
	mov	r1, #0xffffffff
	ldr	r0, =INTMSK
	str	r1, [r0]
# if defined(CONFIG_S3C2410)
	ldr	r1, =0x3ff
	ldr	r0, =INTSUBMSK
	str	r1, [r0]
# endif

#if 0
	/* FCLK:HCLK:PCLK = 1:2:4 */
	/* default FCLK is 120 MHz ! */
	ldr	r0, =CLKDIVN
	mov	r1, #3
	str	r1, [r0]
#endif
#endif	/* CONFIG_S3C2400 || CONFIG_S3C2410 */

#ifndef CONFIG_SKIP_LOWLEVEL_INIT
#if 0
		/* 这些代码会使用SP,在NAND启动时会破坏片内内存的部分代码
         * 导致NAND启动时无法使用休眠-唤醒功能
         */
		/* 设置SP指向片内内存 */
		ldr sp, =4092
		ldr r0, =0x12345678
		str r0, [sp]
		ldr r1, [sp]
		cmp r0, r1
		ldrne sp, =0x40000000+4096
		bl clock_init
#else
	/* 设置时钟, 使用汇编 */
#define S3C2440_MPLL_400MHZ     ((0x5c<<12)|(0x01<<4)|(0x01))
#define S3C2440_UPLL_48MHZ      ((0x38<<12)|(0x02<<4)|(0x02))
#define S3C2440_CLKDIV          (0x05) // | (1<<3))    /* FCLK:HCLK:PCLK = 1:4:8, UCLK = UPLL/2 */

	ldr r1, =CLKDIVN
	mov r2, #S3C2440_CLKDIV
	str r2, [r1]

	mrc p15, 0, r1, c1, c0, 0		// read ctrl register 
	orr r1, r1, #0xc0000000 		// Asynchronous	
	mcr p15, 0, r1, c1, c0, 0		// write ctrl register

    ldr r0,=LOCKTIME
    ldr r1,=0xffffff
    str r1,[r0]
    // delay
    mov     r0, #0x200
1:  subs    r0, r0, #1
    bne     1b

    // Configure MPLL
    ldr r0,=MPLLCON          
    ldr r1,=S3C2440_MPLL_400MHZ
    str r1,[r0]
    // delay
    mov     r0, #0x200
1:  subs    r0, r0, #1
    bne     1b

    //Configure UPLL
    ldr     r0, =UPLLCON          
    ldr     r1, =S3C2440_UPLL_48MHZ
    str     r1, [r0]
    // delay
    mov     r0, #0x200
1:  subs    r0, r0, #1
    bne     1b


#endif
#endif    

	/* 2. 根据 GSTATUS2[1]判断是复位还是唤醒 */	
	ldr r0, =GSTATUS2
	ldr r1, [r0]
	tst r1, #(1<<1)  /* r1 & (1<<1) */
	bne wake_up	

	/*
	 * we do sys-critical inits only at reboot,
	 * not when booting from ram!
	 */
#ifndef CONFIG_SKIP_LOWLEVEL_INIT
	adr	r0, _start		/* r0 <- current position of code   */
	ldr	r1, _TEXT_BASE		/* test if we run from flash or RAM */
	cmp     r0, r1                  /* don't reloc during debug         */
	blne	cpu_init_crit
#endif

	/* Set up the stack						    */
stack_setup:
	ldr	r0, _TEXT_BASE		/* upper 128 KiB: relocated uboot   */
	sub	r0, r0, #CFG_MALLOC_LEN	/* malloc area                      */
	sub	r0, r0, #CFG_GBL_DATA_SIZE /* bdinfo                        */

#ifdef CONFIG_USE_IRQ
	sub	r0, r0, #(CONFIG_STACKSIZE_IRQ+CONFIG_STACKSIZE_FIQ)
#endif
	sub	sp, r0, #12		/* leave 3 words for abort-stack    */

#ifndef CONFIG_SKIP_RELOCATE_UBOOT
relocate:				/* relocate U-Boot to RAM	    */
	adr	r0, _start		/* r0 <- current position of code   */
	ldr	r1, _TEXT_BASE		/* test if we run from flash or RAM */
	cmp     r0, r1                  /* don't reloc during debug         */
	beq     clear_bss
	
	ldr	r2, _armboot_start
	ldr	r3, _bss_start
	sub	r2, r3, r2		/* r2 <- size of armboot            */
#if 1
	bl  CopyCode2Ram	/* r0: source, r1: dest, r2: size */
#else
	add	r2, r0, r2		/* r2 <- source end address         */

copy_loop:
	ldmia	r0!, {r3-r10}		/* copy from source address [r0]    */
	stmia	r1!, {r3-r10}		/* copy to   target address [r1]    */
	cmp	r0, r2			/* until source end addreee [r2]    */
	ble	copy_loop
#endif
#endif	/* CONFIG_SKIP_RELOCATE_UBOOT */

clear_bss:
	ldr	r0, _bss_start		/* find start of bss segment        */
	ldr	r1, _bss_end		/* stop here                        */
	mov 	r2, #0x00000000		/* clear                            */

clbss_l:str	r2, [r0]		/* clear loop...                    */
	add	r0, r0, #4
	cmp	r0, r1
	ble	clbss_l

SetLoadFlag:
	/* Set a global flag, PreLoadedONRAM */
	adr	r0, _start		/* r0 <- current position of code   */
	ldr	r1, _TEXT_BASE		/* test if we run from flash or RAM */
	cmp     r0, r1                  /* don't reloc during debug         */
	ldr r2, =PreLoadedONRAM
	mov r3, #1
	streq r3, [r2]

#if 0
	/* try doing this stuff after the relocation */
	ldr     r0, =pWTCON
	mov     r1, #0x0
	str     r1, [r0]

	/*
	 * mask all IRQs by setting all bits in the INTMR - default
	 */
	mov	r1, #0xffffffff
	ldr	r0, =INTMR
	str	r1, [r0]

	/* FCLK:HCLK:PCLK = 1:2:4 */
	/* default FCLK is 120 MHz ! */
	ldr	r0, =CLKDIVN
	mov	r1, #3
	str	r1, [r0]
	/* END stuff after relocation */
#endif

	ldr	pc, _start_armboot

_start_armboot:	.word start_armboot

/* 1. 按下按键 */
wake_up:
	str r1, [r0]  /* clear GSTATUS2 */
	/* 3. 设置 MISCCR[19:17]=000b, 以释放SDRAM信号 */
	ldr r0, =MISCCR
	ldr r1, [r0]
	bic r1, r1, #(7<<17)
	str r1, [r0]
		
	/* 4. 配置s3c2440的memory controller */
	bl	cpu_init_crit
	
	/* 5. 等待SDRAM退出self-refresh mode */
	mov r0, #1000
1:	subs r0, r0, #1
	cmp r0, #0
	bne 1b
	
	/* 6. 根据GSTATUS[3:4]的值来运行休眠前的函数 */
	ldr r0, =GSTATUS3
	ldr r1, [r0]
	mov pc, r1
	

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


#ifndef CONFIG_SKIP_LOWLEVEL_INIT
cpu_init_crit:
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
	mcr	p15, 0, r0, c1, c0, 0

	/*
	 * before relocating, we have to setup RAM timing
	 * because memory timing is board-dependend, you will
	 * find a lowlevel_init.S in your board directory.
	 */
	mov	ip, lr
	bl	lowlevel_init
	mov	lr, ip
	mov	pc, lr
#endif /* CONFIG_SKIP_LOWLEVEL_INIT */

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
	sub	sp, sp, #S_FRAME_SIZE
	stmia	sp, {r0 - r12}			@ Calling r0-r12
	ldr	r2, _armboot_start
	sub	r2, r2, #(CONFIG_STACKSIZE+CFG_MALLOC_LEN)
	sub	r2, r2, #(CFG_GBL_DATA_SIZE+8)  @ set base 2 words into abort stack
	ldmia	r2, {r2 - r3}			@ get pc, cpsr
	add	r0, sp, #S_FRAME_SIZE		@ restore sp_SVC

	add	r5, sp, #S_SP
	mov	r1, lr
	stmia	r5, {r0 - r3}			@ save sp_SVC, lr_SVC, pc, cpsr
	mov	r0, sp
	.endm

	.macro	irq_save_user_regs
	sub	sp, sp, #S_FRAME_SIZE
	stmia	sp, {r0 - r12}			@ Calling r0-r12
	add     r8, sp, #S_PC
	stmdb   r8, {sp, lr}^                   @ Calling SP, LR
	str     lr, [r8, #0]                    @ Save calling PC
	mrs     r6, spsr
	str     r6, [r8, #4]                    @ Save CPSR
	str     r0, [r8, #8]                    @ Save OLD_R0
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
	ldr	r13, _armboot_start		@ setup our mode stack
	sub	r13, r13, #(CONFIG_STACKSIZE+CFG_MALLOC_LEN)
	sub	r13, r13, #(CFG_GBL_DATA_SIZE+8) @ reserved a couple spots in abort stack

	str	lr, [r13]			@ save caller lr / spsr
	mrs	lr, spsr
	str     lr, [r13, #4]

	mov	r13, #MODE_SVC			@ prepare SVC-Mode
	@ msr	spsr_c, r13
	msr	spsr, r13
	mov	lr, pc
	movs	pc, lr
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
	.align  5
undefined_instruction:
	get_bad_stack
	bad_save_user_regs
	bl 	do_undefined_instruction

	.align	5
software_interrupt:
	get_bad_stack
	bad_save_user_regs
	bl 	do_software_interrupt

	.align	5
prefetch_abort:
	get_bad_stack
	bad_save_user_regs
	bl 	do_prefetch_abort

	.align	5
data_abort:
	get_bad_stack
	bad_save_user_regs
	bl 	do_data_abort

	.align	5
not_used:
	get_bad_stack
	bad_save_user_regs
	bl 	do_not_used

@ thisway.diy, 2006.06.24
.globl Launch
    .align	4
Launch:    
    mov r7, r0
    @ diable interrupt
	@ disable watch dog timer
	mov	r1, #0x53000000
	mov	r2, #0x0
	str	r2, [r1]

    ldr r1,=INTMSK
    ldr r2,=0xffffffff  @ all interrupt disable
    str r2,[r1]

    ldr r1,=INTSUBMSK
    ldr r2,=0x7ff       @ all sub interrupt disable
    str r2,[r1]

    ldr     r1, = INTMOD
    mov r2, #0x0        @ set all interrupt as IRQ (not FIQ)
    str     r2, [r1]

    @ 
	mov	ip, #0
	mcr	p15, 0, ip, c13, c0, 0      @	/* zero PID */
	mcr	p15, 0, ip, c7, c7, 0       @	/* invalidate I,D caches */
	mcr	p15, 0, ip, c7, c10, 4      @	/* drain write buffer */
	mcr	p15, 0, ip, c8, c7, 0       @	/* invalidate I,D TLBs */
	mrc	p15, 0, ip, c1, c0, 0       @	/* get control register */
	bic	ip, ip, #0x0001             @	/* disable MMU */
	mcr	p15, 0, ip, c1, c0, 0       @	/* write control register */

    @ MMU_EnableICache
    @mrc p15,0,r1,c1,c0,0
    @orr r1,r1,#(1<<12)
    @mcr p15,0,r1,c1,c0,0

    @ clear SDRAM: the end of free mem(has wince on it now) to the end of SDRAM
    ldr     r3, FREE_RAM_END
    ldr     r4, =PHYS_SDRAM_1+PHYS_SDRAM_1_SIZE    @ must clear all the memory unused to zero
    mov     r5, #0

    ldr     r1, _armboot_start
    ldr     r2, =On_Steppingstone
    sub     r2, r2, r1
    mov     pc, r2
On_Steppingstone:
2:  stmia   r3!, {r5}
    cmp     r3, r4
    bne     2b

    @ set sp = 0 on sys mode
    mov sp, #0

    @ add by thisway.diy 2006.06.26, switch to SVC mode
	msr	cpsr_c,	#0xdf	@ set the I-bit = 1, diable the IRQ interrupt
	msr	cpsr_c,	#0xd3	@ set the I-bit = 1, diable the IRQ interrupt
    ldr sp, =0x31ff5800	
    
    nop
	nop
    nop
	nop

	mov     pc, r7  @ Jump to PhysicalAddress
	nop
    mov pc, lr

#ifdef CONFIG_USE_IRQ

	.align	5
irq:
/* add by www.100ask.net to use IRQ for USB and DMA */
	sub	lr, lr, #4			        @ the return address
	ldr	sp, IRQ_STACK_START	        @ the stack for irq
	stmdb	sp!, 	{ r0-r12,lr }	@ save registers
	
	ldr	lr,	=int_return		        @ set the return addr
	ldr	pc, =IRQ_Handle		        @ call the isr
int_return:
	ldmia	sp!, 	{ r0-r12,pc }^	@ return from interrupt

	.align	5
fiq:
	get_fiq_stack
	/* someone ought to write a more effiction fiq_save_user_regs */
	irq_save_user_regs
	bl 	do_fiq
	irq_restore_user_regs

#else

	.align	5
irq:
	get_bad_stack
	bad_save_user_regs
	bl 	do_irq

	.align	5
fiq:
	get_bad_stack
	bad_save_user_regs
	bl 	do_fiq

#endif
