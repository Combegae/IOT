#define MAX_EVENTS 128
#include "queue.h"



volatile uint32_t tail_event = 0;
volatile event_t buffer_event[MAX_EVENTS];
volatile uint32_t head_event = 0;

bool_t queue_empty() {
    return (head_event==tail_event);
}

bool_t queue_full() {
    int next = (head_event + 1) % MAX_EVENTS;
    return (next == tail_event);
}

void queue_put(event_t event) {
    uint32_t next = (head_event + 1) % MAX_EVENTS;
    buffer_event[head_event] = event;
    head_event = next;
}
event_t queue_get() {
    event_t event;
    uint32_t next = (tail_event + 1) % MAX_EVENTS;
    event = buffer_event[tail_event];
    tail_event = next;
    return event;
}