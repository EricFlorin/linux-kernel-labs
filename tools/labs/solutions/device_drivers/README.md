# 0. Intro
Using LXR find the definitions of the following symbols in the Linux kernel:
- `struct file`
    - Defined in `include/linux/fs.h`
    - Represents the dynamic image of the file (i.e. flags with which the file was opened, private data).
- `struct file_operations`
    - Defined in `include/linux/fs.h`
    - Contains function pointers that point to functions representing different `file` operations.
- `generic_ro_fops`
    - Defined in `fs/read_write.c`
    - A `struct file_operations` object that point to generic read-only file operations.
- `vfs_read()`
    - Defined in `fs/read_write.c`
    - Responsible for looking up the appropriate `file_operations` structure to perform a read operation.

# 1. Register/Unregister
The driver will control a single device with the `MY_MAJOR` major and `MY_MINOR` minor (the macros defined in the `kernel/so2_cdev.c` file).
1. Create **`/dev/so2_cdev`** character device node using **mknod**.
    - ...
2. Implement the registration and deregistration of the device with the name `so2_cdev`, respectively in the init and exit module functions. Implement **TODO 1**.
    - ...
3. Display, using `pr_info`, a message after the registration and unregistration operations to confirm that they were successful. Then load the module into the kernel and see character devices in `/proc/devices`. Identify the device type registered with major 42 . Note that `/proc/devices` contains only the device types (major) but not the actual devices (i.e. minors).
    - ...
4. Unload the kernel module.
    - ...
