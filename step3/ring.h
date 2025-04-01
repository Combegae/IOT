#ifndef RING_H_
#define RING_H_

typedef uint8_t bool_t;

#include <stdint.h>

bool_t ring_empty();

bool_t ring_full();

void ring_put(uint8_t bits);

uint8_t ring_get();

#endif /* RING_H_ */
