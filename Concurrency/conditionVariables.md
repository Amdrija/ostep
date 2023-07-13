# Condition Variables

In multi-threaded programs, it is often useful for a thread to wait for some condition to become true before proceeding. The simple approach, of just spinning until the condition becomes true, is grossly inefficient and wastes CPU cycles, and in some cases, can be incorrect. Thus, how should a thread wait for a condition?

## Definition and Routines

- A condition variable is an explicit queue that threads can put themselves on when some state of execution (i.e., some condition) is not as desired (by waiting on the condition); some other thread, when it changes said state, can then wake one (or more) of those waiting threads and thus allow them to continue (by signaling on the condition). 
- `wait()` - executed when a thread wishes to put itseld to sleep
- `signal()` - executed when a thread has changed something in the program and thus wants to wake a sleeping thread waiting on this condition.

```c
int done = 0;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t c = PTHREAD_COND_INITIALIZER;

void thr_exit() {
    Pthread_mutex_lock(&m);
    done = 1;
    Pthread_cond_signal(&c);
    Pthread_mutex_unlock(&m);
}

void *child(void *arg) {
    printf("child\n");
    thr_exit();
    return NULL;
}

void thr_join() {
    Pthread_mutex_lock(&m);
    while (done == 0)
        Pthread_cond_wait(&c, &m);
    Pthread_mutex_unlock(&m);
}

int main(int argc, char *argv[]) {
    printf("parent: begin\n");
    pthread_t p;
    Pthread_create(&p, NULL, child, NULL);
    thr_join();
    printf("parent: end\n");
    return 0;
}
```
- The `done` variable seems unnecessary, but imagine if we don't have it. Then if the child thread runs immadeately, it will acquire the lock, signal the sleeping threads waiting on the condition to wake up (in this case non), release the lock and then exit. Afterwards, the main thread continues to run, tries to join the child thread, acquires the lock and then waits indefinitely on the condition because there is no other thread to wake it.

## Producer/Consumer (Bounded Buffer) Problem

- One or more producer threads - generate data items and place them in a buffer
- One or more consumer threads - grab data items from the buffer and consume them in some way
- Because the bounded buffer is a share resource, we must requrie synchgronized access to it
- Mesa semantics - Signaling a thread only wakes them up; it is thus a hint that the state of the world has changed (in this case, that a value has been placed in the buffer), but there is no guarantee that when the woken thread runs, the state will still be as desired
- Hoare semantics - is harder to build but provides a stronger guarantee that the woken thread will run immediately upon being woken [H74]. Virtually every system ever built employs Mesa semantics.
- Because of Mesa semantics, always use while loops with condition variables.
- A consumer should not wake other consumers (same with the producers)
- Use two condition variables, one for consumers to wake up and one for the producers to wake up. Consumers can only signal the produces condition variable and vice versa.
```c
cond_t empty, fill;
mutex_t mutex;

void *producer(void *arg) {
    int i;
    for (i = 0; i < loops; i++) {
        Pthread_mutex_lock(&mutex);
        while (count == 1)
            Pthread_cond_wait(&empty, &mutex);
        put(i);
        Pthread_cond_signal(&fill);
        Pthread_mutex_unlock(&mutex);
    }
}

void *consumer(void *arg) {
    int i;
    for (i = 0; i < loops; i++) {
        Pthread_mutex_lock(&mutex);
        while (count == 0)
            Pthread_cond_wait(&fill, &mutex);
        int tmp = get();
        Pthread_cond_signal(&empty);
        Pthread_mutex_unlock(&mutex);
        printf("%d\n", tmp);
    }
}
```
- Adding more buffer slots just changes the buffer functions `get()` and `put()` and slightly alters the condition for produces to use `MAX` instead of `1`.
```c
int buffer[MAX];
int fill_ptr = 0;
int use_ptr = 0;
int count = 0;

void put(int value) {
    buffer[fill_ptr] = value;
    fill_ptr = (fill_ptr + 1) % MAX;
    count++;
}

int get() {
    int tmp = buffer[use_ptr];
    use_ptr = (use_ptr + 1) % MAX;
    count--;
    return tmp;
    }
```

## Covering condition

```c

```

- Assume we have a multi-threaded memory allocator. When threads call `allocate()`, if there isn't enough free memory, they wait on a condition variable. When some other thread calls `free()`, it signals on the condition, waking up a thread.
- Imagine if there is `0B` available in memory. If thread T1 called `allocate(100)`, it would go to sleep. Then thread T2 calls `allocate(10)` and it too would go to sleep. Thread T3 then calls `free(50)` and signals on the condition variable, however, thread T1 is woken up, there is still not enough memory so it goes back to sleep and thread T2 possibly never gets the allocation even though there is enough memory.
- This is fixed by using a broadcast signal - waking up all the threads waiting on it. Then some may be able to get the allocation, while others are put back to sleep.
- Such a condition is called covering condition - `pthread_cond_broadcast()`, as it covers all cases where a thread needs to wak up.
- Performance cost as not all the threads should be woken up