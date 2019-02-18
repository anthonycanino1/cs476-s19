#include <mpi.h>
#include <stdio.h>
#include <string.h>

#define TAG_DATA 0

int main(int argc, char **argv) {
  MPI_Init(&argc, &argv);

  int rank, size;

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  char buf[100];

  if (rank == 0) {
    char *m = "ping";
    for (int i = 1; i < size; i++) {
      MPI_Send(m, strlen(m), MPI_CHAR, i, TAG_DATA, MPI_COMM_WORLD);
    }
    int count = 0;
    while (count < size - 1) {
      MPI_Status status;
      MPI_Recv(buf, 100, MPI_CHAR, MPI_ANY_SOURCE, TAG_DATA, MPI_COMM_WORLD, &status);
      printf("Got pong from %d\n", status.MPI_SOURCE);
      count++;
    }
  } else {
    MPI_Recv(buf, 100, MPI_CHAR, 0, TAG_DATA, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    char *m = "pong";
    MPI_Send(m, strlen(m), MPI_CHAR, 0, TAG_DATA, MPI_COMM_WORLD);
  }
  
  MPI_Finalize();
  return 0;
}
