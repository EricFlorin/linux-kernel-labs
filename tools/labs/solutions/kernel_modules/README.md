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

# 3. Error
Generate the skeleton for the task named `3-error-mod`. Compile the sources and get the corresponding kernel module. Why have compilation errors occurred? **Hint:** How does this module differ from the previous module?
* When attempting to build `3-error-mod`, we would get the following compilation errors:
```
/home/kernellabs/Desktop/linux-kernel-labs/tools/labs/skels/./kernel_modules/3-error-mod/err_mod.c:5:20: error: expected declaration specifiers or ‘...’ before string constant
    5 | MODULE_DESCRIPTION("Error module");
      |                    ^~~~~~~~~~~~~~
/home/kernellabs/Desktop/linux-kernel-labs/tools/labs/skels/./kernel_modules/3-error-mod/err_mod.c:6:15: error: expected declaration specifiers or ‘...’ before string constant
    6 | MODULE_AUTHOR("Kernel Hacker");
      |               ^~~~~~~~~~~~~~~
/home/kernellabs/Desktop/linux-kernel-labs/tools/labs/skels/./kernel_modules/3-error-mod/err_mod.c:7:16: error: expected declaration specifiers or ‘...’ before string constant
    7 | MODULE_LICENSE("GPL");
      |                ^~~~~
/home/kernellabs/Desktop/linux-kernel-labs/tools/labs/skels/./kernel_modules/3-error-mod/err_mod.c:24:1: warning: data definition has no type or storage class
   24 | module_init(err_init);
      | ^~~~~~~~~~~
/home/kernellabs/Desktop/linux-kernel-labs/tools/labs/skels/./kernel_modules/3-error-mod/err_mod.c:24:1: error: type defaults to ‘int’ in declaration of ‘module_init’ [-Werror=implicit-int]
/home/kernellabs/Desktop/linux-kernel-labs/tools/labs/skels/./kernel_modules/3-error-mod/err_mod.c:24:1: warning: parameter names (without types) in function declaration
/home/kernellabs/Desktop/linux-kernel-labs/tools/labs/skels/./kernel_modules/3-error-mod/err_mod.c:25:1: warning: data definition has no type or storage class
   25 | module_exit(err_exit);
      | ^~~~~~~~~~~
/home/kernellabs/Desktop/linux-kernel-labs/tools/labs/skels/./kernel_modules/3-error-mod/err_mod.c:25:1: error: type defaults to ‘int’ in declaration of ‘module_exit’ [-Werror=implicit-int]
/home/kernellabs/Desktop/linux-kernel-labs/tools/labs/skels/./kernel_modules/3-error-mod/err_mod.c:25:1: warning: parameter names (without types) in function declaration
```
* This is because we're missing the `<linux/module.h>` header, which defines the macros:
    * MODULE_DESCRIPTION
    * MODULE_AUTHOR
    * MODULE_LICENSE
    * module_init
    * module_exit
* To resolve the compilation errors, we just need to add the `<linux/module.h>` header to the source file.

# 4. Sub-modules
Modified the `KBuild` file by adding the following lines:
```
obj-m = multimod.o
multimod-y = mod1.o mod2.o
```

# 5. Kernel Oops
Enter the directory for the task 5-oops-mod and inspect the C source file. Notice where the problem will occur. Add the compilation flag -g in the Kbuild file.
- Kernel oops will occur on Line 15 of `oops_mod.c` due to dereferencing a null pointer.
- Added the following line to enable debugging: `EXTRA_CFLAGS = -g`

Compile the corresponding module and load it into the kernel. Identify the memory address at which the oops appeared.
- **TIP:** Memory address at which the oops appeared is stored in the EIP register.
- The memory address at which the oops appeared is `EIP: 0xb7f90549`

Determine which instruction has triggered the oops.
- The instruction that triggered the oops was `movb   $0x61,0x0`

# 6. Module Parameters
Without modifying the sources, load the kernel module so that the message shown is `Early bird gets tired`.
- Loaded the kernel module with the following parameters: `insmod cmd_mod.ko str="tired"`
