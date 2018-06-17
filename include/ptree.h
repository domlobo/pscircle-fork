#pragma once

#include <stdio.h> 

#include "node.h"

#include "pnode.h"

typedef struct {
	pnode_t *root;

	size_t nprocesses;
	pnode_t processes[PSC_MAX_PROCS_COUNT];

	pnode_t *cpu_toplist[PSC_TOPLIST_MAX_ROWS];
	pnode_t *mem_toplist[PSC_TOPLIST_MAX_ROWS];
} ptree_t;

void
ptree_init(ptree_t *ptree, FILE *input);

void
ptree_dinit(ptree_t *ptree);

pnode_t *
ptree_child_by_pid(ptree_t *ptree, pid_t pid);
