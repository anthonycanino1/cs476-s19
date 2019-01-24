/*********************************************************************
* Warmup Exercise for Lecture 2: Multicore Programming in C
*
* Fill in functions *new_ivec*, *free_ivec*, and *insert* below
*
* Compile with 'gcc --std=c11 warmup.c -o warmup'. Run with `./warmup
*
*********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "warmup.h"

ivec* new_ivec(int cap) {
  ivec* iv = malloc(sizeof(ivec));
  iv->arr = malloc(sizeof(int) * cap);
  iv->size = 0;
  iv->cap = cap;
  return iv;
}

void free_ivec(ivec *iv) {
  free(iv->arr);
  free(iv);
}

void insert(ivec *iv, int val) {
  if (iv->size == iv->cap) {
    int *narr = malloc(sizeof(int) * (iv->cap * 2));
    iv->cap *= 2;
    memcpy(narr, iv->arr, (sizeof(int) * iv->size));
    free(iv->arr);
    iv->arr = narr;
  }
  iv->arr[iv->size] = val;
  iv->size++;
}

/*
int main(int argc, char **argv) {
  // Create an initial int vector with capacity of 8
  ivec *iv = new_ivec(8);

  // Insert 20 values into int vector. This will force two array resizings.
  for (int i = 0; i < 20; i++) {
    insert(iv, i);
  }

  // A simple assert will check for correctness. VERY crude form
  // of testing in C.
  assert(iv->size == 20); 
  assert(iv->cap == 32); 
  assert(iv->arr[10] == 10);

  printf("Correct!\n");

  // Clean up allocation
  free_ivec(iv);

  return 0;
}
*/
