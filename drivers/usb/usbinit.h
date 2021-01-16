/****************************************************************
 NAME: u2440mon.h
 DESC: 
 HISTORY:
 Mar.29.2002:purnnamu: created first
 ****************************************************************/
 
#ifndef __U241MON_H__
#define __U241MON_H__

extern volatile unsigned char *downPt;
extern volatile U32 totalDmaCount;
extern volatile U32 downloadFileSize;
extern volatile U32 downloadAddress;
extern volatile U16 checkSum;

extern int download_run;
extern U32 tempDownloadAddress;

#endif /*__U241MON_H__*/
