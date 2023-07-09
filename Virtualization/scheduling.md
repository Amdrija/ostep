# Scheduling Introduction

## Workload Assunmptions:

1. Each job runs for the same amount of time.
2. All jobs arrive at the same time.
3. Once started, each job runs to completion.
4. All jobs only use the CPU (i.e., they perform no I/O)
5. The run-time of each job is known.

## Scheduling Metrics

- Turnaround time: The time at which the job completes minus the time at which the job
  arrived in the system.
- Turnaround time is a performance metric
- Fairness metrics - more fair distribution of resources
- Trade-off between performance and fairnes

## FIFO

- The first that arrives is the first that runs
- Relax 1st assumption
- Convoy effect: a number of relatively-short potential consumers of a resource get queued behind a heavyweight resource consumer. Bad performance

## Shortest Job First (SJF)

- It runs the shortest job first, then the next shortest, and so on.
- If all jobs arrive at the same time, then SJF is optimal
- Relax 2nd assumption
- If the shortest jobs arrive a bit after a very long job, then they would have to wait for the long job to finish

## Shortest Time-to-Completion First (STCF)

- Relax 3 as well
- preemptively run long job, and then potentially switch if a shorter job arrives
- Any time a new job enters the system, the STCF scheduler determines which of the
  remaining jobs (including the new job) has the least time left, and schedules that one.
- Given assumptions 4 and 5, STCF is optimal

## Response Time Metric

- We define response time as the time from when the job arrives in a system to the first time it is scheduled
- STCF (and others) aren't good for resposne time. If three jobs arrive at the same time,
  for example, the third job has to wait for the previous two jobs to run in
  their entirety before being scheduled just once.

## Round Robin

- The basic idea is simple: instead of running jobs to completion, RR runs a job for a
  time slice (sometimes called a scheduling quantum) and then switches
  to the next job in the run queue. It repeatedly does so until the jobs are finished.
- Length of the time slice must be multiple of the timer-interrupt period.
- However, making the time slice too short is problematic: suddenly the
  cost of context switching will dominate overall performance.
  Thus, deciding on the length of the time slice presents a trade-off to a system designer,
  making it long enough to amortize the cost of switching without
  making it so long that the system is no longer responsive.
- One of the worst policies for turnaround time.

## Incorporating I/O

- Relax 4
- When a process is waiting for I/O, schedlue another job during that time and then decide when I/O completes
  whether to continue that job, or switch to the I/O job.
