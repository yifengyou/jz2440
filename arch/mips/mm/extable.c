/*
 * linux/arch/mips/mm/extable.c
 */
#include <linux/module.h>
#include <linux/spinlock.h>
#include <linux/kgdb.h>
#include <asm/branch.h>
#include <asm/uaccess.h>

int fixup_exception(struct pt_regs *regs)
{
	const struct exception_table_entry *fixup;

	fixup = search_exception_tables(exception_epc(regs));
	if (fixup) {
		regs->cp0_epc = fixup->nextinsn;

		return 1;
	}
#ifdef CONFIG_KGDB
	if (atomic_read(&debugger_active) && kgdb_may_fault)
		/* Restore our previous state. */
		kgdb_fault_longjmp(kgdb_fault_jmp_regs);
		/* Not reached. */
#endif

	return 0;
}
