# pq

Priority queue for a fixed number of entities.

Main features:

* malloc-free
* single header
* GCC only
* not thread safe
* designed for event-driven systems
* embedded-array or flexible-array-member storage
* multiple independently configured queue types can coexist in one translation unit

The design is based on event-driven systems where the main loop waits in a low-power state until the next scheduled event. Event handlers may then insert or reschedule their next requests in the queue.

In many such systems there is only a limited number of event sources. `pq` takes advantage of this by allocating one queue item for each registered entity rather than allocating a new item for every queued event.

Each entity first calls `pq_reg()` to obtain an ID. Preferably registration happens during boot or application initialization, and registration failures are asserted so insufficient queue capacity is detected early.

Once registered, an entity can use its ID with `pq_enq()` any number of times. Enqueuing an already queued ID replaces its priority and repositions it in the queue. Therefore a registered entity can have at most one pending item in the queue.

## Configuration

Configuration options have either **instance scope** or **global scope**.

Instance-scoped options configure one generated queue type. They are undefined by the header after that type has been instantiated, allowing the header to be included again with different settings in the same translation unit.

Global options configure the common implementation shared by all queue types.

### Instance configuration

**`PQ_SIZE`**

Mandatory.

Defines the storage layout of the queue.

If greater than zero, the generated queue structure contains an embedded array capable of storing `PQ_SIZE` registered entities.

If zero, the generated queue uses a C flexible array member (FAM), and storage for the queue items must be supplied by the caller.

An additional internal item at index zero is reserved in both layouts.

**`PQ_TYPE`**

Type of the priority field.

Default:

```c
unsigned long
```

**`PQ_STRUCT`**

Optional additional data stored in every queue item.

When defined, every item contains:

```c
PQ_STRUCT ud;
```

Default: undefined.

**`PQ_NAME_PREFIX`**

Prefix applied to the generated queue and item type names.

Default: empty.

For example:

```c
#define PQ_NAME_PREFIX timer_
#define PQ_TYPE uint32_t
#define PQ_SIZE 16
#include "pq.h"
```

generates:

```c
timer_pq_t
timer_pq_item_t
```

Without a prefix, the generated types are:

```c
pq_t
pq_item_t
```

The operational API (`pq_enq`, `pq_del`, `pq_next`, etc.) consists of generic macros and is not namespaced. The macros obtain the required type information from the queue object passed to them.

### Global configuration

**`PQ_HAVE_STRING_H`**

Controls whether queue initialization uses `memset()` from `<string.h>`.

Default:

```c
1
```

If zero, `pq` uses its own byte-wise zeroing implementation.

This is a global configuration decision. The common implementation is generated only once, so this option should not be changed between queue instances.

## Multiple queue types

The header may be included multiple times to instantiate independently configured queue types in the same translation unit.

For example:

```c
#define PQ_NAME_PREFIX schedule_
#define PQ_TYPE uint32_t
#define PQ_SIZE 32
#include "pq.h"

#define PQ_NAME_PREFIX timeout_
#define PQ_TYPE uint64_t
#define PQ_SIZE 0
#include "pq.h"
```

This generates:

```c
schedule_pq_t
schedule_pq_item_t

timeout_pq_t
timeout_pq_item_t
```

The instance configuration macros are consumed and undefined after each inclusion, so another instance can immediately be configured.

Both types use the same generic queue operations:

```c
schedule_pq_t schedule;

pq_init_static(&schedule, 0);

pq_enq(&schedule, schedule_id, 100);
pq_del(&schedule, schedule_id);
```

The common implementation is emitted only once when `PQ_IMPLEMENTATION` is defined.

## Storage and initialization

Two storage layouts are supported.

### Embedded array

When `PQ_SIZE` is greater than zero, storage is part of the generated queue structure:

```c
#define PQ_SIZE 16
#include "pq.h"

pq_t queue;
```

Initialize it with:

```c
pq_init_static(&queue, priority_floor);
```

For example:

```c
pq_init_static(&queue, 0);
```

The capacity is derived automatically from the embedded array and stored in the queue's `cnt` field.

`pq_init_static()` requires an embedded-array queue. Using it with a flexible-array-member queue results in a compile-time error because the size of a flexible array member cannot be evaluated with `sizeof`.

### Flexible array member

When `PQ_SIZE` is zero, the generated structure ends in a flexible array member:

```c
pq_item_t a[];
```

The caller must provide enough storage for the queue structure and `cnt + 1` items.

