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
