#include <mpi.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define TAG_DATA 0
#define TAG_COMMAND 1

void do_fib(int rank, int size, int target) {
  int fib[3];

  int sendrank = (rank + 1) % size;
  int recvrank = (rank != 0) ? rank - 1 : size - 1;

  while(1) {
    MPI_Status stat;
    MPI_Recv(fib,3,MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &stat);
    if (stat.MPI_TAG == TAG_COMMAND) {
      return;
    }

    int n = fib[0] + fib[1];
    fib[0] = fib[1];
    fib[1] = n;
    fib[2]++;

    printf("Rank %d calculated fib(%d) = %d\n", rank, fib[2], fib[1]);

    if (fib[2] >= target) {
      for (int i = 0; i < size; i++) {
        if (rank == i) continue;
        MPI_Send(fib,3,MPI_INT, i, TAG_COMMAND, MPI_COMM_WORLD);
      }
      return;
    } else {
      MPI_Send(fib,3,MPI_INT, sendrank, TAG_DATA, MPI_COMM_WORLD);
    }
  }
}

int main(int argc, char **argv) {
  MPI_Init(&argc, &argv);

  int rank, size;

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  int target = strtol(argv[1], NULL, 10);

  if (rank == 0) {
    int fib[3];
    fib[0] = 0;
    fib[1] = 1;
    fib[2] = 1;
    MPI_Send(fib,3,MPI_INT, 1, TAG_DATA, MPI_COMM_WORLD);
  }

  do_fib(rank, size, target);

  MPI_Finalize();
  return 0;
}
