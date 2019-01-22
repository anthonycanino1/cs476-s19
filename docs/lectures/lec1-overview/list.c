#include <stdio.h>
#include <stdlib.h>

typedef struct inode2 {
  int v;
  struct inode2 *n;
} inode2;

typedef struct ilist2 {
  inode2 *hd;
  inode2 *tl;
} ilist2;


ilist2* new_ilist2() {
  ilist2 *il = malloc(sizeof(ilist2));
  il->hd = NULL;
  il->tl = NULL;
  return il;
}

void insert_ilist2(ilist2 *il, int v) {
  inode2 *n = malloc(sizeof(inode2));
  n->v = v;
  n->n = NULL;
  if (il->hd == NULL) { il->hd = n; }
  if (il->tl == NULL) { 
    il->tl = n; 
  } else { 
    il->tl->n = n;
    il->tl = n;
  }
}

void free_ilist2(ilist2 *il) {
  inode2 *n = il->hd;
  while (n != NULL) {
    inode2 *p = n;
    n = n->n;
    free(p);
  }
  free(il);
}

int main(int argc, char **argv) {
  ilist2 *il = new_ilist2();
  for (int i = 0; i < 10; i++) {
    insert_ilist2(il, i);
  }

  inode2 *n = il->hd;
  while(n != NULL) {
    printf("%d ", n->v);
    n = n->n;
  }
  printf("\n");

  free_ilist2(il);

  return 0;
}


