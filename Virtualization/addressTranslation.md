# Mechanism: Address Translation

## Assumptions

1. The user’s address space must be placed contiguously in physical memory.
2. The size of the address space is less than the size of physical memory.
3. Each address space is exactly the same size

## Dynamic (Hardware-based) Relocation (base and bounds)

- Base register and bounds register in the CPU
- Each program written and compiled as if it is loaded at address 0
- when a program starts running, the OS decides where in physical memory
  it should be loaded and sets the base register to that value.
- Memory translations formula:
  physical address = virtual address + base
- CPU checks if the memory reference (virtual address) is withing the bounds,
  if it is out of bounds, the CPU raises an exception and the process is likely killed
- Memory Management Unit (MMU) part of the CPU that helps with address translation
- Changing base and bounds is a privileged operation

## Operating System Issues

- When the process is created, the OS will have to search a data structure
  (often called a free list) to find room for the new address space and then mark it used.
- Upon termination of a process, the OS thus puts its memory back on the free
  list, and cleans up any associated data structures as need be
- When the OS decides to stop running a process, it must save the values
  of the base and bounds registers to memory, in some per-process structure such as the
  process structure or process control block (PCB)
- When the OS resumes a running process (or runs it the first time), it must set the values
  of the base and bounds on the CPU to the correct values for this process
- It is possible for the OS to move an address space from one location in memory to another easily.
  To move a process’s address space, the OS
  first deschedules the process; then, the OS copies the address space from
  the current location to the new location; finally, the OS updates the saved
  base register (in the process structure) to point to the new location. When
  the process is resumed, its (new) base register is restored, and it begins
  running again, oblivious that its instructions and data are now in a com-
  pletely new spot in memory.
- The OS must provide exception handlers, the OS installs these handlers at boot time
- Inefficiencies: internal fragmentation - the memory between stack and heap is wasted, because
  the OS allocated too much (because it allocated fixed size slots)
