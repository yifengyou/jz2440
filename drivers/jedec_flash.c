/*
 * (C) Copyright 2007
 * Michael Schwingen, <michael@schwingen.org>
 *
 * based in great part on jedec_probe.c from linux kernel:
 * (C) 2000 Red Hat. GPL'd.
 * Occasionally maintained by Thayne Harbaugh tharbaugh at lnxi dot com
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 */

/* The DEBUG define must be before common to enable debugging */
/*#define DEBUG*/

#include <common.h>
#include <asm/processor.h>
#include <asm/io.h>
#include <asm/byteorder.h>
#include <environment.h>

#define P_ID_NONE               0x0000
#define P_ID_INTEL_EXT          0x0001
#define P_ID_AMD_STD            0x0002
#define P_ID_INTEL_STD          0x0003
#define P_ID_AMD_EXT            0x0004
#define P_ID_WINBOND            0x0006
#define P_ID_ST_ADV             0x0020
#define P_ID_MITSUBISHI_STD     0x0100
#define P_ID_MITSUBISHI_EXT     0x0101
#define P_ID_SST_PAGE           0x0102
#define P_ID_INTEL_PERFORMANCE  0x0200
#define P_ID_INTEL_DATA         0x0210
#define P_ID_RESERVED           0xffff


/* Manufacturers */
#define MANUFACTURER_AMD	0x0001
#define MANUFACTURER_ATMEL	0x001f
#define MANUFACTURER_FUJITSU	0x0004
#define MANUFACTURER_HYUNDAI	0x00AD
#define MANUFACTURER_INTEL	0x0089
#define MANUFACTURER_MACRONIX	0x00C2
#define MANUFACTURER_NEC	0x0010
#define MANUFACTURER_PMC	0x009D
#define MANUFACTURER_SHARP	0x00b0
#define MANUFACTURER_SST	0x00BF
#define MANUFACTURER_ST		0x0020
#define MANUFACTURER_TOSHIBA	0x0098
#define MANUFACTURER_WINBOND	0x00da


/* AMD */
#define AM29DL800BB	0x22C8
#define AM29DL800BT	0x224A

#define AM29F800BB	0x2258
#define AM29F800BT	0x22D6
#define AM29LV400BB	0x22BA
#define AM29LV400BT	0x22B9
#define AM29LV800BB	0x225B
#define AM29LV800BT	0x22DA
#define AM29LV160DT	0x22C4
#define AM29LV160DB	0x2249
#define AM29F017D	0x003D
#define AM29F016D	0x00AD
#define AM29F080	0x00D5
#define AM29F040	0x00A4
#define AM29LV040B	0x004F
#define AM29F032B	0x0041
#define AM29F002T	0x00B0

/* Atmel */
#define AT49BV512	0x0003
#define AT29LV512	0x003d
#define AT49BV16X	0x00C0
#define AT49BV16XT	0x00C2
#define AT49BV32X	0x00C8
#define AT49BV32XT	0x00C9

/* Fujitsu */
#define MBM29F040C	0x00A4
#define MBM29LV650UE	0x22D7
#define MBM29LV320TE	0x22F6
#define MBM29LV320BE	0x22F9
#define MBM29LV160TE	0x22C4
#define MBM29LV160BE	0x2249
#define MBM29LV800BA	0x225B
#define MBM29LV800TA	0x22DA
#define MBM29LV400TC	0x22B9
#define MBM29LV400BC	0x22BA

/* Hyundai */
#define HY29F002T	0x00B0

/* Intel */
#define I28F004B3T	0x00d4
#define I28F004B3B	0x00d5
#define I28F400B3T	0x8894
#define I28F400B3B	0x8895
#define I28F008S5	0x00a6
#define I28F016S5	0x00a0
#define I28F008SA	0x00a2
#define I28F008B3T	0x00d2
#define I28F008B3B	0x00d3
#define I28F800B3T	0x8892
#define I28F800B3B	0x8893
#define I28F016S3	0x00aa
#define I28F016B3T	0x00d0
#define I28F016B3B	0x00d1
#define I28F160B3T	0x8890
#define I28F160B3B	0x8891
#define I28F320B3T	0x8896
#define I28F320B3B	0x8897
#define I28F640B3T	0x8898
#define I28F640B3B	0x8899
#define I82802AB	0x00ad
#define I82802AC	0x00ac

/* Macronix */
#define MX29LV040C	0x004F
#define MX29LV160T	0x22C4
#define MX29LV160B	0x2249
#define MX29F040	0x00A4
#define MX29F016	0x00AD
#define MX29F002T	0x00B0
#define MX29F004T	0x0045
#define MX29F004B	0x0046

/* NEC */
#define UPD29F064115	0x221C

/* PMC */
#define PM49FL002	0x006D
#define PM49FL004	0x006E
#define PM49FL008	0x006A

/* Sharp */
#define LH28F640BF	0x00b0

/* ST - www.st.com */
#define M29W800DT	0x00D7
#define M29W800DB	0x005B
#define M29W160DT	0x22C4
#define M29W160DB	0x2249
#define M29W040B	0x00E3
#define M50FW040	0x002C
#define M50FW080	0x002D
#define M50FW016	0x002E
#define M50LPW080       0x002F

/* SST */
#define SST29EE020	0x0010
#define SST29LE020	0x0012
#define SST29EE512	0x005d
#define SST29LE512	0x003d
#define SST39LF800	0x2781
#define SST39LF160	0x2782
#define SST39VF1601	0x234b
#define SST39LF512	0x00D4
#define SST39LF010	0x00D5
#define SST39LF020	0x00D6
#define SST39LF040	0x00D7
#define SST39SF010A	0x00B5
#define SST39SF020A	0x00B6
#define SST49LF004B	0x0060
#define SST49LF040B	0x0050
#define SST49LF008A	0x005a
#define SST49LF030A	0x001C
#define SST49LF040A	0x0051
#define SST49LF080A	0x005B

/* Toshiba */
#define TC58FVT160	0x00C2
#define TC58FVB160	0x0043
#define TC58FVT321	0x009A
#define TC58FVB321	0x009C
#define TC58FVT641	0x0093
#define TC58FVB641	0x0095

/* Winbond */
#define W49V002A	0x00b0


#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))


struct unlock_addr {
	u32 addr1;
	u32 addr2;
};

/*
 * Unlock address sets for AMD command sets.
 * Intel command sets use the MTD_UADDR_UNNECESSARY.
 * Each identifier, except MTD_UADDR_UNNECESSARY, and
 * MTD_UADDR_NO_SUPPORT must be defined below in unlock_addrs[].
 * MTD_UADDR_NOT_SUPPORTED must be 0 so that structure
 * initialization need not require initializing all of the
 * unlock addresses for all bit widths.
 */
enum uaddr {
	MTD_UADDR_NOT_SUPPORTED = 0,	/* data width not supported */
	MTD_UADDR_0x0555_0x02AA,
	MTD_UADDR_0x0555_0x0AAA,
	MTD_UADDR_0x5555_0x2AAA,
	MTD_UADDR_0x0AAA_0x0555,
	MTD_UADDR_DONT_CARE,		/* Requires an arbitrary address */
	MTD_UADDR_UNNECESSARY,		/* Does not require any address */
};




/*
 * I don't like the fact that the first entry in unlock_addrs[]
 * exists, but is for MTD_UADDR_NOT_SUPPORTED - and, therefore,
 * should not be used.  The  problem is that structures with
 * initializers have extra fields initialized to 0.  It is _very_
 * desireable to have the unlock address entries for unsupported
 * data widths automatically initialized - that means that
 * MTD_UADDR_NOT_SUPPORTED must be 0 and the first entry here
 * must go unused.
 */
static const struct unlock_addr  unlock_addrs[] = {
	[MTD_UADDR_NOT_SUPPORTED] = {
		.addr1 = 0xffff,
		.addr2 = 0xffff
	},

	[MTD_UADDR_0x0555_0x02AA] = {
		.addr1 = 0x0555,
		.addr2 = 0x02aa
	},

	[MTD_UADDR_0x0555_0x0AAA] = {
		.addr1 = 0x0555,
		.addr2 = 0x0aaa
	},

	[MTD_UADDR_0x5555_0x2AAA] = {
		.addr1 = 0x5555,
		.addr2 = 0x2aaa
	},

	[MTD_UADDR_0x0AAA_0x0555] = {
		.addr1 = 0x0AAA,
		.addr2 = 0x0555
	},

	[MTD_UADDR_DONT_CARE] = {
		.addr1 = 0x0000,      /* Doesn't matter which address */
		.addr2 = 0x0000       /* is used - must be last entry */
	},

	[MTD_UADDR_UNNECESSARY] = {
		.addr1 = 0x0000,
		.addr2 = 0x0000
	}
};

