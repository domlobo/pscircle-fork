#pragma once

#include "types.h"

typedef struct {
	size_t max_name_lenght;
	uint_fast8_t memory_unit;
	size_t initial_process_count;
	pid_t root_pid;
	nnodes_t max_children;
	size_t toplist_max_rows;
} cfg_t;

extern cfg_t config;
