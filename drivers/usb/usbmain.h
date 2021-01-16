/****************************************************************
 NAME: usbmain.h
 DESC: 
 HISTORY:
 Mar.25.2002:purnnamu: reuse the source of S3C2400X u24xmon 
 ****************************************************************/
 
#ifndef __USBMAIN_H__
#define __USBMAIN_H__

void UsbdMain(void);
void IsrUsbd(void);

void DbgPrintf(char *fmt,...);


#endif /*__USBMAIN_H__*/
