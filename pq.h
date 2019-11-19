#ifndef PQ_H
#define PQ_H

#ifndef PQ_SIZE
#define PQ_SIZE 50
#endif

#ifndef PQ_TYPE
#define PQ_TYPE unsigned long
#endif


struct pqi
{
  int pr;
  int nx;
  PQ_TYPE pri;
};

struct pq
{
  int fre;
  int top;
  struct pqi a[PQ_SIZE];
};

void pq_init(struct pq *p);
int pq_reg(struct pq *p);
void pq_enq(struct pq *p, int id, PQ_TYPE pri);
int pq_next(struct pq *p);


#define pq_next(p) ({ int r=(p)->top; if(r>=0) { (p)->top=(p)->a[r].nx; (p)->a[(p)->top].pr=-1; (p)->a[r].nx=(p)->a[r].pr=-1; } r; })
#define pq_enq(p,i,r) do { \
          int V_,R_; \
          PQ_TYPE P_=(PQ_TYPE)(r); \
          int I_=(i); \
          (p)->a[I_].pri=P_; \
          if((p)->top==I_) (p)->top=(p)->a[I_].nx; \
          if((p)->a[I_].pr>=0) (p)->a[(p)->a[I_].pr].nx=(p)->a[I_].nx; \
          if((p)->a[I_].nx>=0) (p)->a[(p)->a[I_].nx].pr=(p)->a[I_].pr; \
          for(R_=-1,V_=(p)->top;V_>=0&&(p)->a[V_].pri<P_;V_=(p)->a[V_].nx) R_=V_; \
          (p)->a[I_].nx=V_; \
          if(V_==(p)->top) { if(V_>=0) { (p)->a[V_].pr=I_; } (p)->top=I_; (p)->a[I_].pr=-1; } \
          else { \
            if(R_>=0) { (p)->a[R_].nx=I_; } \
            (p)->a[I_].pr=R_; \
            } \
          } while(0)
#define pq_init(p) do { (p)->fre=0; (p)->top=-1; for(int i=0;i<PQ_SIZE;i++) (p)->a[i].nx=(p)->a[i].pr=-1; } while(0);
#define pq_reg(p) ((p)->fre<PQ_SIZE?((p)->fre)++:-1)
#define pq_iter(p,f) do { for(int i=(p)->top;i>=0;i=(p)->a[i].nx) f(i,(p)->a[i].pri); } while(0)

#endif