struct amd_flash_info {
	const __u16 mfr_id;
	const __u16 dev_id;
	const char *name;
	const int DevSize;
	const int NumEraseRegions;
	const int CmdSet;
	const __u8 uaddr[4];		/* unlock addrs for 8, 16, 32, 64 */
	const ulong regions[6];
};

#define ERASEINFO(size,blocks) (size<<8)|(blocks-1)

#define SIZE_64KiB  16
#define SIZE_128KiB 17
#define SIZE_256KiB 18
#define SIZE_512KiB 19
#define SIZE_1MiB   20
#define SIZE_2MiB   21
#define SIZE_4MiB   22
#define SIZE_8MiB   23


/*
 * Please keep this list ordered by manufacturer!
 * Fortunately, the list isn't searched often and so a
 * slow, linear search isn't so bad.
 */
static const struct amd_flash_info jedec_table[] = {
	{
		.mfr_id		= MANUFACTURER_AMD,
		.dev_id		= AM29F032B,
		.name		= "AMD AM29F032B",
		.uaddr		= {
			[0] = MTD_UADDR_0x0555_0x02AA /* x8 */
		},
		.DevSize	= SIZE_4MiB,
		.CmdSet		= P_ID_AMD_STD,
		.NumEraseRegions= 1,
		.regions	= {
			ERASEINFO(0x10000,64)
		}
	}, {
		.mfr_id		= MANUFACTURER_AMD,
		.dev_id		= AM29LV160DT,
		.name		= "AMD AM29LV160DT",
		.uaddr		= {
			[0] = MTD_UADDR_0x0AAA_0x0555,  /* x8 */
			[1] = MTD_UADDR_0x0555_0x02AA   /* x16 */
		},
		.DevSize	= SIZE_2MiB,
		.CmdSet		= P_ID_AMD_STD,
		.NumEraseRegions= 4,
		.regions	= {
			ERASEINFO(0x10000,31),
			ERASEINFO(0x08000,1),
			ERASEINFO(0x02000,2),
			ERASEINFO(0x04000,1)
		}
	}, {
		.mfr_id		= MANUFACTURER_AMD,
		.dev_id		= AM29LV160DB,
		.name		= "AMD AM29LV160DB",
		.uaddr		= {
			[0] = MTD_UADDR_0x0AAA_0x0555,  /* x8 */
			[1] = MTD_UADDR_0x0555_0x02AA   /* x16 */
		},
		.DevSize	= SIZE_2MiB,
		.CmdSet		= P_ID_AMD_STD,
		.NumEraseRegions= 4,
		.regions	= {
			ERASEINFO(0x04000,1),
			ERASEINFO(0x02000,2),
			ERASEINFO(0x08000,1),
			ERASEINFO(0x10000,31)
		}
	}, {
		.mfr_id		= MANUFACTURER_AMD,
		.dev_id		= AM29LV400BB,
		.name		= "AMD AM29LV400BB",
		.uaddr		= {
			[0] = MTD_UADDR_0x0AAA_0x0555,  /* x8 */
			[1] = MTD_UADDR_0x0555_0x02AA,  /* x16 */
		},
		.DevSize	= SIZE_512KiB,
		.CmdSet		= P_ID_AMD_STD,
		.NumEraseRegions= 4,
		.regions	= {
			ERASEINFO(0x04000,1),
			ERASEINFO(0x02000,2),
			ERASEINFO(0x08000,1),
			ERASEINFO(0x10000,7)
		}
	}, {
		.mfr_id		= MANUFACTURER_AMD,
		.dev_id		= AM29LV400BT,
		.name		= "AMD AM29LV400BT",
		.uaddr		= {
			[0] = MTD_UADDR_0x0AAA_0x0555,  /* x8 */
			[1] = MTD_UADDR_0x0555_0x02AA,  /* x16 */
		},
		.DevSize	= SIZE_512KiB,
		.CmdSet		= P_ID_AMD_STD,
		.NumEraseRegions= 4,
		.regions	= {
			ERASEINFO(0x10000,7),
			ERASEINFO(0x08000,1),
			ERASEINFO(0x02000,2),
			ERASEINFO(0x04000,1)
		}
	}, {
		.mfr_id		= MANUFACTURER_AMD,
		.dev_id		= AM29LV800BB,
		.name		= "AMD AM29LV800BB",
		.uaddr		= {
			[0] = MTD_UADDR_0x0AAA_0x0555,  /* x8 */
			[1] = MTD_UADDR_0x0555_0x02AA,  /* x16 */
		},
		.DevSize	= SIZE_1MiB,
		.CmdSet		= P_ID_AMD_STD,
		.NumEraseRegions= 4,
		.regions	= {
			ERASEINFO(0x04000,1),
			ERASEINFO(0x02000,2),
			ERASEINFO(0x08000,1),
			ERASEINFO(0x10000,15),
		}
	}, {
/* add DL */
		.mfr_id		= MANUFACTURER_AMD,
		.dev_id		= AM29DL800BB,
		.name		= "AMD AM29DL800BB",
		.uaddr		= {
			[0] = MTD_UADDR_0x0AAA_0x0555,  /* x8 */
			[1] = MTD_UADDR_0x0555_0x02AA,  /* x16 */
		},
		.DevSize	= SIZE_1MiB,
		.CmdSet		= P_ID_AMD_STD,
		.NumEraseRegions= 6,
		.regions	= {
			ERASEINFO(0x04000,1),
			ERASEINFO(0x08000,1),
			ERASEINFO(0x02000,4),
			ERASEINFO(0x08000,1),
			ERASEINFO(0x04000,1),
			ERASEINFO(0x10000,14)
		}
	}, {
		.mfr_id		= MANUFACTURER_AMD,
		.dev_id		= AM29DL800BT,
		.name		= "AMD AM29DL800BT",
		.uaddr		= {
			[0] = MTD_UADDR_0x0AAA_0x0555,  /* x8 */
			[1] = MTD_UADDR_0x0555_0x02AA,  /* x16 */
		},
		.DevSize	= SIZE_1MiB,
		.CmdSet		= P_ID_AMD_STD,
		.NumEraseRegions= 6,
		.regions	= {
			ERASEINFO(0x10000,14),
			ERASEINFO(0x04000,1),
			ERASEINFO(0x08000,1),
			ERASEINFO(0x02000,4),
			ERASEINFO(0x08000,1),
			ERASEINFO(0x04000,1)
		}
	}, {
		.mfr_id		= MANUFACTURER_AMD,
		.dev_id		= AM29F800BB,
		.name		= "AMD AM29F800BB",
		.uaddr		= {
			[0] = MTD_UADDR_0x0AAA_0x0555,  /* x8 */
			[1] = MTD_UADDR_0x0555_0x02AA,  /* x16 */
		},
		.DevSize	= SIZE_1MiB,
		.CmdSet		= P_ID_AMD_STD,
		.NumEraseRegions= 4,
		.regions	= {
			ERASEINFO(0x04000,1),
			ERASEINFO(0x02000,2),
			ERASEINFO(0x08000,1),
			ERASEINFO(0x10000,15),
		}
	}, {
		.mfr_id		= MANUFACTURER_AMD,
		.dev_id		= AM29LV800BT,
		.name		= "AMD AM29LV800BT",
		.uaddr		= {
			[0] = MTD_UADDR_0x0AAA_0x0555,  /* x8 */
			[1] = MTD_UADDR_0x0555_0x02AA,  /* x16 */
		},
		.DevSize	= SIZE_1MiB,
		.CmdSet		= P_ID_AMD_STD,
		.NumEraseRegions= 4,
		.regions	= {
			ERASEINFO(0x10000,15),
			ERASEINFO(0x08000,1),
			ERASEINFO(0x02000,2),
			ERASEINFO(0x04000,1)
		}
	}, {
		.mfr_id		= MANUFACTURER_AMD,
		.dev_id		= AM29F800BT,
		.name		= "AMD AM29F800BT",
		.uaddr		= {
			[0] = MTD_UADDR_0x0AAA_0x0555,  /* x8 */
			[1] = MTD_UADDR_0x0555_0x02AA,  /* x16 */
		},
		.DevSize	= SIZE_1MiB,
		.CmdSet		= P_ID_AMD_STD,
		.NumEraseRegions= 4,
		.regions	= {
			ERASEINFO(0x10000,15),
			ERASEINFO(0x08000,1),
			ERASEINFO(0x02000,2),
			ERASEINFO(0x04000,1)
		}
	}, {
		.mfr_id		= MANUFACTURER_AMD,
		.dev_id		= AM29F017D,
		.name		= "AMD AM29F017D",
		.uaddr		= {
			[0] = MTD_UADDR_DONT_CARE     /* x8 */
		},
		.DevSize	= SIZE_2MiB,
		.CmdSet		= P_ID_AMD_STD,
		.NumEraseRegions= 1,
		.regions	= {
			ERASEINFO(0x10000,32),
		}
	}, {
		.mfr_id		= MANUFACTURER_AMD,
		.dev_id		= AM29F016D,
		.name		= "AMD AM29F016D",
		.uaddr		= {
			[0] = MTD_UADDR_0x0555_0x02AA /* x8 */
		},
		.DevSize	= SIZE_2MiB,
		.CmdSet		= P_ID_AMD_STD,
		.NumEraseRegions= 1,
		.regions	= {
			ERASEINFO(0x10000,32),
		}
	}, {
		.mfr_id		= MANUFACTURER_AMD,
		.dev_id		= AM29F080,
		.name		= "AMD AM29F080",
		.uaddr		= {
			[0] = MTD_UADDR_0x0555_0x02AA /* x8 */
		},
		.DevSize	= SIZE_1MiB,
		.CmdSet		= P_ID_AMD_STD,
		.NumEraseRegions= 1,
		.regions	= {
			ERASEINFO(0x10000,16),
		}
	}, {
		.mfr_id		= MANUFACTURER_AMD,
		.dev_id		= AM29F040,
		.name		= "AMD AM29F040",
		.uaddr		= {
			[0] = MTD_UADDR_0x0555_0x02AA /* x8 */
		},
		.DevSize	= SIZE_512KiB,
		.CmdSet		= P_ID_AMD_STD,
		.NumEraseRegions= 1,
		.regions	= {
			ERASEINFO(0x10000,8),
		}
	}, {
		.mfr_id		= MANUFACTURER_AMD,
		.dev_id		= AM29LV040B,
		.name		= "AMD AM29LV040B",
		.uaddr		= {
			[0] = MTD_UADDR_0x0555_0x02AA /* x8 */
		},
		.DevSize	= SIZE_512KiB,
		.CmdSet		= P_ID_AMD_STD,
		.NumEraseRegions= 1,
		.regions	= {
			ERASEINFO(0x10000,8),
		}
	}, {
		.mfr_id		= MANUFACTURER_AMD,
		.dev_id		= AM29F002T,
		.name		= "AMD AM29F002T",
		.uaddr		= {
			[0] = MTD_UADDR_0x0555_0x02AA /* x8 */
		},
		.DevSize	= SIZE_256KiB,
		.CmdSet		= P_ID_AMD_STD,
		.NumEraseRegions= 4,
		.regions	= {
			ERASEINFO(0x10000,3),
			ERASEINFO(0x08000,1),
			ERASEINFO(0x02000,2),
			ERASEINFO(0x04000,1),
		}
	}, {
		.mfr_id		= MANUFACTURER_ATMEL,
		.dev_id		= AT49BV512,
		.name		= "Atmel AT49BV512",
		.uaddr		= {
			[0] = MTD_UADDR_0x5555_0x2AAA /* x8 */
		},
		.DevSize	= SIZE_64KiB,
		.CmdSet		= P_ID_AMD_STD,
		.NumEraseRegions= 1,
		.regions	= {
			ERASEINFO(0x10000,1)
		}
	}, {
		.mfr_id		= MANUFACTURER_ATMEL,
		.dev_id		= AT29LV512,
		.name		= "Atmel AT29LV512",
		.uaddr		= {
			[0] = MTD_UADDR_0x5555_0x2AAA /* x8 */
		},
		.DevSize	= SIZE_64KiB,
		.CmdSet		= P_ID_AMD_STD,
		.NumEraseRegions= 1,
		.regions	= {
			ERASEINFO(0x80,256),
			ERASEINFO(0x80,256)
		}
	}, {
		.mfr_id		= MANUFACTURER_ATMEL,
		.dev_id		= AT49BV16X,
		.name		= "Atmel AT49BV16X",
		.uaddr		= {
			[0] = MTD_UADDR_0x0555_0x0AAA,  /* x8 */
			[1] = MTD_UADDR_0x0555_0x0AAA   /* x16 */
		},
		.DevSize	= SIZE_2MiB,
		.CmdSet		= P_ID_AMD_STD,
		.NumEraseRegions= 2,
		.regions	= {
			ERASEINFO(0x02000,8),
			ERASEINFO(0x10000,31)
		}
	}, {
		.mfr_id		= MANUFACTURER_ATMEL,
		.dev_id		= AT49BV16XT,
		.name		= "Atmel AT49BV16XT",
		.uaddr		= {
			[0] = MTD_UADDR_0x0555_0x0AAA,  /* x8 */
			[1] = MTD_UADDR_0x0555_0x0AAA   /* x16 */
		},
		.DevSize	= SIZE_2MiB,
		.CmdSet		= P_ID_AMD_STD,
		.NumEraseRegions= 2,
		.regions	= {
			ERASEINFO(0x10000,31),
			ERASEINFO(0x02000,8)
		}
	}, {
		.mfr_id		= MANUFACTURER_ATMEL,
		.dev_id		= AT49BV32X,
		.name		= "Atmel AT49BV32X",
		.uaddr		= {
			[0] = MTD_UADDR_0x0555_0x0AAA,  /* x8 */
			[1] = MTD_UADDR_0x0555_0x0AAA   /* x16 */
		},
		.DevSize	= SIZE_4MiB,
		.CmdSet		= P_ID_AMD_STD,
		.NumEraseRegions= 2,
		.regions	= {
			ERASEINFO(0x02000,8),
			ERASEINFO(0x10000,63)
		}
	}, {
		.mfr_id		= MANUFACTURER_ATMEL,
		.dev_id		= AT49BV32XT,
		.name		= "Atmel AT49BV32XT",
		.uaddr		= {
			[0] = MTD_UADDR_0x0555_0x0AAA,  /* x8 */
			[1] = MTD_UADDR_0x0555_0x0AAA   /* x16 */
		},
		.DevSize	= SIZE_4MiB,
		.CmdSet		= P_ID_AMD_STD,
		.NumEraseRegions= 2,
		.regions	= {
			ERASEINFO(0x10000,63),
			ERASEINFO(0x02000,8)
		}
	}, {
		.mfr_id		= MANUFACTURER_FUJITSU,
		.dev_id		= MBM29F040C,
		.name		= "Fujitsu MBM29F040C",
		.uaddr		= {
			[0] = MTD_UADDR_0x0AAA_0x0555, /* x8 */
		},
		.DevSize	= SIZE_512KiB,
		.CmdSet		= P_ID_AMD_STD,
		.NumEraseRegions= 1,
		.regions	= {
			ERASEINFO(0x10000,8)
		}
	}, {
		.mfr_id		= MANUFACTURER_FUJITSU,
		.dev_id		= MBM29LV650UE,
		.name		= "Fujitsu MBM29LV650UE",
		.uaddr		= {
			[0] = MTD_UADDR_DONT_CARE     /* x16 */
		},
		.DevSize	= SIZE_8MiB,
		.CmdSet		= P_ID_AMD_STD,
		.NumEraseRegions= 1,
		.regions	= {
			ERASEINFO(0x10000,128)
		}
	}, {
		.mfr_id		= MANUFACTURER_FUJITSU,
		.dev_id		= MBM29LV320TE,
		.name		= "Fujitsu MBM29LV320TE",
		.uaddr		= {
			[0] = MTD_UADDR_0x0AAA_0x0555,  /* x8 */
			[1] = MTD_UADDR_0x0555_0x02AA,  /* x16 */
		},
		.DevSize	= SIZE_4MiB,
		.CmdSet		= P_ID_AMD_STD,
		.NumEraseRegions= 2,
		.regions	= {
			ERASEINFO(0x10000,63),
			ERASEINFO(0x02000,8)
		}
	}, {
		.mfr_id		= MANUFACTURER_FUJITSU,
		.dev_id		= MBM29LV320BE,
		.name		= "Fujitsu MBM29LV320BE",
		.uaddr		= {
			[0] = MTD_UADDR_0x0AAA_0x0555,  /* x8 */
			[1] = MTD_UADDR_0x0555_0x02AA,  /* x16 */
		},
		.DevSize	= SIZE_4MiB,
		.CmdSet		= P_ID_AMD_STD,
		.NumEraseRegions= 2,
		.regions	= {
			ERASEINFO(0x02000,8),
			ERASEINFO(0x10000,63)
		}
	}, {
		.mfr_id		= MANUFACTURER_FUJITSU,
		.dev_id		= MBM29LV160TE,
		.name		= "Fujitsu MBM29LV160TE",
		.uaddr		= {
			[0] = MTD_UADDR_0x0AAA_0x0555,  /* x8 */
			[1] = MTD_UADDR_0x0555_0x02AA,  /* x16 */
		},
		.DevSize	= SIZE_2MiB,
		.CmdSet		= P_ID_AMD_STD,
		.NumEraseRegions= 4,
		.regions	= {
			ERASEINFO(0x10000,31),
			ERASEINFO(0x08000,1),
			ERASEINFO(0x02000,2),
			ERASEINFO(0x04000,1)
		}
	}, {
		.mfr_id		= MANUFACTURER_FUJITSU,
		.dev_id		= MBM29LV160BE,
		.name		= "Fujitsu MBM29LV160BE",
		.uaddr		= {
			[0] = MTD_UADDR_0x0AAA_0x0555,  /* x8 */
			[1] = MTD_UADDR_0x0555_0x02AA,  /* x16 */
		},
		.DevSize	= SIZE_2MiB,
		.CmdSet		= P_ID_AMD_STD,
		.NumEraseRegions= 4,
		.regions	= {
			ERASEINFO(0x04000,1),
			ERASEINFO(0x02000,2),
			ERASEINFO(0x08000,1),
			ERASEINFO(0x10000,31)
		}
	}, {
		.mfr_id		= MANUFACTURER_FUJITSU,
		.dev_id		= MBM29LV800BA,
		.name		= "Fujitsu MBM29LV800BA",
		.uaddr		= {
			[0] = MTD_UADDR_0x0AAA_0x0555,  /* x8 */
			[1] = MTD_UADDR_0x0555_0x02AA,  /* x16 */
		},
		.DevSize	= SIZE_1MiB,
		.CmdSet		= P_ID_AMD_STD,
		.NumEraseRegions= 4,
		.regions	= {
			ERASEINFO(0x04000,1),
			ERASEINFO(0x02000,2),
			ERASEINFO(0x08000,1),
			ERASEINFO(0x10000,15)
		}
	}, {
		.mfr_id		= MANUFACTURER_FUJITSU,
		.dev_id		= MBM29LV800TA,
		.name		= "Fujitsu MBM29LV800TA",
		.uaddr		= {
			[0] = MTD_UADDR_0x0AAA_0x0555,  /* x8 */
			[1] = MTD_UADDR_0x0555_0x02AA,  /* x16 */
		},
		.DevSize	= SIZE_1MiB,
		.CmdSet		= P_ID_AMD_STD,
		.NumEraseRegions= 4,
		.regions	= {
			ERASEINFO(0x10000,15),
			ERASEINFO(0x08000,1),
			ERASEINFO(0x02000,2),
			ERASEINFO(0x04000,1)
		}
	}, {
		.mfr_id		= MANUFACTURER_FUJITSU,
		.dev_id		= MBM29LV400BC,
		.name		= "Fujitsu MBM29LV400BC",
		.uaddr		= {
			[0] = MTD_UADDR_0x0AAA_0x0555,  /* x8 */
			[1] = MTD_UADDR_0x0555_0x02AA,  /* x16 */
		},
		.DevSize	= SIZE_512KiB,
		.CmdSet		= P_ID_AMD_STD,
		.NumEraseRegions= 4,
		.regions	= {
			ERASEINFO(0x04000,1),
			ERASEINFO(0x02000,2),
			ERASEINFO(0x08000,1),
			ERASEINFO(0x10000,7)
		}
	}, {
		.mfr_id		= MANUFACTURER_FUJITSU,
		.dev_id		= MBM29LV400TC,
		.name		= "Fujitsu MBM29LV400TC",
		.uaddr		= {
			[0] = MTD_UADDR_0x0AAA_0x0555,  /* x8 */
			[1] = MTD_UADDR_0x0555_0x02AA,  /* x16 */
		},
		.DevSize	= SIZE_512KiB,
		.CmdSet		= P_ID_AMD_STD,
		.NumEraseRegions= 4,
		.regions	= {
			ERASEINFO(0x10000,7),
			ERASEINFO(0x08000,1),
			ERASEINFO(0x02000,2),
			ERASEINFO(0x04000,1)
		}
	}, {
		.mfr_id		= MANUFACTURER_HYUNDAI,
		.dev_id		= HY29F002T,
		.name		= "Hyundai HY29F002T",
		.uaddr		= {
			[0] = MTD_UADDR_0x0555_0x02AA /* x8 */
		},
		.DevSize	= SIZE_256KiB,
		.CmdSet		= P_ID_AMD_STD,
		.NumEraseRegions= 4,
		.regions	= {
			ERASEINFO(0x10000,3),
			ERASEINFO(0x08000,1),
			ERASEINFO(0x02000,2),
			ERASEINFO(0x04000,1),
		}
	}, {
		.mfr_id		= MANUFACTURER_INTEL,
		.dev_id		= I28F004B3B,
		.name		= "Intel 28F004B3B",
		.uaddr		= {
			[0] = MTD_UADDR_UNNECESSARY,    /* x8 */
		},
		.DevSize	= SIZE_512KiB,
		.CmdSet		= P_ID_INTEL_STD,
		.NumEraseRegions= 2,
		.regions	= {
			ERASEINFO(0x02000, 8),
			ERASEINFO(0x10000, 7),
		}
	}, {
		.mfr_id		= MANUFACTURER_INTEL,
		.dev_id		= I28F004B3T,
		.name		= "Intel 28F004B3T",
		.uaddr		= {
			[0] = MTD_UADDR_UNNECESSARY,    /* x8 */
		},
		.DevSize	= SIZE_512KiB,
		.CmdSet		= P_ID_INTEL_STD,
		.NumEraseRegions= 2,
		.regions	= {
			ERASEINFO(0x10000, 7),
			ERASEINFO(0x02000, 8),
		}
	}, {
		.mfr_id		= MANUFACTURER_INTEL,
		.dev_id		= I28F400B3B,
		.name		= "Intel 28F400B3B",
		.uaddr		= {
			[0] = MTD_UADDR_UNNECESSARY,    /* x8 */
			[1] = MTD_UADDR_UNNECESSARY,    /* x16 */
		},
		.DevSize	= SIZE_512KiB,
		.CmdSet		= P_ID_INTEL_STD,
		.NumEraseRegions= 2,
		.regions	= {
			ERASEINFO(0x02000, 8),
			ERASEINFO(0x10000, 7),
		}
	}, {
		.mfr_id		= MANUFACTURER_INTEL,
		.dev_id		= I28F400B3T,
		.name		= "Intel 28F400B3T",
		.uaddr		= {
			[0] = MTD_UADDR_UNNECESSARY,    /* x8 */
			[1] = MTD_UADDR_UNNECESSARY,    /* x16 */
		},
		.DevSize	= SIZE_512KiB,
		.CmdSet		= P_ID_INTEL_STD,
		.NumEraseRegions= 2,
		.regions	= {
			ERASEINFO(0x10000, 7),
			ERASEINFO(0x02000, 8),
		}
	}, {
		.mfr_id		= MANUFACTURER_INTEL,
		.dev_id		= I28F008B3B,
		.name		= "Intel 28F008B3B",
		.uaddr		= {
			[0] = MTD_UADDR_UNNECESSARY,    /* x8 */
		},
		.DevSize	= SIZE_1MiB,
		.CmdSet		= P_ID_INTEL_STD,
		.NumEraseRegions= 2,
		.regions	= {
			ERASEINFO(0x02000, 8),
			ERASEINFO(0x10000, 15),
		}
	}, {
		.mfr_id		= MANUFACTURER_INTEL,
		.dev_id		= I28F008B3T,
		.name		= "Intel 28F008B3T",
		.uaddr		= {
			[0] = MTD_UADDR_UNNECESSARY,    /* x8 */
		},
		.DevSize	= SIZE_1MiB,
		.CmdSet		= P_ID_INTEL_STD,
		.NumEraseRegions= 2,
		.regions	= {
			ERASEINFO(0x10000, 15),
			ERASEINFO(0x02000, 8),
		}
	}, {
		.mfr_id		= MANUFACTURER_INTEL,
		.dev_id		= I28F008S5,
		.name		= "Intel 28F008S5",
		.uaddr		= {
			[0] = MTD_UADDR_UNNECESSARY,    /* x8 */
		},
		.DevSize	= SIZE_1MiB,
		.CmdSet		= P_ID_INTEL_EXT,
		.NumEraseRegions= 1,
		.regions	= {
			ERASEINFO(0x10000,16),
		}
	}, {
		.mfr_id		= MANUFACTURER_INTEL,
		.dev_id		= I28F016S5,
		.name		= "Intel 28F016S5",
		.uaddr		= {
			[0] = MTD_UADDR_UNNECESSARY,    /* x8 */
		},
		.DevSize	= SIZE_2MiB,
		.CmdSet		= P_ID_INTEL_EXT,
		.NumEraseRegions= 1,
		.regions	= {
			ERASEINFO(0x10000,32),
		}
	}, {
		.mfr_id		= MANUFACTURER_INTEL,
		.dev_id		= I28F008SA,
		.name		= "Intel 28F008SA",
		.uaddr		= {
			[0] = MTD_UADDR_UNNECESSARY,    /* x8 */
		},
		.DevSize	= SIZE_1MiB,
		.CmdSet		= P_ID_INTEL_STD,
		.NumEraseRegions= 1,
		.regions	= {
			ERASEINFO(0x10000, 16),
		}
	}, {
		.mfr_id		= MANUFACTURER_INTEL,
		.dev_id		= I28F800B3B,
		.name		= "Intel 28F800B3B",
		.uaddr		= {
			[1] = MTD_UADDR_UNNECESSARY,    /* x16 */
		},
		.DevSize	= SIZE_1MiB,
		.CmdSet		= P_ID_INTEL_STD,
		.NumEraseRegions= 2,
		.regions	= {
			ERASEINFO(0x02000, 8),
			ERASEINFO(0x10000, 15),
		}
	}, {
		.mfr_id		= MANUFACTURER_INTEL,
		.dev_id		= I28F800B3T,
		.name		= "Intel 28F800B3T",
		.uaddr		= {
			[1] = MTD_UADDR_UNNECESSARY,    /* x16 */
		},
		.DevSize	= SIZE_1MiB,
		.CmdSet		= P_ID_INTEL_STD,
		.NumEraseRegions= 2,
		.regions	= {
			ERASEINFO(0x10000, 15),
			ERASEINFO(0x02000, 8),
		}
	}, {
		.mfr_id		= MANUFACTURER_INTEL,
		.dev_id		= I28F016B3B,
		.name		= "Intel 28F016B3B",
		.uaddr		= {
			[0] = MTD_UADDR_UNNECESSARY,    /* x8 */
		},
		.DevSize	= SIZE_2MiB,
		.CmdSet		= P_ID_INTEL_STD,
		.NumEraseRegions= 2,
		.regions	= {
			ERASEINFO(0x02000, 8),
			ERASEINFO(0x10000, 31),
		}
	}, {
		.mfr_id		= MANUFACTURER_INTEL,
		.dev_id		= I28F016S3,
		.name		= "Intel I28F016S3",
		.uaddr		= {
			[0] = MTD_UADDR_UNNECESSARY,    /* x8 */
		},
		.DevSize	= SIZE_2MiB,
		.CmdSet		= P_ID_INTEL_STD,
		.NumEraseRegions= 1,
		.regions	= {
			ERASEINFO(0x10000, 32),
		}
	}, {
		.mfr_id		= MANUFACTURER_INTEL,
		.dev_id		= I28F016B3T,
		.name		= "Intel 28F016B3T",
		.uaddr		= {
			[0] = MTD_UADDR_UNNECESSARY,    /* x8 */
		},
		.DevSize	= SIZE_2MiB,
		.CmdSet		= P_ID_INTEL_STD,
		.NumEraseRegions= 2,
		.regions	= {
			ERASEINFO(0x10000, 31),
			ERASEINFO(0x02000, 8),
		}
	}, {
		.mfr_id		= MANUFACTURER_INTEL,
		.dev_id		= I28F160B3B,
		.name		= "Intel 28F160B3B",
		.uaddr		= {
			[1] = MTD_UADDR_UNNECESSARY,    /* x16 */
		},
		.DevSize	= SIZE_2MiB,
		.CmdSet		= P_ID_INTEL_STD,
		.NumEraseRegions= 2,
		.regions	= {
			ERASEINFO(0x02000, 8),
			ERASEINFO(0x10000, 31),
		}
	}, {
		.mfr_id		= MANUFACTURER_INTEL,
		.dev_id		= I28F160B3T,
		.name		= "Intel 28F160B3T",
		.uaddr		= {
			[1] = MTD_UADDR_UNNECESSARY,    /* x16 */
		},
		.DevSize	= SIZE_2MiB,
		.CmdSet		= P_ID_INTEL_STD,
		.NumEraseRegions= 2,
		.regions	= {
			ERASEINFO(0x10000, 31),
			ERASEINFO(0x02000, 8),
		}
	}, {
		.mfr_id		= MANUFACTURER_INTEL,
		.dev_id		= I28F320B3B,
		.name		= "Intel 28F320B3B",
		.uaddr		= {
			[1] = MTD_UADDR_UNNECESSARY,    /* x16 */
		},
		.DevSize	= SIZE_4MiB,
		.CmdSet		= P_ID_INTEL_STD,
		.NumEraseRegions= 2,
		.regions	= {
			ERASEINFO(0x02000, 8),
			ERASEINFO(0x10000, 63),
		}
	}, {
		.mfr_id		= MANUFACTURER_INTEL,
		.dev_id		= I28F320B3T,
		.name		= "Intel 28F320B3T",
		.uaddr		= {
			[1] = MTD_UADDR_UNNECESSARY,    /* x16 */
		},
		.DevSize	= SIZE_4MiB,
		.CmdSet		= P_ID_INTEL_STD,
		.NumEraseRegions= 2,
		.regions	= {
			ERASEINFO(0x10000, 63),
			ERASEINFO(0x02000, 8),
		}
	}, {
		.mfr_id		= MANUFACTURER_INTEL,
		.dev_id		= I28F640B3B,
		.name		= "Intel 28F640B3B",
		.uaddr		= {
			[1] = MTD_UADDR_UNNECESSARY,    /* x16 */
		},
		.DevSize	= SIZE_8MiB,
		.CmdSet		= P_ID_INTEL_STD,
		.NumEraseRegions= 2,
		.regions	= {
			ERASEINFO(0x02000, 8),
			ERASEINFO(0x10000, 127),
		}
	}, {
		.mfr_id		= MANUFACTURER_INTEL,
		.dev_id		= I28F640B3T,
		.name		= "Intel 28F640B3T",
		.uaddr		= {
			[1] = MTD_UADDR_UNNECESSARY,    /* x16 */
		},
		.DevSize	= SIZE_8MiB,
		.CmdSet		= P_ID_INTEL_STD,
		.NumEraseRegions= 2,
		.regions	= {
			ERASEINFO(0x10000, 127),
			ERASEINFO(0x02000, 8),
		}
	}, {
		.mfr_id		= MANUFACTURER_INTEL,
		.dev_id		= I82802AB,
		.name		= "Intel 82802AB",
		.uaddr		= {
			[0] = MTD_UADDR_UNNECESSARY,    /* x8 */
		},
		.DevSize	= SIZE_512KiB,
		.CmdSet		= P_ID_INTEL_EXT,
		.NumEraseRegions= 1,
		.regions	= {
			ERASEINFO(0x10000,8),
		}
	}, {
		.mfr_id		= MANUFACTURER_INTEL,
		.dev_id		= I82802AC,
		.name		= "Intel 82802AC",
		.uaddr		= {
			[0] = MTD_UADDR_UNNECESSARY,    /* x8 */
		},
		.DevSize	= SIZE_1MiB,
		.CmdSet		= P_ID_INTEL_EXT,
		.NumEraseRegions= 1,
		.regions	= {
			ERASEINFO(0x10000,16),
		}
	}, {
		.mfr_id		= MANUFACTURER_MACRONIX,
		.dev_id		= MX29LV040C,
		.name		= "Macronix MX29LV040C",
		.uaddr		= {
			[0] = MTD_UADDR_0x0555_0x02AA,  /* x8 */
		},
		.DevSize	= SIZE_512KiB,
		.CmdSet		= P_ID_AMD_STD,
		.NumEraseRegions= 1,
		.regions	= {
			ERASEINFO(0x10000,8),
		}
	}, {
		.mfr_id		= MANUFACTURER_MACRONIX,
		.dev_id		= MX29LV160T,
		.name		= "MXIC MX29LV160T",
		.uaddr		= {
			[0] = MTD_UADDR_0x0AAA_0x0555,  /* x8 */
			[1] = MTD_UADDR_0x0555_0x02AA,  /* x16 */
		},
		.DevSize	= SIZE_2MiB,
		.CmdSet		= P_ID_AMD_STD,
		.NumEraseRegions= 4,
		.regions	= {
			ERASEINFO(0x10000,31),
			ERASEINFO(0x08000,1),
			ERASEINFO(0x02000,2),
			ERASEINFO(0x04000,1)
		}
	}, {
		.mfr_id		= MANUFACTURER_NEC,
		.dev_id		= UPD29F064115,
		.name		= "NEC uPD29F064115",
		.uaddr		= {
			[0] = MTD_UADDR_0x0555_0x02AA,  /* x8 */
			[1] = MTD_UADDR_0x0555_0x02AA,  /* x16 */
		},
		.DevSize	= SIZE_8MiB,
		.CmdSet		= P_ID_AMD_STD,
		.NumEraseRegions= 3,
		.regions	= {
			ERASEINFO(0x2000,8),
			ERASEINFO(0x10000,126),
			ERASEINFO(0x2000,8),
		}
	}, {
		.mfr_id		= MANUFACTURER_MACRONIX,
		.dev_id		= MX29LV160B,
		.name		= "MXIC MX29LV160B",
		.uaddr		= {
			[0] = MTD_UADDR_0x0AAA_0x0555,  /* x8 */
			[1] = MTD_UADDR_0x0555_0x02AA,  /* x16 */
		},
		.DevSize	= SIZE_2MiB,
		.CmdSet		= P_ID_AMD_STD,
		.NumEraseRegions= 4,
		.regions	= {
			ERASEINFO(0x04000,1),
			ERASEINFO(0x02000,2),
			ERASEINFO(0x08000,1),
			ERASEINFO(0x10000,31)
		}
	}, {
		.mfr_id		= MANUFACTURER_MACRONIX,
		.dev_id		= MX29F040,
		.name		= "Macronix MX29F040",
		.uaddr		= {
			[0] = MTD_UADDR_0x0555_0x02AA /* x8 */
		},
		.DevSize	= SIZE_512KiB,
		.CmdSet		= P_ID_AMD_STD,
		.NumEraseRegions= 1,
		.regions	= {
			ERASEINFO(0x10000,8),
		}
        }, {
		.mfr_id		= MANUFACTURER_MACRONIX,
		.dev_id		= MX29F016,
		.name		= "Macronix MX29F016",
		.uaddr		= {
			[0] = MTD_UADDR_0x0555_0x02AA /* x8 */
		},
		.DevSize	= SIZE_2MiB,
		.CmdSet		= P_ID_AMD_STD,
		.NumEraseRegions= 1,
		.regions	= {
			ERASEINFO(0x10000,32),
		}
        }, {
		.mfr_id		= MANUFACTURER_MACRONIX,
		.dev_id		= MX29F004T,
		.name		= "Macronix MX29F004T",
		.uaddr		= {
			[0] = MTD_UADDR_0x0555_0x02AA /* x8 */
		},
		.DevSize	= SIZE_512KiB,
		.CmdSet		= P_ID_AMD_STD,
		.NumEraseRegions= 4,
		.regions	= {
			ERASEINFO(0x10000,7),
			ERASEINFO(0x08000,1),
			ERASEINFO(0x02000,2),
			ERASEINFO(0x04000,1),
		}
        }, {
		.mfr_id		= MANUFACTURER_MACRONIX,
		.dev_id		= MX29F004B,
		.name		= "Macronix MX29F004B",
		.uaddr		= {
			[0] = MTD_UADDR_0x0555_0x02AA /* x8 */
		},
		.DevSize	= SIZE_512KiB,
		.CmdSet		= P_ID_AMD_STD,
		.NumEraseRegions= 4,
		.regions	= {
			ERASEINFO(0x04000,1),
			ERASEINFO(0x02000,2),
			ERASEINFO(0x08000,1),
			ERASEINFO(0x10000,7),
		}
	}, {
		.mfr_id		= MANUFACTURER_MACRONIX,
		.dev_id		= MX29F002T,
		.name		= "Macronix MX29F002T",
		.uaddr		= {
			[0] = MTD_UADDR_0x0555_0x02AA /* x8 */
		},
		.DevSize	= SIZE_256KiB,
		.CmdSet		= P_ID_AMD_STD,
		.NumEraseRegions= 4,
		.regions	= {
			ERASEINFO(0x10000,3),
			ERASEINFO(0x08000,1),
			ERASEINFO(0x02000,2),
			ERASEINFO(0x04000,1),
		}
	}, {
		.mfr_id		= MANUFACTURER_PMC,
		.dev_id		= PM49FL002,
		.name		= "PMC Pm49FL002",
 		.uaddr		= {
			[0] = MTD_UADDR_0x5555_0x2AAA /* x8 */
		},
		.DevSize	= SIZE_256KiB,
		.CmdSet		= P_ID_AMD_STD,
		.NumEraseRegions= 1,
		.regions	= {
			ERASEINFO( 0x01000, 64 )
		}
	}, {
		.mfr_id		= MANUFACTURER_PMC,
		.dev_id		= PM49FL004,
		.name		= "PMC Pm49FL004",
 		.uaddr		= {
			[0] = MTD_UADDR_0x5555_0x2AAA /* x8 */
		},
		.DevSize	= SIZE_512KiB,
		.CmdSet		= P_ID_AMD_STD,
		.NumEraseRegions= 1,
		.regions	= {
			ERASEINFO( 0x01000, 128 )
		}
	}, {
		.mfr_id		= MANUFACTURER_PMC,
		.dev_id		= PM49FL008,
		.name		= "PMC Pm49FL008",
 		.uaddr		= {
			[0] = MTD_UADDR_0x5555_0x2AAA /* x8 */
		},
		.DevSize	= SIZE_1MiB,
		.CmdSet		= P_ID_AMD_STD,
		.NumEraseRegions= 1,
		.regions	= {
			ERASEINFO( 0x01000, 256 )
		}
	}, {
		.mfr_id		= MANUFACTURER_SHARP,
		.dev_id		= LH28F640BF,
		.name		= "LH28F640BF",
		.uaddr		= {
			[0] = MTD_UADDR_UNNECESSARY,    /* x8 */
		},
		.DevSize	= SIZE_4MiB,
		.CmdSet         = P_ID_INTEL_STD,
		.NumEraseRegions= 1,
		.regions        = {
			ERASEINFO(0x40000,16),
		}
        }, {
		.mfr_id		= MANUFACTURER_SST,
		.dev_id		= SST39LF512,
		.name		= "SST 39LF512",
 		.uaddr		= {
			[0] = MTD_UADDR_0x5555_0x2AAA /* x8 */
		},
		.DevSize	= SIZE_64KiB,
		.CmdSet		= P_ID_AMD_STD,
		.NumEraseRegions= 1,
		.regions	= {
			ERASEINFO(0x01000,16),
		}
        }, {
		.mfr_id		= MANUFACTURER_SST,
		.dev_id		= SST39LF010,
		.name		= "SST 39LF010",
 		.uaddr		= {
			[0] = MTD_UADDR_0x5555_0x2AAA /* x8 */
		},
		.DevSize	= SIZE_128KiB,
		.CmdSet		= P_ID_AMD_STD,
		.NumEraseRegions= 1,
		.regions	= {
			ERASEINFO(0x01000,32),
		}
        }, {
		.mfr_id		= MANUFACTURER_SST,
 		.dev_id 	= SST29EE020,
		.name		= "SST 29EE020",
 		.uaddr		= {
			[0] = MTD_UADDR_0x5555_0x2AAA /* x8 */
		},
 		.DevSize	= SIZE_256KiB,
 		.CmdSet		= P_ID_SST_PAGE,
 		.NumEraseRegions= 1,
 		.regions = {ERASEINFO(0x01000,64),
 		}
         }, {
 		.mfr_id		= MANUFACTURER_SST,
		.dev_id		= SST29LE020,
 		.name		= "SST 29LE020",
 		.uaddr		= {
			[0] = MTD_UADDR_0x5555_0x2AAA /* x8 */
		},
 		.DevSize	= SIZE_256KiB,
 		.CmdSet		= P_ID_SST_PAGE,
 		.NumEraseRegions= 1,
 		.regions = {ERASEINFO(0x01000,64),
 		}
	}, {
		.mfr_id		= MANUFACTURER_SST,
		.dev_id		= SST39LF020,
		.name		= "SST 39LF020",
 		.uaddr		= {
			[0] = MTD_UADDR_0x5555_0x2AAA /* x8 */
		},
		.DevSize	= SIZE_256KiB,
		.CmdSet		= P_ID_AMD_STD,
		.NumEraseRegions= 1,
		.regions	= {
			ERASEINFO(0x01000,64),
		}
        }, {
		.mfr_id		= MANUFACTURER_SST,
		.dev_id		= SST39LF040,
		.name		= "SST 39LF040",
 		.uaddr		= {
			[0] = MTD_UADDR_0x5555_0x2AAA /* x8 */
		},
		.DevSize	= SIZE_512KiB,
		.CmdSet		= P_ID_AMD_STD,
		.NumEraseRegions= 1,
		.regions	= {
			ERASEINFO(0x01000,128),
		}
        }, {
		.mfr_id		= MANUFACTURER_SST,
		.dev_id		= SST39SF010A,
		.name		= "SST 39SF010A",
 		.uaddr		= {
			[0] = MTD_UADDR_0x5555_0x2AAA /* x8 */
		},
		.DevSize	= SIZE_128KiB,
		.CmdSet		= P_ID_AMD_STD,
		.NumEraseRegions= 1,
		.regions	= {
			ERASEINFO(0x01000,32),
		}
        }, {
		.mfr_id		= MANUFACTURER_SST,
		.dev_id		= SST39SF020A,
		.name		= "SST 39SF020A",
 		.uaddr		= {
			[0] = MTD_UADDR_0x5555_0x2AAA /* x8 */
		},
		.DevSize	= SIZE_256KiB,
		.CmdSet		= P_ID_AMD_STD,
		.NumEraseRegions= 1,
		.regions	= {
			ERASEINFO(0x01000,64),
		}
	}, {
		.mfr_id		= MANUFACTURER_SST,
		.dev_id         = SST49LF040B,
		.name           = "SST 49LF040B",
		.uaddr          = {
			[0] = MTD_UADDR_0x5555_0x2AAA /* x8 */
		},
		.DevSize        = SIZE_512KiB,
		.CmdSet         = P_ID_AMD_STD,
		.NumEraseRegions= 1,
		.regions        = {
			ERASEINFO(0x01000,128),
		}
	}, {

		.mfr_id		= MANUFACTURER_SST,
		.dev_id		= SST49LF004B,
		.name		= "SST 49LF004B",
 		.uaddr		= {
			[0] = MTD_UADDR_0x5555_0x2AAA /* x8 */
		},
		.DevSize	= SIZE_512KiB,
		.CmdSet		= P_ID_AMD_STD,
		.NumEraseRegions= 1,
		.regions	= {
			ERASEINFO(0x01000,128),
		}
	}, {
		.mfr_id		= MANUFACTURER_SST,
		.dev_id		= SST49LF008A,
		.name		= "SST 49LF008A",
 		.uaddr		= {
			[0] = MTD_UADDR_0x5555_0x2AAA /* x8 */
		},
		.DevSize	= SIZE_1MiB,
		.CmdSet		= P_ID_AMD_STD,
		.NumEraseRegions= 1,
		.regions	= {
			ERASEINFO(0x01000,256),
		}
	}, {
		.mfr_id		= MANUFACTURER_SST,
		.dev_id		= SST49LF030A,
		.name		= "SST 49LF030A",
 		.uaddr		= {
			[0] = MTD_UADDR_0x5555_0x2AAA /* x8 */
		},
		.DevSize	= SIZE_512KiB,
		.CmdSet		= P_ID_AMD_STD,
		.NumEraseRegions= 1,
		.regions	= {
			ERASEINFO(0x01000,96),
		}
	}, {
		.mfr_id		= MANUFACTURER_SST,
		.dev_id		= SST49LF040A,
		.name		= "SST 49LF040A",
 		.uaddr		= {
			[0] = MTD_UADDR_0x5555_0x2AAA /* x8 */
		},
		.DevSize	= SIZE_512KiB,
		.CmdSet		= P_ID_AMD_STD,
		.NumEraseRegions= 1,
		.regions	= {
			ERASEINFO(0x01000,128),
		}
	}, {
		.mfr_id		= MANUFACTURER_SST,
		.dev_id		= SST49LF080A,
		.name		= "SST 49LF080A",
 		.uaddr		= {
			[0] = MTD_UADDR_0x5555_0x2AAA /* x8 */
		},
		.DevSize	= SIZE_1MiB,
		.CmdSet		= P_ID_AMD_STD,
		.NumEraseRegions= 1,
		.regions	= {
			ERASEINFO(0x01000,256),
		}
	}, {
               .mfr_id         = MANUFACTURER_SST,     /* should be CFI */
               .dev_id         = SST39LF160,
               .name           = "SST 39LF160",
               .uaddr          = {
                       [0] = MTD_UADDR_0x5555_0x2AAA,  /* x8 */
                       [1] = MTD_UADDR_0x5555_0x2AAA   /* x16 */
               },
               .DevSize        = SIZE_2MiB,
               .CmdSet         = P_ID_AMD_STD,
               .NumEraseRegions= 2,
               .regions        = {
                       ERASEINFO(0x1000,256),
                       ERASEINFO(0x1000,256)
               }
	}, {
               .mfr_id         = MANUFACTURER_SST,     /* should be CFI */
               .dev_id         = SST39VF1601,
               .name           = "SST 39VF1601",
               .uaddr          = {
                       [0] = MTD_UADDR_0x5555_0x2AAA,  /* x8 */
                       [1] = MTD_UADDR_0x5555_0x2AAA   /* x16 */
               },
               .DevSize        = SIZE_2MiB,
               .CmdSet         = P_ID_AMD_STD,
               .NumEraseRegions= 2,
               .regions        = {
                       ERASEINFO(0x1000,256),
                       ERASEINFO(0x1000,256)
               }

       }, {
		.mfr_id		= MANUFACTURER_ST,	/* FIXME - CFI device? */
		.dev_id		= M29W800DT,
		.name		= "ST M29W800DT",
 		.uaddr		= {
			[0] = MTD_UADDR_0x5555_0x2AAA,  /* x8 */
			[1] = MTD_UADDR_0x5555_0x2AAA   /* x16 */
		},
		.DevSize	= SIZE_1MiB,
		.CmdSet		= P_ID_AMD_STD,
		.NumEraseRegions= 4,
		.regions	= {
			ERASEINFO(0x10000,15),
			ERASEINFO(0x08000,1),
			ERASEINFO(0x02000,2),
			ERASEINFO(0x04000,1)
		}
	}, {
		.mfr_id		= MANUFACTURER_ST,	/* FIXME - CFI device? */
		.dev_id		= M29W800DB,
		.name		= "ST M29W800DB",
 		.uaddr		= {
			[0] = MTD_UADDR_0x5555_0x2AAA,  /* x8 */
			[1] = MTD_UADDR_0x5555_0x2AAA   /* x16 */
		},
		.DevSize	= SIZE_1MiB,
		.CmdSet		= P_ID_AMD_STD,
		.NumEraseRegions= 4,
		.regions	= {
			ERASEINFO(0x04000,1),
			ERASEINFO(0x02000,2),
			ERASEINFO(0x08000,1),
			ERASEINFO(0x10000,15)
		}
	}, {
		.mfr_id		= MANUFACTURER_ST,	/* FIXME - CFI device? */
		.dev_id		= M29W160DT,
		.name		= "ST M29W160DT",
		.uaddr		= {
			[0] = MTD_UADDR_0x0555_0x02AA,  /* x8 */
			[1] = MTD_UADDR_0x0555_0x02AA,  /* x16 */
		},
		.DevSize	= SIZE_2MiB,
		.CmdSet		= P_ID_AMD_STD,
		.NumEraseRegions= 4,
		.regions	= {
			ERASEINFO(0x10000,31),
			ERASEINFO(0x08000,1),
			ERASEINFO(0x02000,2),
			ERASEINFO(0x04000,1)
		}
	}, {
		.mfr_id		= MANUFACTURER_ST,	/* FIXME - CFI device? */
		.dev_id		= M29W160DB,
		.name		= "ST M29W160DB",
		.uaddr		= {
			[0] = MTD_UADDR_0x0555_0x02AA,  /* x8 */
			[1] = MTD_UADDR_0x0555_0x02AA,  /* x16 */
		},
		.DevSize	= SIZE_2MiB,
		.CmdSet		= P_ID_AMD_STD,
		.NumEraseRegions= 4,
		.regions	= {
			ERASEINFO(0x04000,1),
			ERASEINFO(0x02000,2),
			ERASEINFO(0x08000,1),
			ERASEINFO(0x10000,31)
		}
        }, {
		.mfr_id		= MANUFACTURER_ST,
		.dev_id		= M29W040B,
		.name		= "ST M29W040B",
		.uaddr		= {
			[0] = MTD_UADDR_0x0555_0x02AA /* x8 */
		},
		.DevSize	= SIZE_512KiB,
		.CmdSet		= P_ID_AMD_STD,
		.NumEraseRegions= 1,
		.regions	= {
			ERASEINFO(0x10000,8),
		}
        }, {
		.mfr_id		= MANUFACTURER_ST,
		.dev_id		= M50FW040,
		.name		= "ST M50FW040",
		.uaddr		= {
			[0] = MTD_UADDR_UNNECESSARY,    /* x8 */
		},
		.DevSize	= SIZE_512KiB,
		.CmdSet		= P_ID_INTEL_EXT,
		.NumEraseRegions= 1,
		.regions	= {
			ERASEINFO(0x10000,8),
		}
        }, {
		.mfr_id		= MANUFACTURER_ST,
		.dev_id		= M50FW080,
		.name		= "ST M50FW080",
		.uaddr		= {
			[0] = MTD_UADDR_UNNECESSARY,    /* x8 */
		},
		.DevSize	= SIZE_1MiB,
		.CmdSet		= P_ID_INTEL_EXT,
		.NumEraseRegions= 1,
		.regions	= {
			ERASEINFO(0x10000,16),
		}
        }, {
		.mfr_id		= MANUFACTURER_ST,
		.dev_id		= M50FW016,
		.name		= "ST M50FW016",
		.uaddr		= {
			[0] = MTD_UADDR_UNNECESSARY,    /* x8 */
		},
		.DevSize	= SIZE_2MiB,
		.CmdSet		= P_ID_INTEL_EXT,
		.NumEraseRegions= 1,
		.regions	= {
			ERASEINFO(0x10000,32),
		}
	}, {
		.mfr_id		= MANUFACTURER_ST,
		.dev_id		= M50LPW080,
		.name		= "ST M50LPW080",
		.uaddr		= {
			[0] = MTD_UADDR_UNNECESSARY,    /* x8 */
		},
		.DevSize	= SIZE_1MiB,
		.CmdSet		= P_ID_INTEL_EXT,
		.NumEraseRegions= 1,
		.regions	= {
			ERASEINFO(0x10000,16),
		}
	}, {
		.mfr_id		= MANUFACTURER_TOSHIBA,
		.dev_id		= TC58FVT160,
		.name		= "Toshiba TC58FVT160",
		.uaddr		= {
			[0] = MTD_UADDR_0x0AAA_0x0555, /* x8 */
			[1] = MTD_UADDR_0x0555_0x02AA  /* x16 */
		},
		.DevSize	= SIZE_2MiB,
		.CmdSet		= P_ID_AMD_STD,
		.NumEraseRegions= 4,
		.regions	= {
			ERASEINFO(0x10000,31),
			ERASEINFO(0x08000,1),
			ERASEINFO(0x02000,2),
			ERASEINFO(0x04000,1)
		}
	}, {
		.mfr_id		= MANUFACTURER_TOSHIBA,
		.dev_id		= TC58FVB160,
		.name		= "Toshiba TC58FVB160",
		.uaddr		= {
			[0] = MTD_UADDR_0x0AAA_0x0555, /* x8 */
			[1] = MTD_UADDR_0x0555_0x02AA  /* x16 */
		},
		.DevSize	= SIZE_2MiB,
		.CmdSet		= P_ID_AMD_STD,
		.NumEraseRegions= 4,
		.regions	= {
			ERASEINFO(0x04000,1),
			ERASEINFO(0x02000,2),
			ERASEINFO(0x08000,1),
			ERASEINFO(0x10000,31)
		}
	}, {
		.mfr_id		= MANUFACTURER_TOSHIBA,
		.dev_id		= TC58FVB321,
		.name		= "Toshiba TC58FVB321",
		.uaddr		= {
			[0] = MTD_UADDR_0x0AAA_0x0555, /* x8 */
			[1] = MTD_UADDR_0x0555_0x02AA  /* x16 */
		},
		.DevSize	= SIZE_4MiB,
		.CmdSet		= P_ID_AMD_STD,
		.NumEraseRegions= 2,
		.regions	= {
			ERASEINFO(0x02000,8),
			ERASEINFO(0x10000,63)
		}
	}, {
		.mfr_id		= MANUFACTURER_TOSHIBA,
		.dev_id		= TC58FVT321,
		.name		= "Toshiba TC58FVT321",
		.uaddr		= {
			[0] = MTD_UADDR_0x0AAA_0x0555, /* x8 */
			[1] = MTD_UADDR_0x0555_0x02AA  /* x16 */
		},
		.DevSize	= SIZE_4MiB,
		.CmdSet		= P_ID_AMD_STD,
		.NumEraseRegions= 2,
		.regions	= {
			ERASEINFO(0x10000,63),
			ERASEINFO(0x02000,8)
		}
	}, {
		.mfr_id		= MANUFACTURER_TOSHIBA,
		.dev_id		= TC58FVB641,
		.name		= "Toshiba TC58FVB641",
		.uaddr		= {
			[0] = MTD_UADDR_0x0AAA_0x0555, /* x8 */
			[1] = MTD_UADDR_0x0555_0x02AA, /* x16 */
		},
		.DevSize	= SIZE_8MiB,
		.CmdSet		= P_ID_AMD_STD,
		.NumEraseRegions= 2,
		.regions	= {
			ERASEINFO(0x02000,8),
			ERASEINFO(0x10000,127)
		}
	}, {
		.mfr_id		= MANUFACTURER_TOSHIBA,
		.dev_id		= TC58FVT641,
		.name		= "Toshiba TC58FVT641",
		.uaddr		= {
			[0] = MTD_UADDR_0x0AAA_0x0555, /* x8 */
			[1] = MTD_UADDR_0x0555_0x02AA, /* x16 */
		},
		.DevSize	= SIZE_8MiB,
		.CmdSet		= P_ID_AMD_STD,
		.NumEraseRegions= 2,
		.regions	= {
			ERASEINFO(0x10000,127),
			ERASEINFO(0x02000,8)
		}
	}, {
		.mfr_id		= MANUFACTURER_WINBOND,
		.dev_id		= W49V002A,
		.name		= "Winbond W49V002A",
		.uaddr		= {
			[0] = MTD_UADDR_0x5555_0x2AAA /* x8 */
		},
		.DevSize	= SIZE_256KiB,
		.CmdSet		= P_ID_AMD_STD,
		.NumEraseRegions= 4,
		.regions	= {
			ERASEINFO(0x10000, 3),
			ERASEINFO(0x08000, 1),
			ERASEINFO(0x02000, 2),
			ERASEINFO(0x04000, 1),
		}
	}
};

