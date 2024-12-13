#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/mm_types.h>

MODULE_DESCRIPTION("Displays the virtual memory areas of the current process.");
MODULE_AUTHOR("EricFlorin");
MODULE_LICENSE("GPL");

static int ps_vma_mod_init(void) {
	struct task_struct *p = current;
	struct mm_struct *mm = p->mm;
	struct vm_area_struct *mmap = mm->mmap;
	unsigned long vm_num = 0;
	unsigned long vm_start = 0;
	unsigned long vm_end = 0;

	// Print `current` process information.
	printk("Current task name: %.*s\n", TASK_COMM_LEN, p->comm);
	printk("Current task PID: %d\n", p->pid);

	// Print VMA areas.
	printk("-----------------------\n");
	printk("    VMA #    START_ADDR    END_ADDR\n");
	while (mmap != NULL) {
		vm_start = mmap->vm_start;
		vm_end = mmap->vm_end;
		printk("    %lx    %lx    %lx", vm_num, vm_start, vm_end);
		vm_num++;
		mmap = mmap->vm_next;
	}
	return 0;
}

static void ps_vma_mod_exit(void) {
	pr_debug("Unloading ps_vma_mod...");
}

module_init(ps_vma_mod_init);
module_exit(ps_vma_mod_exit);
