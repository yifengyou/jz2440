/*
 * thisway.diy@163.com, www.100ask.net
 *
 */

#include <common.h>
#include <command.h>
#include <asm/byteorder.h>
#include <def.h>

#ifdef CONFIG_USB_DEVICE

int g_bUSBWait = 1;
u32 g_dwDownloadLen = 0;

extern int download_run;
extern volatile U32 dwUSBBufBase;
extern volatile u32 dwUSBBufSize;

extern __u32 usb_receive(char *buf, size_t len, U32 wait);

int do_usbslave (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
    int i;
    size_t len = ~0UL;
    char buf[32];

    /* download_run为1时表示将文件保存在USB Host发送工具dnw指定的位置
     * download_run为0时表示将文件保存在参数argv[2]指定的位置
     * 要下载程序到内存，然后直接运行时，要设置download_run=1，这也是这个参数名字的来由
     */
    download_run = 1;
    switch (argc) {
        case 1:
        {
            break;
        }
        case 2:
        {
            g_bUSBWait = (int)simple_strtoul(argv[1], NULL, 16);
            break;
        }

        case 3:
        {
            g_bUSBWait = (int)simple_strtoul(argv[1], NULL, 16);
            load_addr = simple_strtoul(argv[2], NULL, 16);
            download_run = 0;
            break;
        }
        
        default: 
        {
            printf ("Usage:\n%s\n", cmdtp->usage);
    		return 1;
        }
    }

    dwUSBBufBase = load_addr;  
    dwUSBBufSize = (FREE_RAM_SIZE&(~(0x80000-1)));  
    if (g_bUSBWait)
        len = FREE_RAM_SIZE;

    g_dwDownloadLen = usb_receive(dwUSBBufBase, len, g_bUSBWait);
    sprintf(buf, "%X", g_dwDownloadLen);
    setenv("filesize", buf);
    
	return 0;
}

U_BOOT_CMD(
	usbslave,	3,	0,	do_usbslave,
	"usbslave - get file from host(PC)\n",
	"[wait] [loadAddress]\n"
	"\"wait\" is 0 or 1, 0 means for return immediately, not waits for the finish of transferring\n"
);

#endif
