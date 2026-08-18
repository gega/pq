/*
    MIT License

    Copyright (c) 2019-2026 Gergely Gati -- gati.gergely@gmail.com

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

#ifndef PQ_TYPE
#define PQ_TYPE unsigned long
#endif

#ifndef PQ_PRIFLOOR
#define PQ_PRIFLOOR 0
#endif

#ifndef PQ_STRUCT
#undef PQ_STRUCT
#endif

#ifndef PQ_STRUCT_DEF
#define PQ_STRUCT_DEF
struct pqi
{
  int pr;
  int nx;
  PQ_TYPE pri;
  #ifdef PQ_STRUCT
  PQ_STRUCT ud;
  #endif
};
#endif

struct pq;

int pq_size(struct pq *p);
void pq_init(struct pq *p, int cnt);
int pq_reg(struct pq *p);
void pq_enq(struct pq *p, int id, PQ_TYPE pri);
struct pqi *pq_next(struct pq *p);
void pq_del(struct pq *p, int id);

#ifdef PQ_IMPLEMENTATION

#ifndef PQ_H
#define PQ_H

#include <strings.h>

#ifndef PQ_SIZE
#error "PQ_SIZE must be defined"
#endif

struct pq
{
  int fre;
  #if PQ_SIZE > 0
  struct pqi a[PQ_SIZE+1];
  #else
  int cnt;
  struct pqi a[];
  #endif
};

#define pq_next(p) ({ int r=(p)->a[0].nx; (p)->a[0].nx=(p)->a[r].nx; (p)->a[(p)->a[0].nx].pr=0; (p)->a[r].nx=(p)->a[r].pr=r; (r==0?NULL:&(p)->a[r]); })
#define pq_peek(p) ( (p)->a[0].nx )
#define pq_peekdata(p) ( 0!=(p)->a[0].nx ? &((p)->a[(p)->a[0].nx]) : NULL )
#define pq_data(p,x) ( &((p)->a[x]) )
#define pq_enq(p,i,r) do { \
          int V_; \
          PQ_TYPE P_=(PQ_TYPE)(r); \
          int I_=(i); \
          (p)->a[I_].pri=P_; \
          (p)->a[(p)->a[I_].pr].nx=(p)->a[I_].nx; \
          (p)->a[(p)->a[I_].nx].pr=(p)->a[I_].pr; \
          for(V_=(p)->a[0].nx;V_>0&&(p)->a[V_].pri<P_;V_=(p)->a[V_].nx); \
          (p)->a[(p)->a[V_].pr].nx=I_; \
          (p)->a[I_].nx=V_; \
          (p)->a[I_].pr=(p)->a[V_].pr; \
          (p)->a[V_].pr=I_; \
          } while(0)
#define pq_del(p,i) do { \
          int I_=(i); \
          (p)->a[(p)->a[I_].pr].nx=(p)->a[I_].nx; \
          (p)->a[(p)->a[I_].nx].pr=(p)->a[I_].pr; \
          (p)->a[I_].nx=I_; \
          (p)->a[I_].pr=I_; \
          } while(0)

#if PQ_SIZE > 0
#define pq_size(p) PQ_SIZE
#define pq_init(p,c) do { (p)->fre=1;               bzero((p)->a,sizeof(struct pqi)*(pq_size(p)+1)); (p)->a[0].pri=PQ_PRIFLOOR; \
                     for(int i=0;i<pq_size(p)+1;i++) (p)->a[i].nx=(p)->a[i].pr=i; } while(0);
#else
#define pq_size(p) (p->cnt)
#define pq_init(p,c) do { (p)->fre=1; (p)->cnt=(c); bzero((p)->a,sizeof(struct pqi)*(pq_size(p)+1)); (p)->a[0].pri=PQ_PRIFLOOR; \
                     for(int i=0;i<pq_size(p)+1;i++) (p)->a[i].nx=(p)->a[i].pr=i; } while(0);
#endif

#define pq_reg(p) ((p)->fre<(pq_size(p)+1)?((p)->fre)++:0)
#define pq_iter(p,f) do { for(int i=(p)->a[0].nx;i>0;i=(p)->a[i].nx) f(i,&(p)->a[i]); } while(0)

#endif
#endif
