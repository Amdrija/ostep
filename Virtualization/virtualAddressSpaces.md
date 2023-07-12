# Virtual Address Spaces

- You can think of it as if a process had all the memory of the
  virtual address space for itself and kernel is in physical memory.
- The problem with this is when a context switch happens (e.g for a syscall),
  the process's virtual addresses need to be translated to physical for the OS
  to do the syscall, and then if there is some address that needs to be returned,
  it would again have to be translated back to the virtual address space of the
  process.
- For this reason, the address space is divided into user space and kernel space
  (in 32bit Linux, kernel space starts at virtual addresses 0xC000 0000). So the
  processes page table contains mapping for the kernel virtual addresses as well
  to their physical addresses in memory. These mapping could be different for
  every process (for process specific data structures in the kernel, e.g page tables),
  but this way, the processes can share some parts of the kernel (they would then have
  the same mappings). Additionally, on context switches, when the kernel gets a virtual
  address, it can continue to use it normally, because when accessing it, the hardware
  will perform the translation.
