#pragma once

#include "types.h"

#include "node.h"
#include "ppoint.h"

typedef uint_fast16_t mem_t;

typedef struct pnode_t pnode_t;

struct pnode_t {
	node_t node;
	pid_t pid;
	pid_t ppid;
	real_t cpu;
	mem_t mem;
	char name[PSC_MAX_NAME_LENGHT];
	pnode_t *stub;
	nnodes_t nstubs;
	ppoint_t position;
};

real_t
pnode_mem_percentage(pnode_t *pnode);

real_t
pnode_cpu_percentage(pnode_t *pnode);
