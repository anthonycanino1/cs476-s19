#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <string.h>
#include <cilk/cilk.h>

int* gen_ivec(size_t size) {
  int* iv = malloc(sizeof(int) * size);
  for (size_t i = 0; i < size; i++) {
    iv[i] = rand() % 100;
  }
  return iv;
}

int* dup_ivec(int *iv, size_t size) {
  int* iv2 = malloc(sizeof(int) * size);
  for (size_t i = 0; i < size; i++) {
    iv2[i] = iv[i];
  }
  return iv2;
}

void serial_process(int *iv1, int* iv2, size_t size, size_t iters) {
  for (size_t n = 0; n < iters; n++) {
    for (size_t i = 0; i < size; i++) {
      iv2[i] = iv1[i] + iv2[i];
    }
  }
}

void do_process(int *iv1, int *iv2, size_t start, size_t end) {
  for (size_t i = start; i < end; i++) {
    iv2[i] = iv1[i] + iv2[i];
  } 
}

void parallel_process(int *iv1, int* iv2, size_t size, size_t iters, size_t chunks) {
  size_t chunksize = size / chunks;
  for (int n = 0; n < iters; n++) {
    cilk_for (int i = 0; i < chunks; i++) {
      size_t start = i * chunksize;
      size_t end = (i < chunks) ? (i + 1) * chunksize : end;
      do_process(iv1, iv2, start, end);
    }

    /*
    int sum = 0;
    for (size_t i = 0; i < size; i++) {
      sum += iv2[i];
    }
    printf("sum:%d\n", sum);
    */
  }
}

int main(int argc, char **argv) {
  if (argc < 4) {
    fprintf(stderr, "%s usage: [ENTRIES] [NUMBER LOOPS] [CHUNKS]\n", argv[0]);
    return 1;
  }

  size_t size = strtoll(argv[1], NULL, 10);
  size_t iters = strtoll(argv[2], NULL, 10);
  size_t chunks = strtoll(argv[3], NULL, 10);
  
  srand(time(NULL));

  int *iv1 = gen_ivec(size);
  int *iv2 = dup_ivec(iv1, size);

  fprintf(stdout, "Vector loaded!\n");


  if (chunks <= 1) {
    serial_process(iv1, iv2, size, iters);
  } else {
    parallel_process(iv1, iv2, size, iters, chunks);
  }

  fprintf(stdout, "Vector processed!\n");

  for (size_t i = 0; i < size; i++) {
    assert(iv1[i] * (iters+1)  == iv2[i] );
  }

  fprintf(stdout, "All good!\n");

  free(iv1);
  free(iv2);

  return 0;
}