This does **not** imply heap allocation. The storage may be reserved statically, supplied by an allocator, or placed at an address or in a region determined at link time.

Initialize such storage with:

```c
pq_init_fam(queue, cnt, priority_floor);
```

where `cnt` is the number of entities for which storage has been provided.

The extra item is reserved internally as item zero.

`pq_init_fam()` may also operate on an embedded-array queue when an explicit logical capacity is desired. In that case the caller is responsible for ensuring that `cnt` does not exceed the physical capacity of the embedded array.

### Priority floor

The priority floor is an initialization parameter rather than a compile-time configuration option.

It initializes the priority of the internal sentinel item:

```c
pq_init_static(&queue, floor);
```

or:

```c
pq_init_fam(queue, cnt, floor);
```

This allows different queue objects to use different priority floors without requiring different queue types.

## API

The API is implemented as macros. The declarations below describe their effective interfaces.

### pq_size

```c
int pq_size(pq_t *p);
```

Returns the registered capacity of the queue.

### pq_init_static

```c
void pq_init_static(pq_t *p, PQ_TYPE floor);
```

Initializes a queue using its embedded array capacity.

The queue must have been instantiated with `PQ_SIZE > 0`.

### pq_init_fam

```c
void pq_init_fam(pq_t *p, int cnt, PQ_TYPE floor);
```

Initializes a queue with an explicitly supplied capacity.

This is normally used with a queue instantiated with `PQ_SIZE == 0` and externally supplied storage.

`cnt` specifies the number of entities that may be registered.

### pq_reg

```c
int pq_reg(pq_t *p);
```

Registers an entity and allocates its queue slot.

Returns a client ID greater than zero. The ID should subsequently be used with `pq_enq()`, `pq_del()`, and `pq_data()`.

Returns zero when no registration slots remain.

Registration is permanent for the lifetime of the initialized queue. Removing an item with `pq_del()` does not release its registered ID.

### pq_enq

```c
void pq_enq(pq_t *p, int id, priority pri);
```

Enqueues the registered client ID with priority `pri`.

If the ID is already queued, its previous position is removed and it is reinserted according to the new priority. `pq_enq()` therefore also acts as a rescheduling operation.

Lower priority values are nearer the head of the queue.

The priority expression is evaluated once and converted to the actual priority type of the queue instance.

### pq_del

```c
void pq_del(pq_t *p, int id);
```

Removes the client ID from the queue.

Calling `pq_del()` for an item that is already removed is permitted.

The ID remains registered and may subsequently be queued again.

### pq_next

```c
pq_item_t *pq_next(pq_t *p);
```

Removes and returns the item with the lowest priority.

Returns `NULL` when the queue is empty.

If `PQ_STRUCT` was configured for the queue type, the returned item contains its additional data in the `ud` member.

### pq_peek

```c
int pq_peek(pq_t *p);
```

Returns the client ID of the lowest-priority queued item, or zero when the queue is empty.

The item is not removed.

### pq_peekdata

```c
pq_item_t *pq_peekdata(pq_t *p);
```

Returns a pointer to the lowest-priority queued item, or `NULL` when the queue is empty.

The item is not removed.

### pq_data

```c
pq_item_t *pq_data(pq_t *p, int id);
```

Returns a pointer to the item associated with the given registered client ID.

### pq_iter

```c
void pq_iter(pq_t *p, void (*f)(int id, pq_item_t *item));
```

Iterates over the currently queued items in priority order and calls `f()` for each item with its client ID and item pointer.

## Design

The queue is intended primarily for single-threaded finite-state-machine and event-driven systems where a fixed number of entities schedule future events into a common queue.

The head of the queue represents the next event to process. This makes the queue suitable as a schedule table in tickless or low-power systems: the application can inspect the next item, sleep until it becomes due, process it, and allow the responsible entity to reschedule itself.

The implementation uses a doubly linked list represented by indices into the item array. Each registered entity owns one stable item slot. This avoids dynamic allocation and turns queue exhaustion into a registration-time problem rather than an enqueue-time failure.

Insertion is linear in the number of currently queued items. This deliberately trades insertion complexity for small and predictable per-item storage and compact code.

The queue object stores its logical capacity in `cnt`, regardless of storage layout. This adds one `int` per queue object but allows all normal queue operations to be independent of how the item storage was allocated.

The implementation currently relies on GCC extensions, including statement expressions and `__typeof__`.
