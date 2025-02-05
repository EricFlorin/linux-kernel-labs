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
    ```
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
    ```
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
Implement the read function in the driver. Follow comments marked with `TODO 4` and implement them.
1. Keep a buffer in `so2_device_data` structure initialized with the value of `MESSAGE` macro. Initializing this buffer will be done in module `init` function.
    - We will define the buffer to be a `char[]`.
    - In `so2_cdev_init()`, use `strncpy()` to copy `BUFSIZ` characters into the buffer held by `so2_device_data`.
2. At a read call, copy the contents of the kernel space buffer into the user space buffer.
    - First, we need to add `so2_cdev_read` to the `read` function pointer stored by `so2_fops`.
    - Second, we need to copy the contents of `so2_device_data->buffer` from kernel-space into user-space using `copy_to_user()`.
        - **REMINDER:** Initialize the `to_read` variable in `so2_cdev_read` to be the number of bytes transmitted from the kernel-space buffer to user-space buffer, given by `copy_to_user()`.
3. After implementation, test using `cat /dev/so2_cdev`.
    - Console Output:
    ```
    root@qemux86:~/skels/device_drivers/kernel# cat /dev/so2_cdev 
    so2_cdev device file was released.
    so2_cdev device file was opened.
    root@qemux86:~/skels/device_drivers/kernel#
    ```

The `cat` command reads to the end of the file, and the end of the file is signaled by returning the value 0 in the read. Thus, for a correct implementation, you will need to update and use the offset received as a parameter in the read function and return the value 0 when the user has reached the end of the buffer.

