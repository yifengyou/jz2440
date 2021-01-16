/****************************************************************
 NAME: usbout.c
 DESC: USB bulk-OUT operation related functions
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

#include <def.h>
 
#include "2440usb.h"
#include "usbmain.h"
#include "usb.h"
#include "usblib.h"
#include "usbsetup.h"
#include "usbout.h"

#include "usbinit.h"

extern volatile U32 dwUSBBufReadPtr;
extern volatile U32 dwUSBBufWritePtr;
extern volatile U32 dwWillDMACnt;
extern volatile U32 bDMAPending;
extern volatile U32 dwUSBBufBase;
extern volatile U32 dwUSBBufSize;

extern S3C24X0_INTERRUPT * intregs;
extern S3C24X0_USB_DEVICE * usbdevregs;
extern S3C24X0_DMAS * dmaregs;

static void PrintEpoPkt(U8 *pt,int cnt);
static void RdPktEp3_CheckSum(U8 *buf,int num);



// ===================================================================
// All following commands will operate in case 
// - out_csr3 is valid.
// ===================================================================

 

#define CLR_EP3_OUT_PKT_READY() usbdevregs->OUT_CSR1_REG= ( out_csr3 &(~ EPO_WR_BITS)\
					&(~EPO_OUT_PKT_READY) ) 
#define SET_EP3_SEND_STALL()	usbdevregs->OUT_CSR1_REG= ( out_csr3 & (~EPO_WR_BITS)\
					| EPO_SEND_STALL) )
#define CLR_EP3_SENT_STALL()	usbdevregs->OUT_CSR1_REG= ( out_csr3 & (~EPO_WR_BITS)\
					&(~EPO_SENT_STALL) )
#define FLUSH_EP3_FIFO() 	usbdevregs->OUT_CSR1_REG= ( out_csr3 & (~EPO_WR_BITS)\
					|EPO_FIFO_FLUSH) )

// ***************************
// *** VERY IMPORTANT NOTE ***
// ***************************
// Prepare for the packit size constraint!!!

// EP3 = OUT end point. 

U8 ep3Buf[EP3_PKT_SIZE];
static U8 tempBuf[64+1];

void Ep3Handler(void)
{
    U8 out_csr3;
    int fifoCnt;
    usbdevregs->INDEX_REG=3;

    out_csr3=usbdevregs->OUT_CSR1_REG;
    
    DbgPrintf("<3:%x]",out_csr3);

    if(out_csr3 & EPO_OUT_PKT_READY)
    {   
	fifoCnt=usbdevregs->OUT_FIFO_CNT1_REG; 
#if 0
	RdPktEp3(ep3Buf,fifoCnt);
	PrintEpoPkt(ep3Buf,fifoCnt);
#else

	if(downloadFileSize==0)
	{
   	    RdPktEp3((U8 *)downPt,8); 	
   	    
   	    if(download_run==0)
   	    {
		    downloadAddress=tempDownloadAddress;
	    }
	    else
	    {
	    	downloadAddress=
	    		*((U8 *)(downPt+0))+
			(*((U8 *)(downPt+1))<<8)+
			(*((U8 *)(downPt+2))<<16)+
			(*((U8 *)(downPt+3))<<24);
            
            dwUSBBufReadPtr = downloadAddress;
            dwUSBBufWritePtr = downloadAddress;
	    }
	    downloadFileSize=
	    	*((U8 *)(downPt+4))+
		(*((U8 *)(downPt+5))<<8)+
		(*((U8 *)(downPt+6))<<16)+
		(*((U8 *)(downPt+7))<<24);
	    checkSum=0;
	    downPt=(U8 *)downloadAddress;

  	    RdPktEp3_CheckSum((U8 *)downPt,fifoCnt-8); //The first 8-bytes are deleted.	    
  	    downPt+=fifoCnt-8;  
  	    
  	#if USBDMA
     	    //CLR_EP3_OUT_PKT_READY() is not executed. 
     	    //So, USBD may generate NAK until DMA2 is configured for USB_EP3;
     	    intregs->INTMSK|=BIT_USBD; //for debug
      	    return;	
  	#endif	
	}
	else
	{
	#if USBDMA    	
	    printf("<ERROR>");
	#endif    
	    RdPktEp3_CheckSum((U8 *)downPt,fifoCnt); 	    
	    downPt+=fifoCnt;  //fifoCnt=64
	}
#endif
   	CLR_EP3_OUT_PKT_READY();
#if 0
       if(((rOUT_CSR1_REG&0x1)==1) && ((rEP_INT_REG & 0x8)==0))
  		{
  		fifoCnt=rOUT_FIFO_CNT1_REG; 
		RdPktEp3_CheckSum((U8 *)downPt,fifoCnt); 	    
	       downPt+=fifoCnt;  //fifoCnt=64
	       CLR_EP3_OUT_PKT_READY();
		}
#endif
  	return;
    }

    
    //I think that EPO_SENT_STALL will not be set to 1.
    if(out_csr3 & EPO_SENT_STALL)
    {   
   	DbgPrintf("[STALL]");
   	CLR_EP3_SENT_STALL();
   	return;
    }	
}



void PrintEpoPkt(U8 *pt,int cnt)
{
    int i;
    DbgPrintf("[BOUT:%d:",cnt);
    for(i=0;i<cnt;i++)
    	DbgPrintf("%x,",pt[i]);
    DbgPrintf("]");
}


void RdPktEp3_CheckSum(U8 *buf,int num)
{
    int i;
    	
    for(i=0;i<num;i++)
    {
        buf[i]=(U8)usbdevregs->fifo[3].EP_FIFO_REG;
        checkSum+=buf[i];
    }
}



void IsrDma2(void)
{
    U8 out_csr3;
    U32 dwEmptyCnt;
    U8 saveIndexReg=usbdevregs->INDEX_REG;
    usbdevregs->INDEX_REG=3;
    out_csr3=usbdevregs->OUT_CSR1_REG;

    ClearPending(BIT_DMA2);	    

    /* thisway.diy, 2006.06.22 
     * When the first DMA interrupt happened, it has received max (0x80000 + EP3_PKT_SIZE) bytes data from PC
     */
    if (!totalDmaCount) 
        totalDmaCount = dwWillDMACnt + EP3_PKT_SIZE;
    else
        totalDmaCount+=dwWillDMACnt;

