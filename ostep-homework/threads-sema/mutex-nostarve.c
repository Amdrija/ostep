#include "common_threads.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//
// Here, you have to write (almost) ALL the code. Oh no!
// How can you show that a thread does not starve
// when attempting to acquire this mutex you build?
//

typedef struct __ns_mutex_t {
  int ticket;
  int turn;
  sem_t lock;
  sem_t cond;
} ns_mutex_t;

void ns_mutex_init(ns_mutex_t *m) {
  Sem_init(&m->lock, 1);
  Sem_init(&m->cond, 0);
  m->ticket = 0;
  m->turn = 1;
}

void ns_mutex_acquire(ns_mutex_t *m) {
  Sem_wait(&m->lock);
  int my_ticket = ++m->ticket;
  while (m->turn != my_ticket) {
    Sem_post(&m->lock);
    Sem_wait(&m->cond);
    Sem_wait(&m->lock);
    // If it's not my turn. Wake someone else
    if (m->turn != my_ticket) {
      Sem_post(&m->cond);
    }
  }
  Sem_post(&m->lock);
}

void ns_mutex_release(ns_mutex_t *m) {
  Sem_wait(&m->lock);
  m->turn++;
  Sem_post(&m->lock);
  Sem_post(&m->cond);
}

ns_mutex_t m;
int counter = 0;
typedef struct __tinfo_t {
  int thread_id;
  int loop_count;
} tinfo_t;

void *worker(void *arg) {
  tinfo_t *t = (tinfo_t *)arg;
  for (int i = 0; i < t->loop_count; i++) {
    printf("Thread %d acquiring lock.\n", t->thread_id);
    ns_mutex_acquire(&m);
    counter++;
    printf("Thread %d incrementing counter: %d\n", t->thread_id, counter);
    ns_mutex_release(&m);
    printf("Thread %d released lock.\n", t->thread_id);
  }
}

int main(int argc, char *argv[]) {
  assert(argc == 3);
  int num_threads = atoi(argv[1]);
  int loop_count = atoi(argv[2]);
  assert(num_threads > 0);

  pthread_t p[num_threads];
  tinfo_t t[num_threads];
  ns_mutex_init(&m);
  int i;
  for (i = 0; i < num_threads; i++) {
    t[i].thread_id = i;
    t[i].loop_count = loop_count;
    Pthread_create(&p[i], NULL, worker, &t[i]);
  }

  for (i = 0; i < num_threads; i++)
    Pthread_join(p[i], NULL);

  printf("Count: %d\n", counter);
  return 0;
}
