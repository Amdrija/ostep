# The Abstraction: Address Spaces

## Address Space

- The address space of a process contains all of the memory state of the
  running program.
- When the OS does this, we say the OS is virtualizing memory, because
  the running program thinks it is loaded into memory at a particular ad-
  dress (say 0) and has a potentially very large address space (say 32-bits or
  64-bits); the reality is quite different.

## Goals

- Transparency - The OS should implement virtual memory in a way that is invisible to
  the running program.
- Efficiency - The OS should strive to make the
  virtualization as efficient as possible, both in terms of time (i.e., not
  making programs run much more slowly) and space (i.e., not using too much
  memory for structures needed to support virtualization).
- Protection - The OS should make sure to protect processes from one another as well as
  the OS itself from processes. Isolation of processes
