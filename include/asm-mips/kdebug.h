/*
 *
 * Copyright (C) 2004  MontaVista Software Inc.
 * Author: Manish Lachwani, mlachwani@mvista.com or manish@koffee-break.com
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */
#ifndef _MIPS_KDEBUG_H
#define _MIPS_KDEBUG_H

#include <linux/notifier.h>

struct pt_regs;

extern struct atomic_notifier_head mips_die_head;

enum die_val {
	DIE_OOPS = 1,
	DIE_PANIC,
	DIE_DIE,
	DIE_KERNELDEBUG,
	DIE_TRAP,
	DIE_PAGE_FAULT,
};

#endif /* _MIPS_KDEBUG_H */
