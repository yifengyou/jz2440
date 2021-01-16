/*
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Alex Zuepke <azu@sysgo.de>
 *
 * (C) Copyright 2002
 * Gary Jennejohn, DENX Software Engineering, <gj@denx.de>
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

#include <common.h>
#if defined(CONFIG_S3C2400) || defined (CONFIG_S3C2410) || defined (CONFIG_TRAB)

#include <arm920t.h>
#if defined(CONFIG_S3C2400)
#include <s3c2400.h>
#elif defined(CONFIG_S3C2410)
#include <s3c2410.h>
#endif

int timer_load_val = 0;

/* macro to read the 16 bit timer */
static inline ulong READ_TIMER(void)
{
	S3C24X0_TIMERS * const timers = S3C24X0_GetBase_TIMERS();

	return (timers->TCNTO4 & 0xffff);
}

static ulong timestamp;
static ulong lastdec;

int interrupt_init (void)
{
	S3C24X0_TIMERS * const timers = S3C24X0_GetBase_TIMERS();

	/* use PWM Timer 4 because it has no output */
	/* prescaler for Timer 4 is 16 */
	timers->TCFG0 = 0x0f00;
	if (timer_load_val == 0)
	{
		/*
		 * for 10 ms clock period @ PCLK with 4 bit divider = 1/2
		 * (default) and prescaler = 16. Should be 10390
		 * @33.25MHz and 15625 @ 50 MHz
		 */
		timer_load_val = get_PCLK()/(2 * 16 * 100);
	}
	/* load value for 10 ms timeout */
	lastdec = timers->TCNTB4 = timer_load_val;
	/* auto load, manual update of Timer 4 */
	timers->TCON = (timers->TCON & ~0x0700000) | 0x600000;
	/* auto load, start Timer 4 */
	timers->TCON = (timers->TCON & ~0x0700000) | 0x500000;
	timestamp = 0;

	return (0);
}

/*
 * timer without interrupts
 */

void reset_timer (void)
{
	reset_timer_masked ();
}

ulong get_timer (ulong base)
{
	return get_timer_masked () - base;
}

void set_timer (ulong t)
{
	timestamp = t;
}

void udelay (unsigned long usec)
{
	ulong tmo;
	ulong start = get_timer(0);

	tmo = usec / 1000;
	tmo *= (timer_load_val * 100);
	tmo /= 1000;

	while ((ulong)(get_timer_masked () - start) < tmo)
		/*NOP*/;
}

void reset_timer_masked (void)
{
	/* reset time */
	lastdec = READ_TIMER();
	timestamp = 0;
}

ulong get_timer_masked (void)
{
	ulong now = READ_TIMER();

	if (lastdec >= now) {
		/* normal mode */
		timestamp += lastdec - now;
	} else {
		/* we have an overflow ... */
		timestamp += lastdec + timer_load_val - now;
	}
	lastdec = now;

	return timestamp;
}

void udelay_masked (unsigned long usec)
{
	ulong tmo;
	ulong endtime;
	signed long diff;

	if (usec >= 1000) {
		tmo = usec / 1000;
		tmo *= (timer_load_val * 100);
		tmo /= 1000;
	} else {
		tmo = usec * (timer_load_val * 100);
		tmo /= (1000*1000);
	}

	endtime = get_timer_masked () + tmo;

	do {
		ulong now = get_timer_masked ();
		diff = endtime - now;
	} while (diff >= 0);
}

/*
 * This function is derived from PowerPC code (read timebase as long long).
 * On ARM it just returns the timer value.
 */
unsigned long long get_ticks(void)
{
	return get_timer(0);
}

/*
 * This function is derived from PowerPC code (timebase clock frequency).
 * On ARM it returns the number of timer ticks per second.
 */
ulong get_tbclk (void)
{
	ulong tbclk;

#if defined(CONFIG_SMDK2400) || defined(CONFIG_TRAB)
	tbclk = timer_load_val * 100;
#elif defined(CONFIG_SBC2410X) || \
      defined(CONFIG_SMDK2410) || \
      defined(CONFIG_VCMA9)
	tbclk = CFG_HZ;
#else
#	error "tbclk not configured"
#endif

	return tbclk;
}

/*
 * reset the cpu by setting up the watchdog timer and let him time out
 */
void reset_cpu (ulong ignored)
{
	volatile S3C24X0_WATCHDOG * watchdog;

#ifdef CONFIG_TRAB
	extern void disable_vfd (void);

	disable_vfd();
#endif

	watchdog = S3C24X0_GetBase_WATCHDOG();

	/* Disable watchdog */
	watchdog->WTCON = 0x0000;

	/* Initialize watchdog timer count register */
	watchdog->WTCNT = 0x0001;

	/* Enable watchdog timer; assert reset at timer timeout */
	watchdog->WTCON = 0x0021;

	while(1);	/* loop forever and wait for reset to happen */

	/*NOTREACHED*/
}

