#include "common_threads.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define NUM_CPUS 16
typedef struct __counter {
  int count;
  pthread_mutex_t lock;
  int local_counts[NUM_CPUS];
  pthread_mutex_t local_locks[NUM_CPUS];
  int threshold;
} counter;

void init(counter *c, int threshold) {
  c->count = 0;
  Pthread_mutex_init(&c->lock, NULL);
  for (int i = 0; i < NUM_CPUS; i++) {
    c->local_counts[i] = 0;
    Pthread_mutex_init(&c->local_locks[i], NULL);
  }

  c->threshold = threshold;
}

void add(counter *c, int thread_id) {
  int i = thread_id % NUM_CPUS;
  Pthread_mutex_lock(&c->local_locks[i]);
  c->local_counts[i]++;
  if (c->local_counts[i] >= c->threshold) {
    Pthread_mutex_lock(&c->lock);
    c->count += c->local_counts[i];
    Pthread_mutex_unlock(&c->lock);
    c->local_counts[i] = 0;
  }
  Pthread_mutex_unlock(&c->local_locks[i]);
}

int get(counter *c) {
  Pthread_mutex_lock(&c->lock);
  int return_value = c->count;
  Pthread_mutex_unlock(&c->lock);

  return return_value;
}

counter c;
typedef struct _worker_args {
  int count;
  int thread_id;
} worker_args;

void *worker(void *arg) {
  worker_args *args = (worker_args *)arg;
  for (int i = 0; i < args->count; i++) {
    add(&c, args->thread_id);
  }

  return NULL;
}

int main(int argc, char *argv[]) {
  int n = atoi(argv[1]);
  int count = atoi(argv[2]);
  int threshold = atoi(argv[3]);
  init(&c, threshold);
  struct timeval start, end;
  pthread_t *threads = (pthread_t *)malloc(sizeof(pthread_t) * n);
  worker_args *thread_args = (worker_args *)malloc(sizeof(worker_args) * n);

  gettimeofday(&start, NULL);
  for (int i = 0; i < n; i++) {
    thread_args[i].count = count;
    thread_args[i].thread_id = i;
    Pthread_create(&threads[i], NULL, worker, (void *)&thread_args[i]);
  }

  for (int i = 0; i < n; i++) {
    Pthread_join(threads[i], NULL);
  }
  gettimeofday(&end, NULL);

  printf("Count: %d, time: %lf\n", get(&c),
         (end.tv_sec - start.tv_sec) +
             (end.tv_usec - start.tv_usec) / 1000000.0);
  free(threads);
  free(thread_args);
  return 0;
}
