#define PQ_SIZE 0
#define PQ_TYPE unsigned long

#define PQ_IMPLEMENTATION
#include "pq.h"

#include <stdio.h>
#include <stdlib.h>

int test(void)
{
    const int cnt = 5;

    struct pq *q = malloc(
        sizeof(*q) + sizeof(struct pqi) * (cnt + 1)
    );

    if (!q)
        return __LINE__;

    pq_init(q, cnt);

    if (pq_size(q) != cnt)
        return __LINE__;

    int a = pq_reg(q);
    int b = pq_reg(q);
    int c = pq_reg(q);

    if (a != 1 || b != 2 || c != 3)
        return __LINE__;

    pq_enq(q, a, 300);
    pq_enq(q, b, 100);
    pq_enq(q, c, 200);

    if (pq_next(q) != pq_data(q, b))
        return __LINE__;

    if (pq_next(q) != pq_data(q, c))
        return __LINE__;

    if (pq_next(q) != pq_data(q, a))
        return __LINE__;

    if (pq_next(q) != NULL)
        return __LINE__;

    free(q);

    return 0;
}

int main(void)
{
  int rc = test();
  if(rc==0) printf("PASS\n");
  else printf("FAIL %d\n",rc);
  return(0);
}
