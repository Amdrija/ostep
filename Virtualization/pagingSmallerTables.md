# Paging: Smaller Tables

Simple array-based page tables (usually called linear page tables) are
too big, taking up far too much memory on typical systems. How can we
make page tables smaller? What are the key ideas? What inefficiencies
arise as a result of these new data structures?

## Bigger Pages

- Just use bigger pages
- Downside: more internal fragmentation

## Hynrid Approach: Paging and Segments

- One page table per logical segment (code, stack and heap)
- Base register in the MMU hold the physical address of the page table of the segment
- Bounds register indicates the end of the page table, i.e. if the code segment is using
  the first 3 pages (0, 1 and 2), the code segment page table will only have three entries
  allocated to it and the bounds register will be set to 3; memory accesses beyone the end
  of the segment will generate an exception and likely lead to the termination of the process.

```c
SN = (VirtualAddress & SEG_MASK) >> SN_SHIFT
VPN = (VirtualAddress & VPN_MASK) >> VPN_SHIFT
AddressOfPTE = Base[SN] + (VPN * sizeof(PTE))
```

- Downside: not quite as flexible, external fragmentation again (because page tables have variable size).

## Multi-level page tables

- Chop up the page table into page-sized units, then if an entire page of page-table entries is invalid,
  don't allocate taht page of the page table at all. To track whether a page of the page table is valid
  (and if valid, where it is in memory), use a new structure, called the page directory.
- The page directory, in a simple two-level table, contains one entry per
  page of the page table. It consists of a number of page directory entries
  (PDE). A PDE (minimally) has a valid bit and a page frame number
  (PFN), similar to a PTE. However, as hinted at above, the meaning of
  this valid bit is slightly different: if the PDE is valid, it means that at least
  one of the pages of the page table that the entry points to (via the PFN)
  is valid, i.e., in at least one PTE on that page pointed to by this PDE, the
  valid bit in that PTE is set to one. If the PDE is not valid (i.e., equal to
  zero), the rest of the PDE is not defined
- Allocates page-table space in proportion to the amount of address space you are using, thus it's
  generally compact and supports sparse address spaces.
- If carefully constructed, each portion of the page table fits neatly within a page, making it easier to
  manage memory; the OS can simply grab the next free page when it needs to allocate or gro a page table.
- Disadvantage: on TLB miss, two memory accesses need to be performed - one for the page directory and
  one for the PTE itself.
- Sometimes, the page-directory itself cannot fit into a page, so we need multiple levels of page directories.

## Inverted Page Tables

- Single page table that has an entry for each physical page of the system (instead of a page table per process).

## Swapping the Page Tables to Disk

- Some systems place page tables into kernel virtual memory, allowing the system to swap some of these page talbes
  to disk when memory pressure gets a little tight.
