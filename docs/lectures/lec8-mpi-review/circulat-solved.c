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
#include <mpi.h>
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

// Struct makes it easier to keep track of what each
// rank will work on (useful for edge case where work not
// evenly divisible by number of workers
typedef struct {
  int range[2];
} work;

// search will search for all circular primes up to a given number n
void search(int n, int rank, int size) {
  int chunk = n / (size-1);

  if (rank == 0) {
    printf("Chunk size: %d\n", chunk);
    // Master only processing

    // This is a very hacky way to keep track of found circular prime numbers without
    // dealing with a resizing array or linked list. 
    int *found = malloc(sizeof(int) * n);
    for (int i = 0; i < n; i++) { found[i] = 0; }
    // Calculate the work each rank should perform
    work *w = malloc(sizeof(work) * size-1);
    for (int r = 1; r < size; r++) {
      w[r-1].range[0] = (r-1) * chunk;
      w[r-1].range[1] = (r + 1 < size) ? (r * chunk) : n;
      printf("%d getting range %d %d\n", r, w[r-1].range[0], w[r-1].range[1]);
      MPI_Send(w[r-1].range, 2, MPI_INT, r, 0, MPI_COMM_WORLD);
    }
    // Now go through and receive everything
    for (int r = 1; r < size; r++) {
      int len = w[r-1].range[1] - w[r-1].range[0];
      printf("Receiving %d items from %d\n", len, r);
      MPI_Recv(found+w[r-1].range[0], len, MPI_INT, r, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    int count = 0;
    for (int i = 0; i < n; i++) {
      if (found[i]) { 
        printf("%d ", i);
        count++;
      }
    }
    printf("\n");
    printf("Found %d circular primes\n", count);
    free(found);
    free(w);

  } else {
    // Worker only processing
    int range[2];
    MPI_Recv(range, 2, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    int mychunk = range[1] - range[0];

    int *isprime = malloc(sizeof(int) * mychunk);
    for (int i = 0; i < mychunk; i++) { isprime[i] = 0; }

    for (int i = range[0]; i < range[1]; i++) {
      if (i < 2) continue;
      if (iscircular(i)) {
        isprime[i-range[0]] = 1;
      }
    }

    MPI_Send(isprime, mychunk, MPI_INT, 0, 0, MPI_COMM_WORLD);

    free(isprime);
  } 
}

int main(int argc, char **argv) {
  MPI_Init(&argc, &argv);

  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  if (argc < 2) {
    printf("%s usage: [NUMBER]\n", argv[0]);
    exit(1);
  }

  int n = strtol(argv[1], NULL, 10);
  search(n, rank, size);

  MPI_Finalize();
  return 0;
}