//    dwUSBBufWritePtr = ((dwUSBBufWritePtr + dwWillDMACnt - USB_BUF_BASE) % USB_BUF_SIZE) + USB_BUF_BASE; /* thisway.diy, 2006.06.21 */
    dwUSBBufWritePtr = ((dwUSBBufWritePtr + dwWillDMACnt - dwUSBBufBase) % dwUSBBufSize) + dwUSBBufBase;

    if(totalDmaCount>=downloadFileSize)// is last?
    {
    	totalDmaCount=downloadFileSize;
	
    	ConfigEp3IntMode();	

    	if(out_csr3& EPO_OUT_PKT_READY)
    	{
       	    CLR_EP3_OUT_PKT_READY();
	    }
        intregs->INTMSK|=BIT_DMA2;  
        intregs->INTMSK&=~(BIT_USBD);  
    }
    else
    {
    	if((totalDmaCount+0x80000)<downloadFileSize)	
    	{
    	    dwWillDMACnt = 0x80000;
	    }
    	else
    	{
    	    dwWillDMACnt = downloadFileSize - totalDmaCount;
    	}

        // dwEmptyCnt = (dwUSBBufReadPtr - dwUSBBufWritePtr - 1 + USB_BUF_SIZE) % USB_BUF_SIZE; /* thisway.diy, 2006.06.21 */
        dwEmptyCnt = (dwUSBBufReadPtr - dwUSBBufWritePtr - 1 + dwUSBBufSize) % dwUSBBufSize;
        if (dwEmptyCnt >= dwWillDMACnt)
        {
    	    ConfigEp3DmaMode(dwUSBBufWritePtr, dwWillDMACnt);
        }
        else
        {
            bDMAPending = 1;
        }
    }
    usbdevregs->INDEX_REG = saveIndexReg;
}


void ClearEp3OutPktReady(void)
{
    U8 out_csr3;
    usbdevregs->INDEX_REG=3;
    out_csr3=usbdevregs->OUT_CSR1_REG;
    CLR_EP3_OUT_PKT_READY();
}
