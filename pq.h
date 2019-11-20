#ifndef PQ_H
#define PQ_H

#include <strings.h>

#ifndef PQ_SIZE
#define PQ_SIZE 50
#endif

#ifndef PQ_TYPE
#define PQ_TYPE unsigned long
#endif

#ifndef PQ_PRIFLOOR
#define PQ_PRIFLOOR 0
#endif

#ifndef PQ_STRUCT
#undef PQ_STRUCT
#endif


struct pqi
{
  int pr;
  int nx;
  PQ_TYPE pri;
  #ifdef PQ_STRUCT
  PQ_STRUCT ud;
  #endif
};

struct pq
{
  int fre;
  struct pqi a[PQ_SIZE+1];
};


void pq_init(struct pq *p);
int pq_reg(struct pq *p);
void pq_enq(struct pq *p, int id, PQ_TYPE pri);
struct pqi *pq_next(struct pq *p);


#ifdef PQ_STRUCT
#define pq_next(p) ({ int r=(p)->a[0].nx; (p)->a[0].nx=(p)->a[r].nx; (p)->a[r].nx=(p)->a[r].pr=r; &((p)->a[r].ud); })
#else
#define pq_next(p) ({ int r=(p)->a[0].nx; (p)->a[0].nx=(p)->a[r].nx; (p)->a[r].nx=(p)->a[r].pr=r; &(p)->a[r]; })
#endif
#define pq_peek(p) ( (p)->a[0].nx )
#ifdef PQ_STRUCT
#define pq_data(p,x) ( &((p)->a[x].ud) )
#else
#define pq_data(p,x) ((void *)0)
#endif
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
#define pq_init(p) do { (p)->fre=1; (p)->a[0].pri=PQ_PRIFLOOR; bzero((p)->a,sizeof(struct pqi)*PQ_SIZE+1); for(int i=0;i<PQ_SIZE+1;i++) (p)->a[i].nx=(p)->a[i].pr=i; } while(0);
#define pq_reg(p) ((p)->fre<(PQ_SIZE+1)?((p)->fre)++:0)
#define pq_iter(p,f) do { f(0,&(p)->a[0]); for(int i=(p)->a[0].nx;i>0;i=(p)->a[i].nx) f(i,&(p)->a[i]); } while(0)

#endif
