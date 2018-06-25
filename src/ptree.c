#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <strings.h>
#include <stdbool.h>
#include <stdio.h>

#include "ptree.h"
#include "cfg.h"

#include "input_stream.h"
#include "input_linux.h"

#define CHECK(x) do { \
	if (x) break; \
	fprintf(stderr, "%s:%d error: %s\n", \
			__FILE__, __LINE__, strerror(errno)); \
	exit(EXIT_FAILURE); \
} while (0)

void
read_procs_stream(ptree_t *ptree, FILE *input);

void
read_procs_linux(ptree_t *ptree);

void
link_process(ptree_t *ptree);

pnode_t *
get_new_process(ptree_t *ptree);

void
reserve_root_memory(ptree_t *ptree);

void
count_as_stub(pnode_t *parent, pnode_t *child);

void
update_mem_toplist(ptree_t *ptree, pnode_t *p);

void
update_cpu_toplist(ptree_t *ptree, pnode_t *p);

void
sort_top_lists(ptree_t *ptree);

void
add_stubs(ptree_t *ptree);

pnode_t *
find_by_pid(ptree_t *ptree, pid_t pid);

void
ptree_init(ptree_t *ptree, FILE *input)
{
	assert(ptree);

#ifndef NDEBUG
	uint8_t zeros[sizeof(ptree_t)];
	memset(zeros, 0, sizeof(zeros));
	assert(memcmp(zeros, ptree, sizeof(ptree_t)) == 0);
#endif

	reserve_root_memory(ptree);

	if (!input)
		read_procs_linux(ptree);
	else
		read_procs_stream(ptree, input);

	link_process(ptree);

	sort_top_lists(ptree);
}

void
ptree_dinit(ptree_t *ptree)
{
	assert(ptree);
}

void
read_procs_stream(ptree_t *ptree, FILE *input)
{
	assert(ptree);

	while (true) {
		pnode_t *p = get_new_process(ptree);
		if (!p)
			break;

		if (!stream_get_next_proc(input, p))
			break;

		for (size_t u = 0; u < config.memory_unit; ++u)
			p->mem *= 1024;
	}
}

void
read_procs_linux(ptree_t *ptree)
{
	assert(ptree);

	linux_procs_t lprocs = {0};
	linux_init(&lprocs);

	while (true) {
		pnode_t *p = get_new_process(ptree);
		if (!p)
			break;

		if (!linux_get_next_proc(&lprocs, p))
			break;
	}

	if (config.interval > 0) {
		linux_delay(&lprocs, config.interval);

		for (size_t i = 0; i < ptree->nprocesses; ++i)
			linux_update_proc(&lprocs, ptree->processes + i);
	}

	linux_dinit(&lprocs);
}

pnode_t *
get_new_process(ptree_t *ptree)
{
	assert(ptree);

	if (ptree->nprocesses == PSC_MAX_PROCS_COUNT) {
		fprintf(stderr,
				"Maximum number of processes (%d) is reached, skipping the rest.\n",
				PSC_MAX_PROCS_COUNT);
		fprintf(stderr, "To increase this value change PSC_MAX_PROCS_COUNT and recompile\n");
		return NULL;
	}

	assert(ptree->processes);
	return ptree->processes + ptree->nprocesses++;
}


pnode_t *
find_by_pid(ptree_t *ptree, pid_t pid)
{
	for (size_t i = 0; i < ptree->nprocesses; ++i) {
		if (ptree->processes[i].pid == pid)
			return ptree->processes + i;
	}

	return NULL;
}

void
link_process(ptree_t *ptree)
{
	assert(ptree);
	assert(!ptree->root);

	pnode_t *found = find_by_pid(ptree, config.root_pid);
	if (!found) {
		ptree->root = ptree->processes;
		ptree->root->pid = config.root_pid;
	} else {
		ptree->root = found;
	}

	// starts from 1 to skip reserved root
	for (size_t i = 1; i < ptree->nprocesses; ++i) {
		pnode_t *p = ptree->processes + i;

		update_cpu_toplist(ptree, p);

		update_mem_toplist(ptree, p);

		if (p == ptree->root)
			continue;

		pnode_t *parent = find_by_pid(ptree, p->ppid);
		if (!parent)
			continue;

		if (node_nchildren(&parent->node) < config.max_children) {
			node_add((node_t *)parent, (node_t *)p);
		} else {
			count_as_stub(parent, p);
		}
	}

	add_stubs(ptree);
}

