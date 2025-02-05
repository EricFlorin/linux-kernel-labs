/*
 * Character device drivers lab
 *
 * All tasks
 */

#include <linux/types.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/uaccess.h>

#include "../include/so2_cdev.h"

MODULE_DESCRIPTION("SO2 character device");
MODULE_AUTHOR("SO2");
MODULE_LICENSE("GPL");

#define LOG_LEVEL	KERN_INFO

#define MY_MAJOR		42
#define MY_MINOR		0
#define NUM_MINORS		1
#define MODULE_NAME		"so2_cdev"
#define MESSAGE			"hello\n"
#define IOCTL_MESSAGE		"Hello ioctl"

#ifndef BUFSIZ
#define BUFSIZ		4096
#endif

// NOTE(EricFlorin): Uncomment the EXTRA macro if you are testing code for "Extra Exercises"
#define EXTRA

struct so2_device_data {
	/* TODO 2: add cdev member */
	struct cdev cdev;
	/* TODO 4: add buffer with BUFSIZ elements */
	char buffer[BUFSIZ];
	/* TODO 7: extra members for home */
	wait_queue_head_t wait_queue;
	atomic_t wake_up_flag;
	/* TODO 3: add atomic_t access variable to keep track if file is opened */
	atomic_t lock;
};

struct so2_device_data devs[NUM_MINORS];

static int so2_cdev_open(struct inode *inode, struct file *file)
{
	struct so2_device_data *data;

	/* TODO 2: print message when the device file is open. */
	pr_info("so2_cdev device file was opened.");

	/* TODO 3: inode->i_cdev contains our cdev struct, use container_of to obtain a pointer to so2_device_data */
	data = container_of(inode->i_cdev, struct so2_device_data, cdev);

	file->private_data = data;

#ifndef EXTRA
	/* TODO 3: return immediately if access is != 0, use atomic_cmpxchg */
	if (atomic_cmpxchg(&data->lock, 0, 1) != 0) return -EBUSY;
#endif

	set_current_state(TASK_INTERRUPTIBLE);
	schedule_timeout(10 * HZ);

	return 0;
}

static int
so2_cdev_release(struct inode *inode, struct file *file)
{
	/* TODO 2: print message when the device file is closed. */
	pr_info("so2_cdev device file was released.");

#ifndef EXTRA
	struct so2_device_data *data =
		(struct so2_device_data *) file->private_data;

	/* TODO 3: reset access variable to 0, use atomic_set */
	atomic_set(&data->lock, 0);
#endif
	return 0;
}

static ssize_t
so2_cdev_read(struct file *file,
		char __user *user_buffer,
		size_t size, loff_t *offset)
{
	struct so2_device_data *data =
		(struct so2_device_data *) file->private_data;
	size_t bytes_not_copied, to_read;

#ifdef EXTRA
	/* TODO 7: extra tasks for home */
#endif

	/* TODO 4: Copy data->buffer to user_buffer, use copy_to_user */
	// Calculate the number of bytes we can actually read from the buffer.
	// DO NOT READ BEYOND THE BUFFER!
	to_read = min(size, BUFSIZ - *offset);

	// If there is nothing to read, return EOF.
	if (to_read == 0)
		return 0;

	// Copy data from kernel buffer to user buffer.
	// NOTE: `copy_to_user` returns the number of bytes that WERE NOT copied.
	// See: https://developer.ibm.com/articles/l-kernel-memory-access/
	bytes_not_copied = copy_to_user(user_buffer, data->buffer + *offset, to_read);

	// If copy was successful, update the offset.
	*offset += to_read - bytes_not_copied;

	// Return the number of bytes actually read.
	return to_read - bytes_not_copied;
}

static ssize_t
so2_cdev_write(struct file *file,
		const char __user *user_buffer,
		size_t size, loff_t *offset)
{
	struct so2_device_data *data =
		(struct so2_device_data *) file->private_data;


	/* TODO 5: copy user_buffer to data->buffer, use copy_from_user */
	copy_from_user(data->buffer, user_buffer, size);

	/* TODO 7: extra tasks for home */

	return size;
}

