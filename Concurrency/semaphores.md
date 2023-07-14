# Semaphores

How can we use semaphores instead of locks and condition variables? What is the definition of a semaphore? What is a binary semaphore? Is it straightforward to build a semaphore out of locks and condition variables? To build locks and condition variables out of semaphores?

## Definition

- A semaphore is an object with an integer value that we can manipulate with two routines; in the POSIX standard, these routines are sem wait() and sem post()
- Semaphore initialization, `0` means that the semaphore is shared between threads in the same process and the third argument is the actual starting value - `1`
```c
#include <semaphore.h>
sem_t s;
sem_init(&s, 0, 1);
```
- Semaphore routines:
```c
int sem_wait(sem_t *s) {
    decrement the value of semaphore s by one
    wait if value of semaphore s is negative
}

int sem_post(sem_t *s) {
    increment the value of semaphore s by one
    if there are one or more threads waiting, wake one
}
```
- The value of the semaphore, when negative, is equal to the number of waiting threads.

## Binary Semaphores (Locks)

- Initial value is 1, because the first thread can access the critical section, decrements the semaphore value to 0, and any subsequent threads which try to `sem_wait()` will decrement the semaphore value to a negative number, meaning that they'll go to sleep, until the first thread exits the critical section and calls `sem_post()` which will increase the semaphore value (but the result could still be negative) and wake one of the threads (which will run in the critical section, because everyone else is waiting, and the first thread has exited the critical section), this continues until all threads have run the critical section.
```c
sem_t m;
sem_init(&m, 0, 1);

sem_wait(&m);
// critical section here
sem_post(&m);
```

## Semaphores for Ordering

- Use semaphores similarly to the use of conditional variables.
- We want the main thread to wait for the child thread to signal something, and then to continue execution of the main thread.
```c
sem_t s;

void *child(void *arg) {
    printf("child\n");
    sem_post(&s); // signal here: child is done
    return NULL;
}

int main(int argc, char *argv[]) {
    sem_init(&s, 0, 0); // what should X be?
    printf("parent: begin\n");
    pthread_t c;
    Pthread_create(&c, NULL, child, NULL);
    sem_wait(&s); // wait here for child
    printf("parent: end\n");
    return 0;
}
```
- The initial value of the semaphore should be `0`. If the main thread runs `sem_wait()` before the child thread runs `sem_post()`, the value of the semaphore would thus be negative, then the main would go to sleep. Now the child thread runs `sem_post()` increments the semaphore value and wakes the main thread. In the other case, when the child runs `sem_post()` before the main thread runs `sem_wait()`, the semaphore value will be 1, then when the main thread runs, it will decrement the semaphore value, realize that it is not negative and simply return instead of going to sleep.

## Produces/Consumer (Bounded Buffer)
- The buffer code:
```c
int buffer[MAX];
int fill = 0;
int use = 0;

void put(int value) {
    buffer[fill] = value; // Line F1
    fill = (fill + 1) % MAX; // Line F2
}

int get() {
    int tmp = buffer[use]; // Line G1
    use = (use + 1) % MAX; // Line G2
    return tmp;
}
```
- Naive implementation:
```c
sem_t empty;
sem_t full;

void *producer(void *arg) {
    int i;
    for (i = 0; i < loops; i++) {
        sem_wait(&empty); // Line P1
        put(i); // Line P2
        sem_post(&full); // Line P3
    }
}

void *consumer(void *arg) {
    int tmp = 0;
    while (tmp != -1) {
        sem_wait(&full); // Line C1
        tmp = get(); // Line C2
        sem_post(&empty); // Line C3
        printf("%d\n", tmp);
    }
}

int main(int argc, char *argv[]) {
    // ...
    sem_init(&empty, 0, MAX); // MAX are empty
    sem_init(&full, 0, 0); // 0 are full
    // ...
}
```
- It works if `MAX = 1`, no matter how many producers / consumers we have, however, if `MAX > 1`, we have race conditions in `get()` and `put()` functions. Let's assume `MAX = 2` and we have 2 producers and 2 consumers. If the producer TP1 runs, up to line `F1` (running it as well) and then is switched so that the producer TP2 can run, again continuing running to `F1`, it will overwrite the value that TP1 put into it, causing a race condition.
- We try to fix it by adding a mutual exclusion lock:
```c
void *producer(void *arg) {
    int i;
    for (i = 0; i < loops; i++) {
        sem_wait(&mutex); // Line P0 (NEW LINE)
        sem_wait(&empty); // Line P1
        put(i); // Line P2
        sem_post(&full); // Line P3
        sem_post(&mutex); // Line P4 (NEW LINE)
    }
}

void *consumer(void *arg) {
    int i;
    for (i = 0; i < loops; i++) {
        sem_wait(&mutex); // Line C0 (NEW LINE)
        sem_wait(&full); // Line C1
        int tmp = get(); // Line C2
        sem_post(&empty); // Line C3
        sem_post(&mutex); // Line C4 (NEW LINE)
        printf("%d\n", tmp);
    }
}
```
- Now imagin if a consumer thread tries to run first, it decrements the mutex semaphore value to 0, then decrements the full semaphore value to -1, and then goes to sleep. Now a produces runs, tries to acquire the lock, decrements the mutex semaphore value to -1, and goes to sleep. We have thus made a deadlock.
- To solve this, we simply must reduce the scope of the block, now, the lock is only for the critical section between producers and consumers. Now, it is not possible to go to sleep while holding the mutex lock (because there is no code that would put you to sleep between holding the lock and releaseing it).
```c
void *producer(void *arg) {
    int i;
    for (i = 0; i < loops; i++) {
        sem_wait(&empty); // Line P1
        sem_wait(&mutex); // Line P1.5 (MUTEX HERE)
        put(i); // Line P2
        sem_post(&mutex); // Line P2.5 (AND HERE)
        sem_post(&full); // Line P3
    }
}

void *consumer(void *arg) {
    int i;
    for (i = 0; i < loops; i++) {
        sem_wait(&full); // Line C1
        sem_wait(&mutex); // Line C1.5 (MUTEX HERE)
        int tmp = get(); // Line C2
        sem_post(&mutex); // Line C2.5 (AND HERE)
        sem_post(&empty); // Line C3
        printf("%d\n", tmp);
    }
}
```

