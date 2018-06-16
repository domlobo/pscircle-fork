#pragma once

#include <stdio.h> 

#include "node.h"

#include "process.h"

typedef struct {
	process_t *root;

	size_t nprocesses;
	process_t *processes;
	size_t _processes_size;

	process_t **cpu_toplist;
	process_t **mem_toplist;
} ptree_t;

ptree_t *
ptree_create(FILE *input);

void
ptree_destroy(ptree_t *ptree);
