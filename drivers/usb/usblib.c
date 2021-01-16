/****************************************************************
 NAME: usblib.c
 DESC: S3C2440X USB library functions
 HISTORY:
 Mar.25.2002:purnnamu: ported for S3C2410X.
 Mar.27.2002:purnnamu: DMA is enabled.
 ****************************************************************/
#include <common.h>
#if defined(CONFIG_S3C2400)
#include <s3c2400.h>
#elif defined(CONFIG_S3C2410)
#include <s3c2410.h>
#endif

#include "2440usb.h"
#include "usbmain.h"
#include "usblib.h"
#include "usbsetup.h"
#include "usbmain.h"

extern volatile U32 dwUSBBufReadPtr;
extern volatile U32 dwUSBBufWritePtr;
extern volatile U32 dwPreDMACnt;
extern volatile U32 dwNextDMACnt;

extern S3C24X0_INTERRUPT * intregs;
extern S3C24X0_USB_DEVICE * usbdevregs;
extern S3C24X0_DMAS * dmaregs;

void ConfigUsbd(void)
{

    ReconfigUsbd();
/*
    pISR_USBD =(unsigned)IsrUsbd;
    ClearPending(BIT_USBD);
    rINTMSK&=~(BIT_USBD);  
*/   
    intregs->INTMSK&=~(BIT_USBD);  
}


void ReconfigUsbd(void)
{
// *** End point information ***
//   EP0: control
//   EP1: bulk in end point
//   EP2: not used
//   EP3: bulk out end point
//   EP4: not used
    
    usbdevregs->PWR_REG=PWR_REG_DEFAULT_VALUE;	//disable suspend mode

    usbdevregs->INDEX_REG=0;	
    usbdevregs->MAXP_REG=FIFO_SIZE_8;   	//EP0 max packit size = 8 
    usbdevregs->EP0_CSR_IN_CSR1_REG=EP0_SERVICED_OUT_PKT_RDY|EP0_SERVICED_SETUP_END;	
 				//EP0:clear OUT_PKT_RDY & SETUP_END
    usbdevregs->INDEX_REG=1;
    #if (EP1_PKT_SIZE==32)
        usbdevregs->MAXP_REG=FIFO_SIZE_32;	//EP1:max packit size = 32
    #else
	usbdevregs->MAXP_REG=FIFO_SIZE_64;	//EP1:max packit size = 64
    #endif	
    usbdevregs->EP0_CSR_IN_CSR1_REG=EPI_FIFO_FLUSH|EPI_CDT;	
    usbdevregs->IN_CSR2_REG=EPI_MODE_IN|EPI_IN_DMA_INT_MASK|EPI_BULK; //IN mode, IN_DMA_INT=masked    
    usbdevregs->OUT_CSR1_REG=EPO_CDT;   	
    usbdevregs->OUT_CSR2_REG=EPO_BULK|EPO_OUT_DMA_INT_MASK;   	

    usbdevregs->INDEX_REG=2;
    usbdevregs->MAXP_REG=FIFO_SIZE_64;	//EP2:max packit size = 64
    usbdevregs->EP0_CSR_IN_CSR1_REG=EPI_FIFO_FLUSH|EPI_CDT|EPI_BULK;
    usbdevregs->IN_CSR2_REG=EPI_MODE_IN|EPI_IN_DMA_INT_MASK; //IN mode, IN_DMA_INT=masked    
    usbdevregs->OUT_CSR1_REG=EPO_CDT;   	
    usbdevregs->OUT_CSR2_REG=EPO_BULK|EPO_OUT_DMA_INT_MASK;   	

    usbdevregs->INDEX_REG=3;
    #if (EP3_PKT_SIZE==32)
        usbdevregs->MAXP_REG=FIFO_SIZE_32;	//EP3:max packit size = 32
    #else
	usbdevregs->MAXP_REG=FIFO_SIZE_64;	//EP3:max packit size = 64
    #endif	
    usbdevregs->EP0_CSR_IN_CSR1_REG=EPI_FIFO_FLUSH|EPI_CDT|EPI_BULK;
    usbdevregs->IN_CSR2_REG=EPI_MODE_OUT|EPI_IN_DMA_INT_MASK; //OUT mode, IN_DMA_INT=masked    
    usbdevregs->OUT_CSR1_REG=EPO_CDT;   	
    	//clear OUT_PKT_RDY, data_toggle_bit.
	//The data toggle bit should be cleared when initialization.
    usbdevregs->OUT_CSR2_REG=EPO_BULK|EPO_OUT_DMA_INT_MASK;   	

    usbdevregs->INDEX_REG=4;
    usbdevregs->MAXP_REG=FIFO_SIZE_64;	//EP4:max packit size = 64
    usbdevregs->EP0_CSR_IN_CSR1_REG=EPI_FIFO_FLUSH|EPI_CDT|EPI_BULK;
    usbdevregs->IN_CSR2_REG=EPI_MODE_OUT|EPI_IN_DMA_INT_MASK; //OUT mode, IN_DMA_INT=masked    
    usbdevregs->OUT_CSR1_REG=EPO_CDT;   	
    	//clear OUT_PKT_RDY, data_toggle_bit.
	//The data toggle bit should be cleared when initialization.
    usbdevregs->OUT_CSR2_REG=EPO_BULK|EPO_OUT_DMA_INT_MASK;   	
    
    usbdevregs->EP_INT_REG=EP0_INT|EP1_INT|EP2_INT|EP3_INT|EP4_INT;
    usbdevregs->USB_INT_REG=RESET_INT|SUSPEND_INT|RESUME_INT; 
    	//Clear all usbd pending bits
    	
    //EP0,1,3 & reset interrupt are enabled
    usbdevregs->EP_INT_EN_REG=EP0_INT|EP1_INT|EP3_INT;
    usbdevregs->USB_INT_EN_REG=RESET_INT;
    ep0State=EP0_STATE_INIT;
    
}


