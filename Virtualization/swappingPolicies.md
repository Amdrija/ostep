# Beyond Physical Memory: Policies

How can the OS decide which page (or pages) to evict from memory?
This decision is made by the replacement policy of the system, which usu-
ally follows some general principles (discussed below) but also includes
certain tweaks to avoid corner-case behaviors.

## Cache Management

- Main memory can be viewed as a cahce for virtual memory pages in the system.
- Goal: minimize the number of cache misses.
- Average memory access time (AMAT): `AMAT = Tm + Pmiss * Td`, Tm cost of accessing
  memory, Td cost of accessing the disk and Pmiss the probability of a cache miss.
- The cost of disk access is so high that even a tiny miss will quickly dominate
  the overall AMAT of running programs.

## The optimal replacement policy

- If you have to throw out a page, why not throw out a page that is needed the furthest from now?
  The reason this is true is simple: you will refer to the other pages before you refer to the one furthest out.
- Since it is not possible for the hardware to predict the future (yet :P), this policy is used only as a benchmark
  to see how close our proposed policy is to the optimum.

## FIFO

- First in first out, the first page in the queue is evicted.
- Stack property: cache of size N + 1 includes all the contents of a cache of size N. Therefore, with increasing
  size hit rate will either stay the same or imporve.
- Doesn't have the stack property.
- Shit performance for looping

## Random

- Pick a random page to evict
- Doesn't have stack property.
- Better than FIFO, worse than optimal
- No weird corner case behaviour

## Least Recently Used (LRU)

- Least-Frequently-Used (LFU) policy replaces the least-frequently-used page when an eviction must take place.
- Least-Recently-Used (LRU) policy replaces the least-recently-used page.
- Based on principle of locality
- There are also opposite Most-Frequently-Used (MFU) and Most-Recently-Used (MRU). In most cases
  (not all!), these policies do not work well, as they ignore the locality most programs exhibit instead of embracing it.
- Shit performance for looping
- Implementing causes us to to some book-keeping on every memory reference, which can become really expensive really fast.
  Hardware could update some time field for each memory page (in the per process page table or some other array in memory) and
  the OS could search it to find the least recently used one, however, if there's a lot of these memory pages, it becomes
  very slow.

## Approximating LRU

- Use bit (one per page). Whenever a page is referenced, the use bit is set by hardware to 1. The hardware never clears the bit
  though, that is the responsibility of the OS.
- Clock algorithm - Imagine all the pages of the system arranged in
  a circular list. A clock hand points to some particular page to begin with
  (it doesn’t really matter which). When a replacement must occur, the OS
  checks if the currently-pointed to page P has a use bit of 1 or 0. If 1, this
  implies that page P was recently used and thus is not a good candidate
  for replacement. Thus, the use bit for P is set to 0 (cleared), and the clock
  hand is incremented to the next page (P + 1). The algorithm continues
  until it finds a use bit that is set to 0, implying this page has not been
  recently used (or, in the worst case, that all pages have been and that we
  have now searched through the entire set of pages, clearing all the bits).
- It's not as good as LRU, but it's there-about. (and much better than Random and FIFO).

## Considering Dirty Pages

- If a page has been modified and is thus dirty, it mus be written back to disk to evict it, which is expensive. If it has not
  been modified (an is thus clean), the eviction is free; the physical frame can simply be reused for other purposes without additional I/O.
  Thus some VM systems prefer to evic lcean pages over dirty pages.
- To support this, the hardware should include a modified bit. This bit is set any time a page is written, and thus can be incorporated into
  the page replacement algorithm. he clock algorithm, for example, could be changed to scan for pages that are both unused
  and clean to evict first; failing to find those, then for unused pages that are dirty, and so forth.

## Other Policies

- Page selection policy - when to bring a page into memory. Usually on-demand (demand-paging) brings a page into memory when it is
  accessed, however, prefetching is also possible, if there's a reasonable chance that some other page is going to be accessed soon
  (i.e if a code page P is brought into memory, that code page P +1 will likely soon be accessed)
- When to write pages out to disk: one at a time or clustering of writes (write a number of pages at a time).

## Trashing

- What if the memory demands of the set of running processes simply exceeds the available physical memory?
  In this case, the system will constantly be paging, a condition sometimes referred to as thrashing.
- Some systems run a subset of processes in the hope that they fit into memory - admission control.
- Some systems kill processes that consume too much.
