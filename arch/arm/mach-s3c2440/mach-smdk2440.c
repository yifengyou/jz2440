/* linux/arch/arm/mach-s3c2440/mach-smdk2440.c
 *
 * Copyright (c) 2004,2005 Simtec Electronics
 *	Ben Dooks <ben@simtec.co.uk>
 *
 * http://www.fluff.org/ben/smdk2440/
 *
 * Thanks to Dimity Andric and TomTom for the loan of an SMDK2440.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
*/

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
#include <asm/arch/regs-gpio.h>
#include <asm/arch/regs-lcd.h>

#include <asm/arch/idle.h>
#include <asm/arch/fb.h>

#include <asm/plat-s3c24xx/s3c2410.h>
#include <asm/plat-s3c24xx/s3c2440.h>
#include <asm/plat-s3c24xx/clock.h>
#include <asm/plat-s3c24xx/devs.h>
#include <asm/plat-s3c24xx/cpu.h>

#include <asm/plat-s3c24xx/common-smdk.h>

static struct map_desc smdk2440_iodesc[] __initdata = {
	/* ISA IO Space map (memory space selected by A24) */

	{
		.virtual	= (u32)S3C24XX_VA_ISA_WORD,
		.pfn		= __phys_to_pfn(S3C2410_CS2),
		.length		= 0x10000,
		.type		= MT_DEVICE,
	}, {
		.virtual	= (u32)S3C24XX_VA_ISA_WORD + 0x10000,
		.pfn		= __phys_to_pfn(S3C2410_CS2 + (1<<24)),
		.length		= SZ_4M,
		.type		= MT_DEVICE,
	}, {
		.virtual	= (u32)S3C24XX_VA_ISA_BYTE,
		.pfn		= __phys_to_pfn(S3C2410_CS2),
		.length		= 0x10000,
		.type		= MT_DEVICE,
	}, {
		.virtual	= (u32)S3C24XX_VA_ISA_BYTE + 0x10000,
		.pfn		= __phys_to_pfn(S3C2410_CS2 + (1<<24)),
		.length		= SZ_4M,
		.type		= MT_DEVICE,
	}
};

#define UCON S3C2410_UCON_DEFAULT | S3C2410_UCON_UCLK
#define ULCON S3C2410_LCON_CS8 | S3C2410_LCON_PNONE | S3C2410_LCON_STOPB
#define UFCON S3C2410_UFCON_RXTRIG8 | S3C2410_UFCON_FIFOMODE

static struct s3c2410_uartcfg smdk2440_uartcfgs[] __initdata = {
	[0] = {
		.hwport	     = 0,
		.flags	     = 0,
		.ucon	     = 0x3c5,
		.ulcon	     = 0x03,
		.ufcon	     = 0x51,
	},
	[1] = {
		.hwport	     = 1,
		.flags	     = 0,
		.ucon	     = 0x3c5,
		.ulcon	     = 0x03,
		.ufcon	     = 0x51,
	},
	/* IR port */
	[2] = {
		.hwport	     = 2,
		.flags	     = 0,
		.ucon	     = 0x3c5,
//		  .ulcon	   = 0x43,	// www.100ask.net
		 .ulcon	   = 0x33,
		.ufcon	     = 0x51,
	}
};

/* LCD driver info */
static struct s3c2410fb_mach_info smdk2440_lcd_cfg_1024x768 __initdata = {
	.regs	= {
		.lcdcon1 =	S3C2410_LCDCON1_TFT16BPP | \
				S3C2410_LCDCON1_TFT | \
				S3C2410_LCDCON1_CLKVAL(0x01),

		.lcdcon2 =	S3C2410_LCDCON2_VBPD(1) | \
				S3C2410_LCDCON2_LINEVAL(1023) | \
				S3C2410_LCDCON2_VFPD(1) | \
				S3C2410_LCDCON2_VSPW(1),

		.lcdcon3 =	S3C2410_LCDCON3_HBPD(15) | \
				S3C2410_LCDCON3_HOZVAL(767) | \
				S3C2410_LCDCON3_HFPD(199),

		.lcdcon4 =	S3C2410_LCDCON4_MVAL(13) | \
				S3C2410_LCDCON4_HSPW(15),
#if 0
		.lcdcon5 =	S3C2410_LCDCON5_FRM565 |
				S3C2410_LCDCON5_INVVLINE |
				S3C2410_LCDCON5_INVVFRAME |
				S3C2410_LCDCON5_PWREN |
				S3C2410_LCDCON5_HWSWP,
#else
		.lcdcon5 =	S3C2410_LCDCON5_FRM565 |
				S3C2410_LCDCON5_HWSWP,
#endif
	},

