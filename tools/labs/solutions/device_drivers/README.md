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

# 2. Register an already registered major
Modify **`MY_MAJOR`** so that it points to an already used major number. See `errno-base.h` and figure out what does the error code mean. Return to the initial configuration of the module.
- Result from `insmod`:
```
root@qemux86:~/skels/device_drivers/kernel# insmod so2_cdev.ko
Error Code: -16
insmod: ERROR: could not insert module so2_cdev.ko: Device or resource busy
root@qemux86:~/skels/device_drivers/kernel#
```
- By setting `MY_MAJOR` to an already used major, it resulted in `-EBUSY`, or that a device or resource is busy.

# 3. Open and close
Run `cat /dev/so2_cdev` to read data from our char device. Reading does not work because the driver does not have the open function implemented. Follow comments marked with `TODO 2` and implement them.
1. Initialize your device.
    - **TIP:** The `cdev` struct represents a character device.
    - To **initialize** the `cdev` struct, use `cdev_init()` after the character device is assigned its identifiers using `register_chrdev_region()`. Then, notify the kernel using `cdev_add()`; note, `cdev_add()` must be called only after the device is ready to receive calls.
    - To **remove** the `cdev` struct from the kernel core, use `cdev_del`; note that this is done when removing devices from the machine.
2. Implement the open and release functions in the driver.
    - **TIP:** `so2_cdev_open()` and `so2_cdev_release()` defined in the kernel module should be passed into the function pointers `open` and `release`, respectively, of the defined `so2_fops` struct.
3. Display a message in the open and release functions.
    - Used `pr_info` to print the message to the debug log.
4. Read again /dev/so2_cdev file. Follow the messages displayed by the kernel. We still get an error because read function is not yet implemented.
    - Error encountered:
    ``` bash
    root@qemux86:~/skels/device_drivers/kernel# cat /dev/so2_cdev 
    cat: read error: Invalid argument
    so2_cdev device file was opened.
    root@qemux86:~/skels/device_drivers/kernel#
    ```

# 4. Access restriction
Restrict access to the device with atomic variables, so that a single process can open the device at a time. The rest will receive the "device busy" error (`-EBUSY`). **Restricting access will be done in the open function displayed by the driver.** Follow comments marked with `TODO 3` and implement them.
1. Add an `atomic_t` variable to the device structure.
2. Initialize the variable at module initialization.
    - **TIP:** Use `atomic_set()` to set the value of the `atomic_t` variable in the device struct.
3. Use the variable in the open function to restrict access to the device. We recommend using `atomic_cmpxchg()`.
    - **TIP:** `int atomic_cmpxchg(atomic_t *v, int old, int new)` represents an "atomic compare-and-exchange" operation.
        - If the value contained in `v` matches `old`, then it is replaced by the `new` value.
        - The **return value** is the "actual old value"; that is, the value that was (or currently is) stored in `v`.
    - Given that we initialize the `atomic_t` variable in the device structure to be 0, we should check that `atomic_t` is either `0` (meaning the device is not in use by someone) or `1` (meaning the device is in use by someone).
    - **WARNING:** Don't forget to initialize the `data` variable (i.e., pointer to a `so2_device_data` struct) using `container_of` as follows: `data = container_of(inode->i_cdev, struct so2_device_data, cdev);`
        - Remember that `container_of` can be used to retrieve the whole structure wrapping one of its members whose pointer you have.
4. Reset the variable in the release function to retrieve access to the device.
    - **TIP:** Use `atomic_set()` to set the value of the `atomic_t` variable in the device struct.
5. To test your deployment, you'll need to simulate a long-term use of your device. To simulate a sleep, call the scheduler at the end of the device opening using `schedule_timeout(10 * HZ)` (which puts the program to sleep for 10 seconds).
    - Console Output
    ``` bash
    root@qemux86:~/skels/device_drivers/kernel# insmod so2_cdev.ko
    Successfully registered char device region.
    root@qemux86:~/skels/device_drivers/kernel# cat /dev/so2_cdev &     # Executes first read command in the background.
    root@qemux86:~/skels/device_drivers/kernel# cat /dev/so2_cdev       # Second read fails with code -EBUSY.
    so2_cdev device file was opened.
    cat: can't open '/dev/so2_cdev': Device or resource busy
    root@qemux86:~/skels/device_drivers/kernel# cat: read error: Invalid argument
    so2_cdev device file was opened.

    [1]+  Done(1)                    cat /dev/so2_cdev
    root@qemux86:~/skels/device_drivers/kernel#
    ```

# 5. Read operation
