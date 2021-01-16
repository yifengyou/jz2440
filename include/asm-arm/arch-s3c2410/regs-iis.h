/* linux/include/asm-arm/arch-s3c2410/regs-iis.h
 *
 * Copyright (c) 2003 Simtec Electronics <linux@simtec.co.uk>
 *		      http://www.simtec.co.uk/products/SWLINUX/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * S3C2410 IIS register definition
 *
 *  Changelog:
 *    19-06-2003     BJD     Created file
 *    26-06-2003     BJD     Finished off definitions for register addresses
 *    12-03-2004     BJD     Updated include protection
 *    07-03-2005     BJD     Added FIFO size flags and S3C2440 MPLL
 *    05-04-2005     LCVR    Added IISFCON definitions for the S3C2400
 *    18-07-2005     DA      Change IISCON_MPLL to IISMOD_MPLL
 *                           Correct IISMOD_256FS and IISMOD_384FS
 *                           Add IISCON_PSCEN

 2005.06 godori from www.aesop-embedded.org(ghcstop@empal.com)
          fix for s3c2440a sound driver                        
*/

#ifndef __ASM_ARCH_REGS_IIS_H
#define __ASM_ARCH_REGS_IIS_H

#include "map.h" // ghcstop add 
#include "bitfield.h" // ghcstop add

#define S3C2410_IISCON	 (0x00)

#define S3C2410_IISCON_LRINDEX	  (1<<8)
#define S3C2410_IISCON_TXFIFORDY  (1<<7)
#define S3C2410_IISCON_RXFIFORDY  (1<<6)
#define S3C2410_IISCON_TXDMAEN	  (1<<5)
#define S3C2410_IISCON_RXDMAEN	  (1<<4)
#define S3C2410_IISCON_TXIDLE	  (1<<3)
#define S3C2410_IISCON_RXIDLE	  (1<<2)
#define S3C2410_IISCON_PSCEN	  (1<<1)
#define S3C2410_IISCON_IISEN	  (1<<0)

#define S3C2410_IISMOD	 (0x04)

#define S3C2440_IISMOD_MPLL	  (1<<9)
#define S3C2410_IISMOD_SLAVE	  (1<<8)
#define S3C2410_IISMOD_NOXFER	  (0<<6)
#define S3C2410_IISMOD_RXMODE	  (1<<6)
#define S3C2410_IISMOD_TXMODE	  (2<<6)
#define S3C2410_IISMOD_TXRXMODE	  (3<<6)
#define S3C2410_IISMOD_LR_LLOW	  (0<<5)
#define S3C2410_IISMOD_LR_RLOW	  (1<<5)
#define S3C2410_IISMOD_IIS	  (0<<4)
#define S3C2410_IISMOD_MSB	  (1<<4)
#define S3C2410_IISMOD_8BIT	  (0<<3)
#define S3C2410_IISMOD_16BIT	  (1<<3)
#define S3C2410_IISMOD_BITMASK	  (1<<3)
#define S3C2410_IISMOD_256FS	  (0<<2)
#define S3C2410_IISMOD_384FS	  (1<<2)
#define S3C2410_IISMOD_16FS	  (0<<0)
#define S3C2410_IISMOD_32FS	  (1<<0)
#define S3C2410_IISMOD_48FS	  (2<<0)
#define S3C2410_IISMOD_FS_MASK	  (3<<0)

#define S3C2410_IISPSR		(0x08)
#define S3C2410_IISPSR_INTMASK	(31<<5)
#define S3C2410_IISPSR_INTSHIFT	(5)
#define S3C2410_IISPSR_EXTMASK	(31<<0)
#define S3C2410_IISPSR_EXTSHFIT	(0)

#define S3C2410_IISFCON  (0x0c)

#define S3C2410_IISFCON_TXDMA	  (1<<15)
#define S3C2410_IISFCON_RXDMA	  (1<<14)
#define S3C2410_IISFCON_TXENABLE  (1<<13)
#define S3C2410_IISFCON_RXENABLE  (1<<12)
#define S3C2410_IISFCON_TXMASK	  (0x3f << 6)
#define S3C2410_IISFCON_TXSHIFT	  (6)
#define S3C2410_IISFCON_RXMASK	  (0x3f)
#define S3C2410_IISFCON_RXSHIFT	  (0)

#define S3C2400_IISFCON_TXDMA     (1<<11)
#define S3C2400_IISFCON_RXDMA     (1<<10)
#define S3C2400_IISFCON_TXENABLE  (1<<9)
#define S3C2400_IISFCON_RXENABLE  (1<<8)
#define S3C2400_IISFCON_TXMASK	  (0x07 << 4)
#define S3C2400_IISFCON_TXSHIFT	  (4)
#define S3C2400_IISFCON_RXMASK	  (0x07)
#define S3C2400_IISFCON_RXSHIFT	  (0)

#define S3C2410_IISFIFO  (0x10)



