#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <stdint.h>

#include "input_linux.h"

#define PATH_BUFSIZE 30

#define CHECK(x) do { \
	if (x) break; \
	fprintf(stderr, "%s:%d error: %s\n", \
			__FILE__, __LINE__, strerror(errno)); \
	exit(EXIT_FAILURE); \
} while (0)

typedef unsigned long ticks_t;
typedef unsigned long long ctime_t;
typedef unsigned long rss_t;

typedef struct {
	int pid;
	int ppid;
	char *comm;
	ticks_t stime;
	ticks_t utime;
	ctime_t starttime;
	rss_t rss;
} proc_t;

ticks_t
read_cputime();

ctime_t
read_uptime();

proc_t *
read_proc(pid_t pid, proc_t *proc);

pnode_t *
proc_to_pnode(linux_procs_t *ctx, pnode_t *pnode, proc_t *proc);

void
linux_init(linux_procs_t *ctx)
{
#ifndef NDEBUG
	uint8_t zeros[sizeof(linux_procs_t)];
	memset(zeros, 0, sizeof(zeros));
	assert(memcmp(zeros, ctx, sizeof(linux_procs_t)) == 0);
#endif

	ctx->procdir = opendir("/proc");
	CHECK(ctx->procdir);

	ctx->hertz = sysconf(_SC_CLK_TCK);

	ctx->pagesize = getpagesize();

	ctx->cputime_st = read_cputime();

	ctx->uptime = read_uptime();
}

void
linux_dinit(linux_procs_t *ctx)
{
	assert(ctx);
	assert(ctx->procdir);

	closedir(ctx->procdir);
}

pnode_t *
linux_get_next_proc(linux_procs_t *ctx, pnode_t *pnode)
{
	assert(ctx);
	assert(pnode);
	assert(ctx->procdir);

	struct dirent *de;
	proc_t p = {0};
	while ((de = readdir(ctx->procdir)) != NULL) {
		if (de->d_type != DT_DIR)
			continue;

		char *e = NULL;
		pid_t pid = strtol(de->d_name, &e, 10);
		if (*e != '\0')
			continue;

		p.comm = pnode->name;

		if (!read_proc(pid, &p))
			continue;

		if (!proc_to_pnode(ctx, pnode, &p))
			continue;

		return pnode;
	}

	return NULL;
}

pnode_t *
proc_to_pnode(linux_procs_t *ctx, pnode_t *pnode, proc_t *proc)
{
	assert(ctx);
	assert(pnode);
	assert(proc);
	assert(pnode->name == proc->comm);

	pnode->pid = proc->pid;

	pnode->ppid = proc->ppid;

	pnode->mem = proc->rss * ctx->pagesize;

	double t = proc->utime + proc->stime;
	double dt = (double) ctx->uptime * ctx->hertz - proc->starttime;
	pnode->cpu = 100. * t / dt;

	pnode->cputime = t;

	return pnode;
}

void
linux_delay(linux_procs_t *ctx, real_t delay)
{
	assert(ctx);
	assert(delay > 0);

	unsigned sec = delay;
	unsigned usec = (delay - sec) * 1e6;

	if (sec > 0)
		sleep(sec);
	if (usec < 1e6)
		usleep(usec);

	ctx->cputime_en = read_cputime();
}

void
linux_update_proc(linux_procs_t *ctx, pnode_t *pnode)
{
	assert(ctx);
	assert(pnode);

	proc_t p = {0};

	if (!read_proc(pnode->pid, &p)) {
		pnode->cpu = 0;
		return;
	}

	double dt = (double)ctx->cputime_en - ctx->cputime_st;
	if (dt == 0) {
		pnode->cpu = 0;
		return;
	}

	double t = (double)(p.stime + p.utime) - pnode->cputime;
	pnode->cpu = 100. * t / dt;
}

ticks_t
read_cputime()
{
	FILE *f = fopen("/proc/stat", "r");
	CHECK(f);

	unsigned long ret = 0;

	fscanf(f, "%*s");
	for (size_t i = 0; i < 10; ++i) {
		unsigned long tmp = 0;
		fscanf(f, "%lu", &tmp);
		ret += tmp;
	}

	fclose(f);

	return ret;
}

ctime_t
read_uptime()
{
	FILE *f = fopen("/proc/uptime", "r");
	CHECK(f);

	uint64_t uptime = 0;

	fscanf(f, "%ld", &uptime);

	fclose(f);

	return uptime;
}

proc_t *
read_proc(pid_t pid, proc_t *proc)
{
	static char path[PATH_BUFSIZE];
	snprintf(path, sizeof(path), "/proc/%d/stat", pid);

	FILE *f = fopen(path, "r");
	if (!f)
		return NULL;

	static char *buf = NULL;
	static size_t n = 0;
	ssize_t l = getline(&buf, &n, f);
	fclose(f);

	if (l <= 0)
		return NULL;

	char *end = buf + l;

	sscanf(buf, "%d", &proc->pid);

	for (size_t i = 52; i != 2; --i) {
		*end = '\0';
		while (*end != ' ')
			end--;
		char *s = end + 1;
		if (i == 24)
			sscanf(s, "%ld", &proc->rss);
		if (i == 22)
			sscanf(s, "%llu", &proc->starttime);
		if (i == 15)
			sscanf(s, "%lu", &proc->utime);
		if (i == 14)
			sscanf(s, "%lu", &proc->stime);
		if (i == 4)
			sscanf(s, "%d", &proc->ppid);
	}

	if (!proc->comm)
		return proc;

	*end = '\0';

	char *s = buf;
	while (*s != ' ')
		s++;

	if (*(s + 1) == '(') {
		do {
			s++;
		} while (*s == '(');
	}

	if (*(end - 1) == ')') {
		do {
			end--;
		} while (*end == ')');

		*(end + 1) = '\0';
	}

	strncpy(proc->comm, s, PSC_MAX_NAME_LENGHT);

	return proc;
}
