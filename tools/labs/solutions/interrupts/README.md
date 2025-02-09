# 0. Intro
Using LXR, find the definitions of the following symbols in the Linux kernel:
- `struct resource`
    - Defined in `linux/ioport.h`
- `request_region()` and `__request_region()`
    - `request_region()` is a macro defined in `linux/ioport.h`
    - `__request_region()` is a function defined in `kernel/resource.c`; used to create a new busy resource region.
- `request_irq()` and `request_threaded_irq()`
    - `request_irq()` is defined in `linux/interrupt.h`; used to add a handler for an interrupt line.
    - `request_threaded_irq()` is defined in `kernel/irq/manage.c`; used for allocating an interrupt line.
        - **NOTE:** According to the documentation, from the point a call to this function is made, the handler function may be invoked. Because the handler function *must clear any interrupt the board raises,* you *must* take care both to initialize your hardware and to set up the interrupt handler in the right order.
        - **NOTE:** `dev_id` must be globally unique; thus, use the address of the device data structure. Since the handler receives this value it makes sense to use it as well.
- `inb()` for the x86 architecture.
    - Defined in `arch/x86/boot/boot.h`.
    - Basic port I/O.

Analyze the following Linux code:
- Keyboard initialization function `i8042_setup_kbd()`
    - Defined in `drivers/input/serio/i8042.c`
    - **Step 1:** Create a `struct i8042_port` object representing the port the keyboard is connected to.
    - **Step 2:** Register the interrupt handler function (i.e., `i8042_interrupt`) for interrupt line `I8042_KBD_IRQ` (i.e., line `1`) using `request_irq()`.
    - **Step 3:** Enable the keyboard port.
    - **Step 4:** Mark the global flag `i8042_kbd_irq_registered` to `true`.
    - **Error Handling:**
        - If an error occurs with the `request_irq()` call, then release the port the keyboard is connected to and return an error.
        - If an error occurs with the `i8042_enable_kbd_port()` call, then remove the interrupt handler from the keyboard interrupt line first, and then free the port the keyboard is connected to and return an error.
- The AT or PS/2 keyboard interrupt function `atkbd_interrupt()`
    - Defined in `drivers/input/keyboard/atkbd.c`
    - Interrupt handler that processes data from the keyboard into events.

# 1. Request the I/O Ports
The `kbd.c` file contains a skeleton for the keyboard driver. Browse the source code and inspect `kbd_init()`. Notice that the I/O ports we need are `I8042_STATUS_REG` and `I8042_DATA_REG`.
- Keyboard driver is for a character device with:
    - Major: 42
    - Beginning Minor: 0
    - Number of Minors: 1
- Keyboard hardware information:
    - Interrupt Line: 1
    - Status Register Address: 0x64
    - Data Register Address: 0x60
- A [**scancode**](https://en.wikipedia.org/wiki/Scancode) is the data that most computer keyboards send to a computer to report which keys have been pressed. A number, or sequence of numbers, is assigned to each key on the keyboard.
    - `is_key_press()` checks if the scancode corresponds to a key press or release.
    - `get_ascii()` returns the character of the given scancode (only alphanumeric/space/enter).
- Functions within the driver:
    - `put_char()`: Add a character to buffer in `struct kbd`.
    - `get_char()`: Get a character from the buffer in `struct kbd`.
    - `reset_buffer()`: Resets the buffer in `struct kbd`.
    - `i8042_read_data()`: Return the value of the DATA register on the keyboard.
    - `kbd_open()`: Called when a process opens the keyboard's device file in the file tree.
    - `kbd_release()`: Called when a process releases the keyboard device file resource.
    - `kbd_read():` Returns data stored in the keyboard's buffer.
    - `kbd_init():` Initializes the keyboard driver.
    - `kbd_exit():` Cleans up the keyboard driver upon unloading.

Follow the sections maked with **TODO 1** in the skeleton. Request the I/O ports in `kbd_init()` and make sure to check for errors and to properly clean-up in case of errors. When requesting, set the reserving caller's ID string (`name`) with `MODULE_NAME` macro. Also, add code to release the I/O ports in `kbd_exit()`.
- **TIP:** To request the I/O ports, use `request_region()` where:
    - `first`: Starting port address of the I/O region.
    - `n`: I/O region size in bytes.
    - `name`: Reserving caller's ID string.
- **NOTE:** In the actual i8042 driver code, the `n` parameter for `request_region()` was set to 16.
- **TIP:** To handle errors encountered with `request_region()`, do the following:
    ``` C
    if (!request_region(...)) {
        // Handle error ...
        return -ENODEV;
    }
    ```
- **TIP:** To release the I/O ports, use `release_region()` where:
    - `start`: Starting port address of the I/O region.
    - `n`: I/O region size in bytes.

**Final Console Output:**
```
root@qemux86:~# insmod skels/interrupts/kbd.ko 
Driver kbd loaded
root@qemux86:~# cat /proc/ioports | grep kbd
  0061-0061 : kbd
  0065-0065 : kbd
root@qemux86:~# rmmod skels/interrupts/kbd.ko 
Driver kbd unloaded
root@qemux86:~# cat /proc/ioports | grep kbd
root@qemux86:~#
```
