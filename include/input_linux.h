#pragma once

#include <dirent.h>

#include "pnode.h"

typedef int pid_t;
typedef unsigned long ticks_t;
typedef unsigned long long ctime_t;
typedef unsigned long rss_t;

typedef struct {
	double r;
	DIR *procdir;
	ctime_t cputime_st;
	ctime_t cputime_en;
	long hertz;
	long uptime;
	int pagesize;
} linux_procs_t;

void
linux_init(linux_procs_t *linux_procs);

void
linux_dinit(linux_procs_t *linux_procs);

pnode_t *
linux_get_next_proc(linux_procs_t *linux_procs, pnode_t *pnode);

void
linux_delay(linux_procs_t *linux_procs, real_t delay);

void
linux_update_proc(linux_procs_t *linux_procs, pnode_t *pnode);