Modify the driver so that the cat commands ends:
- **IMPORTANT:**  According to [this article](https://developer.ibm.com/articles/l-kernel-memory-access/), the `copy_to_user()` function returns the number of bytes that **weren't** copied to User-space; that is, `copy_to_user()` returns **0 if all bytes were transferred** and **non-negative representing the number of bytes that DID NOT transfer.**
- **IMPORTANT:** `size` is the number of bytes the User wants to read, and `offset` is the location the read operation should start from the base buffer address.
- The process for copying the buffer from kernel-space into user-space is as follows:
    1. Initialize `to_read` to be the number of bytes we can actually read from the buffer without going over.
        - Use the `min(size, BUFSIZ - *offset)` macro to set `to_read` to be either the number of bytes the User wants to read or the number of bytes that is left to read in the buffer, which ever one is less.
        - **If there is nothing left to read,** then notify EOF to the caller by returning `0`.
    2. Copy data from the kernel buffer to the user buffer.
        - Use `copy_to_user(user_buffer, data->buffer + *offset, to_read)` to read the next set of bytes in the buffer, starting from the given offset.
        - **IMPORTANT:** Store the *number of bytes not transferred* into `size_t bytes_not_copied` to later update the `offset` value.
    3. Update the `offset` value.
        - We want the new `offset` value to be the starting position of the next read.
        - **TIP:** Recall that `to_read` is the number of bytes we attempted to read from the buffer, and `bytes_not_copied` is the number of bytes that failed to transfer from kernel-space to user-space. Thus, the next read starting position should be after the bytes we were able to transfer from kernel-space to user-space, or `*offset += to_read - bytes_not_copied`.
    4. Return the number of bytes we were able to read.
        - That is, `return to_read - bytes_not_copied`.

**Final console output:**
```
root@qemux86:~/skels/device_drivers/kernel# cat /dev/so2_cdev
hello
so2_cdev device file was opened.
root@qemux86:~/skels/device_drivers/kernel#
```

# 6. Write operation
Add the ability to write a message into kernel buffer to replace the predefined message. Implement the write function in the driver. Follow comments marked with `TODO 5`

Ignore the offset parameter at this time. You can assume that the driver buffer is large enough. You do not need to check the validity of the write function size argument.

Process
1. Add the `write` function to `so2_fops`.
    - Basically, `.write = so2_cdev_write`
2. Use `copy_from_user()` to copy memory from user-space into kernel-space.
    - **IMPORTANT:** `copy_from_user()` returns 0 on success and non-negative representing the number of bytes that *failed* to copy.

**Final Console Output:**
```
root@qemux86:~/skels/device_drivers/kernel# echo "arpeggio" > /dev/so2_cdev
so2_cdev device file was opened.
root@qemux86:~/skels/device_drivers/kernel# cat /dev/so2_cdev
so2_cdev device file was released.
arpeggio
so2_cdev device file was opened.
root@qemux86:~/skels/device_drivers/kernel#
```

# 7. ioctl operation
For this exercise, we want to add the ioctl `MY_IOCTL_PRINT` to display the message from the `IOCTL_MESSAGE` macro in the driver. Follow the comments marked with `TODO 6`
1. Implement the ioctl function in the driver.
    - Added a case to the given switch statement in `so2_cdev_ioctl` that will print `IOCTL_MESSAGE` to the log file.
    - **NOTE:** When adding the `so2_cdev_ioctl` function to `so2_fops`, you find that there are two versions of the `ioctl` function pointer:
        - `compat_ioctl()` is used by 64-bit systems for 32-bit applications to make a `ioctl()` call; think of `compat_ioctl()` being used for backward compatibility with 32-bit applications.
        - `unlocked_ioctl()` could be viewed as the "default" implementation of `ioctl()`; have the `unlocked_ioctl()` function pointer in `so2_fops` point to `so2_cdev_ioctl`.
2. We need to use `user/so2_cdev_test.c` to call the ioctl function with the appropriate parameters.
3. To test, we will use an user-space program (`user/so2_cdev_test.c`) which will call the `ioctl` function with the required arguments.
    - **NOTE:** The user-space code is compiled automatically at `make build` and copied at `make copy`.
    - **NOTE:** The qemu machine is 32-bits; thus if the host is 64-bits, you will need to install the `gcc-multilib` package.

**Final Console Output:**
```
root@qemux86:~/skels/device_drivers/kernel# insmod so2_cdev.ko
Successfully registered char device region.
root@qemux86:~/skels/device_drivers/kernel# cd ../user/
root@qemux86:~/skels/device_drivers/user# ./so2_cdev_test p
so2_cdev device file was opened.
Hello ioctl
root@qemux86:~/skels/device_drivers/user#
```

# Extra Exercises: Ioctl with messaging
Add two ioctl operations to modify the message associated with the driver. Use fixed-length buffer ( `BUFFER_SIZE` ).
1. Add the `ioctl` function from the driver the following operations:
    - `MY_IOCTL_SET_BUFFER` for writing a message to the device;
    - `MY_IOCTL_GET_BUFFER` to read a message from your device.
2. For testing, pass the required command line arguments to the user-space program.

**TIP:** If the User is passing a buffer through `ioctl()`, then the `arg` parameter will be a pointer to it. Thus, `arg` will be casted to the appropriate pointer type and accessed with `copy_from_user()` and `copy_to_user()`.

**Final Console Output:**
```
root@qemux86:~/skels/device_drivers/user# ./so2_cdev_test s "New message from us
er-space"
so2_cdev device file was opened.
root@qemux86:~/skels/device_drivers/user# ./so2_cdev_test g
so2_cdev device file was released.
IOCTL buffer contains New message from user-space
so2_cdev device file was opened.
root@qemux86:~/skels/device_drivers/user#
```

# Extra Exercises: Ioctl with waiting queues
Add two ioctl operations to the device driver for queuing.
1. Add the `ioctl` function from the driver the following operations:
    - `MY_IOCTL_DOWN` to add the process to a queue;
    - `MY_IOCTL_UP` to remove the process from a queue.
2. Fill the device structure with a `wait_queue_head_t` field and a flag.
3. Do not forget to initialize the wait queue and flag.
4. Remove exclusive access condition from previous exercise
5. For testing, pass the required command line arguments to the user-space program.

**NOTE:** A *waiting queue* is a list of processes that are waiting for a specific event.
- **Before the event occurs,** processes in the waiting queue are blocked from execution.
- **After the event occurs,** processes in the waiting queue are woken up.

**TIP:** Useful wait queue macros/functions we'll be using in this part of the lab include:
- `init_waitqueue_head()`: Initializes the queue.
- `wait_event()` adds the current thread to the queue.
- `wake_up()` removes threads marked as `TASK_INTERRUPTIBLE` and `TASK_UNINTERRUPTIBLE` from the queue.

**Final Console Output**
Small note on the console output; the lab specification said to use two terminals connected to the QEMU machine to test the new modifications to the kernel module, with one terminal to be use to add a process to the wait queue and another terminal to wake up the threads in the wait queue.

I was having issues getting this setup to work, so instead I added some debug statements into the kernel module that would print out the PID of the process being added to the waitqueue and the PID of the process waking up the threads in the waitqueue.

```
root@qemux86:~/skels/device_drivers/user# ./so2_cdev_test d &
root@qemux86:~/skels/device_drivers/user# so2_cdev device file was opened.
Adding process 232 to wait queue...

root@qemux86:~/skels/device_drivers/user# ./so2_cdev_test u
so2_cdev device file was opened.
Process 233 is waking up threads in wait queue...
so2_cdev device file was released.
[1]+  Done                       ./so2_cdev_test d
root@qemux86:~/skels/device_drivers/user#
```
