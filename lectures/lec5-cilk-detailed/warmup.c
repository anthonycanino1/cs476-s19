/*********************************************************************
* Warmup Exercise for Lecture 5: More Cilk
*
* Convert the search to a parallel search using cilk constructs.
*
* Compile with 'gcc --std=c11 -fcilkplus warmup.c -o warmup
*
*
*********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <cilk/cilk.h>
#include <string.h>

// For your convience, here is the completed int vector code
typedef struct {
  int *arr;
  int size;
  int cap;
} ivec;

ivec* new_ivec(int cap) {
  ivec* iv  = malloc(sizeof(ivec));
  iv->arr   = malloc(sizeof(int) * cap);
  iv->cap   = cap;
  iv->size  = 0;
	return iv;
}

void free_ivec(ivec *iv) {
  free(iv->arr);
  free(iv);
}

void insert(ivec *iv, int val) {
  if (iv->size == iv->cap) {
    iv->cap *= 2;
    int *narr = malloc(sizeof(int) * iv->cap);
    memcpy(narr, iv->arr, sizeof(int) * iv->size);
    free(iv->arr);
    iv->arr = narr;
  }
  iv->arr[iv->size] = val;
  iv->size++;
}

// The next few functions, mpow, digits, rotate, are helper functions
// for rotating the numbers
int mpow(int b, int x) {
  int p = 1;
  while (x > 0) {
    p *= b;
    x--;
  }
  return p;
}

int digits(int number) {
  int d = 0;
  while (number != 0) {
    d++;
    number /= 10;
  }
  return d;
}

int rotate(int number, int d) {
  int l = number % 10;
  return  (number / 10) + (l * (mpow(10,d-1)));
}

// Brute force test on primality
int isprime(int n) {
  for (int i = 2; i < n; i++) {
    if (n % i == 0) return 0;
  }
  return 1;
}

// You will need to modify iscircular and search
// 1. First, make the modifications you see fit to turn the search into
//    a parallel one using cilk constructs
// 2. After that, make another modification to save the found circular prime
//    numbers in a shared int vector. You will likely need to protect access
//    to the vector with a lock.
// 3. Cleanly print the entire vector of found circular primes.

// iscircular will test a single number for circular primality
int iscircular(int n) {
  int nn = n;
  int d = digits(n);
  do {
    if (!isprime(nn)) return 0;
    nn = rotate(nn, d);
  } while (nn != n);

  return 1;
}

// search will search for all circular primes up to a given number n
int search(int n) {
  int p = 2;
  int count = 0;
  while (p < n) {
    if (iscircular(p)) {
      printf("%d ", p);
      count++;
    }
    p++;
  }
  printf("\n");
  return count;
}

int main(int argc, char **argv) {
  if (argc < 2) {
    printf("%s usage: [NUMBER]\n", argv[0]);
    exit(1);
  }

  int n = strtol(argv[1], NULL, 10);
  int count = search(n);

  printf("Found %d circular primes under %d\n", count, n);

  return 0;
}
