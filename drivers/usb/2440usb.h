/*********************************************
  NAME: 2440usb.h
  DESC: 
  HISTORY:
  03.23.2002:purnnamu: first release for 2410
 *********************************************/
#ifndef __24XUSB_H__
#define __24XUSB_H__

//*************************************************************************
// USB definitons
//*************************************************************************

/* Power Management Register */
#define DISABLE_SUSPEND          0x00   
#define ENABLE_SUSPEND           0x01
#define SUSPEND_MODE		 0x02
#define MCU_RESUME               0x04
#define ISO_UPDATE		 (1<<7)

/* MAXP Register */
#define FIFO_SIZE_0              0x00  /* 0x00 * 8 = 0  */
#define FIFO_SIZE_8              0x01  /* 0x01 * 8 = 8  */
#define FIFO_SIZE_16             0x02  /* 0x02 * 8 = 16 */
#define FIFO_SIZE_32             0x04  /* 0x04 * 8 = 32 */
#define FIFO_SIZE_64             0x08  /* 0x08 * 8 = 64 */

/* ENDPOINT0 CSR (Control Status Register) : Mapped to IN CSR1 */
#define EP0_OUT_PKT_READY        0x01  /* USB sets, MCU clears by setting SERVICED_OUT_PKT_RDY */
#define EP0_IN_PKT_READY         0x02  /* MCU sets, USB clears after sending FIFO */
#define EP0_SENT_STALL           0x04  /* USB sets */       
#define EP0_DATA_END             0x08  /* MCU sets */
#define EP0_SETUP_END            0x10  /* USB sets, MCU clears by setting SERVICED_SETUP_END */
#define EP0_SEND_STALL           0x20  /* MCU sets */
#define EP0_SERVICED_OUT_PKT_RDY 0x40  /* MCU writes 1 to clear OUT_PKT_READY */
#define EP0_SERVICED_SETUP_END   0x80  /* MCU writes 1 to clear SETUP_END        */

#define EP0_WR_BITS              0xc0  

//EP_INT_REG / EP_INT_EN_REG
#define EP0_INT                	 0x01  // Endpoint 0, Control   
#define EP1_INT                  0x02  // Endpoint 1, (Bulk-In) 
#define EP2_INT                  0x04  // Endpoint 2 
#define EP3_INT			 0x08  // Endpoint 3, (Bulk-Out)   
#define EP4_INT			 0x10  // Endpoint 4

//USB_INT_REG / USB_INT_EN_REG
#define SUSPEND_INT            	 0x01  
#define RESUME_INT               0x02  
#define RESET_INT                0x04  

//IN_CSR1
#define EPI_IN_PKT_READY         0x01  
#define EPI_UNDER_RUN		 0x04
#define EPI_FIFO_FLUSH		 0x08
#define EPI_SEND_STALL           0x10  
#define EPI_SENT_STALL           0x20  
#define EPI_CDT			 0x40	
#define EPI_WR_BITS              (EPI_FIFO_FLUSH|EPI_IN_PKT_READY|EPI_CDT) 
					//(EPI_FIFO_FLUSH) is preferred  (???)
//IN_CSR2
#define EPI_IN_DMA_INT_MASK	(1<<4)
#define EPI_MODE_IN		(1<<5)
#define EPI_MODE_OUT		(0<<5)
#define EPI_ISO			(1<<6)
#define EPI_BULK		(0<<6)
#define EPI_AUTO_SET		(1<<7)

//OUT_CSR1
#define EPO_OUT_PKT_READY        0x01  
#define EPO_OVER_RUN		 0x04  
#define EPO_DATA_ERROR		 0x08  
#define EPO_FIFO_FLUSH		 0x10
#define EPO_SEND_STALL           0x20  
#define EPO_SENT_STALL           0x40
#define EPO_CDT			 0x80	
#define EPO_WR_BITS              (EPO_FIFO_FLUSH|EPO_SEND_STALL|EPO_CDT)
					//(EPO_FIFO_FLUSH) is preferred (???)

//OUT_CSR2
#define EPO_OUT_DMA_INT_MASK	(1<<5)
#define EPO_ISO		 	(1<<6)
#define EPO_BULK	 	(0<<6)
#define EPO_AUTO_CLR		(1<<7)

//USB DMA control register
#define UDMA_IN_RUN_OB		(1<<7)
#define UDMA_IGNORE_TTC		(1<<7)
#define UDMA_DEMAND_MODE	(1<<3)
#define UDMA_OUT_RUN_OB		(1<<2)
#define UDMA_OUT_DMA_RUN	(1<<2)
#define UDMA_IN_DMA_RUN		(1<<1)
#define UDMA_DMA_MODE_EN	(1<<0)

#define rEP1_DMA_TTC	(rEP1_DMA_TTC_L+(rEP1_DMA_TTC_M<<8)+(rEP1_DMA_TTC_H<<16))
#define rEP2_DMA_TTC	(rEP2_DMA_TTC_L+(rEP2_DMA_TTC_M<<8)+(rEP2_DMA_TTC_H<<16))
#define rEP3_DMA_TTC	(rEP3_DMA_TTC_L+(rEP3_DMA_TTC_M<<8)+(rEP3_DMA_TTC_H<<16))
#define rEP4_DMA_TTC	(rEP4_DMA_TTC_L+(rEP4_DMA_TTC_M<<8)+(rEP4_DMA_TTC_H<<16))

#define ADDR_EP0_FIFO 		(0x520001c0) //Endpoint 0 FIFO
#define ADDR_EP1_FIFO		(0x520001c4) //Endpoint 1 FIFO
#define ADDR_EP2_FIFO		(0x520001c8) //Endpoint 2 FIFO
#define ADDR_EP3_FIFO		(0x520001cc) //Endpoint 3 FIFO
#define ADDR_EP4_FIFO		(0x520001d0) //Endpoint 4 FIFO

//If you chane the packet size, the source code should be changed!!!
//USB Device Options
#define USBDMA		TRUE
//#define USBDMA		FALSE
#define USBDMA_DEMAND 	FALSE	//the downloadFileSize should be (64*n)
#define BULK_PKT_SIZE	32

#define EP0_PKT_SIZE             8	
#define EP1_PKT_SIZE             BULK_PKT_SIZE
#define EP3_PKT_SIZE             BULK_PKT_SIZE

#endif //__24XUSB_H__
