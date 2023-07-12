# Interlude: Thread API

What interfaces should the OS present for thread creation and control?
How should these interfaces be designed to enable ease of use as well as
utility?

## Thread Creation

```c
#include <pthread.h>
int pthread_create(pthread_t *thread,
                   const pthread_attr_t *attr,
                   void *(*start_routine)(void*),
                   void *arg);
```

- `pthread_t` - the structure to interact with the thread
- `pthread_attr_t` - the structure to specify thread attributes (pass NULL for defaults)
- `start_routine` - the pointer to a function that the thread should execute
- `arg` - the argument passed to the start_routine function

## Thread Completion

```c
int pthread_join(pthread_t thread, void **value_ptr);
```

- `thread` - the thread that you want to wait for
- `value_ptr` - the pointer to the return value of the function the thread is executing. It's `void**`
  because the function should reaturn `void*` which is a value of any type.

## Locks

```c
int pthread_mutex_lock(pthread_mutex_t *mutex);
int pthread_mutex_unlock(pthread_mutex_t *mutex);
int pthread_mutex_trylock(pthread_mutex_t *mutex);
int pthread_mutex_timedlock(pthread_mutex_t *mutex, struct timespec *abs_timeout);
```

- If no other thread holds the lock when pthread mutex lock() is called, the thread will acquire the lock and enter the critical section. If another thread does indeed hold the lock, the thread trying to grab the lock will not return from the call until it has acquired the lock (implying that the thread holding the lock has released it via the unlock call).
- All locks must be initialized in order to guarantee that they have the correct values to being with.
- `PTHREAD MUTEX INITIALIZER` for initializing locks to default value.
- `pthread_mutex_init(&lock, NULL)` for initializing the locks at runtime. Second argument are optional set of attributes.
- When done with the lock, you should call `pthread mutex destroy()`.
- Always check the return codes to see if the locks
- `pthread_mutex_trylock()` return failure if the lock is already held.
- `pthread_mutex_timedlock()` returns after a timeout or after acquiring the lock

## Condition Variables

- Condition variables are useful when some kind of signaling must take place between threads, if one thread is waiting for another to do something before it can continue.

```c
int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);
int pthread_cond_signal(pthread_cond_t *cond);
```

- To use a condition variable, one has to in addition have a lock that is associated with this condition. When calling either of the above routines, this lock should be held.
- `pthread_cond_wait()` puts the calling thread to sleep and wait for another thread to signal it.

```c
// Thread 0 waits for signal on ready
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
Pthread_mutex_lock(&lock);
while (ready == 0)
    Pthread_cond_wait(&cond, &lock);
Pthread_mutex_unlock(&lock);
```

```c
// Thread 1 signals thread 0
Pthread_mutex_lock(&lock);
ready = 1;
Pthread_cond_signal(&cond);
Pthread_mutex_unlock(&lock);
```

- When signaling (as well as when modifying the global variable ready), we always make sure to have the lock held. This ensures that we don’t accidentally introduce a race condition into our code.
- You might notice that the wait call takes a lock as its second parameter, whereas the signal call only takes a condition. The reason for this difference is that the wait call, in addition to putting the calling thread to sleep, releases the lock when putting said caller to sleep. Imagine if it did not: how could the other thread acquire the lock and signal it to wake up? However, before returning after being woken, the pthread cond wait() re-acquires the lock, thus ensuring that any time the waiting thread is running between the lock acquire at the beginning of the wait sequence, and the lock release at the end, it holds the lock.
- The while loop is there instead of if, because some implementations of pthread sometimes wake a sleeping thread without the condition being changed, therefore, the waiting thread will think that the condition changed, while in fact it haven't

## Tips

- Keep it simple. Above all else, any code to lock or signal between threads should be as simple as possible. Tricky thread interactions lead to bugs.
- Minimize thread interactions. Try to keep the number of ways in which threads interact to a minimum. Each interaction should be carefully thought out and constructed with tried and true approaches (many of which we will learn about in the coming chapters).
- Initialize locks and condition variables. Failure to do so will lead to code that sometimes works and sometimes fails in very strange ways.
- Check your return codes. Of course, in any C and UNIX programming you do, you should be checking each and every return code, and it’s true here as well. Failure to do so will lead to bizarre and hard to understand behavior, making you likely to (a) scream, (b) pull some of your hair out, or (c) both.
- Be careful with how you pass arguments to, and return values from, threads. In particular, any time you are passing a reference to a variable allocated on the stack, you are probably doing something wrong.
- Each thread has its own stack. As related to the point above, please remember that each thread has its own stack. Thus, if you have a locally-allocated variable inside of some function a thread is executing, it is essentially private to that thread; no other thread can (easily) access it. To share data between threads, the values must be in the heap or otherwise some locale that is globally accessible
- Always use condition variables to signal between threads. While it is often tempting to use a simple flag, don’t do it.
- Use the manual pages. On Linux, in particular, the pthread man pages are highly informative and discuss much of the nuances presented here, often in even more detail. Read them carefully!
