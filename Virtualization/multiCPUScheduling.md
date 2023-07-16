# Multiprocessor Scheduling

How should the OS schedule jobs on multiple CPUs? What new problems arise? Do the same old techniques work, or are new ideas required?

## Multiprocessor Architecture Background

- Cache coherence- when multiple CPUs have their own cache, what if one updates the value at a certain address in memory, but because the memory is slow, it waits a bit to write it to memory, then the program starts running on another CPU, again accessing the same address, it doesn't have it in cache, so it fetches it from memory (the old not-updated value)
- Bus snooping - each cache pays attention to memory updates by observing the bus that connect them to main memory When a CPU then sees an update for a data item it holds in its cache, it will notice the change and either invalidate its copy (i.e., remove it from its own cache) or update it (i.e., put the new value into its cache too).
- Write-back caches make bus snooping more complicated, because the write to main memory isn't visible until later.

## Cache Affinity

- A process, when run on a particular CPU, builds up a fair bit of state in the caches (and TLBs) of the CPU. The next time the process runs, it is often advantageous to run it on the same CPU, as it will run faster if some of its state is already present in the caches on that CPU. If, instead, one runs a pro- cess on a different CPU each time, the performance of the process will be worse, as it will have to reload the state each time it runs (note it will run correctly on a different CPU thanks to the cache coherence protocols of the hardware).

## Single-Queue Scheduling

- Put all jobs that need to be scheduled into a single queue
- Simple
- Lack of scalability - to ensure the scheduler works correctly on multiple CPUs, the use of some form of locking is required, thus slowing the scheduler down because of lock contentions
- Cache affinity - the jobs could bounce around from CPU to CPU. Solution is to provide some mechanism to make it more likely that a process will continue to run on the same CPU if possible.

## Multi-Queue Scheduling

- One queue per CPU
- Each queue will likely follow a particular scheduling discipline (such as round-robin)
- More scalable than SQMS, because lock and cache contetnion should not becoma a central problme
- Intrinsically provides cache affinity, jobs stay on the same CPU and thus reap the advantage of reusing cached contents therein.
- Load imbalance- not all queues (CPUS) will have equal number of processes running on them.
- Migration - move jobs around between the CPUs
- Work stealing - With a work-stealing approach, a (source) queue that is low on jobs will occasionally peek at another (target) queue, to see how full it is. If the target queue is (notably) more full than the source queue, the source will “steal” one or more jobs from the target to help balance load. If you do it too often, there's going to be high overhead, if you do it too rarely, there's going to be imbalance (what you need then is black magic)