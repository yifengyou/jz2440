/*
 *  linux/arch/arm/mm/extable.c
 */
#include <linux/module.h>
#include <linux/kgdb.h>
#include <asm/uaccess.h>

int fixup_exception(struct pt_regs *regs)
{
	const struct exception_table_entry *fixup;

	fixup = search_exception_tables(instruction_pointer(regs));
	if (fixup)
		regs->ARM_pc = fixup->fixup;
#ifdef CONFIG_KGDB
	if (atomic_read(&debugger_active) && kgdb_may_fault)
		/* Restore our previous state. */
		kgdb_fault_longjmp(kgdb_fault_jmp_regs);
		/* Not reached. */
#endif

	return fixup != NULL;
}
