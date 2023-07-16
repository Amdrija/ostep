# Common Concurrency Problems

Concurrency bugs tend to come in a variety of common patterns. Knowing which ones to look out for is the first step to writing more robust, correct concurrent code.

## Atomicity violation
-The desired serializability among multiple memory accesses is violated (i.e. a code region is intended to be atomic, but the atomicity is not enforced during execution).
- Example from MySQL:

```c++
Thread 1::
if (thd->proc_info) {
    fputs(thd->proc_info, ...);
}

Thread 2::
thd->proc_info = NULL;
```

- If thread 1 checks that `proc_info` is not `NULL`, gets interrupted and then thread 2 starts running, sets `proc_info` to NULL, thread 1 will crash, because of a `NULL` pointer dereference in fputs.
- Fix is simple, add a lock around the sections in thread 1 and thread 2, so that only 1 thread can read/write to `proc_info`.

## Order-Violation Bugs
- The desired order between two (groups of) memory accesses is flipped (i.e., A should always be executed before B, but the order is not enforced during execution)
- Example:
```c++
Thread 1::
void init() {
    mThread = PR_CreateThread(mMain, ...);
}

Thread 2::
void mMain(...) {
    mState = mThread->State;
}
```
- The code in thread 2 assumes that the variable `mThread` is initialized and not `NULL`. If thread 2 runs first, this will obviously not be the case as thread 1 is the one which does the initialization.
- Enforce ordering, for example by using condition variables.

## Deadlock Bugs

- A cycle of threads waiting on each other to acquire locks.
- Deadlocks occur because of complex systems interacting with each other, or sometimes, a hidden implementation detail.
- Conditions needed for a deadlock:
    1. Mutual exclusion: Threads claim exclusive control of resources that they require (e.g., a thread grabs a lock)
    2. Hold-and-wait: Threads hold resources allocated to them (e.g., locks that they have already acquired) while waiting for additional resources (e.g., locks that they wish to acquire)
    3. No preemption: Resources (e.g., locks) cannot be forcibly removed from threads that are holding them.
    4. Circular wait: There exists a circular chain of threads such that each thread holds one or more resources (e.g., locks) that are being requested by the next thread in the chain

### Circular wait
- Prevent deadlocks by writing your code in such a way that a circular wait can never occur.
- Total ordering on lock acquisition (locks are always acquired in the same order).
- Partial ordering - require some subset of locks to always be acquired in the same order.
- Acquire locks by address order
- Sometimes very hard to do
- Easy to circumvent

### Hold-and-wait

- Hold-and-wait can be avoided by acquiring all locks at once, atomically, by wrapping the lock acquisition in another lock, ensuring only 1 thread can acquire all the locks.
- Decreases concurrency
- We must know which locks must be held and acquire them ahead of time

### No Preemption

- Release the lock if we cannot get another lock:
```c++
top:
    pthread_mutex_lock(L1);
        if (pthread_mutex_trylock(L2) != 0) {
            pthread_mutex_unlock(L1);
        goto top;
    }
```
- Order agnostic way, however, we could introduce a livelock if thread interleave in such a way that they never acquire both locks.
- If the routing that is called is more complex, the jump to being is tmore complex to implement (freeing resources etc.)
- If the code has acquired some resources along the way, it must ensure to release them as well.
- It doesn't really add preemption, just let's you sort of opt out of acquiring the lock.

### Mutual Exclusion

- Avoid the need for mutual exclusion at all.
- Very difficult
- Using powerful hardware instructions, you can build data structures in a manner that does not require explicit locking.
- Compare and swap instruction (atomic):
```c
int CompareAndSwap(int *address, int expected, int new) {
    if (*address == expected) {
        *address = new;
        return 1; // success
    }

    return 0; // failure
}
```
- Increment a value by a certain amount:
```c
void AtomicIncrement(int *value, int amount) {
    do {
        int old = *value;
    } while (CompareAndSwap(value, old, old + amount) == 0);
}
```
- This way no deadlock is possible, however a livelock could still arise.
- Inserting into a list:
```c
void insert(int value) {
    node_t *n = malloc(sizeof(node_t));
    assert(n != NULL);
    n->value = value;
    // The race condition is here, let's assume empty list, T1 and T2 are here,
    // T1 runs the next statement, then T2 runs it
    n->next = head;
    // Now, both nodes will point to NULL,
    // So whichever thread runs this statement last, it's node will be in the list,
    // but the other thread's node won't
    head = n;
}

// Thread-safe insert without locks
void insert_thread_safe(int value) {
    node_t *n = malloc(sizeof(node_t));
    assert(n != NULL);
    n->value = value;
    do {
        n->next = head;
    } while (CompareAndSwap(&head, n->next, n) == 0);
}
```

### Avoidance via Scheduling

- Requires some global knowledge of which locks various threads might grab during their execution, and subsequently schedules said threads in  a way as to guarantee no deadlock can occur.
- Smart scheduler could determine that some threads should never run concurrently, because they could produce a deadlock (i.e these threads try to acquire the same locks)
- Generally worse performance
- We don't always know in advance which locks which thread needs.

### Detect and Recover

- Allow deadlocks to occur and then take some action once such a deadlock has been detected.
- If an OS froze once a year, just reboot it and call it a day.
- Not everything worth doing is worth doing well - Tom West