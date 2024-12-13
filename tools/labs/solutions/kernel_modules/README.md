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

# 7. Proc Info
Check the skeleton for the task named 7-list-proc. Add code to display the Process ID (PID) and the executable name for the current process.
- **TIP:** To get the `task_struct` of the current process, use the `current` macro.
- **TIP:** Processes are kept internally as a doubly linked list that starts with the `init` process. To access this list, use the `for_each_process(struct task_struct *)` macro.

Output from loading/unloading the kernel module:
```
Current task name: insmod
Current task PID: 257
-----------------------
Name: init | PID: 1
Name: kthreadd | PID: 2
Name: rcu_gp | PID: 3
Name: rcu_par_gp | PID: 4
Name: kworker/0:0H | PID: 6
Name: mm_percpu_wq | PID: 8
Name: ksoftirqd/0 | PID: 9
Name: rcu_sched | PID: 10
Name: migration/0 | PID: 11
Name: cpuhp/0 | PID: 12
Name: kdevtmpfs | PID: 13
Name: netns | PID: 14
Name: oom_reaper | PID: 15
Name: writeback | PID: 16
Name: kblockd | PID: 43
Name: kworker/0:1 | PID: 44
Name: kworker/0:1H | PID: 45
Name: kswapd0 | PID: 46
Name: cifsiod | PID: 47
Name: smb3decryptd | PID: 48
Name: cifsfileinfoput | PID: 49
Name: cifsoplockd | PID: 50
Name: acpi_thermal_pm | PID: 52
Name: kworker/u2:1 | PID: 53
Name: khvcd | PID: 54
Name: kworker/0:2 | PID: 55
Name: ipv6_addrconf | PID: 56
Name: kmemleak | PID: 57
Name: cifsd | PID: 59
Name: kworker/u2:2 | PID: 65
Name: cifsd | PID: 98
Name: syslogd | PID: 213
Name: klogd | PID: 216
Name: ttyrun | PID: 221
Name: ttyrun | PID: 222
Name: getty | PID: 223
Name: sh | PID: 224
root@qemux86:~/skels/kernel_modules/7-list-proc# rmmod list_proc.ko 
Name: insmod | PID: 257
Exiting...
Current task name: rmmod
Current task PID: 258
root@qemux86:~/skels/kernel_modules/7-list-proc#
```

# Extra Exercises: 1. KDB
Analyze the stacktrace and determine the code that generated the bug.
- The code that generated the bug is in `dummy_func18` of the kernel module.

```
Kernel panic - not syncing: Hello KDB has paniced!
CPU: 0 PID: 224 Comm: sh Tainted: G           O      5.10.14+ #2
Hardware name: QEMU Standard PC (i440FX + PIIX, 1996), BIOS 1.16.2-debian-1.16.2-1 04/01/2014
Call Trace:
 dump_stack+0x6d/0x8b
 panic+0xb2/0x262
 ? dummy_func1+0x8/0x8 [hello_kdb]
 dummy_func18+0xd/0xd [hello_kdb]
 dummy_func17+0x8/0x8 [hello_kdb]
 dummy_func16+0x8/0x8 [hello_kdb]
 dummy_func15+0x8/0x8 [hello_kdb]
 dummy_func14+0x8/0x8 [hello_kdb]
 dummy_func13+0x8/0x8 [hello_kdb]
 dummy_func12+0x8/0x8 [hello_kdb]
 dummy_func11+0x8/0x8 [hello_kdb]
 dummy_func10+0x8/0x8 [hello_kdb]
 dummy_func9+0x8/0x8 [hello_kdb]
 dummy_func8+0x8/0x8 [hello_kdb]
 dummy_func7+0x8/0x8 [hello_kdb]
 dummy_func6+0x8/0x8 [hello_kdb]
 dummy_func5+0x8/0x8 [hello_kdb]
 dummy_func4+0x8/0x8 [hello_kdb]
 dummy_func3+0x8/0x8 [hello_kdb]
 dummy_func2+0x8/0x8 [hello_kdb]
 dummy_func1+0x8/0x8 [hello_kdb]
 bug_write+0x8/0x8 [hello_kdb]
 proc_reg_write+0x76/0xb0
 ? proc_reg_read+0xb0/0xb0
 vfs_write+0xe9/0x450
 ? ksys_write+0x67/0xf0
 ? lock_acquire+0x281/0x400
 ? lock_acquired+0xe9/0x3a0
 ? pick_file+0x1e/0xc0
 ? lock_release+0x171/0x440
 ? __might_sleep+0x32/0x90
 ksys_write+0x67/0xf0
 __ia32_sys_write+0x10/0x20
 __do_fast_syscall_32+0x54/0x80
 do_fast_syscall_32+0x32/0x70
 do_SYSENTER_32+0x15/0x20
 entry_SYSENTER_32+0x9f/0xf2
EIP: 0xb7f9b549
Code: 03 74 c0 01 10 05 03 74 b8 01 10 06 03 74 b4 01 10 07 03 74 b0 01 10 08 03 74 d8 01 00 00 00 00 00 51 52 55 89 e5 0f 34 cd 80 <5d> 5a 59 c3 90 90 90 90 8d 76 00 58 b8 77 00 00 00 cd 80 90 8d 76
EAX: ffffffda EBX: 00000001 ECX: 006341d0 EDX: 00000002
ESI: b7eb2e2c EDI: 006341d0 EBP: 00000001 ESP: bf819b20
DS: 007b ES: 007b FS: 0000 GS: 0033 SS: 007b EFLAGS: 00000246
PANIC: Hello KDB has paniced!
```

