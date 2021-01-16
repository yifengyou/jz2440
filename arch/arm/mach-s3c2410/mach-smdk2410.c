/* linux/arch/arm/mach-s3c2410/mach-smdk2410.c
 *
 * linux/arch/arm/mach-s3c2410/mach-smdk2410.c
 *
 * Copyright (C) 2004 by FS Forth-Systeme GmbH
 * All rights reserved.
 *
 * $Id: mach-smdk2410.c,v 1.1 2004/05/11 14:15:38 mpietrek Exp $
 * @Author: Jonas Dietsche
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
 *
 * @History:
 * derived from linux/arch/arm/mach-s3c2410/mach-bast.c, written by
 * Ben Dooks <ben@simtec.co.uk>
 *
 ***********************************************************************/

#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/interrupt.h>
#include <linux/list.h>
#include <linux/timer.h>
#include <linux/init.h>
#include <linux/serial_core.h>
#include <linux/platform_device.h>

#include <asm/mach/arch.h>
#include <asm/mach/map.h>
#include <asm/mach/irq.h>

#include <asm/hardware.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/mach-types.h>

#include <asm/arch/regs-serial.h>

#include <asm/plat-s3c24xx/devs.h>
#include <asm/plat-s3c24xx/cpu.h>

#include <asm/plat-s3c24xx/common-smdk.h>
#include <asm/arch/fb.h>

static struct map_desc smdk2410_iodesc[] __initdata = {
  /* nothing here yet */
};

#define UCON S3C2410_UCON_DEFAULT
#define ULCON S3C2410_LCON_CS8 | S3C2410_LCON_PNONE | S3C2410_LCON_STOPB
#define UFCON S3C2410_UFCON_RXTRIG8 | S3C2410_UFCON_FIFOMODE

static struct s3c2410_uartcfg smdk2410_uartcfgs[] __initdata = {
	[0] = {
		.hwport	     = 0,
		.flags	     = 0,
		.ucon	     = UCON,
		.ulcon	     = ULCON,
		.ufcon	     = UFCON,
	},
	[1] = {
		.hwport	     = 1,
		.flags	     = 0,
		.ucon	     = UCON,
		.ulcon	     = ULCON,
		.ufcon	     = UFCON,
	},
	[2] = {
		.hwport	     = 2,
		.flags	     = 0,
		.ucon	     = UCON,
		.ulcon	     = ULCON,
		.ufcon	     = UFCON,
	}
};

/* LCD driver info, add by www.100ask.net */

static struct s3c2410fb_mach_info smdk2410_lcd_cfg __initdata = {
    .regs   = {
        .lcdcon1 =  S3C2410_LCDCON1_TFT16BPP | \
                S3C2410_LCDCON1_TFT | \
                S3C2410_LCDCON1_CLKVAL(0x04),

        .lcdcon2 =  S3C2410_LCDCON2_VBPD(1) | \
                S3C2410_LCDCON2_LINEVAL(319) | \
                S3C2410_LCDCON2_VFPD(5) | \
                S3C2410_LCDCON2_VSPW(1),

        .lcdcon3 =  S3C2410_LCDCON3_HBPD(36) | \
                S3C2410_LCDCON3_HOZVAL(239) | \
                S3C2410_LCDCON3_HFPD(19),

        .lcdcon4 =  S3C2410_LCDCON4_MVAL(13) | \
                S3C2410_LCDCON4_HSPW(5),

        .lcdcon5 =  S3C2410_LCDCON5_FRM565 |
                S3C2410_LCDCON5_INVVLINE |
                S3C2410_LCDCON5_INVVFRAME |
                S3C2410_LCDCON5_PWREN |
                S3C2410_LCDCON5_HWSWP,
    },

    .gpccon      =  0xaaaaaaaa,
    .gpccon_mask =  0xffffffff,
    .gpcup       =  0xffffffff,
    .gpcup_mask  =  0xffffffff,

    .gpdcon      =  0xaaaaaaaa,
    .gpdcon_mask =  0xffffffff,
    .gpdup       =  0xffffffff,
    .gpdup_mask  =  0xffffffff,

    .fixed_syncs =  1,
    .type        =  S3C2410_LCDCON1_TFT, 
    .width       =  240,
    .height      =  320,

    .xres   = {
        .min    =   240,
        .max    =   240,
        .defval =   240,
    },

    .yres   = {
        .max    =   320,
        .min    =   320,
        .defval =   320,
    },

    .bpp    = {
        .min    =   16,
        .max    =   16,
        .defval =   16,
    },
};


/* SDI */
static struct resource s3c2410_sdi_resource[] = {
    [0] = {
        .start = S3C2410_PA_SDI,
        .end   = S3C2410_PA_SDI + S3C24XX_SZ_SDI - 1,
        .flags = IORESOURCE_MEM,
    },
    [1] = {
        .start = IRQ_SDI,
        .end   = IRQ_SDI,
        .flags = IORESOURCE_IRQ,
    }

};

static struct platform_device s3c2410_device_sdi = {
    .name         = "s3c2410-sdi",
    .id       = -1,
    .num_resources    = ARRAY_SIZE(s3c2410_sdi_resource),
    .resource     = s3c2410_sdi_resource,
};


static struct platform_device *smdk2410_devices[] __initdata = {
	&s3c_device_usb,
	&s3c_device_lcd,
	&s3c_device_wdt,
	&s3c_device_i2c,
	&s3c_device_iis,
    &s3c2410_device_sdi,
};

static void __init smdk2410_map_io(void)
{
	s3c24xx_init_io(smdk2410_iodesc, ARRAY_SIZE(smdk2410_iodesc));
	s3c24xx_init_clocks(0);
	s3c24xx_init_uarts(smdk2410_uartcfgs, ARRAY_SIZE(smdk2410_uartcfgs));
}

static void __init smdk2410_init(void)
{
    s3c24xx_fb_set_platdata(&smdk2410_lcd_cfg); // add by www.100ask.net
	platform_add_devices(smdk2410_devices, ARRAY_SIZE(smdk2410_devices));
	smdk_machine_init();
}

MACHINE_START(SMDK2410, "SMDK2410") /* @TODO: request a new identifier and switch
				    * to SMDK2410 */
	/* Maintainer: Jonas Dietsche */
	.phys_io	= S3C2410_PA_UART,
	.io_pg_offst	= (((u32)S3C24XX_VA_UART) >> 18) & 0xfffc,
	.boot_params	= S3C2410_SDRAM_PA + 0x100,
	.map_io		= smdk2410_map_io,
	.init_irq	= s3c24xx_init_irq,
	.init_machine	= smdk2410_init,
	.timer		= &s3c24xx_timer,
MACHINE_END


