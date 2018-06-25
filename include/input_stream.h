#pragma once

#include <stdio.h>

#include "pnode.h"

typedef struct {
	FILE *input;
} in_stream_t;

pnode_t *
stream_get_next_proc(FILE *input, pnode_t *pnode_t);
