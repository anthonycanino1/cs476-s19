#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>
#include <math.h>

double sample() {
  double s = -1.0+2.0*((double)rand())/RAND_MAX;
  return s;
}

int main(int argc, char **argv) {
  MPI_Init(&argc, &argv);

  int rank, nproc;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nproc);

  if (argc < 2) {
    fprintf(stderr, "%s usage: [NUM-SAMPLES]\n", argv[0]);
    exit(1);
  }

  srand(time(NULL));

  long long nsamples = 0;
  long long incircle = 0;

  if (rank == 0) {
    nsamples = strtoll(argv[1],NULL,10);
  } 

  MPI_Bcast(&nsamples, 1, MPI_LONG_LONG_INT, 0, MPI_COMM_WORLD);

  for (long long i = 0; i < nsamples; i++) {
    double x = sample();
    double y = sample();
    if (x * x + y * y < 1.0) incircle++;
  }

  long long total_incircle = 0;

  MPI_Reduce(&incircle, &total_incircle, 1, MPI_LONG_LONG_INT, MPI_SUM, 0, MPI_COMM_WORLD);

  if (rank == 0) {
    double pi = 4 * ((double) total_incircle / (double) (nsamples * nproc));

    double error = (M_PI - pi) / M_PI;

    printf("%lld samples gives pi: %.16g error:%.16f\n", nsamples, pi, error);
  }
  
  MPI_Finalize();
  return 0;
}
