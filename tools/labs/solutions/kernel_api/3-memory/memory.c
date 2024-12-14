/*
 * SO2 lab3 - task 3
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>
#include <linux/jiffies.h>

MODULE_DESCRIPTION("Memory processing");
MODULE_AUTHOR("SO2");
MODULE_LICENSE("GPL");

struct task_info {
	pid_t pid;
	unsigned long timestamp;
};

static struct task_info *ti1, *ti2, *ti3, *ti4;

static struct task_info *task_info_alloc(int pid)
{
	struct task_info *ti;

	/* TODO 1: allocated and initialize a task_info struct */
	ti = kmalloc(sizeof(struct task_info), GFP_KERNEL);
	if (ti == NULL)
		return NULL;
	ti->pid = pid;
	ti->timestamp = jiffies;
	return ti;
}

static int memory_init(void)
{
	struct task_struct *p;

	/* TODO 2: call task_info_alloc for current pid */
	p = current;
	ti1 = task_info_alloc(p->pid);

	/* TODO 2: call task_info_alloc for parent PID */
	p = p->parent;
	ti2 = task_info_alloc(p->pid);

	/* TODO 2: call task_info alloc for next process PID */
	p = next_task(current);
	ti3 = task_info_alloc(p->pid);

	/* TODO 2: call task_info_alloc for next process of the next process */
	p = next_task(p);
	p = next_task(p);
	ti4 = task_info_alloc(p->pid);

	return 0;
}

static void memory_exit(void)
{

	/* TODO 3: print ti* field values */
	if (ti1 != NULL)
		printk("PID: %d, Timestamp: %lu", ti1->pid, ti1->timestamp);
	if (ti2 != NULL)
		printk("PID: %d, Timestamp: %lu", ti2->pid, ti2->timestamp);
	if (ti3 != NULL)
		printk("PID: %d, Timestamp: %lu", ti3->pid, ti3->timestamp);
	if (ti4 != NULL)
		printk("PID: %d, Timestamp: %lu", ti4->pid, ti4->timestamp);

	/* TODO 4: free ti* structures */
	kfree(ti1);
	kfree(ti2);
	kfree(ti3);
	kfree(ti4);
}

module_init(memory_init);
module_exit(memory_exit);
