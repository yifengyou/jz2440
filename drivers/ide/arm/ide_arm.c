/*
 * ARM/ARM26 default IDE host driver
 *
 * Copyright (C) 2004 Bartlomiej Zolnierkiewicz
 * Based on code by: Russell King, Ian Molton and Alexander Schulz.
 *
 * May be copied or modified under the terms of the GNU General Public License.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/ide.h>

#include <asm/mach-types.h>
#include <asm/irq.h>

#ifdef CONFIG_ARM26
# define IDE_ARM_HOST	(machine_is_a5k())
#else
# define IDE_ARM_HOST	(1)
#endif

#ifdef CONFIG_ARCH_CLPS7500
# include <asm/arch/hardware.h>
#
# define IDE_ARM_IO	(ISASLOT_IO + 0x1f0)
# define IDE_ARM_IRQ	IRQ_ISA_14
#elif defined(CONFIG_ARCH_S3C2410)
#include <linux/irq.h>
#include <asm/arch-s3c2410/regs-mem.h>
#include <asm/arch-s3c2410/regs-gpio.h>
# define IDE_ARM_IOs    {0x08000000, 0x10000000}
# define IDE_ARM_IRQPIN S3C2410_GPF6
#else
# define IDE_ARM_IO	0x1f0
# define IDE_ARM_IRQ	IRQ_HARDDISK
#endif

#ifdef CONFIG_ARCH_S3C2410
/* Set hwif for S3C2410/S3C2440, by www.100ask.net */
static void __init ide_s3c24xx_init(void)
{
    int i;
    unsigned int oldval_bwscon;     /* 用来保存BWSCON寄存器的值 */
    unsigned long mapaddr0;
    unsigned long mapaddr1;
    unsigned long baseaddr[] = IDE_ARM_IOs;
    hw_regs_t hw;

    /* 设置BANK1/2: 总线宽度为16 */
    oldval_bwscon = readl(S3C2410_BWSCON);
    writel((oldval_bwscon & ~((3<<4)|(3<<8))) \
        | S3C2410_BWSCON_DW1_16 | S3C2410_BWSCON_WS1 
        | S3C2410_BWSCON_DW2_16 | S3C2410_BWSCON_WS2, S3C2410_BWSCON);

    /* 设置BANK1/2的时间参数 */
    writel((S3C2410_BANKCON_Tacs4 | S3C2410_BANKCON_Tcos4 | S3C2410_BANKCON_Tacc14
          | S3C2410_BANKCON_Tcoh4 | S3C2410_BANKCON_Tcah4 | S3C2410_BANKCON_Tacp6 
          | S3C2410_BANKCON_PMCnorm), S3C2410_BANKCON1);
    writel((S3C2410_BANKCON_Tacs4 | S3C2410_BANKCON_Tcos4 | S3C2410_BANKCON_Tacc14
          | S3C2410_BANKCON_Tcoh4 | S3C2410_BANKCON_Tcah4 | S3C2410_BANKCON_Tacp6 
          | S3C2410_BANKCON_PMCnorm), S3C2410_BANKCON2);
    
    /* 
     * 设置IDE接口的地址, ADDR3~1接到IDE接口的A02~00
     * 注意：没有使用ADDR0，所以下面确定地址时，都左移1位
     */
    mapaddr0 = (unsigned long)ioremap(baseaddr[0], 16);
    mapaddr1 = (unsigned long)ioremap(baseaddr[1], 16);

    memset(&hw, 0, sizeof(hw));
    
    for (i = IDE_DATA_OFFSET; i <= IDE_STATUS_OFFSET; i++)
        hw.io_ports[i] = mapaddr0 + (i<<1);
    
    hw.io_ports[IDE_CONTROL_OFFSET] = mapaddr1 + (6<<1);

    /* 设置中断引脚 */
    hw.irq = s3c2410_gpio_getirq(IDE_ARM_IRQPIN);
    s3c2410_gpio_cfgpin(IDE_ARM_IRQPIN, S3C2410_GPIO_IRQ);
    set_irq_type(hw.irq, IRQF_TRIGGER_RISING);

    /* 注册IDE接口 */
    ide_register_hw(&hw, 1, NULL);
}
#endif

void __init ide_arm_init(void)
{
#ifdef CONFIG_ARCH_S3C2410
    if (IDE_ARM_HOST) {
        ide_s3c24xx_init();
    }
#else    
	if (IDE_ARM_HOST) {
		hw_regs_t hw;

		memset(&hw, 0, sizeof(hw));
		ide_std_init_ports(&hw, IDE_ARM_IO, IDE_ARM_IO + 0x206);
		hw.irq = IDE_ARM_IRQ;
		ide_register_hw(&hw, 1, NULL);
	}
#endif    
}
