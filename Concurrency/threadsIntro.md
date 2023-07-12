# Concurrency Introduction

- A multi-threaded program has more than one point of execution
- Each thread is like a separate process, except they share the same address
  space and thus can access the same data.
- If there are two threads that are running on a single processor, when switching from
  running one (T1) to running the other (T2), a context switch must take place.
  The context switch between threads is quite similar to the context switch between processes, as the
  register state of T1 must be saved and the register state of T2 restored before running T2. For
  saving state we'll need one or more thread control blocks (TCBs)
- Multiple stacks, one for each thread - thread-local storage.

## Why use threads?

- Parallelism
- Avoid blocking the program progress due to slow I/O, while one thread is blocked, use the others
  to continue the program.
- No reason to assume that a thread that is created first will run first, the scheduler may choose
  to run another thread first.

## Shared Data

- When multiple threads access the same data, we have to be careful, because a thread T1 could read
  the data, thread T2 could update it, and then thread T1 could update it again, resulting in a value
  produced by stale data and possibly making the program wrong.

## Uncontroled Scheduling

- Race condition (data race): the results depend on the timing execution of the code.
- Critical section: A critical section is a piece of code that accesses a shared variable
  (or more generally, a shared resource) and must not be concurrently executed by more than one thread.
- Mutual exclusion: This property guarantees that if one thread is executing within the critical
  section, the others will be prevented from doing so.

## Atomic operations

- It should either appear as if all of the actions you wish to group together occurred, or that none of them occurred,
  with no in-between state visible.
- Hardware should provide a few useful instruction so that we can build synchronisation primitives on top of them.
- What support do we need from the hardware in order to build use-
  ful synchronization primitives? What support do we need from the OS?
  How can we build these primitives correctly and efficiently? How can
  programs use them to get the desired results?

## Waiting for another

- As it turns out, there is another common interaction that arises, where one thread must
  wait for another to complete some action before it continues.

## Summary

- A critical section is a piece of code that accesses a shared resource, usually a variable or data structure.
- A race condition (or data race [NM92]) arises if multiple threads of execution enter the critical section at roughly the same time; both attempt to update the shared data structure, leading to a surprising (and perhaps undesirable) outcome.
- An indeterminate program consists of one or more race conditions; the output of the program varies from run to run, depending on which threads ran when. The outcome is thus not deterministic, something we usually expect from computer systems.
- To avoid these problems, threads should use some kind of mutual exclusion primitives; doing so guarantees that only a single thread ever enters a critical section, thus avoiding races, and resulting in deterministic program outputs.