void
reserve_root_memory(ptree_t *ptree)
{
	pnode_t *r = get_new_process(ptree);
	assert(r);
	r->pid = -1;
}

void
count_as_stub(pnode_t *parent, pnode_t *p)
{
	assert(parent);
	assert(p);

	if (!parent->stub) {
		assert(parent->nstubs == 0);
		parent->stub = p;
		parent->nstubs = 1;
		return;
	}

	assert(parent->nstubs > 0);

	if (p->mem > parent->stub->mem)
		parent->stub->mem = p->mem;
	if (p->cpu > parent->stub->cpu)
		parent->stub->cpu = p->cpu;

	parent->nstubs++;
}

void
update_cpu_toplist(ptree_t *ptree, pnode_t *p)
{
	assert(ptree);
	assert(p);

	pnode_t **found = NULL;

	for (size_t i = 0; i < PSC_TOPLIST_MAX_ROWS; ++i) {
		pnode_t **pp = &(ptree->cpu_toplist[i]);
		if (*pp == NULL) {
			found = pp;
			break;
		}

		if ((*pp)->cpu < p->cpu)
			found = pp;
	}

	if (found)
		*found = p;
}

void
update_mem_toplist(ptree_t *ptree, pnode_t *p)
{
	assert(ptree);
	assert(p);

	pnode_t **found = NULL;

	for (size_t i = 0; i < PSC_TOPLIST_MAX_ROWS; ++i) {
		pnode_t **pp = &(ptree->mem_toplist[i]);
		if (*pp == NULL) {
			found = pp;
			break;
		}

		if ((*pp)->mem < p->mem)
			found = pp;
	}

	if (found)
		*found = p;
}

int cpu_comp(const void *a, const void *b) {
	const pnode_t *pa = *(const pnode_t **) a;
	const pnode_t *pb = *(const pnode_t **) b;

	if (!pa && !pb)
		return 0;
	if (!pa && pb)
		return 1;
	if (pa && !pb)
		return -1;

	if (pa->cpu < pb->cpu)
		return 1;

	return -1;
}

int mem_comp(const void *a, const void *b) {
	const pnode_t *pa = *(const pnode_t **) a;
	const pnode_t *pb = *(const pnode_t **) b;

	if (!pa && !pb)
		return 0;
	if (!pa && pb)
		return 1;
	if (pa && !pb)
		return -1;

	if (pa->mem < pb->mem)
		return 1;

	return -1;
}

void
sort_top_lists(ptree_t *ptree)
{
	assert(ptree);

	qsort(ptree->cpu_toplist, PSC_TOPLIST_MAX_ROWS,
			sizeof(pnode_t *), cpu_comp);

	qsort(ptree->mem_toplist, PSC_TOPLIST_MAX_ROWS,
			sizeof(pnode_t *), mem_comp);
}

void
add_stubs(ptree_t *ptree)
{
	for (size_t i = 0; i < ptree->nprocesses; ++i) {
		pnode_t *p = ptree->processes + i;
		if (!p->stub)
			continue;
		assert(p->nstubs > 0);

		snprintf(p->stub->name, PSC_MAX_NAME_LENGHT,
				"<%zd omitted>", p->nstubs);

		node_add((node_t *)p, (node_t *)p->stub);
	}
}

void
ptree_child_by_pid_recurcive(pnode_t **found, pnode_t *p, pid_t pid)
{
	if (*found)
		return;

	if (p->pid == pid) {
		*found = p;
		return;
	}

	for (node_t *n = p->node.first; n != NULL; n = n->next)
		ptree_child_by_pid_recurcive(found, (pnode_t *) n, pid);
}

pnode_t *
ptree_child_by_pid(ptree_t *ptree, pid_t pid)
{
	pnode_t *found = NULL;

	ptree_child_by_pid_recurcive(&found, ptree->root, pid);

	return found;
}
