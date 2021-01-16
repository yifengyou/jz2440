
	/* s3c2410_cpu_suspend
	 *
	 * put the cpu into sleep mode
	*/
#define S3C2440_REFRESH_SELF		(1<<22)
#define S3C2440_MISCCR_SDSLEEP	    (7<<17)
#define S3C2440_CLKCON_POWER	     (1<<3)
		
#define GSTATUS2       (0x560000B4)
#define GSTATUS3       (0x560000B8)
#define GSTATUS4       (0x560000BC)

#define REFRESH        (0x48000024)
#define MISCCR         (0x56000080)
#define CLKCON         (0x4C00000C)

.globl s3c2440_cpu_suspend
	@@ prepare cpu to sleep
s3c2440_cpu_suspend:
	stmdb	sp!, { r4-r12,lr }

	/* GSTATUS3中存放唤醒时要执行的函数 */
	ldr r0, =s3c2440_do_resume
	ldr r1, =GSTATUS3
	str r0, [r1]

	ldr r1, =GSTATUS4
	str sp, [r1]
	
	ldr	r4, =REFRESH
	ldr	r5, =MISCCR
	ldr	r6, =CLKCON
	ldr	r7, [ r4 ]		@ get REFRESH 
	ldr	r8, [ r5 ]		@ get MISCCR 
	ldr	r9, [ r6 ]		@ get CLKCON 

	orr	r7, r7, #S3C2440_REFRESH_SELF	@ SDRAM sleep command
	orr	r8, r8, #S3C2440_MISCCR_SDSLEEP @ SDRAM power-down signals
	orr	r9, r9, #S3C2440_CLKCON_POWER	@ power down command

	teq	pc, #0			@ first as a trial-run to load cache
	bl	s3c2440_do_sleep
	teq	r0, r0			@ now do it for real
	b	s3c2440_do_sleep	@

	@@ align next bit of code to cache line
	.align	5
s3c2440_do_sleep:
	streq	r7, [ r4 ]			@ SDRAM sleep command
	streq	r8, [ r5 ]			@ SDRAM power-down config
	streq	r9, [ r6 ]			@ CPU sleep
1:	beq	1b
	mov	pc, r14

s3c2440_do_resume:
	/* 返回到do_suspend函数 */
	ldr r1, =GSTATUS4
	ldr sp, [r1]
	
	ldmia	sp!, 	{ r4-r12,pc }

