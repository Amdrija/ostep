# File System Implementation

How can we build a simple file system? What structures are needed on the disk? What do they need to track? How are they accessed?

## Overall Organization

- Divide disk into 4KB blocks
- Data region - the region of the disk we use for storing user data
- Inode - stores metadata
- Inode table - portion of the disk which holds an array of inodes
- Bitmap - each bit is used to indicate whether the corresponding object/block is free (0) or in-use (1). We have two bitmaps, one for inodes and one for data
- Superblock - contains information about the particular file system, where inode table begins etc. When mounting a file system, the OS will read the superblock first, to initialize various parameters and then attach the volume to the file-system tree.

## Inode

- I-number - low-level name of the file.
- You could use direct pointers inside the inode to denote in which block the data is. However, this is limited, if you have a fie that is really big (bigger than the block size multiplited by the number of direct pointers in the inode), you are out of luck.
- Indirect pointers - if the file is large enough, we allocated a block to store the pointers of the file blocks, while the inode has an indirect pointer set to point to the block of pointers. If you want even bigger size, you could allocate a block which contains indirect pointers and then point the inode's indirect pointer to this block. This can continue for multiple levels.
- Extent - store a pointer to a block and a length, instead of multiple pointers to blocks. A single extent is limiting, because it becomes hard to find contiguous block of free space on the disk. Extent based file systems usually allow for more than 1 extent
- Pointer based approach - more flexible, but requires a lot of metadata
- Extent based approach - less flexible, but more compact

## Directory organization

- Contains a list of (entry name, inode number) pairs. For each file or directory in a given directory, there is a string and a number in the data block(s) of the directory.
- Deleting a file leaves an empty space in the middle of the directory, which should be marked (e.g with reserved inode number 0). Such a delete is one reason the record length is used, a new entry may reuse an old, bigger entry and thus have extra space within.
- File systems treat directories as special types of files, so they have an inode somewhere in the inode table. Their data blocks live in the data region.

## Free Space Management

- When creating a file, we have to allocate an inode for that file. The file system will thus search through the bitmap for an inode that is free, and allocate it to the file; the file system will have to mark the inode as used (with a 1) and eventually update the on-disk bitamp with the correct information. Similar things happens when the system needs to allocate a data block.

## Access Paths: Reading and Writing

### Opening a file
- To open a file `/foo/bar`, the system parses the pathname and starts the traversal from the root directory.
- Root directory inode is commonly 2 on most UNIX file systems
- The FS reads the block that contains inode number 2, once it is read, the FS can look inside of it to find pointer to data blocks, which contain the contents of the root directory.
- The DS uses these on-disk pointer to read through the directory, looking for `foo`
- Once found, the FS will also have found the inode number of foo (say it is 44)
- Then we repeat this process until we find the desired inode (the inode of the `bar` in `foo`).
- The final step of open is to read `bar` inode into memory, check the permissions and allocate a file descriptor for this process in the per-process open-file table, and returns it to the user.

### Reading a file

- Once open, the program can then issue a `read()` syscall to read from the file. The first read (at offset 0 unless `lseek()` has been called) will thus read in the first block of the file, consulting the inode to find the location of such a block; it may also update the inode with a new last-accessed time. The read will further update the in-memory open file table for this file descriptor, updating the file offset such that the next read will read th second file block.

### Writing a file

- Each write to a file logically generates five I/Os: one to read the data bitmap (which is then updated to mark the newly-allocated block as used), one to write the bitmap (to reflect its new state to disk), two more to read and then write the inode (which is updated with the new block’s location), and finally one to write the actual block itself.

### Closing a file

- Deallocate the file descriptor

## Caching and Buffering

- Most file systems aggressively use system memory to cache important blocks
- Static partitioning - allocate a fixed amount of RAM for file system caching
- Dynamic partitioning - flexibly allocate memory for file system caching - unified page cache.
- Write buffering - by delaying writes, the file system can batch some updates into a smaller set of I/Os; econd, by buffering a number of writes in memory, the system can then schedule the subsequent I/Os and thus increase performance. Finally, some writes are avoided altogether by delaying them;