static inline void fill_info(flash_info_t *info, const struct amd_flash_info *jedec_entry, ulong base)
{
	int i,j;
	int sect_cnt;
	int size_ratio;
	int total_size;
	enum uaddr uaddr_idx;

	size_ratio = info->portwidth / info->chipwidth;

	debug("Found JEDEC Flash: %s\n", jedec_entry->name);
	info->vendor = jedec_entry->CmdSet;
	/* Todo: do we need device-specific timeouts? */
	info->erase_blk_tout = 30000;
	info->buffer_write_tout = 1000;
	info->write_tout = 100;
	info->name = jedec_entry->name;

	/* copy unlock addresses from device table to CFI info struct. This
	   is just here because the addresses are in the table anyway - if
	   the flash is not detected due to wrong unlock addresses,
	   flash_detect_legacy would have to try all of them before we even
	   get here. */
	switch(info->chipwidth) {
	case FLASH_CFI_8BIT:
		uaddr_idx = jedec_entry->uaddr[0];
		break;
	case FLASH_CFI_16BIT:
		uaddr_idx = jedec_entry->uaddr[1];
		break;
	case FLASH_CFI_32BIT:
		uaddr_idx = jedec_entry->uaddr[2];
		break;
	default:
		uaddr_idx = MTD_UADDR_NOT_SUPPORTED;
		break;
	}

	debug("unlock address index %d\n", uaddr_idx);
	info->addr_unlock1 = unlock_addrs[uaddr_idx].addr1;
	info->addr_unlock2 = unlock_addrs[uaddr_idx].addr2;
	debug("unlock addresses are 0x%x/0x%x\n", info->addr_unlock1, info->addr_unlock2);

	sect_cnt = 0;
	total_size = 0;
	for (i = 0; i < jedec_entry->NumEraseRegions; i++) {
		ulong erase_region_size = jedec_entry->regions[i] >> 8;
		ulong erase_region_count = (jedec_entry->regions[i] & 0xff) + 1;

		total_size += erase_region_size * erase_region_count;
		debug ("erase_region_count = %d erase_region_size = %d\n",
		       erase_region_count, erase_region_size);
		for (j = 0; j < erase_region_count; j++) {
			if (sect_cnt >= CFG_MAX_FLASH_SECT) {
				printf("ERROR: too many flash sectors\n");
				break;
			}
			info->start[sect_cnt] = base;
			base += (erase_region_size * size_ratio);
			sect_cnt++;
		}
	}
	info->sector_count = sect_cnt;
	info->size = total_size * size_ratio;
}

