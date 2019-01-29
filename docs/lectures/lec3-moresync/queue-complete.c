/*********************************************************************
* Concurrent Queue implementation in C
*********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

typedef struct {
  int *q;
  int front, end;
  int cap;
  pthread_mutex_t qlock;

  pthread_mutex_t mutate;

  pthread_cond_t has_space_cond;
  pthread_cond_t has_items_cond;

} queue;

typedef enum { ENONE, EFULL, EEMPTY } qerror;

void init_queue(queue *q, int cap) {
  q->q = malloc(sizeof(int) * cap);
  q->cap = cap+1;
  q->front = 0;
  q->end = 0;

  pthread_mutex_init(&q->mutate, NULL);

  pthread_cond_init(&q->has_space_cond, NULL);
  pthread_cond_init(&q->has_items_cond, NULL);
}

void release_queue(queue *q) {
  free(q->q);
  pthread_mutex_destroy(&q->mutate);
  pthread_cond_destroy(&q->has_space_cond);
  pthread_cond_destroy(&q->has_items_cond);
} 

qerror pop(queue *q, int *v) {
  pthread_mutex_lock(&q->mutate);
  while (q->front == q->end) {
    pthread_cond_wait(&q->has_items_cond, &q->mutate);
  }

  *v = q->q[q->front];
  q->front = (q->front + 1) % q->cap;

  pthread_cond_signal(&q->has_space_cond);
  pthread_mutex_unlock(&q->mutate);

  return ENONE;
}

qerror push(queue *q, int v) {
  pthread_mutex_lock(&q->mutate);
  while ((q->end + 1) % q->cap == q->front) {
    pthread_cond_wait(&q->has_space_cond, &q->mutate);
  }

  q->q[q->end] = v;
  q->end = (q->end + 1) % q->cap;

  pthread_cond_signal(&q->has_items_cond);
  pthread_mutex_unlock(&q->mutate);
  return ENONE;
}

#define QCAP 8

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

  pthread_t t1, t2, t3, t4;

  pthread_create(&t1, NULL, do_thread1, (void*) &q);
  sleep(2);
  //pthread_create(&t2, NULL, do_thread1, (void*) &q);

  pthread_create(&t3, NULL, do_thread2, (void*) &q);
  //pthread_create(&t4, NULL, do_thread2, (void*) &q);


  pthread_join(t1, NULL);
  //pthread_join(t2, NULL);
  pthread_join(t3, NULL);
  //pthread_join(t4, NULL);
  
  release_queue(&q);

  printf("All good!\n");

  return 0;
}


