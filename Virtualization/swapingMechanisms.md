# Beyond Physical Memory: Mechanisms

How can the OS make use of a larger, slower device to transparently pro-
vide the illusion of a large virtual address space?

## Swap Space

- Space reserved on disk for moving pages back and forth
- Space where the instructions of a binary is located on the disk is not swap,
  however, the system will sometimes re-use the memory space for the code pages
  of a process, knowing that it can later swap them in again from the on-disk binary.

## The Present Bit

- If the present bit is set to one, it means the page is present in physical memory; if
  it is set to zero, the page is not in memory but rather on disk somewhere.
- Page fault - the act of accessing a page that is not in physical memory.
- When a page fault occurs, the hardware raises an exception for the OS to handle in
  the page-fault handler

## The page fault

- In the PTE for the page, the OS stores the disk address of the page (instead of PFN)
- Then the OS issues a request to the disk to fetch the page into memory.
- When I/O completes, the OS will then update the page table to mark the page as present,
  update the PFN field of the page-table entry to record the in-memory location
  of the newly-fetched page, and retry the instruction. This could produces a TLB miss,
  so you could update the TLB as well.

## What if memory is full?

- Page-replacement policy - the process of picking a page to kick out
- Usually the OS will always keep a small amount of memory free, so that it doesn't
  have to evict the pages upon swapping. There is High Watermark (HW) and Low Watermark(LW),
  if there are less free pages than the LW, the OS will start a background thread (swap daemon) to evict
  pages until there are more than HW pages free.
