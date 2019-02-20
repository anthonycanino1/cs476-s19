#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

double sample() {
  double s = -1.0+2.0*((double)rand())/RAND_MAX;
  return s;
}

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "%s usage: [NUM-SAMPLES]\n", argv[0]);
    exit(1);
  }

  srand(time(NULL));

  long long nsamples = strtoll(argv[1],NULL,10);

  long long incircle = 0;

  for (long long i = 0; i < nsamples; i++) {
    double x = sample();
    double y = sample();
    if (x * x + y * y < 1.0) incircle++;
  }

  double pi = 4 * ((double) incircle / (double) nsamples);
  double error = (M_PI - pi);
  printf("%lld samples gives pi: %.16g error:%.16f\n", nsamples, pi, error);
  return 0;
}
