#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <string.h>
#include <cilk/cilk.h>
#include <pthread.h>

int* mergesort(int *arr, int s, int e) { 
  if (e == s) {
    int *v = malloc(sizeof(int));
    *v = arr[s];
    return v;
  }
  int m = (s + e) / 2;

  int *left = cilk_spawn mergesort(arr,s,m);
  int *right = cilk_spawn mergesort(arr,m+1,e);
  cilk_sync;

  size_t l = 0;
  size_t r = 0;
  size_t i = 0;
  size_t lsize = (m-s)+1;
  size_t rsize = (e-m);

  int *narr = malloc(sizeof(int) * (lsize+rsize));

  while (l < lsize && r < rsize) {
    if (left[l] <= right[r]) {
      narr[i] = left[l];
      l++;
    } else {
      narr[i] = right[r];
      r++;
    }
    i++;
  }
  while (l < lsize) {
    narr[i] = left[l];
    l++;
    i++;
  }
  while (r < rsize) {
    narr[i] = right[r];
    r++;
    i++;
  }

  free(left);
  free(right);
  
  return narr;
}

/*
void mergesort(int *arr, int *narr, int s, int e) {
  if (e <= s) return;
  int m = (s + e) / 2;

  cilk_spawn mergesort(arr,narr,s,m);
  cilk_spawn mergesort(arr,narr,m+1,e);
  cilk_sync;

  size_t l = s;
  size_t r = m+1;
  size_t i = 0;

  while (l <= m && r <= e) {
    if (arr[l] <= arr[r]) {
      narr[i] = arr[l];
      l++;
    } else {
      narr[i] = arr[r];
      r++;
    }
    i++;
  }
  while (l <= m) {
    narr[i] = arr[l];
    l++;
    i++;
  }
  while (r <= e) {
    narr[i] = arr[r];
    r++;
    i++;
  }
  for (size_t i = 0; i <= (e-s); i++) {
    arr[s+i] = narr[i];
  }
  
}
*/

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "%s usage: [ENTRIES]\n", argv[0]);
    return 1;
  }

  size_t size = strtoll(argv[1], NULL, 10);

  int *arr = malloc(sizeof(int) * size);
  for (size_t i = 0; i < size; i++) {
    arr[i] = size - i - 1;
  }

  int *narr = mergesort(arr, 0, size-1);
  //mergesort(arr, narr, 0, size-1);

  for (size_t i = 0; i < size; i++) {
    printf("%d ", narr[i]);
  }
  printf("\n");


  for (size_t i = 0; i < size-1; i++) {
    assert(narr[i] <= narr[i+1]);
  }

  free(narr);
  free(arr);

}
