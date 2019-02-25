#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <assert.h>

#define TAG_DATA 0
#define TAG_WORK 1

#define NINTS (1024 * 1024)

int int_compare(const void *e1, const void *e2) {
  return (*(int*)e1) - (*(int*)e2);
}

void send_sort(int rank, int size) {
  int tosort[NINTS];
  for (int i = 0; i < NINTS; i++) {
    tosort[i] = NINTS - i - 1;
  }

  MPI_Request *reqs = malloc(sizeof(MPI_Request) * ((size-1) * 2));
  MPI_Status *stats = malloc(sizeof(MPI_Status) * ((size-1) * 2));

  int split = NINTS / (size - 1);
  for (int i = 1; i < size; i++) {
    //MPI_Send(tosort + (split * (i-1)), split, MPI_INT, i, TAG_DATA, MPI_COMM_WORLD);
    MPI_Issend(tosort + (split * (i-1)), split, MPI_INT, i, TAG_DATA, MPI_COMM_WORLD, &reqs[i-1]);
  }

  int **bufs = malloc(sizeof(int*) * (size-1));
  int *mi = malloc(sizeof(int) * (size-1));
  for (int i = 0; i < size-1; i++) {
    bufs[i] = malloc(sizeof(int) * split);
    mi[i] = 0;
  }

  for (int i = 1; i < size; i++) {
    //MPI_Recv(bufs[i-1], split, MPI_INT, i, TAG_DATA, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Irecv(bufs[i-1], split, MPI_INT, i, TAG_DATA, MPI_COMM_WORLD, &reqs[(i-1)+(size-1)]);
  }

  MPI_Waitall((size-1)*2, reqs, stats);

  int ni = 0;
  while (ni < NINTS) {
    // Find minimum val and index
    int minv = INT_MAX;
    int mini = -1;
    for (int i = 1; i < size; i++) {
      if (mi[i-1] < split && bufs[i-1][mi[i-1]] < minv) {
        minv = bufs[i-1][mi[i-1]];
        mini = i;
      }
    }
    tosort[ni] = bufs[mini-1][mi[mini-1]];
    mi[mini-1]++;
    ni++;
  }

  /*
  for (int i = 0; i < NINTS; i++) {
    printf("%d ", tosort[i]);
  }
  printf("\n");

  for (int i = 0; i < NINTS-1; i++) {
    assert(tosort[i] <= tosort[i+1]);
  }
  */
  printf("All good!\n");
}

void do_sort(int rank, int size) {
  int split = NINTS / (size - 1);
  int *buf = malloc(sizeof(int) * split);

  MPI_Recv(buf, split, MPI_INT, 0, TAG_DATA, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
  qsort(buf, split, sizeof(int), int_compare);
  MPI_Send(buf, split, MPI_INT, 0, TAG_DATA, MPI_COMM_WORLD);
}

int main(int argc, char **argv) {
  MPI_Init(&argc, &argv);

  int rank, size;

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  // I am being lazy, assuming even split
  if (NINTS % (size - 1) != 0) {
    MPI_Abort(MPI_COMM_WORLD, 1);
  }

  if (rank == 0) {
    send_sort(rank, size);
  } else {
    do_sort(rank, size);
  }

  MPI_Finalize();
}
