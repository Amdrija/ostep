# Free-Space Management

How should free space be managed, when satisfying variable-sized re-
quests? What strategies can be used to minimize fragmentation? What
are the time and space overheads of alternate approaches?

## Assumptions

- Basic interface - malloc() and free()
- Free list structure tracks free space
- We are trying to solve external fragmentation
- When memory is handed out to a client, it cannot be relocated to another location in memory,
  as a consequence, no compaction of free space is possible (however, the OS could do it)
- The allocator manages a contiguous region of bytes, which is fixed size (for simplicity)

## Low-level Mechanisms

### Splitting and Coalescing

- If the request is for something smaller than the size of the elements
  in the free list. Then, the allocator will perform an action called splitting: it will find a free
  chunk of memory that can satisfy the request and split it into two. The first chunk it will return
  to the caller; the second chunk will remain on the list.
- When returning a free chunk in memory, look at the addresses of the chunk you are returning as well
  as the addresses of the nearby free chunks. If the newely freed space sits right next to one (or two)
  existing free chunks, merge them into a single larger free chunk.

### Tracking the size of allocated regions

- Since free doesn't take a size parameter, it is assumed that the memory allocation library tracks
  the size of the allocated memory the pointer points to.
- This is accomplished by keeping a header block in memory (with size information, etc), usually just
  before the handed-out chunk of memory
- When freeing the library can get the size of the memory region by using simple pointer arithmetic and
  then adding the size of the header block

### Embedding a free list

- When making a dynamically sized data structure, such as a list, you would usually call malloc() to allocate
  more memory for the new node. However, in the memory allocation library, you cannot call malloc(), so you have
  to embedd the list in the free space.
- Simple code for initializing the list:

```c
// mmap() returns a pointer to a chunk of free space
node_t *head = mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_ANON|MAP_PRIVATE, -1, 0);
head->size = 4096 - sizeof(node_t);
head->next = NULL;
```

- Now, when a chunk of memory is requested, the library will find a large enough free chunk, split it into two
  one chunk big enough to service the request(plus the header) and the remaining free chunk. It then shrunks the
  remaining free chunk.
- When freeing a chunk (no coalescing), the library adds the free chunk at the head of the list.
  In the free memory reagion, it write the size(e.g 100 bytes) and next chunk pointer (the previous value of the head).
  Then it will move the head pointer to to beginning of the new free memory region (pointer to be freed value - header size)
- Afterwards, go through the list to check if a merge should happen.

### Growing the Heap

- When running out of heap memory, issue a system call to the OS to request more. The OS finds free physical pages, maps them into the
  address space of the requesting process, and then returns the value of the end of the new heap; at that point, a larger heap is available,
  and the request can be successfully serviced.

## Basic strategies

- Best Fit - first, search through the free list and find chunks of free memory that are as big or bigger than the requested
  size. Then, return the one that is the smallest in that group of candidates; Naive implementations have a high performance cost
- Worst Fit - opposite of Best Fit, find the largest chunk and return the requested amount; keep the remaining (large) chunk on
  the free list. It tries to leave big chunks free, instead of lots of small chunks that can arise from a best-fit approach.
  Performance penalty because of the exaustive search and it's shown to perform badly, leaving excess fragmentation.
- First Fit - finds the first block that is big enough and returns the requested amount to the user. Speed advantage (no exhaustive search),
  but sometimes pollutes the beginning of the free lsit with small objects.
- Next Fit - Instead of always beginning the first-fit search at the beginning of the list, the next fit algorithm keeps an
  extra pointer to the location within the list where one was looking last. Avoids splintering the beginning of the list, while being just
  as performant (no exhaustive search)

## Other Approaches

- Segrergated Lists - if a particular application has one (or a few) popular-sized request that it makes, keep a separate list just
  to manage objects of that size; all other requests are forwarded to a more general memory allocator.
- Buddy Allocation - Start out with a blokc of size 2^N and then recursively split this block until you get a chunk of the lowest
  size that can fulfill the request and then return that chunk. You are only allowed to return power of 2 sized blocks, so it
  suffers from internal fragmentation. When the block is freed, it will check if the "buddy" block of the same size
  is free, and them murge them together, this process continues recursively. It is easy to check the buddy block, because their
  addresses differ only in 1 bit, which is the level of the tree.
