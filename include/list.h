#pragma once

#include "types.h"

typedef struct {
	real_t d[PSC_ARGLIST_LENGHT];
} list_t;

char *
list_real_to_string(list_t items);