How can we find out from KDB the address where the module was loaded?
- To find the address where the module was loaded using KDB, we can use the `lsmod` command.
- In this case, the `8-kdb` kernel module is loaded in `0xe0884000` or `0xe0886000`.

In parallel, use GDB in a new window to view the code based on KDB information.
```
(gdb) x/30i 0xb7f9b549-30
   0xb7f9b52b:	add    0x1(%esp,%esi,4),%esi
   0xb7f9b52f:	adc    %al,(%edi)
   0xb7f9b531:	add    0x1(%eax,%esi,4),%esi
   0xb7f9b535:	adc    %cl,(%eax)
   0xb7f9b537:	add    0x1(%eax,%ebx,8),%esi
   0xb7f9b53b:	add    %al,(%eax)
   0xb7f9b53d:	add    %al,(%eax)
   0xb7f9b53f:	add    %dl,0x52(%ecx)
   0xb7f9b542:	push   %ebp
   0xb7f9b543:	mov    %esp,%ebp
   0xb7f9b545:	sysenter
   0xb7f9b547:	int    $0x80
   0xb7f9b549:	pop    %ebp         # Error seems to occur when popping the item at %EBP, which had a value of 1.
   0xb7f9b54a:	pop    %edx
   0xb7f9b54b:	pop    %ecx
   0xb7f9b54c:	ret
   0xb7f9b54d:	nop
   0xb7f9b54e:	nop
   0xb7f9b54f:	nop
   0xb7f9b550:	nop
   0xb7f9b551:	lea    0x0(%esi),%esi
   0xb7f9b554:	pop    %eax
   0xb7f9b555:	mov    $0x77,%eax
   0xb7f9b55a:	int    $0x80
   0xb7f9b55c:	nop
   0xb7f9b55d:	lea    0x0(%esi),%esi
   0xb7f9b560:	mov    $0xad,%eax
   0xb7f9b565:	int    $0x80
   0xb7f9b567:	nop
   0xb7f9b568:	nop
(gdb)
```

# Extra Exercises: 2. PS Module
Update the created kernel module at proc-info in order to display information about all the processes in the system, when inserting the kernel module, not just about the current process.
- Completed in section "7. Proc Info"

