# Event-based Concurrency

How can we build a concurrent server without using threads, and thus retain control over concurrency as well as avoid some of the problems that seem to plague multi-threaded applications?

## An Event Loop

- Event-based concurrency - wait for something (i.e an event) to occur, when it does, you check what type of event it is and do the samll amount of work it requires (which may include issuin I/O requests, or scheduling other events for future handling, etc)
- Event loop:
```c
while (1) {
    events = getEvents();
    for (e in events)
        processEvent(e);
}
```
- Event handler - the code that processes each event
- When a handler processes an event, it is the only activity taking place in the system; thus, deciding which event to handle next is equivalent to scheduling. This explicit control over scheduling is one of the fundamental advantages of the event-based approach.
- Simpler - no locks needed because only 1 event is handled at a time

## select() / poll()

- API to receive events
- What these interfaces enable a program to do is simple: check whether there is any incoming I/O that should be attended to. 
- These basic primitives give us a way to build a non-blocking event loop.
- Blocking (or synchronous) interfaces do all of their work before returning to the caller;
- Non-blocking (or asynchronous) interfaces begin some work but return immediately, thus letting whatever work that needs to be done get done in the background.

## Blocking system calls

- If an event handler issues a call that blocks, the entire event loop will just block until the call completes.
- Therefore, no blocking calls are allowed in event-based systems

## Asynchronous I/O

- These interfaces enable an application to issue an I/O request and return control immediately to the caller, before the I/O has completed; additional interfaces enable an application to determine whether various I/Os have completed.
- Checking whether an I/O has completed can be done by continuosly polling the relevant API. If therer are a lot of async I/O requests, this can get quite slow
- Some systems provide an approach based on interrupt, it uses Unix signals to inform applications when an async I/O completes, thus removing the need to repeatedly ask the system.

## State Management

- Event-based code is more complicated to write than thread-based code.
- When an event handler issues an asynchronous I/O, it must package up some program state for the next event handler to use when the I/O finally completes; this additional work is not needed in thread-based programs, as the state the program needs is on the stack of the thread. Adya et al. call this work manual stack management, and it is fundamental to event-based programming 
- The solution is to record the needed information to finish processing this event in some data structure; when the event happens (i.e., when the disk I/O completes), look up the needed information and process the event.

## What is still difficult with events

- On multiprocessors, the event loop has to run some event handlers in parallel, which then requires some thread synchronization anyways.
- It doesn't integrate well with certain kinds of systems activity, such as paging. If an event-handler page faults, it will block, and thus the server will not make progress until the page fault completes. Even though the server has been structured to avoid explicit blocking, this type of implicit blocking due to page faults is hard to avoid and thus can lead to large performance problems when prevalent.
- It never integrates quite as well as we hope.