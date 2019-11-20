#include <stdio.h>
#include <stdlib.h>

struct dt
{
  int id;
  void *pnt;
};

#define PQ_STRUCT struct dt

#include "pq.h"


void pr(int i, struct pqi *p)
{
  printf("PR %d: %ld (%d:%d)\n",i,p->pri,p->nx,p->pr);
}

int main(void)
{
  struct pq P;
  int aa,bb,cc;
  struct dt *u;
 
  pq_init(&P);
  aa=pq_reg(&P);
  u=pq_data(&P,aa);
  u->id=65;
  bb=pq_reg(&P);
  pq_data(&P,bb)->id=66;
  cc=pq_reg(&P);
  pq_data(&P,cc)->id=67;
  printf("reg: %d,%d,%d\n",aa,bb,cc);

  pq_enq(&P,aa,10);
  pq_enq(&P,bb,3);
  pq_enq(&P,bb,22);
  pq_enq(&P,bb,1);
  pq_enq(&P,cc,30);

  pq_iter(&P,pr);

  printf("topid=%d\n",pq_peek(&P));
  u=pq_next(&P);
  printf("next=%d\n",(u!=NULL?u->id:-1));
  u=pq_next(&P);
  printf("next=%d\n",(u!=NULL?u->id:-1));
  u=pq_next(&P);
  printf("next=%d\n",(u!=NULL?u->id:-1));
  u=pq_next(&P);
  printf("next=%d\n",(u!=NULL?u->id:-1));
  u=pq_next(&P);
  printf("next=%d\n",(u!=NULL?u->id:-1));

  return(0);
}
