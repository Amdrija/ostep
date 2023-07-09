# Scheduling: The Multi-Level Feedback Queue

## Basic Rules

- Number of disting queue, each assigned a different priority level
- A job that is ready to run is on a single queue
- A job with higher priority is chosen to run
- RoundRobin scheduling between jobs in the same queue
- Use history of the job to predict its future behaviour and then assign a priorty

## How to change priority

- When a jobn enters the system, it is placed at the highes priority
- If a job uses up an entire time slice while running, its priority is reduced
- If a job gives up the CPU before the time slice is up, it stays at the same priority level.
- because it doesn’t know whether a job will be a short job or a long-running job, it first
  assumes it might be a short job, thus giving the job high priority. If it actually is a short job,
  it will run quickly and complete; if it is not a short job, it will slowly move down the queues,
  and thus soon prove itself to be a long-running more batch-like process. In this manner,
  MLFQ approximates SJF.
- First, there is the problem of starvation: if there are “too many” in-
  teractive jobs in the system, they will combine to consume all CPU time,
  and thus long-running jobs will never receive any CPU time (they starve)
- You can game the scheduler: efore the time slice is over, issue an I/O operation
  (to some file you don’t care about) and thus relinquish the CPU; doing so
  allows you to remain in the same queue, and thus gain a higher percent-
  age of CPU time. When done right (e.g., by running for 99% of a time slice
  before relinquishing the CPU), a job could nearly monopolize the CPU.

## Priority Boost

- After some time period S, move all the jobs in the system
  to the topmost queue.
- We solve starvation and the penalty to the long running processes in case when they change to interactive
- What value for S? - vodoo constant, blakc magic

## Better Accounting

- Instead of forgetting the CPU time the job used up before, the CPU will keep track, so it couldn't be gamed
- Once a job uses up its time allotment at a given level (regardless of how many times it has given up the CPU),
  its priority is reduced (i.e., it moves down one queue).

## Tuning MLFQ and Other Issues

- How many queues? How big should the time slice be per queue? How often
  should priority be boosted in order to avoid starvation and account for
  changes in behavior?
- The lower the priority, the higher the time slice in the queue.
- Some use formula to calculate priority instead of tables (queues)
- Some give the highest priority to the OS
- Some schedulers allow users to increase/decrese priorities of a process - called advice
