/*********************************************************************
* Concurrent Queue Exercise for Lecture 3: More Sync
* 
* Convert *pop* and *push* into blocking, thread-safe operations
*
* Compile with 'gcc --std=c11 pthread queue.c -o queue'. Run with './queue'
*********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <unistd.h>

typedef struct {
  int *q;
  int front, end;
  int cap;
} queue;

typedef enum { ENONE, EFULL, EEMPTY } qerror;

void init_queue(queue *q, int cap) {
  q->q = malloc(sizeof(int) * cap);
  q->cap = cap+1;
  q->front = 0;
  q->end = 0;
}

void release_queue(queue *q) {
  free(q->q);
} 

qerror pop(queue *q, int *v) {
  if (q->front == q->end) {
    return EEMPTY;
  }
  *v = q->q[q->front];
  q->front = (q->front + 1) % q->cap;
  return ENONE;
}

qerror push(queue *q, int v) {
  int n = (q->end + 1) % q->cap;
  if (n == q->front) {
    return EFULL;
  }
  q->q[q->end] = v;
  q->end = n;
  return ENONE;
}

#define QCAP 8

int main(int argc, char **argv) {
  queue q;
  init_queue(&q, QCAP);

  // Serial test

  int v;
  assert(pop(&q, &v) == EEMPTY);

  // Fill queue 
  for (int i = 0; i < 10; i++) {
    if (i < QCAP) {
      assert(push(&q,i) == ENONE);
    } else {
      assert(push(&q,i) == EFULL);
    }
  }

  for (int i = 0; i < 10; i++) {
    if (i < QCAP) {
      int v;
      assert(pop(&q, &v) == ENONE);
      assert(v == i);
    } else {
      assert(pop(&q, &v) == EEMPTY);
    }
  } 

  release_queue(&q);

  printf("All good!\n");

  return 0;
}

/*
 * Uncomment the following for the concurrent version 

void* do_thread1(void *arg) {
  queue *q = (queue*) arg;
  int v;
  for (int i = 0; i < 100; i++) {
    pop(q,&v);
    printf("Got %d\n", v);
  }
  return NULL;
}

void *do_thread2(void *arg) {
  queue *q = (queue*) arg;
  for (int i = 0; i < 100; i++) {
    push(q,i);
  }
  return NULL;
}


int main(int argc, char **argv) {
  queue q;
  init_queue(&q, QCAP);

  pthread_t t1, t2;

  pthread_create(&t1, NULL, do_thread1, (void*) &q);
  sleep(2);
  //pthread_create(&t2, NULL, do_thread1, (void*) &q);

  pthread_create(&t2, NULL, do_thread2, (void*) &q);
  //pthread_create(&t4, NULL, do_thread2, (void*) &q);


  pthread_join(t1, NULL);
  pthread_join(t2, NULL);
  
  release_queue(&q);

  printf("All good!\n");

  return 0;
}
*/



