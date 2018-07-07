#pragma once

#include <stdint.h>
#include "types.h"

#include "node.h"
#include "ppoint.h"

typedef struct pnode_t pnode_t;

struct pnode_t {
	node_t node;
	int pid;
	int ppid;
	real_t cpu;
	uint64_t mem;
	char name[PSC_MAX_NAME_LENGHT];

	real_t cputime;
	
	ppoint_t position;
};

real_t
pnode_mem_percentage(pnode_t *pnode);

real_t
pnode_cpu_percentage(pnode_t *pnode);

bool
pnode_is_null(pnode_t *pnode);
