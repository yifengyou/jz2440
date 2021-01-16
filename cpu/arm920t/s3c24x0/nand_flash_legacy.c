/* add by www.100ask.net */
/*-----------------------------------------------------------------------
 * NAND flash settings
 */
#include <common.h>
#include <s3c2410.h>
#if (CONFIG_COMMANDS & CFG_CMD_NAND)

DECLARE_GLOBAL_DATA_PTR;

/* only has two value: NFCE_LOW, NFCE_HIGH */
#define NFCE_STATE  int 

void NF_Conf(u16 conf)
{
	S3C2410_NAND * const s3c2410nand = S3C2410_GetBase_NAND();
	S3C2440_NAND * const s3c2440nand = S3C2440_GetBase_NAND();

    if (gd->bd->bi_arch_number == MACH_TYPE_SMDK2410)
    	s3c2410nand->NFCONF = conf;
    else
        s3c2440nand->NFCONF = conf;
}

/* only for s3c2440 */
void NF_Cont(u16 cont)
{
	S3C2440_NAND * const s3c2440nand = S3C2440_GetBase_NAND();

    if (gd->bd->bi_arch_number != MACH_TYPE_SMDK2410)
        s3c2440nand->NFCONT = cont;
}

void NF_Cmd(u8 cmd)
{
	S3C2410_NAND * const s3c2410nand = S3C2410_GetBase_NAND();
	S3C2440_NAND * const s3c2440nand = S3C2440_GetBase_NAND();

    if (gd->bd->bi_arch_number == MACH_TYPE_SMDK2410)
    	s3c2410nand->NFCMD = cmd;
    else
    	s3c2440nand->NFCMD = cmd;
}

void NF_CmdW(u8 cmd)
{
	NF_Cmd(cmd);
    udelay(1);
}

void NF_Addr(u8 addr)
{
	S3C2410_NAND * const s3c2410nand = S3C2410_GetBase_NAND();
	S3C2440_NAND * const s3c2440nand = S3C2440_GetBase_NAND();

    if (gd->bd->bi_arch_number == MACH_TYPE_SMDK2410)
    	s3c2410nand->NFADDR = addr;
    else
    	s3c2440nand->NFADDR = addr;
    udelay(15);
}

void NF_SetCE(NFCE_STATE s)
{
	S3C2410_NAND * const s3c2410nand = S3C2410_GetBase_NAND();
	S3C2440_NAND * const s3c2440nand = S3C2440_GetBase_NAND();

	switch (s) {
		case NFCE_LOW:
            if (gd->bd->bi_arch_number == MACH_TYPE_SMDK2410)
		    	s3c2410nand->NFCONF &= ~(1<<11);
            else
		    	s3c2440nand->NFCONT &= ~(1<<1);                
			break;

		case NFCE_HIGH:
            if (gd->bd->bi_arch_number == MACH_TYPE_SMDK2410)
    			s3c2410nand->NFCONF |= (1<<11);
            else
		    	s3c2440nand->NFCONT |= (1<<1);
			break;
	}
}

/* only for s3c2440 */
void NF_ClrRB(void)
{
	S3C2440_NAND * const s3c2440nand = S3C2440_GetBase_NAND();

    if (gd->bd->bi_arch_number != MACH_TYPE_SMDK2410)
    {
        s3c2440nand->NFSTAT |= (1<<2);
    }
}

void NF_WaitRB(void)
{
	S3C2410_NAND * const s3c2410nand = S3C2410_GetBase_NAND();
	S3C2440_NAND * const s3c2440nand = S3C2440_GetBase_NAND();

    if (gd->bd->bi_arch_number == MACH_TYPE_SMDK2410)
    {
    	while (!(s3c2410nand->NFSTAT & (1<<0)));
    }
    else
    {
    	while (!(s3c2440nand->NFSTAT & (1<<0)));
        s3c2440nand->NFSTAT |= (1<<2);
    }
}

