#include <stdio.h>
#include <string.h>
#include <assert.h>

#define PQ_SIZE 8
#define PQ_TYPE unsigned long

#define PQ_IMPLEMENTATION
#include "pq.h"

static int tests_passed = 0;
static int tests_failed = 0;

#define CHECK(cond)                                  \
    do {                                             \
        if (!(cond)) {                               \
            printf("\n  CHECK failed: %s\n", #cond); \
            return __LINE__;                         \
        }                                            \
    } while (0)

#define RUN_TEST(fn)                                              \
    do {                                                          \
        printf("%-38s : ", #fn);                                  \
        int fail = fn();                                          \
        printf("%s", !fail ? "PASS\n" : "");                      \
        if (!fail)                                                \
            tests_passed++;                                       \
        else {                                                    \
            tests_failed++;                                       \
            printf("  FAIL at %d\n", fail);                       \
        }                                                         \
    } while (0)


static int test_init(void)
{
    struct pq q;

    pq_init(&q, 123);

    CHECK(pq_size(&q) == 8);
    CHECK(pq_peek(&q) == 0);
    CHECK(pq_peekdata(&q) == NULL);

    return 0;
}


static int test_registration(void)
{
    struct pq q;

    pq_init(&q, 0);

    for (int i = 1; i <= 8; i++) CHECK(pq_reg(&q) == i);

    CHECK(pq_reg(&q) == 0);

    return 0;
}


static int test_single_entry(void)
{
    struct pq q;
    pq_init(&q, 0);

    int id = pq_reg(&q);

    CHECK(id == 1);
    CHECK(pq_peek(&q) == 0);
    CHECK(pq_peekdata(&q) == NULL);

    pq_enq(&q, id, 100);

    CHECK(pq_peek(&q) == id);
    CHECK(pq_peekdata(&q) == pq_data(&q, id));
    CHECK(pq_peekdata(&q)->pri == 100);

    struct pqi *e = pq_next(&q);

    CHECK(e == pq_data(&q, id));
    CHECK(e->pri == 100);
    CHECK(pq_peek(&q) == 0);
    CHECK(pq_peekdata(&q) == NULL);

    return 0;
}


static int test_priority_order(void)
{
    struct pq q;
    pq_init(&q, 0);

    int a = pq_reg(&q);
    int b = pq_reg(&q);
    int c = pq_reg(&q);
    int d = pq_reg(&q);

    pq_enq(&q, a, 40);
    pq_enq(&q, b, 10);
    pq_enq(&q, c, 30);
    pq_enq(&q, d, 20);

    CHECK(pq_peek(&q) == b);
    CHECK(pq_next(&q) == pq_data(&q, b));

    CHECK(pq_peek(&q) == d);
    CHECK(pq_next(&q) == pq_data(&q, d));

    CHECK(pq_peek(&q) == c);
    CHECK(pq_next(&q) == pq_data(&q, c));

    CHECK(pq_peek(&q) == a);
    CHECK(pq_next(&q) == pq_data(&q, a));

    CHECK(pq_peek(&q) == 0);

    return 0;
}


static int test_requeue(void)
{
    struct pq q;
    pq_init(&q, 0);

    int a = pq_reg(&q);
    int b = pq_reg(&q);
    int c = pq_reg(&q);

    pq_enq(&q, a, 100);
    pq_enq(&q, b, 200);
    pq_enq(&q, c, 300);

    CHECK(pq_peek(&q) == a);

    pq_enq(&q, c, 50); // move to front

    CHECK(pq_peek(&q) == c);
    CHECK(pq_next(&q) == pq_data(&q, c));

    CHECK(pq_peek(&q) == a);
    CHECK(pq_next(&q) == pq_data(&q, a));

    CHECK(pq_peek(&q) == b);
    CHECK(pq_next(&q) == pq_data(&q, b));

    CHECK(pq_peek(&q) == 0);

    return 0;
}


static int test_requeue_to_middle(void)
{
    struct pq q;
    pq_init(&q, 0);

    int a = pq_reg(&q);
    int b = pq_reg(&q);
    int c = pq_reg(&q);

    pq_enq(&q, a, 10);
    pq_enq(&q, b, 20);
    pq_enq(&q, c, 30);

    pq_enq(&q, c, 15); // a c b

    CHECK(pq_next(&q) == pq_data(&q, a));
    CHECK(pq_next(&q) == pq_data(&q, c));
    CHECK(pq_next(&q) == pq_data(&q, b));
    CHECK(pq_next(&q) == NULL);

    return 0;
}


static int test_delete(void)
{
    struct pq q;
    pq_init(&q, 0);

    int a = pq_reg(&q);
    int b = pq_reg(&q);
    int c = pq_reg(&q);

    pq_enq(&q, a, 10);
    pq_enq(&q, b, 20);
    pq_enq(&q, c, 30);

    pq_del(&q, b);

    CHECK(pq_peek(&q) == a);
    CHECK(pq_next(&q) == pq_data(&q, a));

    CHECK(pq_peek(&q) == c);
    CHECK(pq_next(&q) == pq_data(&q, c));

    CHECK(pq_peek(&q) == 0);

    pq_del(&q, b); // already deleted

    CHECK(pq_peek(&q) == 0);

    return 0;
}


static int test_delete_first(void)
{
    struct pq q;
    pq_init(&q, 0);

    int a = pq_reg(&q);
    int b = pq_reg(&q);
    int c = pq_reg(&q);

    pq_enq(&q, a, 10);
    pq_enq(&q, b, 20);
    pq_enq(&q, c, 30);

    pq_del(&q, a);

    CHECK(pq_peek(&q) == b);
    CHECK(pq_next(&q) == pq_data(&q, b));
    CHECK(pq_next(&q) == pq_data(&q, c));
    CHECK(pq_next(&q) == NULL);

    return 0;
}


static int test_update_existing_entry(void)
{
    struct pq q;
    pq_init(&q, 0);

    int a = pq_reg(&q);
    int b = pq_reg(&q);

    pq_enq(&q, a, 100);
    pq_enq(&q, b, 200);

    pq_enq(&q, a, 300);

    CHECK(pq_peek(&q) == b);
    CHECK(pq_next(&q) == pq_data(&q, b));

    CHECK(pq_peek(&q) == a);
    CHECK(pq_next(&q) == pq_data(&q, a));

    CHECK(pq_next(&q) == NULL);

    return 0;
}


static int test_next_then_requeue(void)
{
    struct pq q;
    pq_init(&q, 0);

    int a = pq_reg(&q);
    int b = pq_reg(&q);

    pq_enq(&q, a, 100);
    pq_enq(&q, b, 200);

    struct pqi *e = pq_next(&q);

    CHECK(e == pq_data(&q, a));
    CHECK(pq_peek(&q) == b);

    pq_enq(&q, a, 300);

    CHECK(pq_next(&q) == pq_data(&q, b));
    CHECK(pq_next(&q) == pq_data(&q, a));
    CHECK(pq_next(&q) == NULL);

    return 0;
}


static int test_iter(void)
{
    struct pq q;
    pq_init(&q, 0);

    int a = pq_reg(&q);
    int b = pq_reg(&q);
    int c = pq_reg(&q);

    pq_enq(&q, a, 30);
    pq_enq(&q, b, 10);
    pq_enq(&q, c, 20);

    int expected[] = { b, c, a };
    int n = 0;

    #define CHECK_ITER(id, elm)                 \
        do {                                    \
            CHECK(n < 3);                       \
            CHECK((id) == expected[n]);         \
            CHECK((elm) == pq_data(&q, id));    \
            n++;                                \
        } while (0)

    pq_iter(&q, CHECK_ITER);

    #undef CHECK_ITER

    CHECK(n == 3);

    return 0;
}


int main(void)
{
    RUN_TEST(test_init);
    RUN_TEST(test_registration);
    RUN_TEST(test_single_entry);
    RUN_TEST(test_priority_order);
    RUN_TEST(test_requeue);
    RUN_TEST(test_requeue_to_middle);
    RUN_TEST(test_delete);
    RUN_TEST(test_delete_first);
    RUN_TEST(test_update_existing_entry);
    RUN_TEST(test_next_then_requeue);
    RUN_TEST(test_iter);

    printf("\nTOTAL: %d passed, %d failed\n", tests_passed, tests_failed);

    return tests_failed == 0 ? 0 : 1;
}
