# Scheduling: Proportional Share

How can we design a scheduler to share the CPU in a proportional manner? What are the key mechanisms for doing so? How effective are they?

## Tickets Represent Your Share

- Scheduler holds a lottery, by randomly selecting a ticker (eg. between 0-99),
  If A has 75% of CPU - it has 75 tickets, say B has 25% CPU - 25 tickets. Pick
  a random number between 0-99, if it is 0-74 then A is running if its 75-99 then
  B runs.
- Probabilistic

## Ticket Mechanisms

- Ticket currency - converts local tickets to the global tickets (e.g when 2 users
  use ticketing to allocate their time of CPU to the jobs)
- Ticket transfer - a process
  can temporarily hand off its tickets to another process.
- Ticket inflation - With inflation, a process can temporarily raise or
  lower the number of tickets it owns.
- How to assign tickets?

## Stride Scheduling

- Deterministic fair share scheduler
- Each job has a stride - inverse in proportion of tickets it has
- every time a process runs, we will increment a counter for it
  (called its pass value) by its stride to track its global progress.
- at any given time, pick the process to run that has the lowest pass value so far;
  when you run a process, increment its pass counter by its stride.
- The problem is global state, what happens when a new process enters, what would its
  pass value be?

## Linux Completely Fair Scheduler (CFS)

- As each process runs, it accumulates vruntime. In the most basic
  case, each process’s vruntime increases at the same rate, in proportion
  with physical (real) time. When a scheduling decision occurs, CFS will
  pick the process with the lowest vruntime to run next.
- Time slice = sched_latency / number of processes. (sched_latency - parameter)
- min_granularity - minimum time-slice
- niceness - the nicer you are, the less runtime you get (-19 to + 20, default 0)
- vruntime = vruntime + weight0 \* runtime / weightI
- Uses red-black trees to ensure logarithmic search time for the minimum vruntime
- If the process is sleeping for a long time then it will monopolize the CPU. This
  is fixed by setting its vruntime to the minimum value found in the tree.