void NF_Write(u8 data)
{
	S3C2410_NAND * const s3c2410nand = S3C2410_GetBase_NAND();
	S3C2440_NAND * const s3c2440nand = S3C2440_GetBase_NAND();
    volatile unsigned char *puc = (volatile unsigned char *)&s3c2440nand->NFDATA;

    if (gd->bd->bi_arch_number == MACH_TYPE_SMDK2410)
    	s3c2410nand->NFDATA = data;
    else
    	*puc = data;    /* s3c2440's NFDATA must be accessed by 'u8*', www.100ask.net */
    
}

u8 NF_Read(void)
{
	S3C2410_NAND * const s3c2410nand = S3C2410_GetBase_NAND();
	S3C2440_NAND * const s3c2440nand = S3C2440_GetBase_NAND();
    volatile unsigned char *puc = (volatile unsigned char *)&s3c2440nand->NFDATA;

    if (gd->bd->bi_arch_number == MACH_TYPE_SMDK2410)
    	return(s3c2410nand->NFDATA);
    else
    	return(*puc);   /* s3c2440's NFDATA must be accessed by 'u8*', www.100ask.net */
}

void NF_Init_ECC(void)
{
	S3C2410_NAND * const s3c2410nand = S3C2410_GetBase_NAND();
	S3C2440_NAND * const s3c2440nand = S3C2440_GetBase_NAND();

    if (gd->bd->bi_arch_number == MACH_TYPE_SMDK2410)
    	s3c2410nand->NFCONF |= (1<<12);
    else
    {
        s3c2440nand->NFCONT |= (1<<4);   // Reset ECC
        s3c2440nand->NFCONT &= ~(1<<5);  // UnLock MECC
    }
}

u32 NF_Read_ECC(void)
{
	S3C2410_NAND * const s3c2410nand = S3C2410_GetBase_NAND();
	S3C2440_NAND * const s3c2440nand = S3C2440_GetBase_NAND();

    if (gd->bd->bi_arch_number == MACH_TYPE_SMDK2410)
    	return(s3c2410nand->NFECC);
    else
    {
        s3c2440nand->NFCONT |= (1<<5);  // Lock MECC
        return(s3c2440nand->NFMECC0);   // Read MECC
    }
}

extern ulong
nand_probe(ulong physadr);


static inline void NF_Reset(void)
{
    int i;

    NF_Cont((1<<4)|(0<<1)|(1<<0));  /* only for s3c2440, Initialize ECC, enable chip select, NAND flash controller enable */
    NF_SetCE(NFCE_LOW); /* enable chip select */
    NF_ClrRB();         /* only for s3c2440 */
    NF_Cmd(0xFF);		/* reset command */
    for(i = 0; i < 10; i++);	/* tWB = 100ns. */
    NF_WaitRB();		/* wait 200~500us; */
    NF_SetCE(NFCE_HIGH);
}

static inline void NF_Init(void)
{
#if 0 /* a little bit too optimistic */
#define TACLS   0
#define TWRPH0  3
#define TWRPH1  0
#else
#define TACLS   0
#define TWRPH0  4
#define TWRPH1  2
#endif

    /* Set flash memory timing */
    if (gd->bd->bi_arch_number == MACH_TYPE_SMDK2410)
    {
        NF_Conf((1<<15)|(0<<14)|(0<<13)|(1<<12)|(1<<11)|(TACLS<<8)|(TWRPH0<<4)|(TWRPH1<<0));
        /*nand->NFCONF = (1<<15)|(1<<14)|(1<<13)|(1<<12)|(1<<11)|(TACLS<<8)|(TWRPH0<<4)|(TWRPH1<<0); */
        /* 1  1    1     1,   1      xxx,  r xxx,   r xxx */
        /* En 512B 4step ECCR nFCE=H tACLS   tWRPH0   tWRPH1 */
    }
    else
    {
        NF_Conf((3<<12)|(7<<8)|(7<<4));
    }

    NF_Reset();
}

void
nand_init(void)
{
	S3C2410_NAND * const nand = S3C2410_GetBase_NAND();

	NF_Init();
#ifdef DEBUG
	printf("NAND flash probing at 0x%.8lX\n", (ulong)nand);
#endif
	printf ("%4lu MB\n", nand_probe((ulong)nand) >> 20);
}
#endif	/* CONFIG_COMMANDS & CFG_CMD_NAND */

