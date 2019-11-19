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

## Api

### pq_init
void pq_init(struct pq *p);

Initialize the given pq struct which should be provided by the caller

### pq_req

int pq_reg(struct pq *p);

Registering a new client for the queue. Returns the client ID (>0) which should be used when calling pq_enq() or 0 when no more space available

### pq_enq

void pq_enq(struct pq *p, int id, PQ_TYPE pri);

Replacing the priority value of the corresponding client ID or add it if it was missing

### pq_next

int pq_next(struct pq *p);

Processing the item with the lowest priority value of the queue. The returned client ID has the lowest priority field and it's also removed from the queue. Returns 0 when the queue is empty.

### pq_iter

void pq_iter(struct pq *p, void (*f)(int idx, struct pqi *elm));

Iterating over the queue and calling the f() function on every item with the idx (client ID) and a pointer to the actual item

## Design

The queue is designed for single threaded finite state machine based event driven systems where multiple but fixed number of entities (modes/functions/etc) can add time based events to a common queue which can be used as a single schedule table for waking up at the closest event.
