/*
    MIT License

    Copyright (c) 2019 Gergely Gati -- gati.gergely@gmail.com

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
 */

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
