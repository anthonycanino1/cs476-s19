#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>
#include <time.h>
#include <semaphore.h>

#include "warmup.h"

#define MB_1 (1024 * 1024)

ivec* gen_ivec(int nentries) {
  ivec *iv = new_ivec(nentries);
  for (int i = 0; i < nentries; i++) {
    insert(iv, rand() % 100);
  }
  return iv;
}

ivec* dup_ivec(ivec *iv) {
  ivec *iv2 = new_ivec(iv->cap);
  memcpy(iv2->arr, iv->arr, sizeof(int) * iv->size);
  iv2->size = iv->size;
  return iv2;
}

// Global state
ivec *iv1, *iv2;
int niters = 0;
int nthreads = 0;

void serial_process() {
} 

typedef struct {
  sem_t *sems;
  int nthds;

  pthread_mutex_t waitonlock;
  int waiton;
} barrier;

void init_barrier(barrier *b, int n) {
  b->sems = malloc(sizeof(sem_t) * n);
  for (int i = 0; i < n; i++) {
    sem_init(&b->sems[i], 0, 1);
  }
  pthread_mutex_init(&b->waitonlock, NULL);
  b->nthds = n;
  b->waiton = 0;
}

void wait(barrier *b, int id) {
  sem_wait(&b->sems[id]);
}

void hit(barrier *b, int id) {
  pthread_mutex_lock(&b->waitonlock);
  b->waiton++;
  pthread_mutex_unlock(&b->waitonlock);

  if (id == 0) {
    int waiton;
    do {
      pthread_mutex_lock(&b->waitonlock);
      waiton = b->waiton;
      pthread_mutex_unlock(&b->waitonlock);
    } while (waiton != b->nthds);
    pthread_mutex_lock(&b->waitonlock);
    b->waiton = 0;
    pthread_mutex_unlock(&b->waitonlock);
    for (int i = 0; i < b->nthds; i++) {
      sem_post(&b->sems[i]);
    }
  }
}

barrier b;

void* do_parallel_process(void *ptr) {
  int id = *((int*)ptr);
  int size = iv1->size / nthreads;
  for (int i = 0; i < niters; i++) {
    wait(&b, id);

    int s = (size * id);
    int e = (id < nthreads-1) ? (size * (id+1)) : iv1->size;

    for (int j = s; j < e; j++) {
      iv1->arr[j] = iv1->arr[j] + iv2->arr[j];
    }

    hit(&b, id);
  }

  return NULL;
}

void parallel_process() {
  pthread_t *threads = malloc(sizeof(pthread_t) * nthreads);
  int *id = malloc(sizeof(int) * nthreads);

  init_barrier(&b, nthreads);

  for (int i = 0; i < nthreads; i++) {
    id[i] = i;
    pthread_create(&threads[i], NULL, do_parallel_process, &id[i]);
  }

  for (int i = 0; i < nthreads; i++) {
    pthread_join(threads[i], NULL);
  }

  free(threads);
}

int main(int argc, char **argv) {
  if (argc < 4) {
    fprintf(stderr, "%s usage: [ENTRIES] [NUMBER THREADS] [NUMBER LOOPS]\n", argv[0]);
    return 1;
  }

  size_t nentries = strtoll(argv[1], NULL, 10);
  nthreads = strtol(argv[2], NULL, 10);
  niters = strtoll(argv[3], NULL, 10);
  
  srand(time(NULL));

  iv1 = gen_ivec(nentries);
  iv2 = dup_ivec(iv1);

  fprintf(stdout, "Vector loaded!\n");

  if (nthreads <= 1) {
    serial_process();
  } else {
    parallel_process();
  }

  fprintf(stdout, "Vector processed!\n");

  for (int i = 0; i < iv1->size; i++) {
    assert(iv1->arr[i] == iv2->arr[i] * (niters+1) );
  }

  fprintf(stdout, "All good!\n");

  free_ivec(iv1);
  free_ivec(iv2);

  return 0;
}