// ghcstop add, 임시 땜빵코드
#define IISCON			(*(volatile unsigned *)(S3C24XX_VA_IIS+0x00)) //IIS Control      
#define IISMOD			(*(volatile unsigned *)(S3C24XX_VA_IIS+0x04)) //IIS Mode         
#define IISPSR			(*(volatile unsigned *)(S3C24XX_VA_IIS+0x08)) //IIS Prescaler    
#define IISFIFOC		(*(volatile unsigned *)(S3C24XX_VA_IIS+0x0c)) //IIS FIFO control 
#define IISFIFOE		((volatile unsigned short *)(S3C24XX_VA_IIS+0x10)) // little endian, 실제사용시는 PA를 써야할 것 같다...2410 swl 커널 참조


#define IISCON_CH_RIGHT (1 << 8)        /* Right channel */
#define IISCON_CH_LEFT  (0 << 8)        /* Left channel */
#define IISCON_TX_RDY   (1 << 7)        /* Transmit FIFO is ready(not empty) */
#define IISCON_RX_RDY   (1 << 6)        /* Receive FIFO is ready (not full) */
#define IISCON_TX_DMA   (1 << 5)        /* Transmit DMA service reqeust */
#define IISCON_RX_DMA   (1 << 4)        /* Receive DMA service reqeust */
#define IISCON_TX_IDLE  (1 << 3)        /* Transmit Channel idle */
#define IISCON_RX_IDLE  (1 << 2)        /* Receive Channel idle */
#define IISCON_PRESCALE (1 << 1)        /* IIS Prescaler Enable */
#define IISCON_EN       (1 << 0)        /* IIS enable(start) */

#define IISMOD_SEL_PCLK (0 << 9)        /* Master clock 0:PCLK in */
#define IISMOD_SEL_MPLL (1 << 9)        /* Master clock MPLL in*/
#define IISMOD_SEL_MA   (0 << 8)        /* Master mode (IISLRCK, IISCLK are Output) */
#define IISMOD_SEL_SL   (1 << 8)        /* Slave mode  (IISLRCK, IISCLK are Input) */
#define fIISMOD_SEL_TR  Fld(2, 6)       /* Transmit/Receive mode */
#define IISMOD_SEL_TR   FMsk(fIISMOD_SEL_TR)
#define IISMOD_SEL_NO   FInsrt(0x0, fIISMOD_SEL_TR)     /* No Transfer */
#define IISMOD_SEL_RX   FInsrt(0x1, fIISMOD_SEL_TR)     /* Receive */
#define IISMOD_SEL_TX   FInsrt(0x2, fIISMOD_SEL_TR)     /* Transmit */
#define IISMOD_SEL_BOTH FInsrt(0x3, fIISMOD_SEL_TR)     /* Tx & Rx */
#define IISMOD_CH_RIGHT (0 << 5)        /* high for right channel */
#define IISMOD_CH_LEFT  (1 << 5)        /* high for left channel */
#define IISMOD_FMT_IIS  (0 << 4)        /* IIS-compatible format */
#define IISMOD_FMT_MSB  (1 << 4)        /* MSB(left)-justified format */
#define IISMOD_BIT_8    (0 << 3)        /* Serial data bit/channel is 8 bit*/
#define IISMOD_BIT_16   (1 << 3)        /* Serial data bit/channel is 16 bit*/
#define IISMOD_FREQ_256 (0 << 2)        /* Master clock freq = 256 fs */
#define IISMOD_FREQ_384 (1 << 2)        /* Master clock freq = 384 fs */
#define fIISMOD_SFREQ   Fld(2, 0)       /* Serial bit clock frequency */
#define IISMOD_SFREQ    FMsk(fIISMOD_SFREQ)     /* fs = sampling frequency */
#define IISMOD_SFREQ_16 FInsrt(0x0, fIISMOD_SFREQ)      /* 16 fs */
#define IISMOD_SFREQ_32 FInsrt(0x1, fIISMOD_SFREQ)      /* 32 fs */
#define IISMOD_SFREQ_48 FInsrt(0x2, fIISMOD_SFREQ)      /* 48 fs */

#define fIISPSR_A       Fld(5, 5)       /* Prescaler Control A */
#define IISPSR_A(x)     FInsrt((x), fIISPSR_A)
#define fIISPSR_B       Fld(5, 0)       /* Prescaler Control B */
#define IISPSR_B(x)     FInsrt((x), fIISPSR_B)  

#define IISFCON_TX_NORM (0 << 15)       /* Transmit FIFO access mode: normal */
#define IISFCON_TX_DMA  (1 << 15)       /* Transmit FIFO access mode: DMA */
#define IISFCON_RX_NORM (0 << 14)       /* Receive FIFO access mode: normal */
#define IISFCON_RX_DMA  (1 << 14)       /* Receive FIFO access mode: DMA */
#define IISFCON_TX_EN   (1 << 13)        /* Transmit FIFO enable */
#define IISFCON_RX_EN   (1 << 12)        /* Recevice FIFO enable */
#define fIISFCON_TX_CNT Fld(6, 6)       /* Tx FIFO data count (Read-Only) */
#define IISFCON_TX_CNT  FMsk(fIISFCON_TX_CNT)
#define fIISFCON_RX_CNT Fld(6, 0)       /* Rx FIFO data count (Read-Only) */
#define IISFCON_RX_CNT  FMsk(fIISFCON_RX_CNT)

#endif /* __ASM_ARCH_REGS_IIS_H */
