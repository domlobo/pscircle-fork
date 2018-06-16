#pragma once

#include "types.h"

#include "node.h"

typedef uint_fast16_t mem_t;

typedef struct process_t process_t;

struct process_t {
	node_t node;
	pid_t pid;
	pid_t ppid;
	real_t pcpu;
	mem_t mem;
	char *comm;
	process_t *stub;
	nnodes_t nstubs;
};
