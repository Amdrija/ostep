# Process

- A running program
- The OS virtualizes the CPU so it appers that each process has its own CPU. This is done through a mechanism called context-switching, which enables time-sharing of the CPU, where each processor has some time to execute on the CPU. There are different policies which decide which process gets priority access to CPU.
- Space-sharing is when we are sharing space, e.g. each file has its own space on the disk.
- A mechanism represents HOW something is done.
- A policy represents decision-making.

## Abstraction

- Each process has its own address space.
- A machine state is everything that the program can read or update while it is running. This includes the process's address space, register, PC / IP, stack pointer, frame pointer, I/O.

## Process API

- Create
- Destroy
- Wait
- Miscellaneous Control
- Status

## Process States

- Initial: in creation
- Running: executing instructions
- Ready: ready to run, but the OS chose not to run it at this moment
- Blocked: performed an operations that makes it not ready to run until some other event takes place (i.e writing to I/O)
- Zombie: finished, but not yet cleaned up

- Scheduled: moved from ready to running
- Descheduled: moved from running to ready

## Data Structures

- Process (task) list: list of running programs
