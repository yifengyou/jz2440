//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Use of this source code is subject to the terms of the Microsoft end-user
// license agreement (EULA) under which you licensed this SOFTWARE PRODUCT.
// If you did not accept the terms of the EULA, you are not authorized to use
// this source code. For a copy of the EULA, please see the LICENSE.RTF on your
// install media.
//
/*++
THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
PARTICULAR PURPOSE.
--*/
#ifndef __DEF_H__
#define __DEF_H__
#ifndef U32
#define U32 unsigned int
#endif

#ifndef U16
#define U16 unsigned short
#endif

#ifndef S32
#define S32 int
#endif

#ifndef S16
#define S16 short int
#endif

#ifndef U8
#define U8  unsigned char
#endif

#ifndef S8
#define	S8  char
#endif

typedef unsigned char BOOL;
typedef unsigned char UCHAR;
typedef UCHAR *PUCHAR;
typedef unsigned long DWORD;
typedef DWORD *PDWORD;
typedef unsigned long LDWORD;
typedef DWORD *LPDWORD;
typedef unsigned char   BYTE;
typedef unsigned short  WORD;

typedef unsigned long ULONG;
typedef ULONG *PULONG;
typedef unsigned short USHORT;
typedef USHORT *PUSHORT;

typedef BYTE *LPBYTE;

typedef void *PVOID;

typedef  char*          LPSTR;


#ifndef NULL
#define NULL 0
#endif

#define TRUE 	1   
#define FALSE 	0

#endif /*__DEF_H__*/


