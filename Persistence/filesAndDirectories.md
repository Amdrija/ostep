# Files and Directories

How should the OS manage a persistent device? What are the APIs? What are the important aspects of the implementation?

## Files and Directories

- File - a linear array of bytes, has a low-level name - inode number
- Directory - contains a list of (user-readable name, low-level name) pairs.

## Creating Files

- `open()` syscall with `O_CREAT` flag
- Returns a file descriptor, an integer, private per process, used for accessing files

## Reading and writing files

- Each running process already has 3 files open: 0 - standard input, 1 - standard output and 2 - standard error
- Each process maintains an array of file descriptors, each of which refers to an entry in the system-wide open file table. Each entry in this table tracks which underlying file the descriptor refers to, the current offset, and other relevant details such as whether the file is readable or writable.

## Non-sequential read and write

- `lseek()` - changes the file offset so that we can specify where the next read / write will start. Usually, reading and writing implicitly changes this offset.
- If we open the same file twice by calling `open()` twice, we will get two file descriptors, and each will have its own entry in the open file table, meaning that each will have its own offset which updates independently.

## Shared File Table Entries
- `fork()` - shares the open file table entry with the child, reference count goes up
`dup()` - allows a process to create a new file descriptor that refers to the same underlying open file as an existing descriptor.

## Writing immadeately with `fsync()`

- Most times when a program calls write(), it is just telling the file system: please write this data to persistent storage, at some point in the future.
- `fsync()` - forces all dirty data of the file referenced by the file descriptor to disk. Returns once all the writes are complete.

## Renaming files

- `rename()` - renames a file with a given name. Atomic with respect to system craashes.
- Atomic file update:
```c
int fd = open("foo.txt.tmp", O_WRONLY|O_CREATO_TRUNC,S_IRUSR|S_IWUSR);
write(fd, buffer, size); // write out new version of file
fsync(fd);
close(fd);
rename("foo.txt.tmp", "foo.txt");
```

## Getting information about files

- `stat()` or `fstat()` - view file metadata

## Removing files

- `unlink()` - deletes a file

## Making directories

- `mkdir()` - creates a directory
- Every directory has two entries: `.` which referes to itself and `..` which refers to the parent directory (what about root directory?)

## Reading directories

- `opendir()` - opens a directory
- `readdir()` - read directory entry
- `closedir()` - closes a directory

## Deleteing Directories

- `rmdir()` - deletes an empty directory, if it's not empty, the syscall fails

## Hard links

- `link()` - creates another way (new file name) to refer to some file (specified by the pathname). Creates another name in the directory you are creating the link in, and refers it to the same inode number
- `unlink()` - removes a link to the inode number, it decrements the reference count in the inode number. If the reference count reaches 0, then the file system will also remove the inode and related data blocks, and thus truly "delete" the file.
- Cannot create a hard link to a directory - so that there won't be any cycles in the directory tree
- Cannot create a hard link to files in other disk partitions (because inode numbers are only unique within a particular file system, not acroos file systems)

## Symbolic (Soft) Links

- A file itself, of a different type.
- Leave a possibility of a dangling reference - removing the file which the symbolic link points to doesn't remove the symbolic link

## Permission Bits

- 9 bits split into three groups: what the owner of the file can do, what someone in a group can do and what anyone can do.
- The abilities the owner, group member or others can have include the ability to read the file, write it, or execute it.
- `chmod` - the owner can change permissions
- For directories, the execute bit allows changing directories (i.e. cd) and in combination with the writeable bit, crate files therein.

## Access Control List

- Enables a user to create a very specific list of who can and cannot readt a set of files, in contrast to the limited owner/group/everyone model of permimssion bits.

## Making and mounting a file system

- Most file systems provide a tool to make them - `makefs`
- `mount` program - takes an existing directory as a target mount point and essentialyl pase a new file system onto the directory tree at that point.