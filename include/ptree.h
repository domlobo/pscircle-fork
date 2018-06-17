#pragma once

#include <stdio.h> 

#include "node.h"

#include "pnode.h"

typedef struct {
	pnode_t *root;

	size_t nprocesses;
	pnode_t *processes;
	size_t _processes_size;

	pnode_t **cpu_toplist;
	pnode_t **mem_toplist;
} ptree_t;

ptree_t *
ptree_create(FILE *input);

void
ptree_destroy(ptree_t *ptree);
