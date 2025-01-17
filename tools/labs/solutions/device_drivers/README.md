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
    - **Command to use:** `mknod /dev/so2_cdev c 42 0`
    - Result:
    ```
    root@qemux86:~# mknod /dev/so2_cdev c 42 0
    root@qemux86:~# ls -l /dev/ | grep so2_cdev
    crw-r--r--    1 root     root       42,   0 Jan  7 04:55 so2_cdev
    ```
2. Implement the registration and deregistration of the device with the name `so2_cdev`, respectively in the init and exit module functions. Implement **TODO 1**.
    - To **register** the device with the name "so2_cdev", do the following:
    ``` C
    int err;

    // MKDEV() is a macro used to create a `dev_t` containing the "major" and "minor" of
    // the device.
    //
    // NOTE: Under the hood, `dev_t` is defined as an unsigned integer
    err = register_chrdev_region(MKDEV(YOUR_MAJOR, YOUR_MINOR), YOUR_MINOR_COUNT, "DEVICE NAME")
    
    // Check for and report any errors.
    if (err != 0)
        return err;
    ```
    - To **unregister** the device, do the following:
    ``` C
    unregister_chrdev_region(MKDEV(YOUR_MAJOR, YOUR_MINOR), YOUR_MAX_MINORS);
    ```
    - **NOTE:** Remember that "MAJOR" specifies the device type and "MINOR" specifies the specific device categorize as that type.
    - **NOTE:** In both `register_chrdev_region()` and `unregister_chrdev_region()`, the parameter `dev_t first` specifies the *first* device in the desired range of `unsigned count` device numbers; so the range is `[first, first + count)`
3. Display, using `pr_info`, a message after the registration and unregistration operations to confirm that they were successful. Then load the module into the kernel and see character devices in `/proc/devices`. Identify the device type registered with major 42 . Note that `/proc/devices` contains only the device types (major) but not the actual devices (i.e. minors).
    - Output of `/proc/devices` after loading kernel module.
    ```
    root@qemux86:~/skels/device_drivers/kernel# cat /proc/devices
    Character devices:
    1 mem
    2 pty
    3 ttyp
    4 /dev/vc/0
    4 tty
    5 /dev/tty
    5 /dev/console
    5 /dev/ptmx
    7 vcs
    10 misc
    13 input
    42 so2_cdev
    128 ptm
    136 pts
    229 hvc
    253 virtio-portsdev
    254 bsg

    Block devices:
    7 loop
    254 virtblk
    259 blkext
    root@qemux86:~/skels/device_drivers/kernel#
    ```
4. Unload the kernel module.
    - Output of `/proc/devices` after unloading the kernel module.
    ```
    root@qemux86:~/skels/device_drivers/kernel# cat /proc/devices
    Character devices:
    1 mem
    2 pty
    3 ttyp
    4 /dev/vc/0
    4 tty
    5 /dev/tty
    5 /dev/console
    5 /dev/ptmx
    7 vcs
    10 misc
    13 input
    128 ptm
    136 pts
    229 hvc
    253 virtio-portsdev
    254 bsg

    Block devices:
    7 loop
    254 virtblk
    259 blkext
    root@qemux86:~/skels/device_drivers/kernel#
    ```
