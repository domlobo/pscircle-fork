#pragma once

#include <stddef.h>

#include "types.h"

#define REALS(...) (reals_t) { \
	.data = {__VA_ARGS__}, \
	.size = sizeof((real_t[]){__VA_ARGS__})/sizeof(real_t) \
}

typedef struct {
	real_t data[PSC_REALS_COUNT];
	size_t size;
} reals_t;

void
reals_add(reals_t *reals, real_t x);

real_t
reals_get(reals_t *reals, size_t i);

char *
reals_to_string(reals_t reals);

