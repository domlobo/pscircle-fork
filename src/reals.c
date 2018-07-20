#include <stddef.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

#include "reals.h"

#define ITEM_LENGHT 16

#define CHECK(x) do { \
	if (x) break; \
	fprintf(stderr, "%s:%d error: %s\n", \
			__FILE__, __LINE__, strerror(errno)); \
	exit(EXIT_FAILURE); \
} while (0)

void
reals_add(reals_t *reals, real_t x)
{
	if(reals->size >= PSC_REALS_COUNT)
		return;

	reals->data[reals->size++] = x;
}

real_t
reals_get(reals_t *reals, size_t i)
{
	assert(i < reals->size);
	return reals->data[i];
}

char *
reals_to_string(reals_t reals)
{
	assert(reals.size > 0);

	char *buf = (char *) calloc(reals.size * ITEM_LENGHT, sizeof(char));
	CHECK(buf);

	char *end = buf;
	bool comma = false;

	for (size_t i = 0; i < reals.size; ++i) {
		if (comma)
			*(end++) = ',';

		end += snprintf(end, ITEM_LENGHT, "%.0f", reals_get(&reals, i));

		comma = true;
	}

	return buf;
}
