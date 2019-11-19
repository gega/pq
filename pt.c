#include <stdio.h>
#include <stdlib.h>


#include "pq.h"


void pr(int i, struct pqi *p)
{
  printf("PR %d: %ld (%d:%d)\n",i,p->pri,p->nx,p->pr);
}

int main(void)
{
  struct pq P;
  int aa,bb,cc;
 
  pq_init(&P);
  aa=pq_reg(&P);
  bb=pq_reg(&P);
  cc=pq_reg(&P);
  printf("reg: %d,%d,%d\n",aa,bb,cc);

  pq_enq(&P,aa,10);
  pq_enq(&P,bb,3);
  pq_enq(&P,bb,25);
  pq_enq(&P,bb,44);
  pq_enq(&P,cc,30);

  pq_iter(&P,pr);

  printf("next=%d\n",pq_next(&P));
  printf("next=%d\n",pq_next(&P));
  printf("next=%d\n",pq_next(&P));
  printf("next=%d\n",pq_next(&P));
  printf("next=%d\n",pq_next(&P));

  return(0);
}
