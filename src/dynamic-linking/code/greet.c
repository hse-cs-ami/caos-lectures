#include "greeting.h"
#include <stdio.h>


void greet(struct greeting *g) {
    printf("I greet you %d times, o %s!\n", g->count, g->name);
}
