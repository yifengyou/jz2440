#ifdef __KERNEL__
#ifndef __PPC_KGDB_H__
#define __PPC_KGDB_H__
#include <asm-powerpc/kgdb.h>
#ifndef __ASSEMBLY__
/* For taking exceptions
 * these are defined in traps.c
 */
struct pt_regs;
extern void (*debugger)(struct pt_regs *regs);
extern int (*debugger_bpt)(struct pt_regs *regs);
extern int (*debugger_sstep)(struct pt_regs *regs);
extern int (*debugger_iabr_match)(struct pt_regs *regs);
extern int (*debugger_dabr_match)(struct pt_regs *regs);
extern void (*debugger_fault_handler)(struct pt_regs *regs);
#endif /* !__ASSEMBLY__ */
#endif /* __PPC_KGDB_H__ */
#endif /* __KERNEL__ */
