/*
 * (C) Copyright 2001-2004
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * (C) Copyright 2002
 * David Mueller, ELSOFT AG, d.mueller@elsoft.ch
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

/* This code should work for both the S3C2400 and the S3C2410
 * as they seem to have the same PLL and clock machinery inside.
 * The different address mapping is handled by the s3c24xx.h files below.
 */

#include <common.h>
#if defined(CONFIG_S3C2400) || defined (CONFIG_S3C2410) || defined (CONFIG_TRAB)

#if defined(CONFIG_S3C2400)
#include <s3c2400.h>
#elif defined(CONFIG_S3C2410)
#include <s3c2410.h>
#endif

DECLARE_GLOBAL_DATA_PTR;

#define MPLL 0
#define UPLL 1

/* ------------------------------------------------------------------------- */
/* NOTE: This describes the proper use of this file.
 *
 * CONFIG_SYS_CLK_FREQ should be defined as the input frequency of the PLL.
 *
 * get_FCLK(), get_HCLK(), get_PCLK() and get_UCLK() return the clock of
 * the specified bus in HZ.
 */
/* ------------------------------------------------------------------------- */

static ulong get_PLLCLK(int pllreg)
{
    S3C24X0_CLOCK_POWER * const clk_power = S3C24X0_GetBase_CLOCK_POWER();
    ulong r, m, p, s;

    if (pllreg == MPLL)
    r = clk_power->MPLLCON;
    else if (pllreg == UPLL)
    r = clk_power->UPLLCON;
    else
    hang();

    m = ((r & 0xFF000) >> 12) + 8;
    p = ((r & 0x003F0) >> 4) + 2;
    s = r & 0x3;

    /* support both of S3C2410 and S3C2440, by www.100ask.net */
    if (isS3C2410)
        return((CONFIG_SYS_CLK_FREQ * m) / (p << s));
    else
        return((CONFIG_SYS_CLK_FREQ * m * 2) / (p << s));   /* S3C2440 */
}

/* return FCLK frequency */
ulong get_FCLK(void)
{
    return(get_PLLCLK(MPLL));
}

/* for s3c2440 */
#define S3C2440_CLKDIVN_PDIVN        (1<<0)
#define S3C2440_CLKDIVN_HDIVN_MASK   (3<<1)
#define S3C2440_CLKDIVN_HDIVN_1      (0<<1)
#define S3C2440_CLKDIVN_HDIVN_2      (1<<1)
#define S3C2440_CLKDIVN_HDIVN_4_8    (2<<1)
#define S3C2440_CLKDIVN_HDIVN_3_6    (3<<1)
#define S3C2440_CLKDIVN_UCLK         (1<<3)

#define S3C2440_CAMDIVN_CAMCLK_MASK  (0xf<<0)
#define S3C2440_CAMDIVN_CAMCLK_SEL   (1<<4)
#define S3C2440_CAMDIVN_HCLK3_HALF   (1<<8)
#define S3C2440_CAMDIVN_HCLK4_HALF   (1<<9)
#define S3C2440_CAMDIVN_DVSEN        (1<<12)

/* return HCLK frequency */
ulong get_HCLK(void)
{
    S3C24X0_CLOCK_POWER * const clk_power = S3C24X0_GetBase_CLOCK_POWER();
    unsigned long clkdiv;
    unsigned long camdiv;
    int hdiv = 1;

    /* support both of S3C2410 and S3C2440, by www.100ask.net */
    if (isS3C2410)
        return((clk_power->CLKDIVN & 0x2) ? get_FCLK()/2 : get_FCLK());
    else
    {
        clkdiv = clk_power->CLKDIVN;
        camdiv = clk_power->CAMDIVN;

        /* work out clock scalings */

        switch (clkdiv & S3C2440_CLKDIVN_HDIVN_MASK) {
        case S3C2440_CLKDIVN_HDIVN_1:
            hdiv = 1;
            break;

        case S3C2440_CLKDIVN_HDIVN_2:
            hdiv = 2;
            break;

        case S3C2440_CLKDIVN_HDIVN_4_8:
            hdiv = (camdiv & S3C2440_CAMDIVN_HCLK4_HALF) ? 8 : 4;
            break;

        case S3C2440_CLKDIVN_HDIVN_3_6:
            hdiv = (camdiv & S3C2440_CAMDIVN_HCLK3_HALF) ? 6 : 3;
            break;
        }

        return get_FCLK() / hdiv;
    }
}

/* return PCLK frequency */
ulong get_PCLK(void)
{
    S3C24X0_CLOCK_POWER * const clk_power = S3C24X0_GetBase_CLOCK_POWER();
    unsigned long clkdiv;
    unsigned long camdiv;
    int hdiv = 1;

    /* support both of S3C2410 and S3C2440, by www.100ask.net */
    if (isS3C2410)
        return((clk_power->CLKDIVN & 0x1) ? get_HCLK()/2 : get_HCLK());
    else
    {   
        clkdiv = clk_power->CLKDIVN;
        camdiv = clk_power->CAMDIVN;

        /* work out clock scalings */

        switch (clkdiv & S3C2440_CLKDIVN_HDIVN_MASK) {
        case S3C2440_CLKDIVN_HDIVN_1:
            hdiv = 1;
            break;

        case S3C2440_CLKDIVN_HDIVN_2:
            hdiv = 2;
            break;

        case S3C2440_CLKDIVN_HDIVN_4_8:
            hdiv = (camdiv & S3C2440_CAMDIVN_HCLK4_HALF) ? 8 : 4;
            break;

        case S3C2440_CLKDIVN_HDIVN_3_6:
            hdiv = (camdiv & S3C2440_CAMDIVN_HCLK3_HALF) ? 6 : 3;
            break;
        }

        return get_FCLK() / hdiv / ((clkdiv & S3C2440_CLKDIVN_PDIVN)? 2:1);
    }        
}

/* return UCLK frequency */
ulong get_UCLK(void)
{
    return(get_PLLCLK(UPLL));
}

#endif /* defined(CONFIG_S3C2400) || defined (CONFIG_S3C2410) || defined (CONFIG_TRAB) */
