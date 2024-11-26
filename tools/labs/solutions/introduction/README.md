# Solutions to ["Introduction"](https://linux-kernel-labs.github.io/refs/heads/master/labs/introduction.html#lab-objectives)

## 1. Booting the Virtual Machine
Followed the [instructions provided](https://linux-kernel-labs.github.io/refs/heads/master/labs/introduction.html#booting-the-virtual-machine) to bootup the QEMU virtual machine for the first time.

## 2. Adding and Using a Virtual Disk
Followed the [instructions provided](https://linux-kernel-labs.github.io/refs/heads/master/labs/introduction.html#adding-and-using-a-virtual-disk) to add BTRFS support to the kernel.

## 3. GDB and QEMU
Followed the [instructions provided](https://linux-kernel-labs.github.io/refs/heads/master/labs/introduction.html#gdb-and-qemu) to attach GDB to QEMU.

## 4. GDB Spelunking
Used gdb to display the source code of the function that creates kernel threads (`kernel_threads`).
```
(gdb) list kernel_thread
2488                    if (!wait_for_vfork_done(p, &vfork))
2489                            ptrace_event_pid(PTRACE_EVENT_VFORK_DONE, pid);
2490            }
2491
2492            put_pid(pid);
2493            return nr;
2494    }
2495
2496    /*
2497     * Create a kernel thread.
2498     */
2499    pid_t kernel_thread(int (*fn)(void *), void *arg, unsigned long flags)
2500    {
2501            struct kernel_clone_args args = {
2502                    .flags          = ((lower_32_bits(flags) | CLONE_VM |
2503                                        CLONE_UNTRACED) & ~CSIGNAL),
2504                    .exit_signal    = (lower_32_bits(flags) & CSIGNAL),
2505                    .stack          = (unsigned long)fn,
2506                    .stack_size     = (unsigned long)arg,
2507            };
2508
2509            return kernel_clone(&args);
2510    }
2511
2512    #ifdef __ARCH_WANT_SYS_FORK
(gdb) 
```

Used gdb to display the memory address and contents of the `jiffies` variable.
```
(gdb) x &jiffies
0xc1a2f500 <jiffies_64>:        0x0000bb12
(gdb) 
```

## 5. Cscope Spelunking
Find the file in which the following data types are defined:
- `struct task_struct` -> Defined in `include/linux/sched.h`.
- `struct semaphore` -> Defined in `include/linux/semaphore.h`.
- `struct list_head` -> Defined in `include/linux/types.h`.
- `spinlock_t` -> Defined in `include/linux/spinlock_types.h`.
- `struct file_system_type`-> Defined in `include/linux/fs.h`.

&nbsp;  

Find the file in which the following global kernel variables are declared:
- `sys_call_table` -> Declared in `arch/x86/entry/syscall_64.c`.
- `file_systems` -> Declared in `fs/filesystems.c`.
- `current` -> Declared in `arch/x86/include/asm/current.h`.
- `chrdevs` -> Declared in `fs/char_dev.c`.

&nbsp;  


Find the file in which the following functions are declared:
- `copy_from_user` -> Declared in `include/linux/uaccess.h`.
- `vmalloc` -> Declared in `mm/vmalloc.c`.
- `schedule_timeout` -> Declared in `kernel/time/timer.c`.
- `add_timer` -> Declared in `kernel/time/timer.c`.

&nbsp;  


Scroll through the following sequence of structures:
- `struct task_struct`
- `struct mm_struct`
- `struct vm_area_struct`
- `struct vm_operations_struct`

That is, you access a structure and then you find fields with the data type of the next structure, access the respective fields and so on. Note in which files these structures are defined; this will be useful to the following labs.
- `struct task_struct` -> Defined in `include/linux/sched.h`.
- `struct mm_struct` -> Defined in `include/linux/mm_types.h`.
- `struct vm_area_struct` -> Defined in `include/linux/mm_types.h`.
- `struct vm_operations_struct` -> Defined in `include/linux/mm.h`.

&nbsp;  


Following the above instructions, find and go through the function call sequence:
- `bio_alloc` -> Defined in `include/linux/bio.h` as an inline function.
- `bio_alloc_bioset` -> Defined in `block/bio.c` as a function.
- `bvec_alloc` -> Defined in `block/bio.c` as a function.
- `kmem_cache_alloc` -> Defined in either `mm/slab.c`, `mm/slob.c`, or `mm/slub.c`; depends on the compilation options.
- `slab_alloc` -> Defined in either `mm/slab.c` or `mm/slub.c`; depends on the compilation options.
