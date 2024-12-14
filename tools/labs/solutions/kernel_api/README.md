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
