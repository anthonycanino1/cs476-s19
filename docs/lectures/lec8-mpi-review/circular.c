/*********************************************************************
* Exercise for Lecture 7: MPI Introduction
*
* Convert the search to a parallel search using MPI
*
* Compile with 'mpicc --std=c11 circular.c -o circlur
* Run with 'mpirun -np 4 ./circular NUM
*
*
*********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <cilk/cilk.h>
#include <string.h>

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

// 1. Using MPI, distribute the workload of numbers to search for 
//    cirular primality. How you split this work is up to you (I suggest
//    splitting the search, not the circular test)
// 2. Return the result of a search of a number (or group of numbers) back
//    to the master. You may assume a fixed size for the recieve.
// 3. When all workers have finished searching, print all found
//    numbers and cleanly exit

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

  // This is a very hacky way to keep track of found circular prime numbers without
  // dealing with a resizing array or linked list. 
  int *found = malloc(sizeof(int) * n);
  for (int i = 0; i < n; i++) {
    found[i] = 0;
  }

  while (p < n) {
    if (iscircular(p)) {
      found[p] = 1;
    }
    p++;
  }

  int count = 0;
  for (int i = 0; i < n; i++) {
    if (found[i]) { 
      printf("%d ", i);
      count++;
    }
  }
  printf("\n");
  free(found);

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
