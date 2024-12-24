# 0. Intro
Using [LXR](http://elixir.free-electrons.com/linux/latest/source) find the definitions of the following symbols in the Linux kernel:
- `struct list_head`
    - Defined in `include/linux/types.h`
    - Represents a node in a linked list.
- `INIT_LIST_HEAD()`
    - Defined in `include/linux/types.h`
    - Inline function that initializes a `list_head` structure. Initializes the `list_head` (passed as `list`) to point to itself. If `list` is a list header, the result is an empty list.
- `list_add()`
    - Defined in `include/linux/types.h`
    - Inline function that adds a new entry (`new`) to the specified linked list (`head`).
- `list_for_each`
    - Defined in `include/linux/list.h`
    - Macro definition: `list_for_each(pos, head)`
    - Macro that iterates over the linked list specified by `head` using `pos` as a loop cursor.
- `list_entry`
    - Defined in `include/linux/list.h`
    - Macro definition: `list_entry(ptr, type, member)`
    - Macro that returns the struct (of `type`) that `list_head *ptr` represents.
- `container_of`
    - Defined in `include/linux/kernel.h`
    - Macro definition: `container_of(ptr, type, member)`
    - Macro that cast a member of a structure out to the containing structure, where `ptr` is a pointer to the member, `type` is the type of container struct this is embedded in, and `member` being the name of the member within the struct.
- `offsetof`
    - Defined in `include/linux/stddef.h`
    - Macro definition: `offsetof(TYPE, MEMBER)`
    - Macro that returns the memory offset of `MEMBER` within `struct TYPE`.

# 1. Memory allocation in the Linux kernel
Generate the skeleton for the task named **1-mem** and browse the contents of the `mem.c` file. Observe the use of `kmalloc()` call for memory allocation.
- `kmalloc` is allocating (4096 * sizeof(*mem)) bytes of memory for `mem`.
- The allocated memory is then freed when the kernel module is unloaded (through `mem_exit`).

Compile the source code and load the `mem.ko` module using **insmod**.
- Done

View the kernel messages using the **dmesg** command.
- Result: Printed a lot of "z" characters.

Unload the kernel module using the **rmmod mem** command.
- Done

# 2. Sleeping in atomic context
Generate the skeleton for the task named **2-sched-spin** and browse the contents of the `sched-spin.c` file.
- In `sched_spin_init` of the generated skeleton:
    1. The spin lock, `lock`, is initialized using `spin_lock_init()`.
    2. The task is set to be interruptable.
    3. The task is then put to sleep for 5 seconds.

Compile the source code and load the module, according the above info: (**make build** and **make copy**)
- Done

Notice that it is waiting for 5 seconds until the insertion order is complete.
- Done

Unload the kernel module.
- Done

Look for the lines marked with: `TODO 0` to create an atomic section. Re-compile the source code and reload the module into the kernel.
- Done

You should now get an error. Look at the stack trace. What is the cause of the error?
- **HINT:** In the error message, follow the line containing the BUG for a description of the error. You are not allowed to sleep in atomic context. The atomic context is given by a section between a lock operation and an unlock on a spinlock.
- The error is caused by a scheduling operation occurring in the atomic region marked by the `spin_lock` and `spin_unlock` calls. Specifically, that scheduling operation occurs when `schedule_timeout` is called.
```
BUG: scheduling while atomic: insmod/240/0x00000002
1 lock held by insmod/240:
 #0: c2999d6c (&lock){+.+.}-{2:2}, at: sched_spin_init+0x32/0x90 [sched_spin]
Modules linked in: sched_spin(O+) [last unloaded: sched_spin]
CPU: 0 PID: 240 Comm: insmod Tainted: G           O      5.10.14+ #2
Hardware name: QEMU Standard PC (i440FX + PIIX, 1996), BIOS 1.16.2-debian-1.16.2-1 04/01/2014
Call Trace:
 dump_stack+0x6d/0x8b
 __schedule_bug.cold+0x6e/0x81
 __schedule+0x5f7/0x760
 ? __mod_timer+0x1ed/0x320
 schedule+0x56/0xd0
 ? 0xe0845000
 schedule_timeout+0xaa/0x1c0
 ? trace_raw_output_hrtimer_start+0xa0/0xa0
 sched_spin_init+0x61/0x90 [sched_spin]
 ? sched_spin_init+0x32/0x90 [sched_spin]
 do_one_initcall+0x57/0x2d0
 ? do_init_module+0x1f/0x230
 ? rcu_read_lock_sched_held+0x41/0x70
 ? kmem_cache_alloc_trace+0x2cb/0x340
 do_init_module+0x4e/0x230
 load_module+0x2370/0x2820
 ? kernel_read+0x39/0x50
 ? kernel_read_file_from_fd+0x4c/0x90
 __ia32_sys_finit_module+0x9a/0xd0
 __do_fast_syscall_32+0x54/0x80
 do_fast_syscall_32+0x32/0x70
 do_SYSENTER_32+0x15/0x20
 entry_SYSENTER_32+0x9f/0xf2
EIP: 0xb7f6f549
Code: 03 74 c0 01 10 05 03 74 b8 01 10 06 03 74 b4 01 10 07 03 74 b0 01 10 08 03 74 d8 01 00 00 00 00 00 51 52 55 89 e5 0f 34 cd 80 <5d> 5a 59 c3 90 90 90 90 8d 76 00 58 b8 77 00 00 00 cd 80 90 8d 76
EAX: ffffffda EBX: 00000003 ECX: 0048b9d1 EDX: 00000000
ESI: 02145fc0 EDI: 02145ee0 EBP: 0048e8f4 ESP: bfcafddc
DS: 007b ES: 007b FS: 0000 GS: 0033 SS: 007b EFLAGS: 00200296

=============================
WARNING: suspicious RCU usage
5.10.14+ #2 Tainted: G        W  O     
-----------------------------
include/trace/events/initcall.h:48 suspicious rcu_dereference_check() usage!

other info that might help us debug this:


rcu_scheduler_active = 2, debug_locks = 1
no locks held by insmod/240.

stack backtrace:
CPU: 0 PID: 240 Comm: insmod Tainted: G        W  O      5.10.14+ #2
Hardware name: QEMU Standard PC (i440FX + PIIX, 1996), BIOS 1.16.2-debian-1.16.2-1 04/01/2014
Call Trace:
 dump_stack+0x6d/0x8b
 lockdep_rcu_suspicious+0xbb/0xc4
 ? 0xe0845000
 do_one_initcall+0x265/0x2d0
 ? do_init_module+0x1f/0x230
 ? rcu_read_lock_sched_held+0x41/0x70
 ? kmem_cache_alloc_trace+0x2cb/0x340
 do_init_module+0x4e/0x230
 load_module+0x2370/0x2820
 ? kernel_read+0x39/0x50
 ? kernel_read_file_from_fd+0x4c/0x90
 __ia32_sys_finit_module+0x9a/0xd0
 __do_fast_syscall_32+0x54/0x80
 do_fast_syscall_32+0x32/0x70
 do_SYSENTER_32+0x15/0x20
 entry_SYSENTER_32+0x9f/0xf2
EIP: 0xb7f6f549
Code: 03 74 c0 01 10 05 03 74 b8 01 10 06 03 74 b4 01 10 07 03 74 b0 01 10 08 03 74 d8 01 00 00 00 00 00 51 52 55 89 e5 0f 34 cd 80 <5d> 5a 59 c3 90 90 90 90 8d 76 00 58 b8 77 00 00 00 cd 80 90 8d 76
EAX: ffffffda EBX: 00000003 ECX: 0048b9d1 EDX: 00000000
ESI: 02145fc0 EDI: 02145ee0 EBP: 0048e8f4 ESP: bfcafddc
DS: 007b ES: 007b FS: 0000 GS: 0033 SS: 007b EFLAGS: 00200296
------------[ cut here ]------------
initcall sched_spin_init+0x0/0x90 [sched_spin] returned with preemption imbalance 
WARNING: CPU: 0 PID: 240 at init/main.c:1230 do_one_initcall+0x207/0x2d0
Modules linked in: sched_spin(O+) [last unloaded: sched_spin]
CPU: 0 PID: 240 Comm: insmod Tainted: G        W  O      5.10.14+ #2
Hardware name: QEMU Standard PC (i440FX + PIIX, 1996), BIOS 1.16.2-debian-1.16.2-1 04/01/2014
EIP: do_one_initcall+0x207/0x2d0
Code: 7c 00 64 ff c1 e9 7b fe c1 e9 7b fe 26 00 90 89 26 00 90 89 45 b0 89 44 45 b0 89 44 24 88 1a 96 24 88 1a 96 78 00 0f 0b 78 00 <0f> 0b ff 66 90 64 ff 66 90 64 c1 0f a3 05 c1 0f a3 05 0f 83 2f fe
EAX: 00000052 EBX: 00000000 ECX: ffffffff EDX: 00000000
ESI: e0845000 EDI: 00000000 EBP: c2999df0 ESP: c2999d94
DS: 007b ES: 007b FS: 00d8 GS: 00e0 SS: 0068 EFLAGS: 00200282
CR0: 80050033 CR2: 00482010 CR3: 04f47000 CR4: 00000690
Call Trace:
 ? 0xe0845000
 ? rcu_read_lock_sched_held+0x41/0x70
 ? kmem_cache_alloc_trace+0x2cb/0x340
 do_init_module+0x4e/0x230
 load_module+0x2370/0x2820
 ? kernel_read+0x39/0x50
 ? kernel_read_file_from_fd+0x4c/0x90
 __ia32_sys_finit_module+0x9a/0xd0
 __do_fast_syscall_32+0x54/0x80
 do_fast_syscall_32+0x32/0x70
 do_SYSENTER_32+0x15/0x20
 entry_SYSENTER_32+0x9f/0xf2
EIP: 0xb7f6f549
Code: 03 74 c0 01 10 05 03 74 b8 01 10 06 03 74 b4 01 10 07 03 74 b0 01 10 08 03 74 d8 01 00 00 00 00 00 51 52 55 89 e5 0f 34 cd 80 <5d> 5a 59 c3 90 90 90 90 8d 76 00 58 b8 77 00 00 00 cd 80 90 8d 76
EAX: ffffffda EBX: 00000003 ECX: 0048b9d1 EDX: 00000000
ESI: 02145fc0 EDI: 02145ee0 EBP: 0048e8f4 ESP: bfcafddc
DS: 007b ES: 007b FS: 0000 GS: 0033 SS: 007b EFLAGS: 00200296
irq event stamp: 21881
hardirqs last  enabled at (21901): [<c10bd65c>] console_unlock+0x57c/0x5b0
hardirqs last disabled at (21920): [<c10bd46d>] console_unlock+0x38d/0x5b0
softirqs last  enabled at (21938): [<c10238a7>] call_on_stack+0x47/0x60
softirqs last disabled at (21949): [<c10238a7>] call_on_stack+0x47/0x60
---[ end trace 2f4995680d0d1daa ]---
```

# 3. Working with kernel memory.
Generate the skeleton for the task named **3-memory** directory and browse the contents of the `memory.c` file. Notice the comments marked with `TODO`. You must allocate 4 structures of type `struct task_info` and initialize them (in `memory_init()`), then print and free them (in `memory_exit()`).

(TODO 1) Allocate memory for `struct task_info` structure and initialize its fields:
- The `pid` field to the PID transmitted as a parameter.
- The `timestamp` field to the value of the `jiffies` variable, which holds the number of ticks that have occurred since the system booted.
- **NOTE:** To allocate memory for `struct task_info`, we will use `kmalloc` to allocate `sizeof(struct task_info)` bytes of memory. Check that `kmalloc` did not return a null pointer before initializing the struct's fields.
- **NOTE:** The `jiffies` variable is declared in `include/linux/jiffies.h` as an `extern` variable.

(TODO 2) Allocate `struct task_info` for the current process, the parent process, the next process, the next process of the next process, with the following information:
- PID of the current process, which can be retrieved from `struct task_struct` structure, returned by `current` macro.
    - **NOTE:** To get the PID of the current process, access the `pid_t pid` field in `struct task_struct`.
- PID of the parent process of the current process.
    - **NOTE:** To get the `struct task_struct` of the parent process, access the `struct task_struct *parent` field.
- PID of the next process from the list of processes, relative to the current process.
    - **NOTE:** To get the `struct task_struct` of the next process in the list, use the `next_task` macro.
- PID of the next process of the next process, relative to the current process.
    - **NOTE:** To get the "next process of the next process", just use the `next_task` macro two times.

(TODO 3) Display the four structures.
- Use `printk()` to display their two fields: `pid` and `timestamp`.

(TODO 4) Release the memory occupied by the structures (use `kfree()`).
```
root@qemux86:~/skels/kernel_api/3-memory# insmod memory.ko
root@qemux86:~/skels/kernel_api/3-memory# rmmod memory.ko
PID: 2, Timestamp: 4294926017
PID: 231, Timestamp: 4294961381
PID: 224, Timestamp: 4294961381
PID: 0, Timestamp: 4294961381
root@qemux86:~/skels/kernel_api/3-memory#
```

# 4. Working with kernel lists
Generate the skeleton for the task named **4-list**. Browse the contents of the `list.c` file and notice the comments marked with `TODO`. The current process will add the four structures from the previous exercise into a list. The list will be built in the `task_info_add_for_current()` function which is called when module is loaded. The list will be printed and deleted in the `list_exit()` function and the `task_info_purge_list()` function.

(TODO 1) Complete the `task_info_add_to_list()` function to allocate a `struct task_info` structure and add it to the list.
- **TIP:** The variable `head` represents the linked list of `struct task_info` objects.
- **TIP:** Use `task_info_alloc()` to allocate `struct task_info` structures.
- **TIP:** Use `list_add()` to add the allocated `struct task_info` structure into the list.

(TODO 2) Complete the `task_info_purge_list()` function to delete all the elements in the list.
- **TIP:** When deleting all the elements in the list, we need to remove:
    1. The `struct list_head` entry in the list.
    2. The `struct task_info` object associated with the `struct list_head` entry.
- **TIP:** To iterate through the list while removing elements, we need to use `list_for_each_safe()`.
- **TIP:** Use `list_entry()` to retrieve the `struct task_info` object associated with a `struct list_head` pointer.
- **TIP:** Use `list_del()` to delete a `struct list_head` pointer.

Compile the kernel module. Load and unload the module by following the messages displayed by the kernel.
```
root@qemux86:~/skels/kernel_api/4-list# insmod list.ko
list: loading out-of-tree module taints kernel.
root@qemux86:~/skels/kernel_api/4-list# rmmod list.ko
before exiting: [ 
(1, 4294923540) 
(0, 4294923540) 
(224, 4294923540) 
(228, 4294923540) 
]
root@qemux86:~/skels/kernel_api/4-list#
```

# 5. Working with kernel lists for process handling
(TODO 1) Implement the `task_info_find_pid()` function.
- **TIP:** Use `list_for_each()` to iterate through the linked list.
- **TIP:** Use `list_entry()` to get the `struct task_info` object associated with the list entry.
- Essentially, loop through each element in the linked list until you either find the `struct task_info` object with the given PID or not.

(TODO 2) Change a field of an item in the list so it does not expire. It must not satisfy a part of the expiration condition from `task_info_remove_expired()`.
- **TIP:** A process is considered "expired" if it was added more than 3 seconds ago and its `count` is less than or equal to 5. A simple way of keeping one task as unexpired would be to set the `count` of one process in the list to be a value greater than or equal to 5.
- **TIP:** The `count` member is of type `atomic_t`; thus, we are required to use `atomic_set()` in cases where we want to give it a value.

Compile, copy, load and unload the kernel module following the displayed messages. Kernel module loading will take some time, because `sleep()` is being called by `schedule_timeout()` function.
```
root@qemux86:~/skels/kernel_api/5-list-full# insmod list-full.ko 
list_full: loading out-of-tree module taints kernel.
after first add: [ 
(1, 187770) 
(0, 187770) 
(224, 187770) 
(229, 187770) 
]
root@qemux86:~/skels/kernel_api/5-list-full# rmmod list-full.ko 
after removing expired: [ 
(1, 187770) 
]
root@qemux86:~/skels/kernel_api/5-list-full#
```

# 6. Synchronizing list work
Generate the skeleton for the task named `6-list-sync`.
1. Browse the code and look for `TODO 1` string.
    - 
2. Use a spinlock or a read-write lock to synchronize access to the list.
    - 
3. Compile, load and unload the kernel module.
    - 
