#pragma once

#include "types.h"

typedef struct {
	uint_fast8_t memory_unit;
	pid_t root_pid;
	nnodes_t max_children;
} cfg_t;

extern cfg_t config;
