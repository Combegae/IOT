#ifndef COOKIE_H_
#define COOKIE_H_

#define MAX_CHARS 128

#include <stdint.h>

typedef uint8_t bool_t;

typedef struct cookie {
    uint32_t uartno;
    char *c;
    char line[MAX_CHARS];
    uint32_t head;
    uint32_t tail;
    bool_t processing;
    void (*event)(void *);
} cookie_t;

#endif /* define cookie_h_*/