## Reader-Writer Locks

- Sometimes, we want to be able to read from the datastructure as long as there isn't an update hapenning - we allow multiple threads to read at the same time, but the moment one thread starts to write, no threads should be able to read, until the write has been finished.

```c
typedef struct _rwlock_t {
    sem_t lock; // binary semaphore (basic lock)
    sem_t writelock; // allow ONE writer/MANY readers
    int readers; // #readers in critical section
} rwlock_t;

void rwlock_init(rwlock_t *rw) {
    rw->readers = 0;
    sem_init(&rw->lock, 0, 1);
    sem_init(&rw->writelock, 0, 1);
}

void rwlock_acquire_readlock(rwlock_t *rw) {
    sem_wait(&rw->lock);
    rw->readers++;
    if (rw->readers == 1) // first reader gets writelock
        sem_wait(&rw->writelock);
    sem_post(&rw->lock);
}

void rwlock_release_readlock(rwlock_t *rw) {
    sem_wait(&rw->lock);
    rw->readers--;
    if (rw->readers == 0) // last reader lets it go
        sem_post(&rw->writelock);
    sem_post(&rw->lock);
}

void rwlock_acquire_writelock(rwlock_t *rw) {
    sem_wait(&rw->writelock);
}

void rwlock_release_writelock(rwlock_t *rw) {
    sem_post(&rw->writelock);
}
```
- Because the writer has to wait for all the readers to finish reading, it is possible to starve the writing threads.
- Use with caution, because they are more complex, they might not speed up performance compared to just using a simple and fast locking primitives.

## Dining Philosophers

- Assume there are five “philosophers” sitting around a table. Between each pair of philosophers is a single fork (and thus, five total). The philosophers each have times where they think, and don’t need any forks, and times where they eat. In order to eat, a philosopher needs two forks, both the one on their left and the one on their right. The contention for these forks, and the synchronization problems that ensue, are what makes this a problem we study in concurrent programming.

```c
// Basic philosopher loop
while (1) {
    think();
    get_forks(p);
    eat();
    put_forks(p);
}

// Some helper code
int left(int p) { return p; } // returns the left fork
int right(int p) { return (p + 1) % 5; } // returns the right fork

// semaphores, one for each fork:
sem_t forks[5]
```
- Assume
- Broken solution - deadlock, if every philosopher grabs a fork to their left, before any of them can grab a fork to their right, they will all wait for each other and thus starve:
```c
void get_forks(int p) {
    sem_wait(&forks[left(p)]);
    sem_wait(&forks[right(p)]);
}

void put_forks(int p) {
    sem_post(&forks[left(p)]);
    sem_post(&forks[right(p)]);
}
```
- Simplest way to break the deadlock is to change how forks are acquired by at least one of the philosophers, because the last philosopher tries to grab right before left, there is no situation where each philosopher grabs one fork and is stuck waiting for another; the cycle of waiting is broken:
```c
void get_forks(int p) {
    if (p == 4) {
        sem_wait(&forks[right(p)]);
        sem_wait(&forks[left(p)]);
    } else {
        sem_wait(&forks[left(p)]);
        sem_wait(&forks[right(p)]);
    }
}
```

## Thread throttling

- How can a programmer prevent “too many” threads from doing something at once and bogging the system down?
- Decide upon a threshold for “too many”, and then use a semaphore to limit the number of threads concurrently executing the piece of code in question.
- By initiating the value of the semaphore to the maximum number of threads you wish to enter the critical section and then putting a `sem_wait()` and `sem_post()` around it.

## How to implement semaphores

- This implementation never allows the semaphore value to be negative and thus signal how many threads are waiting:
```c
typedef struct __Zem_t {
    int value;
    pthread_cond_t cond;
    pthread_mutex_t lock;
} Zem_t;

// only one thread can call this
void Zem_init(Zem_t *s, int value) {
    s->value = value;
    Cond_init(&s->cond);
    Mutex_init(&s->lock);
}

void Zem_wait(Zem_t *s) {
    Mutex_lock(&s->lock);
    while (s->value <= 0)
        Cond_wait(&s->cond, &s->lock);
    s->value--;
    Mutex_unlock(&s->lock);
}

void Zem_post(Zem_t *s) {
    Mutex_lock(&s->lock);
    s->value++;
    Cond_signal(&s->cond);
    Mutex_unlock(&s->lock);
}
```
- Building conditional variables out of semaphores is much harder.