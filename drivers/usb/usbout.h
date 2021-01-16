/****************************************************************
 NAME: usbout.h
 DESC: 
 HISTORY:
 Mar.25.2002:purnnamu: reuse the source of S3C2400X u24xmon 
 ****************************************************************/
 
#ifndef __USBOUT_H__
#define __USBOUT_H__

void Ep3Handler(void);
void Ep3HandlerOptimized(void);

void IsrDma2(void);
void ClearEp3OutPktReady(void);

#endif /*__USBOUT_H__*/
