# Segmentation

How do we support a large address space with (potentially) a lot of
free space between the stack and the heap? Note that in our examples,
with tiny (pretend) address spaces, the waste doesn’t seem too bad. Imag-
ine, however, a 32-bit address space (4 GB in size); a typical program will
only use megabytes of memory, but still would demand that the entire
address space be resident in memory.

## Segmentation: Generalized Base/Bounds

- Instead of having just one base and bounds pair in our MMU, why not have a base and bounds pair per
  logical segment of the address space?
- The hardware structure in our MMU required to support segmentation is a set of three base and bounds
  register pairs.
- For heap, we have to calculate the offset in the code from heap start, i.e. code is 0-4k, heap is 4k-7k,
  if we access the address 4200, the offset in the heap is actually 4200-4096 = 104, and the physical address
  is heapBase + 104.
- Something similar has to happen for stack as well, we just need a different formula for offset since the
  stack grows towards lower addresses.

## Which Segment Are We Referring To?

- Explicit approach: chop up the address space into segments based on the top few bits
  of the virtual address; For example, top 2 bits are used to denote the segment, if heap is 01, then
  the offset is the remaining bits (in the example above, the remaining 12 bits are equal to 104)
- The problem, for example if we have code + stack + heap, we need 2 bits, but only 3 parts are used
- Limits the use of the address space
- Implicit approach: hardware determines the segment by noticing how the address was formed. If, for ex-
  ample, the address was generated from the program counter (i.e., it was
  an instruction fetch), then the address is within the code segment; if the
  address is based off of the stack or base pointer, it must be in the stack
  segment; any other address must be in the heap.

## What About The Stack?

- Extra hardware support: needs also a bit to denote in which way the segment is growing (positive or negative)
- In this example, assume we wish to access virtual address 15KB, which should map to physical address 27KB.
  Our virtual address, in binary form, thus looks like this: 11 1100 0000 0000 (hex 0x3C00).
  The hardware uses the top two bits (11) to designate the segment, but then we areleft with an offset of 3KB.
  To obtain the correct negative offset, we must subtract the maximum segment size from 3KB: in this example,
  a segment can be 4KB, and thus the correct negative offset is 3KB minus 4KB which equals -1KB.
  We simply add the negative offset (-1KB) to the base (28KB) to arrive at the correct physical address: 27KB.
  The bounds check can be calculated by ensuring the absolute value of the negative offset isless than or equal
  to the segment’s current size (in this case, 2KB).

## Support for sharing

- Sometimes it is useful to share certain memory segments between address spaces
- Extra support: protection bits - Basic support adds a few bits per segment,
  indicating whether or not a program can read or write a segment, or perhaps
  execute code that lies within the segment.
- By setting a code segment to read-only, the same code can be shared across multiple processes,
  without worry of harming isolation;
- The hardware also has to check whether a particular access is permissible.

## Fine-grained vs. Coarse-grained Segmentation

- Coarse-grained - it chops up the address space into relatively large coarse chunks.
- Fine-grained - address spaces to consist of a large number of smaller segments

## OS Support

- What should the OS do on a context switch? The segment registers must be saved and restored.
- What happens when segments grow or shrink? If the segment has to grow, The OS
  will then (usually) provide more space, updating the segment size register to the new (bigger) size,
  and informing the library of success; OS could reject the request, if no more physical memory is available,
  or if it decides that the calling process already has too much.
- When a new address space is created, the OS has to be able to find space in physical memory for its segments.
- External fragmentation: physical memory quickly becomes full of little holes of free space,
  making it difficult to allocate new segments, or to grow existing ones.
- Compacting physical memory by rearranging the existing segments - expensive and makes request to grow existing
  segments hard to serve
- There are free-list management algorithms that attempt to minimze external fragmentation

## Homework

- Potential bug? When running segmentation.py -a 128 -p 512 -b 0 -l 20 -B 512 -L 20 -s 0, if the address is 108,
  then the offset is -20, which is >= limit, so according to the code it should cause a segfault, however, the solver
  says that it is a valid address. The solver computes the right answer, because the maximum virtual address is 127,
  so starting from it, 20th address is 108, and it is valid. However, then the shown code is a bit confusing.
