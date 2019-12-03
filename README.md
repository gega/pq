# pq
priority queue for fixed number of entities

main features:

- malloc-free
- single header
- gcc only
- not thread safe
- designed for event driven systems

The design is based on the use case of an event driven system where the main loop waits for the next event in a sleep mode to reduce power consumption and precesses the next event calling event handler(s) which may reinserting event requests in the queue for the future. In the majority of the similar systems there are only a limited number of sources of such events and those are known at compile time. This queue uses this assumption and has a fixed number of entries in the queue which is implemented as an array. To prevent the sudden queue full exceptions, all entities needs to allocate an entry in this array calling the pq_reg() API. Preferably these pq_reg() calls happens near boot time or application start time and the results should be assert()ed to find out queue size issues early. After each pq_reg() successfully registered an entity in the queue, it can be used safely to insert and reinsert requests. The pq_reg() will return an array index for the caller and pq_enq()ing on this ID will overwrite the previous value and also reorder the queue when needed.

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