/*-----------------------------------------------------------------------
 * match jedec ids against table. If a match is found, fill flash_info entry
 */
int jedec_flash_match(flash_info_t *info, ulong base)
{
	int ret = 0;
	int i;
	ulong mask = 0xFFFF;
	if (info->chipwidth == 1)
		mask = 0xFF;

	for (i = 0; i < ARRAY_SIZE(jedec_table); i++) {
		if ((jedec_table[i].mfr_id & mask) == (info->manufacturer_id & mask) &&
		    (jedec_table[i].dev_id & mask) == (info->device_id & mask)) {
			fill_info(info, &jedec_table[i], base);
			ret = 1;
			break;
		}
	}
	return ret;
}

extern void flash_read_jedec_ids (flash_info_t * info);

int jedec_flash_probe(flash_info_t *info, ulong base)
{
	int u;
	for (u = MTD_UADDR_0x0555_0x02AA; u <= MTD_UADDR_UNNECESSARY; u++) {
		info->addr_unlock1 = unlock_addrs[u].addr1;
		info->addr_unlock2 = unlock_addrs[u].addr2;
		flash_read_jedec_ids(info);
		debug("JEDEC PROBE: ID %x %x %x\n",
				info->manufacturer_id,
				info->device_id,
				info->device_id2);
		if (jedec_flash_match(info, base))
			return 1;
	}
	return 0;
}