Afterwards, compare the obtained result with the output of the ps command.
- Result from **kernel module:**
```
Name: init | PID: 1
Name: kthreadd | PID: 2
Name: rcu_gp | PID: 3
Name: rcu_par_gp | PID: 4
Name: kworker/0:0H | PID: 6
Name: mm_percpu_wq | PID: 8
Name: ksoftirqd/0 | PID: 9
Name: rcu_sched | PID: 10
Name: migration/0 | PID: 11
Name: cpuhp/0 | PID: 12
Name: kdevtmpfs | PID: 13
Name: netns | PID: 14
Name: oom_reaper | PID: 15
Name: writeback | PID: 16
Name: kblockd | PID: 43
Name: kworker/0:1 | PID: 44
Name: kworker/0:1H | PID: 45
Name: kswapd0 | PID: 46
Name: cifsiod | PID: 47
Name: smb3decryptd | PID: 48
Name: cifsfileinfoput | PID: 49
Name: cifsoplockd | PID: 50
Name: acpi_thermal_pm | PID: 52
Name: kworker/u2:1 | PID: 53
Name: khvcd | PID: 54
Name: kworker/0:2 | PID: 55
Name: ipv6_addrconf | PID: 56
Name: kmemleak | PID: 57
Name: cifsd | PID: 59
Name: kworker/u2:2 | PID: 97
Name: cifsd | PID: 98
Name: syslogd | PID: 213
Name: klogd | PID: 216
Name: ttyrun | PID: 221
Name: ttyrun | PID: 222
Name: getty | PID: 223
Name: sh | PID: 224
```
- Result from **ps command:**
```
  PID USER       VSZ STAT COMMAND
    1 root      2828 S    init [5]
    2 root         0 SW   [kthreadd]
    3 root         0 IW<  [rcu_gp]
    4 root         0 IW<  [rcu_par_gp]
    6 root         0 IW<  [kworker/0:0H-ev]
    8 root         0 IW<  [mm_percpu_wq]
    9 root         0 SW   [ksoftirqd/0]
   10 root         0 IW   [rcu_sched]
   11 root         0 SW   [migration/0]
   12 root         0 SW   [cpuhp/0]
   13 root         0 SW   [kdevtmpfs]
   14 root         0 IW<  [netns]
   15 root         0 SW   [oom_reaper]
   16 root         0 IW<  [writeback]
   43 root         0 IW<  [kblockd]
   44 root         0 IW   [kworker/0:1-cif]
   45 root         0 IW<  [kworker/0:1H-kb]
   46 root         0 SW   [kswapd0]
   47 root         0 IW<  [cifsiod]
   48 root         0 IW<  [smb3decryptd]
   49 root         0 IW<  [cifsfileinfoput]
   50 root         0 IW<  [cifsoplockd]
   52 root         0 IW<  [acpi_thermal_pm]
   53 root         0 IW   [kworker/u2:1-ev]
   54 root         0 SW   [khvcd]
   55 root         0 IW   [kworker/0:2-cif]
   56 root         0 IW<  [ipv6_addrconf]
   57 root         0 SWN  [kmemleak]
   59 root         0 SW   [cifsd]
   97 root         0 IW   [kworker/u2:2-ev]
   98 root         0 SW   [cifsd]
  213 root      4224 S    /sbin/syslogd -n -O /var/log/messages
  216 root      4228 S    /sbin/klogd -n
  221 root      2656 S    /usr/sbin/ttyrun ttyS0 /bin/start_getty 115200 ttyS0
  222 root      2656 S    /usr/sbin/ttyrun ttyS1 /bin/start_getty 115200 ttyS1
  223 root      4228 S    /sbin/getty 38400 tty1
  224 root      4268 S    -sh
  238 root      4272 R    ps
```
- Both list the same processes with their respective PID.

# Extra Exercises: 3. Memory Info
Create a kernel module that displays the virtual memory areas (VMAs) of the current process; for each memory area it will display the start address and the end address.
- First, we will use one of the other kernel modules as a template for our new `ps-vma-mod` kernel module.
- Second, we will use the `current` macro to retrieve the `task_struct` of the current process.
- Third, we will iterate through the VMAs stored by the `mm` field in the task struct. This will be done by iterating through the linked list in `mm->mmap`.

Result:
```
Current task name: insmod
Current task PID: 233
-----------------------
    VMA #    START_ADDR    END_ADDR
    0    44b000    44d000
    1    44d000    468000
    2    468000    477000
    3    477000    478000
    4    478000    479000
    5    1881000    18a3000
    6    b781e000    b7820000
    7    b7820000    b7821000
    8    b7821000    b7822000
    9    b7822000    b7823000
    a    b7823000    b7824000
    b    b7824000    b7825000
    c    b7825000    b7844000
    d    b7844000    b79c5000
    e    b79c5000    b7a4c000
    f    b7a4c000    b7a4e000                                                   
    10    b7a4e000    b7a4f000                                                  
    11    b7a4f000    b7a59000                                                  
    12    b7a59000    b7abc000                                                  
    13    b7abc000    b7d25000                                                  
    14    b7d25000    b7e98000                                                  
    15    b7e98000    b7ecd000                                                  
    16    b7ecd000    b7ecf000                                                  
    17    b7ecf000    b7ed1000                                                  
    18    b7ed1000    b7ed3000                                                  
    19    b7ed3000    b7ee2000                                                  
    1a    b7ee2000    b7ee9000                                                  
    1b    b7ee9000    b7eea000                                                  
    1c    b7eea000    b7eeb000                                                  
    1d    b7eeb000    b7eee000                                                  
    1e    b7eee000    b7f10000                                                  
    1f    b7f10000    b7f20000                                                  
    20    b7f20000    b7f21000                                                  
    21    b7f21000    b7f22000                                                  
    22    b7f23000    b7f25000                                                  
    23    b7f25000    b7f29000                                                  
    24    b7f29000    b7f2b000                                                  
    25    b7f2b000    b7f2c000                                                  
    26    b7f2c000    b7f50000                                                  
    27    b7f50000    b7f5e000                                                  
    28    b7f5e000    b7f60000                                                  
    29    b7f60000    b7f61000
```