static long
so2_cdev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	struct so2_device_data *data =
		(struct so2_device_data *) file->private_data;
	int ret = 0;
	int remains;

	switch (cmd) {
	/* TODO 6: if cmd = MY_IOCTL_PRINT, display IOCTL_MESSAGE */
	case MY_IOCTL_PRINT:
		pr_info("%s\n", IOCTL_MESSAGE);
		break;
	/* TODO 7: extra tasks, for home */
	case MY_IOCTL_SET_BUFFER:
		// Writing a message to the device.
		ret = copy_from_user(data->buffer, (void *) arg, BUFFER_SIZE);
		break;
	case MY_IOCTL_GET_BUFFER:
		// Read a message from your device.
		ret = copy_to_user((void *) arg, data->buffer, BUFFER_SIZE);
		break;
	case MY_IOCTL_DOWN:
		// Add the process to the wait queue.
		printk("Adding process %d to wait queue...\n", current->pid);
		atomic_set(&data->wake_up_flag, 0);
		ret = wait_event_interruptible(data->wait_queue, atomic_read(&data->wake_up_flag) != 0);
		break;
	case MY_IOCTL_UP:
		// Remove the process from the wait queue.
		printk("Process %d is waking up threads in wait queue...\n", current->pid);
		atomic_set(&data->wake_up_flag, 1);
		wake_up(&data->wait_queue);
		break;
	default:
		ret = -EINVAL;
	}

	return ret;
}

static const struct file_operations so2_fops = {
	.owner = THIS_MODULE,
/* TODO 2: add open and release functions */
	.open = so2_cdev_open,
	.release = so2_cdev_release,
/* TODO 4: add read function */
	.read = so2_cdev_read,
/* TODO 5: add write function */
	.write = so2_cdev_write,
/* TODO 6: add ioctl function */
	.unlocked_ioctl = so2_cdev_ioctl
};

static int so2_cdev_init(void)
{
	int err;
	int i;

	/* TODO 1: register char device region for MY_MAJOR and NUM_MINORS starting at MY_MINOR */
	err = register_chrdev_region(MKDEV(MY_MAJOR, MY_MINOR), NUM_MINORS, "so2_cdev");
	if (err != 0)
	{
		pr_info("Error Code: %d\n", err);
		return err;
	}
	pr_info("Successfully registered char device region.\n");

	for (i = 0; i < NUM_MINORS; i++) {
#ifdef EXTRA
		/* TODO 7: extra tasks, for home */
		// Initialize wait queue.
		init_waitqueue_head(&devs[i].wait_queue);
		atomic_set(&devs[i].wake_up_flag, 0);
#else
		/*TODO 4: initialize buffer with MESSAGE string */
		strncpy(devs[i].buffer, MESSAGE, BUFSIZ);

		/* TODO 3: set access variable to 0, use atomic_set */
		atomic_set(&devs[i].lock, 0);
#endif
		/* TODO 7: extra tasks for home */

		/* TODO 2: init and add cdev to kernel core */
		cdev_init(&devs[i].cdev, &so2_fops);
		cdev_add(&devs[i].cdev, MKDEV(MY_MAJOR, i), 1);
	}

	return 0;
}

static void so2_cdev_exit(void)
{
	int i;

	for (i = 0; i < NUM_MINORS; i++) {
		/* TODO 2: delete cdev from kernel core */
		cdev_del(&devs[i].cdev);
	}

	/* TODO 1: unregister char device region, for MY_MAJOR and NUM_MINORS starting at MY_MINOR */
	unregister_chrdev_region(MKDEV(MY_MAJOR, MY_MINOR), NUM_MINORS);
	pr_info("Successfully unregistered char device region.\n");
}

module_init(so2_cdev_init);
module_exit(so2_cdev_exit);