void RdPktEp0(U8 *buf,int num)
{
    int i;
    	
    for(i=0;i<num;i++)
    {
        buf[i]=(U8)usbdevregs->fifo[0].EP_FIFO_REG;
    }
}
    

void WrPktEp0(U8 *buf,int num)
{
    int i;
    	
    for(i=0;i<num;i++)
    {
        usbdevregs->fifo[0].EP_FIFO_REG=buf[i];	
    }
}


void WrPktEp1(U8 *buf,int num)
{
    int i;
    	
    for(i=0;i<num;i++)
    {
        usbdevregs->fifo[1].EP_FIFO_REG=buf[i];	
    }
}


void WrPktEp2(U8 *buf,int num)
{
    int i;
    	
    for(i=0;i<num;i++)
    {
        usbdevregs->fifo[2].EP_FIFO_REG=buf[i];	
    }
}


void RdPktEp3(U8 *buf,int num)
{
    int i;
    	
    for(i=0;i<num;i++)
    {
        buf[i]=(U8)usbdevregs->fifo[3].EP_FIFO_REG;	
    }
}


void RdPktEp4(U8 *buf,int num)
{
    int i;
    	
    for(i=0;i<num;i++)
    {
        buf[i]=(U8)usbdevregs->fifo[4].EP_FIFO_REG;	
    }
}


void ConfigEp3DmaMode(U32 bufAddr,U32 count)
{
    int i;

    usbdevregs->INDEX_REG=3;
    count=count&0xfffff; //transfer size should be <1MB
    
    dmaregs->dma[2].DISRCC=(1<<1)|(1<<0);
    dmaregs->dma[2].DISRC=ADDR_EP3_FIFO; //src=APB,fixed,src=EP3_FIFO
    dmaregs->dma[2].DIDSTC=(0<<1)|(0<<0);  
    dmaregs->dma[2].DIDST=bufAddr;       //dst=AHB,increase,dst=bufAddr
#if USBDMA_DEMAND
    dmaregs->dma[2].DCON=(count)|(0<<31)|(0<<30)|(1<<29)|(0<<28)|(0<<27)|(4<<24)|(1<<23)|(0<<22)|(0<<20); 
        //demand,requestor=APB,CURR_TC int enable,unit transfer,
        //single service,src=USBD,H/W request,autoreload,byte,CURR_TC
#else
//    rDCON2=(count)|(1<<31)|(0<<30)|(1<<29)|(0<<28)|(0<<27)|(4<<24)|(1<<23)|(0<<22)|(0<<20); 
    /* changed by thisway.diy to disable autoreload */
    dmaregs->dma[2].DCON=(count)|(1<<31)|(0<<30)|(1<<29)|(0<<28)|(0<<27)|(4<<24)|(1<<23)|(1<<22)|(0<<20); 
        //handshake,requestor=APB,CURR_TC int enable,unit transfer,
        //single service,src=USBD,H/W request,autoreload,byte,CURR_TC
#endif        
    dmaregs->dma[2].DMASKTRIG= (1<<1); 
        //DMA 2 on

    //rEP3_DMA_FIFO=0x40; //not needed for OUT operation. 	

    usbdevregs->ep3.EP_DMA_TTC_L=0xff;
    usbdevregs->ep3.EP_DMA_TTC_M=0xff;
    usbdevregs->ep3.EP_DMA_TTC_H=0x0f;

    usbdevregs->OUT_CSR2_REG=usbdevregs->OUT_CSR2_REG|EPO_AUTO_CLR|EPO_OUT_DMA_INT_MASK; 
    	//AUTO_CLR(OUT_PKT_READY is cleared automatically), interrupt_masking.
#if USBDMA_DEMAND
    usbdevregs->ep3.EP_DMA_UNIT=EP3_PKT_SIZE; //DMA transfer unit=64 bytes
    usbdevregs->ep3.EP_DMA_CON=UDMA_DEMAND_MODE|UDMA_OUT_DMA_RUN|UDMA_DMA_MODE_EN; 
        // deamnd enable,out_dma_run=run,in_dma_run=stop,DMA mode enable
#else        
    usbdevregs->ep3.EP_DMA_UNIT=0x01; //DMA transfer unit=1byte
    usbdevregs->ep3.EP_DMA_CON=UDMA_OUT_DMA_RUN|UDMA_DMA_MODE_EN;
        // deamnd disable,out_dma_run=run,in_dma_run=stop,DMA mode enable
#endif  
    //wait until DMA_CON is effective.
    usbdevregs->ep3.EP_DMA_CON;
    for(i=0;i<10;i++);    	

    /* add by thisway.diy for non-autoreload */
    dmaregs->dma[3].DMASKTRIG = (1<<1);
}


void ConfigEp3IntMode(void)
{
    usbdevregs->INDEX_REG=3;
    
    dmaregs->dma[2].DMASKTRIG= (0<<1);  // EP3=DMA ch 2
        //DMA channel off
    usbdevregs->OUT_CSR2_REG=usbdevregs->OUT_CSR2_REG&~(EPO_AUTO_CLR/*|EPO_OUT_DMA_INT_MASK*/); 
    	//AUTOCLEAR off,interrupt_enabled (???)
    usbdevregs->ep3.EP_DMA_UNIT=1;	
    usbdevregs->ep3.EP_DMA_CON=0; 
    	// deamnd disable,out_dma_run=stop,in_dma_run=stop,DMA mode disable
    //wait until DMA_CON is effective.
    usbdevregs->ep3.EP_DMA_CON;
    
}
