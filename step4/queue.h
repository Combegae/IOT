#ifndef QUEUE_H
#define QUEUE_H
#include <stdint.h>

typedef uint8_t bool_t;

typedef struct event {
    void* cookie;
    void (*fct)(void* cookie);
} event_t;

bool_t queue_empty();

bool_t queue_full();

void queue_put(event_t event);

event_t queue_get();

#endif /* RING_H_ */
