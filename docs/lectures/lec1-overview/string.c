#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// strlen
int mstrlen(char *s) {
  int l = 0;
  while (s[l] != 0) {
    l++;
  }
  return l;
}

void mstrcpy(char *dst, char *src) {
  int len = mstrlen(src)+1;
  for (int i = 0; i < len; i++) {
    dst[i] = src[i];
  }
}

int main(int argc, char **argv) {
  char *s1 = "this is a string";
  char *s2 = "this is a longer string";
  char *s3 = "easy";

  printf("%s len:%d\n", s1, mstrlen(s1));
  printf("%s len:%d\n", s2, mstrlen(s2));
  printf("%s len:%d\n", s3, mstrlen(s3));

  char *s4 = malloc(strlen(s2)+1);
  mstrcpy(s4,s2);
  printf("s2:%s s4:%s\n", s2, s4);

  free(s4);

  return 0;
}
