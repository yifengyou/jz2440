
/*
 * weidongshan@qq.com, www.100ask.net
 *
 */

#include <common.h>
#include <command.h>
#include <def.h>
#include <nand.h>
#include <s3c24x0.h>

extern void s3c2440_cpu_suspend(void);

static void delay(volatile int d)
{
	while(d--);
}

int do_suspend (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	/* 休眠: */
	
	/* 1. 配置GPIO: 比如想维持LED亮或灭, 用于唤醒CPU的引脚要设为中断功能 */
	/* 对于NAND启动: 要设置EINT23,22,21为输入引脚 */
	rGPGCON &= ~((3<<30) | (3<<28) | (3<<26));

	/* JZ2440只有S2/S3/S4可用作唤醒源,设置它们对应的GPIO用于中断模式 */
	rGPFCON &= ~((3<<0) | (3<<4));
	rGPFCON |= ((2<<0) | (2<<4));

	rGPGCON &= ~(3<<6);
	rGPGCON |= (2<<6);

	/* 2. 设置INTMSK屏蔽所有中断: 在sleep模式下,这些引脚只是用于唤醒系统,当CPU正常运行时可以重新设置INTMSK让这些引脚用于中断功能 */	
	rINTMSK = ~0;

	/* 3. 配置唤醒源 */
	rEXTINT0 |= (6<<0) | (6<<8); /* EINT0,2双边沿触发 */
	rEXTINT1 |= (6<<12);   /* EINT11双边沿触发 */

	/* 4. 设置MISCCR[13:12]=11b, 使得USB模块进入休眠 */
	rMISCCR |= (3<<12);

	/* 5. 在GSTATUS[4:3]保存某值, 它们可以在系统被唤醒时使用 */
	//rGSTATUS3 = ;  /* 唤醒时首先执行的函数的地址 */
	//rGSTATUS4 = ;  /*  */

	/* 6. 设置 MISCCR[1:0] 使能数据总线的上拉电阻 */
	rMISCCR &= ~(3);

	/* 7. 清除 LCDCON1.ENVID 以停止LCD */
	rLCDCON1 &= ~1;

	/* 8~12使用汇编来实现,参考内核源码:
	 *    arch\arm\mach-s3c2410\sleep.S
	*/

	/* 8. 读这2个寄存器: rREFRESH and rCLKCON, 以便填充TLB
	 *    如果不使用MMU的话,这个目的可以忽略 
	 */

	/* 9. 设置 REFRESH[22]=1b,让SDRAM进入self-refresh mode */

	/* 10. 等待SDRAM成功进入self-refresh mode  */

	/* 11.设置 MISCCR[19:17]=111b以保护SDRAM信号(SCLK0,SCLK1 and SCKE) */

	/* 12. 设置CLKCON的SLEEP位让系统进入sleep mode */	
	printf("suspend ...");
	delay(1000000);
	s3c2440_cpu_suspend();  /* 执行到这里就不会返回,直到CPU被唤醒 */

	/* 恢复运行: 重新初始化硬件 */
	serial_init();
	printf("wake up\n");
	
	
    return 0;
}


U_BOOT_CMD(
	suspend,	1,	0,	do_suspend,
	"suspend - suspend the board\n",
	" - suspend the board"
);