/* Start : add by www.100ask.net */
void (*isr_handle_array[50])(void);
S3C24X0_INTERRUPT * intregs;
    
extern void IsrUsbd(void);
extern void IsrDma2(void);

/************************* Timer ********************************/
static int intCount;

void IsrWatchdog(void);

void ClearPending(int bit) 
{
    intregs->SRCPND = bit;
    intregs->INTPND = bit;
}

void Timer_InitEx(void)
{
	intCount=0;	
	intregs->SUBSRCPND	= (1<<13);
	ClearPending(BIT_WDT_AC97/*BIT_WDT*/);
	intregs->INTMSK&=~(BIT_WDT_AC97 /*BIT_WDT*/);
	intregs->INTSUBMSK &= ~(1<<13);
}


void Timer_StartEx(void)
{
    S3C24X0_WATCHDOG * const wdtregs = S3C24X0_GetBase_WATCHDOG();
    
	wdtregs->WTCON=((get_PCLK()/1000000-1)<<8)|(0<<3)|(1<<2);	// 16us
	wdtregs->WTDAT=0xffff;
	wdtregs->WTCNT=0xffff;   

	// 1/16/(65+1),interrupt enable,reset disable,watchdog enable
	wdtregs->WTCON=((get_PCLK()/1000000-1)<<8)|(0<<3)|(1<<2)|(0<<0)|(1<<5);   
}

unsigned int Timer_StopEx(void)
{
	int count;
    S3C24X0_WATCHDOG * const wdtregs = S3C24X0_GetBase_WATCHDOG();

	wdtregs->WTCON=((get_PCLK()/1000000-1)<<8);
	intregs->INTMSK|=BIT_WDT_AC97; //BIT_WDT;
	intregs->INTSUBMSK |= (1<<13);
	
	count=(0xffff-wdtregs->WTCNT)+(intCount*0xffff);
	return ((unsigned int)count*16/1000000);
}


void  IsrWatchdog(void)
{
	intregs->SUBSRCPND	= (1<<13);
	ClearPending(BIT_WDT_AC97 /* BIT_WDT */);
	intCount++;   	
}

int g_TimerIntHappen;
void  IsrTimer4(void)
{
	ClearPending(BIT_TIMER4);
	*(volatile int *)&g_TimerIntHappen = 1;
}


void Dummy_isr(void)
{
    printf("Dummy_isr error, interrupt number: %d, INTMSK = 0x%x\n", intregs->INTOFFSET, intregs->INTMSK);
    while(1);
}

void Isr_Init(void)
{
    int i = 0;
    intregs = S3C24X0_GetBase_INTERRUPT();
    
    for (i = 0; i < sizeof(isr_handle_array) / sizeof(isr_handle_array[0]); i++ )
    {
        isr_handle_array[i] = Dummy_isr;
    }

    intregs->INTMOD=0x0;	      // All=IRQ mode
    intregs->INTMSK=BIT_ALLMSK;	  // All interrupt is masked.

    //pISR_URXD0=(unsigned)Uart0_RxInt;	
    //rINTMSK=~(BIT_URXD0);   //enable UART0 RX Default value=0xffffffff

	isr_handle_array[ISR_TIMER4_OFT] = IsrTimer4;
	isr_handle_array[ISR_WDT_OFT]  = IsrWatchdog;

#ifdef CONFIG_USB_DEVICE
    isr_handle_array[ISR_USBD_OFT] = IsrUsbd;
    isr_handle_array[ISR_DMA2_OFT] = IsrDma2;
    ClearPending(BIT_DMA2);
    ClearPending(BIT_USBD);
#endif    
}


void IRQ_Handle()
{
	unsigned long oft = intregs->INTOFFSET;
    S3C24X0_GPIO * const gpio = S3C24X0_GetBase_GPIO();
    
//    printk("IRQ_Handle: %d\n", oft);
    
	//清中断
	if( oft == 4 ) gpio->EINTPEND = 1<<7;		//EINT4-7合用IRQ4，注意EINTPEND[3:0]保留未用，向这些位写入1可能导致未知结果
	intregs->SRCPND = 1<<oft;	
	intregs->INTPND	= intregs->INTPND;	 

    /* run the isr */
    isr_handle_array[oft]();
}

/* End : add by www.100ask.net */

#endif /* defined(CONFIG_S3C2400) || defined (CONFIG_S3C2410) || defined (CONFIG_TRAB) */
