#pragma once

#include "types.h"

#include "node.h"

typedef uint_fast16_t mem_t;

typedef struct pnode_t pnode_t;

struct pnode_t {
	node_t node;
	pid_t pid;
	pid_t ppid;
	real_t cpu;
	mem_t mem;
	char comm[PSC_MAX_NAME_LENGHT];
	pnode_t *stub;
	nnodes_t nstubs;
};
