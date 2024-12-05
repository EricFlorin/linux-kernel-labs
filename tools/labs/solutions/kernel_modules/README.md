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
