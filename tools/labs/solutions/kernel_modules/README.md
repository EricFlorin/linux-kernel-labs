# 0. Intro
Using cscope or LXR find the definitions of the following symbols in the Linux kernel source code:

* `module_init()` and `module_exit()`
    * What do the two macros do? What is `init_module` and `cleanup_module`?
        * `module_init()` defines the function to run at kernel boot time or module insertion for a kernel driver.
        * `module_exit()` is the opposite of `module_init()`; defines the function to run when the kernel driver is removed.
        * `init_module` is a system call responsible for loading a given kernel driver into the kernel.
        * `cleanup_module` is a function responsible for unloading a given kernel driver from the kernel.
* `ignore_loglevel`
    * What is this variable used for?
        * The `ignore_loglevel` variable is a boolean that, when set to `true`, prints all kernel messages to the console regardless of log level.

# 1. Kernel Module
Start the VM using `make console`, and perform the following tasks:
* Load the kernel module.
    * Loaded the kernel module using the command: `insmod hello_mod.ko`
* List the kernel modules and check if current module is present.
    * Listed kernel modules using the command: `cat /proc/modules`
    * Result of command:
    ```
    root@qemux86:~/skels/kernel_modules/1-2-test-mod# cat /proc/modules             
    hello_mod 16384 0 - Live 0xe086f000 (O)
    ```
* Unload the kernel module.
    * Unloaded the kernel module using the command: `rmmod hello_mod.ko`
* View the messages displayed at loading/unloading the kernel module using `dmesg` command.
    * Result:
    ```
    CIFS: Attempting to mount //10.0.2.1/skels                                      
    CIFS: No dialect specified on mount. Default has changed to a more secure diale.
    udevd[116]: starting version 3.2.14                                             
    random: udevd: uninitialized urandom read (16 bytes read)                       
    random: udevd: uninitialized urandom read (16 bytes read)                       
    random: udevd: uninitialized urandom read (16 bytes read)                       
    udevd[117]: starting eudev-3.2.14                                               
    udevd[117]: inotify_init failed: Function not implemented                       
    udevd[117]: error initializing inotify                                          
    urandom_read: 1 callbacks suppressed                                            
    random: dd: uninitialized urandom read (512 bytes read)                         
    traps: hostname[192] trap invalid opcode ip:4ab277 sp:bfba31c0 error:0 in busyb]
    random: crng init done                                                          
    hello_mod: loading out-of-tree module taints kernel.                            
    Hello!                                                                          
    Goodbye!
    ```

# 2. Printk
Watch the virtual machine console. Why were the messages displayed directly to the virtual machine console?
* There were messages displayed directly to the virtual machine console because the `console_loglevel` variable is set to a value of `15`, allowing every kernel message to be printed to the console.
    * See https://docs.kernel.org/core-api/printk-basics.html on how to check the value of `console_loglevel`.

Configure the system such that the messages are not displayed directly on the serial console, and they can only be inspected using `dmesg`.
* Set the `console_loglevel` to a value of `4` using the command: `echo 4 > /proc/sys/kernel/printk`.