	.lpcsel =	0xf82,

	.gpccon =	0xaa955699,
	.gpccon_mask =	0xffc003cc,
	.gpcup =	0x0000ffff,
	.gpcup_mask =	0xffffffff,

	.gpdcon =	0xaa95aaa1,
	.gpdcon_mask =	0xffc0fff0,
	.gpdup =	0x0000faff,
	.gpdup_mask =	0xffffffff,

	.fixed_syncs =	1,
        .type        =  S3C2410_LCDCON1_TFT, 
	.width  =	1024,
	.height =	768,

	.xres	= {
		.min =		1024,
		.max =		1024,
		.defval =	1024,
	},

	.yres	= {
		.max =		768,
		.min =		768,
		.defval	=	768,
	},

	.bpp	= {
		.min =		16,
		.max =		16,
		.defval =	16,
	},
};

/* 240x320 */
static struct s3c2410fb_mach_info smdk2440_lcd_cfg_240x320 __initdata = {
    .regs   = {
        .lcdcon1 =  S3C2410_LCDCON1_TFT16BPP | \
                S3C2410_LCDCON1_TFT | \
				  S3C2410_LCDCON1_CLKVAL(0x04),

        .lcdcon2 =  S3C2410_LCDCON2_VBPD(5) | \
                S3C2410_LCDCON2_LINEVAL(319) | \
                S3C2410_LCDCON2_VFPD(3) | \
                S3C2410_LCDCON2_VSPW(1),

        .lcdcon3 =  S3C2410_LCDCON3_HBPD(10) | \
                S3C2410_LCDCON3_HOZVAL(239) | \
                S3C2410_LCDCON3_HFPD(1),

        .lcdcon4 =  S3C2410_LCDCON4_MVAL(13) | \
                S3C2410_LCDCON4_HSPW(0),

		.lcdcon5	= S3C2410_LCDCON5_FRM565 |
				  S3C2410_LCDCON5_INVVLINE |
				  S3C2410_LCDCON5_INVVFRAME |
				  S3C2410_LCDCON5_PWREN |
				  S3C2410_LCDCON5_HWSWP,
	},

    .gpccon      =  0xaaaa56aa,
	.gpccon_mask	= 0xffffffff,
    .gpcup       =  0xffffffff,
	.gpcup_mask	= 0xffffffff,

    .gpdcon      =  0xaaaaaaaa,
	.gpdcon_mask	= 0xffffffff,
    .gpdup       =  0xffffffff,
	.gpdup_mask	= 0xffffffff,

    .fixed_syncs =  1,
    .type        =  S3C2410_LCDCON1_TFT, 
	.width		= 240,
	.height		= 320,

	.xres		= {
		.min	= 240,
		.max	= 240,
		.defval	= 240,
	},

