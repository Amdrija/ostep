# Limited Direct Execution

The OS must virtualize the CPU in an efficient manner while retaining control over the system. To do so, both hardware and operating-system support will be required. The OS will often use a judicious bit of hard- ware support in order to accomplish its work effectively.

- Direct Execution: run the program directly on the CPU

## Restricted Operations

- How to let processes access more CPU/memory/I/O, while still preventing them from taking over the system?
- New processor mode: user mode, restricts what the code can do, can't issue I/O requests
- Kernel mode: OS runs in kernel mode, code can do whatever it likes
- When a user process wants to do a privileged operations, it initiates a system call (hardware support)
- Trap instruction: jumps into the kernel and raises the
  privilege level to kernel mode; once in the kernel, the system can now per-
  form whatever privileged operations are needed (if allowed), and thus do
  the required work for the calling process. When finished, the OS calls a
  special return-from-trap instruction, which, as you might expect, returns
  into the calling user program while simultaneously reducing the privi-
  lege level back to user mode.
- When the machine boots up, the OS informs hardware of the location of trap handlers in the trap table.
- When a syscall occurs, the hardware pushes the stack and registers of the user process onto the kernel stack.
- Changes to the kernel mode
- Initiates the trap handler (from the trap table set up at boot)
- The os then handles the trap, does the syscall and returns
- Hardware then restores stack and registers from the user process
- Converts to user mode
- The user process then continues to run

## Switching between processes

- How can the operating system regain control of the CPU so that it can
  switch between processes?
- Cooperative Approach: OS trusts the processes of the system to behave reasonably.Processes
  that run for too long are assumed to periodically give up the CPU so that
  the OS can decide to run some other task. Or when exceptional behaviour occurs. Downside: what if the process is in an infinite loop?
- A Non-cooperative Approach: A timer device can be programmed to raise an interrupt every
  so many milliseconds; when the interrupt is raised, the currently running
  process is halted, and a pre-configured interrupt handler in the OS runs. At boot time, the OS configures the interrupt handler and starts the timer.
- Context switch: all the OS has to do is save a few register values
  for the currently-executing process (onto its kernel stack, for example)
  and restore a few for the soon-to-be-executing process (from its kernel
  stack). By doing so, the OS thus ensures that when the return-from-trap
  instruction is finally executed, instead of returning to the process that was
  running, the system resumes execution of another process
