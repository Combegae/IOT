#ifndef RING_H_
#define RING_H_
#include <stdint.h>

typedef uint8_t bool_t;


bool_t ring_empty();

bool_t ring_full();

void ring_put(uint8_t bits);

uint8_t ring_get();

#endif /* RING_H_ */
