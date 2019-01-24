#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>
#include <time.h>

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
  for (int k = 0; k < niters; k++) {
    for (int i = 0; i < iv1->size; i++) {
      iv2->arr[i] = iv1->arr[i] + iv2->arr[i];
    }
  }
} 

typedef struct {
  int id;
  int (*f)(int, int);
} pstate;

int add(int x, int y) { return x + y; }

typedef struct {
  int *wait;
  int nthreads;
} barrier;

barrier b;

void barrier_init(barrier *b, int nthreads) {
  b->wait = malloc(sizeof(int) * nthreads);
  b->nthreads = nthreads;
  for (int i = 0; i < nthreads; i++) {
    b->wait[i] = 0;
  }
}

void wait2(barrier *b, int id) {
  while(b->wait[id] == 1);
}

void hit(barrier *b, int id) {
  b->wait[id] = 1;
  if (id == 0) {
    int check;
    do {
      check = 1;
      for (int i = 0; i < b->nthreads; i++) {
        if (b->wait[i] == 0) check = 0;
      }
    } while (check == 0);
    for (int i = 0; i < nthreads; i++) {
      b->wait[i] = 0;
    }
  }
}

void* do_parallel_process(void *ptr) {

  int chunksize = iv1->size / nthreads;

  pstate *p = (pstate*) ptr;
  int id = p->id;

  for (int i = 0; i < niters; i++) {
    wait2(&b, id);

    fprintf(stderr, "Thread %d starting iteration %d\n", id, i);
    fflush(stderr);

    // wait
    int s = id * chunksize;
    int e = (id < nthreads-1) ? (chunksize * (id+1)) : iv1->size;

    for (int j = s; j < e; j++) {
      iv2->arr[j] = p->f(iv1->arr[j],iv2->arr[j]);
    }

    hit(&b, id);
  }

  return NULL;
}

void parallel_process() {

  pthread_t *threads = malloc(sizeof(pthread_t) * nthreads);
  pstate *states = malloc(sizeof(pstate) * nthreads);
  barrier_init(&b, nthreads);

  for (int i = 0; i < nthreads; i++) {
    states[i].id = i;
    states[i].f = add;
    pthread_create(&threads[i], NULL, do_parallel_process, &states[i]);
  }

  for (int i = 0; i < nthreads; i++) {
    pthread_join(threads[i], NULL);
  }

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
    assert(iv1->arr[i] * (niters+1) == iv2->arr[i]);
  }

  fprintf(stdout, "All good!\n");

  free_ivec(iv1);
  free_ivec(iv2);

  return 0;
}


