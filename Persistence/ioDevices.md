# I/O Devices

How should I/O be integrated into systems? What are the general mechanisms? How can we make them efficient?

## A Canonical Device

- All devices have some specified interface and protocol for typical interaction.
- Internal structure - implementation of the interface of a device.

## The Canonical Protocol

- A devices with three registers: status, command and data.
- Simple interaction that the OS might have with the device:
```c
While (STATUS == BUSY)
; // wait until device is not busy
Write data to DATA register
Write command to COMMAND register
(starts the device and executes the command)
While (STATUS == BUSY)
; // wait until device is done with your request
```
- Polling - repeatedly read some data (status register) so that you can react to change (when device is not busy)
- Programmed I/O (PIO) - when the main CPU is involved with the data movement
- Simple
- Inefficient (polling) - a lot of wasted CPU cycles

## Lowering CPU overhead with interrupts

- Instead of polling the device, the OS can issue a request, put the calling process to sleep and context switch to another task. When the device is finally finished with the operation, it will raise a hardware interrupt, causing the CPU to jump into the OS at a predetermined interrupt sevice routine (ISR) or more simply and interrupt handler. The handler is a piece of OS code that will finish the request and wake the process waiting for the I/O, which can then procees as desired.
- Using interrupts is not always the best solution, if the device is fast, the first poll usually finds that the device is already done with a task. Thus, using interrupts will introduce slow down, due to context switching overhead.
- Slow device - interrupts
- Fast device - polling
- Slow and fast - hybrid - start with polling and then use interrupts if it's not complete fastly.
- When there is a flood of interrupts, it may overload a system and lead it to livelock (i.e a lot of incoming network packets).
- Coalescing - wait a bit before raising an interrupt, so that multiple interrupts could be coalesced into 1.

## More efficient data movement with DMA

- The CPU has to copy data from memory to the I/O device - wasted cycles
- Direct Memory Access - a very specific device within a system that can orchestrate transfers between devices and main memory without much CPU intervention.
- To transfer data to the device, for example, the OS would program the DMA engine by telling it where the data lives in memory, how much data to copy, and which device to send it to. At that point, the OS is done with the transfer and can proceed with other work. When the DMA is complete, the DMA controller raises an interrupt, and the OS thus knows the transfer is complete.

## Methods of device interaction

- Explicit I/O instructions (privileged) - specify a way for the OS to send data to specific device registers and thus allow the construction of the protocols described above.
- Memory mapped I/O - the hardware makes device registers available as if they were memory locations. To access a particular register, the OS issues a load (to read) or store (to write) the address; the hardware then routes the load/store to the device instead of main memory.

## Device driver

- Device driver must know in detail how a device works.
- The file system has some generic interface that it uses to issue commands to the device, while the driver actually handles how to execute these commands.
- If the device has some bespoke functionality that cannot be accesses by the generic interface, it will go unused
- Raw interface - which enables special applications (such as a file-system checker, described later [AD14], or a disk defragmentation tool) to directly read and write blocks without using the file abstraction.
- Most of the OS are just device drivers (70% for Linux)