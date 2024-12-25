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
