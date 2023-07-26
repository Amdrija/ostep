# Flash-based SSDs

How can we build a flash-based SSD? How can we handle the expensive nature of erasing? How can we build a device that lasts a long time, given that repeated overwrite will wear the device out? Will the march of progress in technology ever cease? Or cease to amaze?

## Storing a single bit

- Single-level cell - stores 1 bit within a transistor
- Multi-level cell - can store multiple bits

## From bits to Banks/Planes

- Banks / Planes consist of a large number of cells
- A bank is accessed in tow different size units: blocks - 128KB and pages - 4KB
- Withing each bank there are a large number of blocks; withing each block there are a large number of pages

## Basic Flash Operations

- Read - reads an entire page, it's fast and doesn't depend on the location of a previous read (random access device)
- Erase - Before writing to a page within a flash, you must first erase the entire block the page lies within by setting each bit to 1. Expensive
- Program - After erasing a block, you can set some bits to 0 and write the desired contents of a page to the flash. Less expansive than erasing a block, but more costly than reading a page.

## Flash performance and reliabiility

- Wear out - when a flash chip is erased, it accumulates some charge and then it becomes increasingly diffcult to differentiate a 0 from 1. 
- Disturbance - when accessing a particular page within a flash, it is possible that some bits get flipped in neighboring pages, such bit flips are knows as read disturbs or program disturbs, depending on whether the page is being read or programmed.

## From raw flash to flash-based SSDs

- Flash translation layer - takes read and write requests on logical blocks and turns them into low-level read, erase and program commands on the underlying physical blocks and physical pages.

## FTL organization: A bad approach

- Direct mapped - a read to logical page N is mapped directly to a read og physical page N. A write to logical page N is more complicated, the FTL first has to read in the entire block that page N is contained within, it then has to erase the block and finally the FTL programs the old pages as well as the new one. 
- This is bad for performance, because there's a lot of reading, programming and erasing invloved for a single write.
- It's even worse for reliability, because file system metadata blocks will be erased and programmed a lot more than data blocks.

## Log-Structured FTL

- Logging - upon a write to logical block N, the device appends the write to the next free spot in the currently-being-written-to block
- To allow for subsequent reads of block N, the device keeps a mapping table (in its memory and persistent in some form on the device). This table stores the physical address of each logical block in the system.
- Overwrites to logical blocks lead to garbage, i.e., old bersion of data around the drive and taking up space. The device has to periodically perform garbage collection to ginf said blocks and free space for future writes, excessive garbage collection drives up write amplification and lowers performance.

## Grabage collection

- Find a block that contains one or more garbage pages, read in the live (non-garbage) pages from that block, write out those live pages to the log, and reclaim the entire block for use in writing
- For this to work, the SSD needs to keep track of which blocks are live or dead. One way to do it is to store information about which logical blocks are stored withing each page. The device can then use the mapping table to determine whether each page within the block holds live data or not