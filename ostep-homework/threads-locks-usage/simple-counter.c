#include "common_threads.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

typedef struct __counter {
  int count;
  pthread_mutex_t lock;
} counter;

void init(counter *c) {
  c->count = 0;
  Pthread_mutex_init(&c->lock, NULL);
}

void add(counter *c) {
  Pthread_mutex_lock(&c->lock);
  c->count++;
  Pthread_mutex_unlock(&c->lock);
}

int get(counter *c) {
  Pthread_mutex_lock(&c->lock);
  int return_value = c->count;
  Pthread_mutex_unlock(&c->lock);

  return return_value;
}

counter c;

void *worker(void *arg) {
  int count = (long long)arg;
  for (int i = 0; i < count; i++) {
    add(&c);
  }

  return NULL;
}

int main(int argc, char *argv[]) {
  init(&c);
  int n = atoi(argv[1]);
  int count = atoi(argv[2]);
  struct timeval start, end;
  pthread_t *threads = (pthread_t *)malloc(sizeof(pthread_t) * n);
  gettimeofday(&start, NULL);
  for (int i = 0; i < n; i++) {
    Pthread_create(&threads[i], NULL, worker, (void *)(long long)count);
  }

  for (int i = 0; i < n; i++) {
    Pthread_join(threads[i], NULL);
  }

  gettimeofday(&end, NULL);
  printf("Count: %d, time: %lf\n", get(&c),
         (end.tv_sec - start.tv_sec) +
             (end.tv_usec - start.tv_usec) / 1000000.0);
  free(threads);
  return 0;
}
