# pq
priority queue for fixed number of entities

- malloc-free
- single header
- gcc only
- not thread safe

## Configuration

__PQ_SIZE__: maximum number of registered entities for the queue (default: 50)

__PQ_TYPE__: type of the priority field of the queue (default: unsigned long)

__PQ_PRIFLOOR__: minimum value of the priorities (default: 0)

__PQ_STRUCT__: additional struct data stored for items (default: undefined)

## Api

### pq_init

void pq_init(struct pq *p);

Initialize the given pq struct which should be provided by the caller.

### pq_req

int pq_reg(struct pq *p);

Registering a new client for the queue. Returns the client ID (>0) which should be used when calling
__pq_enq()__. Returns 0 when no more space available.

### pq_enq

void pq_enq(struct pq *p, int id, PQ_TYPE pri);

Replacing the priority value of the corresponding client ID or add it if it was missing

### pq_next

struct pqi *pq_next(struct pq *p);

Consuming the item with the lowest priority of the queue. Returns
NULL when the queue is empty. Returns __struct pqi *__ item pointer which
have a __ud__ field for the __PQ_STRUCT__ when it is defined.
The returned item has the lowest priority and it's also removed from the queue.

### pq_peek

int pq_peek(struct pq *p);

Returns the client ID of the lowest priority item or 0 when the queue is
empty. The item will not be removed from the queue.

### pq_data

struct pqi *pq_data(struct pq *p, int id);

Returns the __struct pqi *__ pointer of the given client ID.

### pq_peekdata

Returns the __struct pqi *__ pointer of the lowest priority item or NULL when the queue is
empty. The item will not be removed from the queue.

### pq_iter

void pq_iter(struct pq *p, void (*f)(int id, struct pqi *elm));

Iterating over the queue and calling the __f()__ function on every item with the client ID and a pointer to the actual
item.

## Design

The queue is designed for single threaded finite state machine based event driven systems where multiple but fixed number of
entities (modes/functions/etc) can add time based events to a common queue which can be used as a single schedule table for 
waking up at the closest event.
