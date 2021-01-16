/****************************************************************
 NAME: usblib.h
 DESC: 
 HISTORY:
 Mar.25.2002:purnnamu: ported for S3C2410X.
 ****************************************************************/
#ifndef __USBLIB_H__
#define __USBLIB_H__

#include "2440usb.h"
#include <def.h>

#define PWR_REG_DEFAULT_VALUE (DISABLE_SUSPEND)

void ConfigUsbd(void);
void ReconfigUsbd(void);

void RdPktEp0(U8 *buf,int num);
void WrPktEp0(U8 *buf,int num);
void WrPktEp1(U8 *buf,int num);
void WrPktEp2(U8 *buf,int num);
void RdPktEp3(U8 *buf,int num);
void RdPktEp4(U8 *buf,int num);

void ConfigEp3IntMode(void);
void ConfigEp3DmaMode(U32 bufAddr,U32 count);

#endif /*__USBLIB_H__*/