	.yres		= {
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

/* 480x272 */
static struct s3c2410fb_mach_info smdk2440_lcd_cfg __initdata = {
    .regs   = {
        .lcdcon1 =  S3C2410_LCDCON1_TFT16BPP | \
                S3C2410_LCDCON1_TFT | \
				  S3C2410_LCDCON1_CLKVAL(0x04),

        .lcdcon2 =  S3C2410_LCDCON2_VBPD(1) | \
                S3C2410_LCDCON2_LINEVAL(271) | \
                S3C2410_LCDCON2_VFPD(1) | \
                S3C2410_LCDCON2_VSPW(9),

        .lcdcon3 =  S3C2410_LCDCON3_HBPD(1) | \
                S3C2410_LCDCON3_HOZVAL(479) | \
                S3C2410_LCDCON3_HFPD(1),

        .lcdcon4 =  S3C2410_LCDCON4_HSPW(40),

		.lcdcon5	= S3C2410_LCDCON5_FRM565 |
				  S3C2410_LCDCON5_INVVLINE |
				  S3C2410_LCDCON5_INVVFRAME |
				  S3C2410_LCDCON5_PWREN |
				  S3C2410_LCDCON5_HWSWP,
	},

    .gpccon      =  0xaaaaaaaa,
	.gpccon_mask	= 0xffffffff,
    .gpcup       =  0xffffffff,
	.gpcup_mask	= 0xffffffff,

    .gpdcon      =  0xaaaaaaaa,
	.gpdcon_mask	= 0xffffffff,
    .gpdup       =  0xffffffff,
	.gpdup_mask	= 0xffffffff,

    .fixed_syncs =  1,
    .type        =  S3C2410_LCDCON1_TFT, 
	.width		= 480,
	.height		= 272,

	.xres		= {
		.min	= 480,
		.max	= 480,
		.defval	= 480,
	},

	.yres		= {
        .max    =   272,
        .min    =   272,
        .defval =   272,
    },

    .bpp    = {
        .min    =   16,
        .max    =   16,
        .defval =   16,
    },
};


/* 320x240 */
static struct s3c2410fb_mach_info smdk2440_lcd_cfg_320x240 __initdata = {
    .regs   = {
        .lcdcon1 =  S3C2410_LCDCON1_TFT16BPP | \
                S3C2410_LCDCON1_TFT | \
                S3C2410_LCDCON1_CLKVAL(0x04),

        .lcdcon2 =  S3C2410_LCDCON2_VBPD(1) | \
                S3C2410_LCDCON2_LINEVAL(239) | \
                S3C2410_LCDCON2_VFPD(5) | \
                S3C2410_LCDCON2_VSPW(1),

        .lcdcon3 =  S3C2410_LCDCON3_HBPD(36) | \
                S3C2410_LCDCON3_HOZVAL(319) | \
                S3C2410_LCDCON3_HFPD(19),

        .lcdcon4 =  S3C2410_LCDCON4_MVAL(13) | \
                S3C2410_LCDCON4_HSPW(5),

        .lcdcon5 =  S3C2410_LCDCON5_FRM565 |
                S3C2410_LCDCON5_INVVLINE |
                S3C2410_LCDCON5_INVVFRAME |
				S3C2410_LCDCON5_INVVDEN |
                S3C2410_LCDCON5_PWREN |
                S3C2410_LCDCON5_HWSWP,
    },

    .gpccon      =  0xaaaa56aa,
    .gpccon_mask =  0xffffffff,
    .gpcup       =  0xffffffff,
    .gpcup_mask  =  0xffffffff,

    .gpdcon      =  0xaaaaaaaa,
    .gpdcon_mask =  0xffffffff,
    .gpdup       =  0xffffffff,
    .gpdup_mask  =  0xffffffff,

    .fixed_syncs =  1,
    .type        =  S3C2410_LCDCON1_TFT, 
    .width       =  320,
    .height      =  240,

    .xres   = {
		.min	= 320,
		.max	= 320,
		.defval = 320,
	},

    .yres   = {
        .max    =   240,
        .min    =   240,
        .defval =   240,
    },

	.bpp		= {
		.min	= 16,
		.max	= 16,
		.defval = 16,
	},
};


/* SDI */
static struct resource s3c2440_sdi_resource[] = {
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

static struct platform_device s3c2440_device_sdi = {
    .name         = "s3c2440-sdi",
    .id       = -1,
    .num_resources    = ARRAY_SIZE(s3c2440_sdi_resource),
    .resource     = s3c2440_sdi_resource,
};

static struct platform_device *smdk2440_devices[] __initdata = {
	&s3c_device_usb,
	&s3c_device_lcd,
	&s3c_device_wdt,
	&s3c_device_i2c,
	&s3c_device_iis,
    &s3c2440_device_sdi,
};

static void __init smdk2440_map_io(void)
{
	s3c24xx_init_io(smdk2440_iodesc, ARRAY_SIZE(smdk2440_iodesc));
    s3c24xx_init_clocks(12000000);
	s3c24xx_init_uarts(smdk2440_uartcfgs, ARRAY_SIZE(smdk2440_uartcfgs));
}

static void __init smdk2440_machine_init(void)
{
	s3c24xx_fb_set_platdata(&smdk2440_lcd_cfg);

	platform_add_devices(smdk2440_devices, ARRAY_SIZE(smdk2440_devices));
	smdk_machine_init();
}

MACHINE_START(S3C2440, "SMDK2440")
	/* Maintainer: Ben Dooks <ben@fluff.org> */
	.phys_io	= S3C2410_PA_UART,
	.io_pg_offst	= (((u32)S3C24XX_VA_UART) >> 18) & 0xfffc,
	.boot_params	= S3C2410_SDRAM_PA + 0x100,

	.init_irq	= s3c24xx_init_irq,
	.map_io		= smdk2440_map_io,
	.init_machine	= smdk2440_machine_init,
	.timer		= &s3c24xx_timer,
MACHINE_END
