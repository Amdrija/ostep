# Process API

What interfaces should the OS present for process creation and control? How should these interfaces be designed to enable powerful functionality, ease of use, and high performance?

## fork()

- Creates a child process that continues on the same fork() call
- Parent receives the PID of newly created child
- Child receives 0
- Depending on the OS scheduler, the child could run before the parent, or vice versa.

## wait()

- Wait for another process to finish before continuing execution

## exec()

- Run different program
- Loads coad and static data from an executable and overwrites its current code segment (and current static data) with it.
- Reinitializes heap, stack and other parts of the memory space of the program
- OS then runs the program, passing in any arguments
- Doesn't create a new process, but rather transforms the current one.
- Successful call to exec never returns

## Why

- Essential in building a UNIX shell, because it lets the shell run code after
  the call to fork() but before the call to exec()
- You then type a command (i.e.,
  the name of an executable program, plus any arguments) into it; in most
  cases, the shell then figures out where in the file system the executable
  resides, calls fork() to create a new child process to run the command,
  calls some variant of exec() to run the command, and then waits for the
  command to complete by calling wait(). When the child completes, the
  shell returns from wait() and prints out a prompt again, ready for your
  next command.

## pipe()

- In this case, the output of one process is connected to an in-
  kernel pipe (i.e., queue), and the input of another process is connected
  to that same pipe; thus, the output of one process seamlessly is used as
  input to the next, and long and useful chains of commands can be strung
  together.

## Process Control and Users

- kill() - send signals to a process
- signal() - catch various signals so that when a particular signal is deliv-
  ered to a process, it will suspend its normal execution and run a particu-
  lar piece of code in response to the signal.
- Users can send signals to control their own processes
