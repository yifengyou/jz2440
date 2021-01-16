/*
 * Nand flash interface of s3c2410/s3c2440, by www.100ask.net
 * Changed from drivers/mtd/nand/s3c2410.c of kernel 2.6.13
 */

#include <common.h>

#if (CONFIG_COMMANDS & CFG_CMD_NAND) && !defined(CFG_NAND_LEGACY)
#include <s3c2410.h>
#include <nand.h>

DECLARE_GLOBAL_DATA_PTR;

#define S3C2410_NFSTAT_READY    (1<<0)
#define S3C2410_NFCONF_nFCE     (1<<11)

#define S3C2440_NFSTAT_READY    (1<<0)
#define S3C2440_NFCONT_nFCE     (1<<1)


/* select chip, for s3c2410 */
static void s3c2410_nand_select_chip(struct mtd_info *mtd, int chip)
{
    S3C2410_NAND * const s3c2410nand = S3C2410_GetBase_NAND();

    if (chip == -1) {
        s3c2410nand->NFCONF |= S3C2410_NFCONF_nFCE;
    } else {
        s3c2410nand->NFCONF &= ~S3C2410_NFCONF_nFCE;
    }
}

/* command and control functions, for s3c2410 
 *
 * Note, these all use tglx's method of changing the IO_ADDR_W field
 * to make the code simpler, and use the nand layer's code to issue the
 * command and address sequences via the proper IO ports.
 *
*/
static void s3c2410_nand_hwcontrol(struct mtd_info *mtd, int cmd)
{
    S3C2410_NAND * const s3c2410nand = S3C2410_GetBase_NAND();
    struct nand_chip *chip = mtd->priv;

    switch (cmd) {
    case NAND_CTL_SETNCE:
    case NAND_CTL_CLRNCE:
        printf("%s: called for NCE\n", __FUNCTION__);
        break;

    case NAND_CTL_SETCLE:
        chip->IO_ADDR_W = (void *)&s3c2410nand->NFCMD;
        break;

    case NAND_CTL_SETALE:
        chip->IO_ADDR_W = (void *)&s3c2410nand->NFADDR;
        break;

        /* NAND_CTL_CLRCLE: */
        /* NAND_CTL_CLRALE: */
    default:
        chip->IO_ADDR_W = (void *)&s3c2410nand->NFDATA;
        break;
    }
}

/* s3c2410_nand_devready()
 *
 * returns 0 if the nand is busy, 1 if it is ready
 */
static int s3c2410_nand_devready(struct mtd_info *mtd)
{
    S3C2410_NAND * const s3c2410nand = S3C2410_GetBase_NAND();

    return (s3c2410nand->NFSTAT & S3C2410_NFSTAT_READY);
}


/* select chip, for s3c2440 */
static void s3c2440_nand_select_chip(struct mtd_info *mtd, int chip)
{
    S3C2440_NAND * const s3c2440nand = S3C2440_GetBase_NAND();

    if (chip == -1) {
        s3c2440nand->NFCONT |= S3C2440_NFCONT_nFCE;
    } else {
        s3c2440nand->NFCONT &= ~S3C2440_NFCONT_nFCE;
    }
}

/* command and control functions */
static void s3c2440_nand_hwcontrol(struct mtd_info *mtd, int cmd)
{
    S3C2440_NAND * const s3c2440nand = S3C2440_GetBase_NAND();
    struct nand_chip *chip = mtd->priv;

    switch (cmd) {
    case NAND_CTL_SETNCE:
    case NAND_CTL_CLRNCE:
        printf("%s: called for NCE\n", __FUNCTION__);
        break;

    case NAND_CTL_SETCLE:
        chip->IO_ADDR_W = (void *)&s3c2440nand->NFCMD;
        break;

    case NAND_CTL_SETALE:
        chip->IO_ADDR_W = (void *)&s3c2440nand->NFADDR;
        break;

        /* NAND_CTL_CLRCLE: */
        /* NAND_CTL_CLRALE: */
    default:
        chip->IO_ADDR_W = (void *)&s3c2440nand->NFDATA;
        break;
    }
}

/* s3c2440_nand_devready()
 *
 * returns 0 if the nand is busy, 1 if it is ready
 */
static int s3c2440_nand_devready(struct mtd_info *mtd)
{
    S3C2440_NAND * const s3c2440nand = S3C2440_GetBase_NAND();

    return (s3c2440nand->NFSTAT & S3C2440_NFSTAT_READY);
}

/*
 * Nand flash hardware initialization:
 * Set the timing, enable NAND flash controller
 */
static void s3c24x0_nand_inithw(void)
{
    S3C2410_NAND * const s3c2410nand = S3C2410_GetBase_NAND();
    S3C2440_NAND * const s3c2440nand = S3C2440_GetBase_NAND();

#define TACLS   0
#define TWRPH0  4
#define TWRPH1  2

    if (isS3C2410)
    {
        /* Enable NAND flash controller, Initialize ECC, enable chip select, Set flash memory timing */
        s3c2410nand->NFCONF = (1<<15)|(1<<12)|(1<<11)|(TACLS<<8)|(TWRPH0<<4)|(TWRPH1<<0);
    }
    else
    {
        /* Set flash memory timing */
        s3c2440nand->NFCONF = (TACLS<<12)|(TWRPH0<<8)|(TWRPH1<<4);
        /* Initialize ECC, enable chip select, NAND flash controller enable */
        s3c2440nand->NFCONT = (1<<4)|(0<<1)|(1<<0);
    }
}

/*
 * Called by drivers/nand/nand.c, initialize the interface of nand flash
 */
void board_nand_init(struct nand_chip *chip)
{
    S3C2410_NAND * const s3c2410nand = S3C2410_GetBase_NAND();
    S3C2440_NAND * const s3c2440nand = S3C2440_GetBase_NAND();

    s3c24x0_nand_inithw();

    if (isS3C2410) {
        chip->IO_ADDR_R    = (void *)&s3c2410nand->NFDATA;
        chip->IO_ADDR_W    = (void *)&s3c2410nand->NFDATA;
        chip->hwcontrol    = s3c2410_nand_hwcontrol;
        chip->dev_ready    = s3c2410_nand_devready;
        chip->select_chip  = s3c2410_nand_select_chip;
        chip->options      = 0;
    } else {
        chip->IO_ADDR_R    = (void *)&s3c2440nand->NFDATA;
        chip->IO_ADDR_W    = (void *)&s3c2440nand->NFDATA;
        chip->hwcontrol    = s3c2440_nand_hwcontrol;
        chip->dev_ready    = s3c2440_nand_devready;
        chip->select_chip  = s3c2440_nand_select_chip;
        chip->options      = 0;
    }

    chip->eccmode       = NAND_ECC_SOFT;
}

#endif
