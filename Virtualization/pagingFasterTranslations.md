# Paging: Faster Translations (TLBs)

How can we speed up address translation, and generally avoid the
extra memory reference that paging seems to require? What hardware
support is required? What OS involvement is needed?

- Translation-lokaside buffer (TLB) - part of the chip's MMU, hardware cache
  of popular virtual-tophysical address translations.

## TLB Basic Algorithm

- First extract the virtual page number (VPN) from the virtual address
- Check if the TLB hold the translation, if it does, extract the page frame
  number (PFN) from the relevant TLB entry and calculate the physical address
  using the same algorithm as with paging
- If it doesn't hold the translation, access the page table to find the translation
  and update the TLB if it is a valid page. Then retry the instruction.
- Like any cache, TLBs rely on spatial and temporal locality.

## Who handles the TLB miss?

- In the old days (CISC architecture), the hardware would handle TLB misses, it would
  need to know exactly where the page tables are located in memory (via a page-table base register)
  as well as their exact format. On a miss, the hardware would “walk” the page table, find the
  correct page-table entry and extract the desired translation, update the TLB
  with the translation, and retry the instruction.
- More modern architectures (RISC) have a software-managed TLB. On a TLB miss, the hardware
  raises an exception, which pauses the current instruction stream, raises the privilage level to kernel
  mode and jump to a trap handler. This trap handler is code within the OS that is written with the purpose
  of handling TLB misses. When run, the code will lookup the translation in the page table, use spe-
  cial “privileged” instructions to update the TLB, and return from the trap;
  at this point, the hardware retries the instruction (resulting in a TLB hit).
- This return-from-trap needs to be a little bit different than the standard one, because this one has to
  resume execution at the current instruciton (retries the instruction) while the standard one resumes execution
  at the next instruction.
- When running TLB miss code, the OS needs to be careful not to cause an infinite chain of TLB misses. Solutions:
  keep TLB miss handlers in physical memory (where they are unmapped and not subject to address translation) or reserve
  some entries in the TLB for permanently-valid translations and use some of those permanent translations slots for the
  handler code itself. these wired translations always hit in the TLB.
- Primary advantage of the software-managed approach is flexibility, as the OS can use any data structure it wants
  to implement the page table (without necessitating hardware change)
- Another advantage is simplicity, the hardware doesn't do much on a miss, just raise an exception and let the OS do the rest.

## TLB Contents

- A hardware TLB might have 32, 64 or 128 entries and be fully associative.
- Entry: VPN | PFN | other bits, it needs both VPN and PFN, because the mapping
  can be at any of the locations

## TLB Isssue: Context Switches

- When switching context, the entries in the TLB are not meaningful for the soon-to-be switched process.
- Flush TLB before running the next process. Software-based: privilaged hardware instruction.
  Hardware-based: hardware flushes the cache when the PTBR changes (the OS has to change it anyhow).
  Downside: TLB misses until the cache has been populated.
- Sharing of the TLB accross context switches: some hardware has support for address space identifier (ASID)
  field in the TLB, used to differentiate between processes. On context switch, the OS has to switch some
  privilaged register to the ASID of the current process.

## Issue: Replacement Policy

- LRU: evict the least-recently-used entry.
- Random

## A real TLB entry
