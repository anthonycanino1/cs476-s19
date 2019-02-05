#include <stdio.h>
#include <stdlib.h>
#include <cilk/cilk.h>

int fib(int f) {
  if (f < 2) return f;

  int f2 = cilk_spawn fib(f-2);
  int f1 = cilk_spawn fib(f-1);

  cilk_sync;

  return f1+f2;
}

int main(int argc, char **argv) {
  int v1 = fib(20);
  int v2 = fib(30);

  printf("fib(%d) = %d\n", 20, v1);
  printf("fib(%d) = %d\n", 30, v2);

  return 0;
}
