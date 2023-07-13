# Interlude: Memory API

In UNIX/C programs, understanding how to allocate and manage memory is critical in building robust and reliable software. What interfaces are commonly used? What mistakes should be avoided?

## Types of Memory

- stack/automatic
- heap - allocated/deallocated by the programer

## malloc()

- sizeof - compile time operator
- sizof(variable) - the size of the variable type, checked at compile time,
  i.e sizeof(char[10]) = 10, sizeof(char\*) = 8
- not a system call, built on top of the sys call - brk
- brk - changes the location of the program's break: the location of the end of the heap.
- mmap() - creates anonymous region of memory, outside heap and stack

## Common errors

- not allocating memory
- not allocating enough
- uninitialized reads - allocated memory, but didn't set
- memory leak - forget free()
- dangling pointer - using the pointer after freeing memory
- double free - calling free twice on the same pointer
- invalid free - passing a pointer not coming from malloc
