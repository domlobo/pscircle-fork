#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <strings.h>
#include <stdbool.h>
#include <stdio.h>

#include "ptree.h"
#include "cfg.h"

#define CHECK(x) do { \
	if (x) break; \
	fprintf(stderr, "%s:%d error: %s\n", \
			__FILE__, __LINE__, strerror(errno)); \
	exit(EXIT_FAILURE); \
} while (0)

void
read_word_and_skip_to_lf(FILE *input, char *buf, size_t len);

void
read_proccesses(ptree_t *ptree, FILE *input);

void
link_process(ptree_t *ptree);

pnode_t *
get_new_process(ptree_t *ptree);

pnode_t *
find_by_pid(ptree_t *ptree, pid_t pid);

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

ptree_t *
ptree_create(FILE *input)
{
	assert(input);

	ptree_t *ptree = calloc(1, sizeof(ptree_t));
	CHECK(ptree);

	ptree->cpu_toplist = calloc(config.toplist_max_rows, sizeof(pnode_t *));
	CHECK(ptree->cpu_toplist);

	ptree->mem_toplist = calloc(config.toplist_max_rows, sizeof(pnode_t *));
	CHECK(ptree->mem_toplist);

	reserve_root_memory(ptree);

	read_proccesses(ptree, input);

	link_process(ptree);

	sort_top_lists(ptree);

	return ptree;
}

void
ptree_destroy(ptree_t *ptree)
{
	assert(ptree);

	for (size_t i = 0; i < ptree->nprocesses; ++i)
		free(ptree->processes[i].comm);

	if (ptree->processes)
		free(ptree->processes);

	free(ptree->mem_toplist);

	free(ptree->cpu_toplist);

	free(ptree);
}

void
read_proccesses(ptree_t *ptree, FILE *input)
{
	assert(ptree);
	assert(input);

	pnode_t p;

	while (!feof(input)) {

#ifdef PSC_USE_FLOAT
		int rc = fscanf(input, "%d %d %f %zd ", &p.pid, &p.ppid, &p.cpu, &p.mem);
#else
		int rc = fscanf(input, "%d %d %lf %zd ", &p.pid, &p.ppid, &p.cpu, &p.mem);
#endif
		if (rc == EOF)
			break;

		for (size_t u = 0; u < config.memory_unit; ++u)
			p.mem *= 1024;

		pnode_t *pp = get_new_process(ptree);
		assert(pp);

		read_word_and_skip_to_lf(input, pp->comm, config.max_name_lenght);

		pp->pid = p.pid;
		pp->ppid = p.ppid;
		pp->cpu = p.cpu;
		pp->mem = p.mem;
	}
}

pnode_t *
get_new_process(ptree_t *ptree)
{
	assert(ptree);

	if (ptree->nprocesses == ptree->_processes_size) {
		size_t prev = ptree->_processes_size;

		if (ptree->_processes_size == 0) {
			ptree->_processes_size = config.initial_process_count;
		} else {
			assert(config.initial_process_count > 1);
			ptree->_processes_size *= 2;
		}

		ptree->processes = realloc(ptree->processes,
				ptree->_processes_size * sizeof(pnode_t));
		CHECK(ptree->processes);

		bzero(ptree->processes + prev,
				(ptree->_processes_size - prev) * sizeof(pnode_t));
	}

	pnode_t *p = ptree->processes + ptree->nprocesses++;

	p->comm = calloc(config.max_name_lenght, sizeof(char));
	CHECK(p->comm);

	return p;
}

void
read_word_and_skip_to_lf(FILE *input, char *buf, size_t len)
{
	assert(input);
	assert(buf);

	int c = 0;
	for (size_t i = 0; i < len - 1; ++i) {
		c = fgetc(input);
		if (c == ' ' || c == '\n' || c == EOF) {
			buf[i] = '\0';
			if (c == ' ')
				break;
			return;
		}
		buf[i] = c;
	}

	buf[len - 1] = '\0';

	do {
		c = fgetc(input);
	} while (c != '\n' && c != EOF);

	return;
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
		if (!parent) {
			fprintf(stderr, "Orphan process: %d\n", p->pid);
			continue;
		}

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

	for (size_t i = 0; i < config.toplist_max_rows; ++i) {
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

	for (size_t i = 0; i < config.toplist_max_rows; ++i) {
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

	qsort(ptree->cpu_toplist, config.toplist_max_rows,
			sizeof(pnode_t *), cpu_comp);

	qsort(ptree->mem_toplist, config.toplist_max_rows,
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

		snprintf(p->stub->comm, config.max_name_lenght,
				"<%zd omitted>", p->nstubs);

		node_add((node_t *)p, (node_t *)p->stub);
	}
}
