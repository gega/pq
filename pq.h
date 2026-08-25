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

#include <stddef.h>

// instance
#ifndef PQ_TYPE
#define PQ_TYPE unsigned long
#endif

// instance
#ifndef PQ_SIZE
#error PQ_SIZE is mandatory parameter, it could be 0 for FAM or positive for array storage
#endif

// instance
#ifndef PQ_STRUCT
#undef PQ_STRUCT
#endif

// instance
#ifndef PQ_NAME_PREFIX
#define PQ_NAME_PREFIX
#endif

// system global
#ifndef PQ_HAVE_STRING_H
#define PQ_HAVE_STRING_H 1
#endif

#define PQ_CAT2(a, b) a##b
#define PQ_CAT(a, b)  PQ_CAT2(a, b)
#define PQ_NAME(name) PQ_CAT(PQ_NAME_PREFIX, name)

typedef struct
{
  int pr;       // previous item, or id if unqueued
  int nx;       // next item, or id if unqueued
  PQ_TYPE pri;  // ordering key 0->9
  #ifdef PQ_STRUCT
  PQ_STRUCT ud;
  #endif
} PQ_NAME(pq_item_t);

struct PQ_NAME(pq);
typedef struct PQ_NAME(pq) PQ_NAME(pq_t);
struct PQ_NAME(pq)
{
  int fre;      // free item
  int cnt;
  #if PQ_SIZE > 0
  PQ_NAME(pq_item_t) a[PQ_SIZE+1];
  #else
  PQ_NAME(pq_item_t) a[];
  #endif
};

#if PQ_SIZE == 0
_Static_assert(sizeof(PQ_NAME(pq_t)) == offsetof(PQ_NAME(pq_t), a), "pq runtime header must end where its flexible array begins");
#endif

#undef PQ_SIZE
#undef PQ_STRUCT
#undef PQ_TYPE
#undef PQ_NAME_PREFIX
#undef PQ_CAT2
#undef PQ_CAT
#undef PQ_NAME

#ifdef PQ_IMPLEMENTATION

// implementation defined only once
#ifndef PQ_H
#define PQ_H

#if PQ_HAVE_STRING_H == 0
#define pq_zero_(m,s) do { size_t z_=(s); unsigned char *p_=(unsigned char *)(m);  while(z_--!=0u) *p_++=0u; } while(0)
#else
#include <string.h>
#define pq_zero_(m,s) memset((m), 0, (s))
#endif

#define pq_next(p) ({ const int r=((p)->a)[0].nx; \
          ((p)->a)[0].nx=((p)->a)[r].nx; \
          ((p)->a)[((p)->a)[0].nx].pr=0; \
          ((p)->a)[r].nx=((p)->a)[r].pr=r; \
          (r==0?NULL:&((p)->a)[r]); })

#define pq_peek(p) ( ((p)->a)[0].nx )

#define pq_peekdata(p) ( 0!=((p)->a)[0].nx ? &(((p)->a)[((p)->a)[0].nx]) : NULL )

#define pq_data(p,x) ( &(((p)->a)[x]) )

#define pq_enq(p,i,r) do { \
          int V_; \
          const int I_=(i); \
          const __typeof__((p)->a[0].pri) P_=(r); \
          ((p)->a)[I_].pri=P_; \
          ((p)->a)[((p)->a)[I_].pr].nx=((p)->a)[I_].nx; \
          ((p)->a)[((p)->a)[I_].nx].pr=((p)->a)[I_].pr; \
          for(V_=((p)->a)[0].nx;V_>0&&((p)->a)[V_].pri<P_;V_=((p)->a)[V_].nx); \
          ((p)->a)[((p)->a)[V_].pr].nx=I_; \
          ((p)->a)[I_].nx=V_; \
          ((p)->a)[I_].pr=((p)->a)[V_].pr; \
          ((p)->a)[V_].pr=I_; \
          } while(0)

#define pq_del(p,i) do { \
          const int I_=(i); \
          ((p)->a)[((p)->a)[I_].pr].nx=((p)->a)[I_].nx; \
          ((p)->a)[((p)->a)[I_].nx].pr=((p)->a)[I_].pr; \
          ((p)->a)[I_].nx=I_; \
          ((p)->a)[I_].pr=I_; \
          } while(0)

#define pq_init_static(p,f) do { \
          (p)->cnt=( sizeof((p)->a) / sizeof((p)->a[0]) - 1 ); \
          (p)->fre=1; \
          pq_zero_(((p)->a),sizeof(*(p)->a)*(pq_size(p)+1)); \
          ((p)->a)[0].pri=(f); \
          for(int i=0;i<pq_size(p)+1;i++) ((p)->a)[i].nx=((p)->a)[i].pr=i; } while(0)

#define pq_init_fam(p,c,f) do { \
          (p)->cnt=(c); \
          (p)->fre=1; \
          pq_zero_(((p)->a),sizeof(*(p)->a)*(pq_size(p)+1)); \
          ((p)->a)[0].pri=(f); \
          for(int i=0;i<pq_size(p)+1;i++) ((p)->a)[i].nx=((p)->a)[i].pr=i; } while(0)

#define pq_size(p) ((p)->cnt)

#define pq_reg(p) ((p)->fre<(pq_size(p)+1)?((p)->fre)++:0)

#define pq_iter(p,f) do { for(int i=((p)->a)[0].nx;i>0;i=((p)->a)[i].nx) f(i,&((p)->a)[i]); } while(0)

#endif
#endif
