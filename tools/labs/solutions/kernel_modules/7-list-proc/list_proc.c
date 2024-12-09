#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/fs.h>
/* TODO: add missing headers */

MODULE_DESCRIPTION("List current processes");
MODULE_AUTHOR("Kernel Hacker");
MODULE_LICENSE("GPL");

static int my_proc_init(void)
{
	struct task_struct *p = NULL;

	/* TODO: print current process pid and its name */
	p = current;
	printk("Current task name: %.*s\n", TASK_COMM_LEN, p->comm);
	printk("Current task PID: %d\n", p->pid);
	printk("-----------------------\n");

	/* TODO: print the pid and name of all processes */
	for_each_process(p)
		printk("Name: %.*s | PID: %d", TASK_COMM_LEN, p->comm, p->pid);

	return 0;
}

static void my_proc_exit(void)
{
	struct task_struct *p = NULL;
	
	/* TODO: print current process pid and name */
	printk("Exiting...\n");
	p = current;
	printk("Current task name: %.*s\n", TASK_COMM_LEN, p->comm);
	printk("Current task PID: %d\n", p->pid);
}

module_init(my_proc_init);
module_exit(my_proc_exit);
