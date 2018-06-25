#include <assert.h>

#include "input_stream.h"

void
read_word_and_skip_to_lf(FILE *input, char *buf, size_t len);

pnode_t *
stream_get_next_proc(FILE *input, pnode_t *pnode)
{
	assert(input);
	assert(pnode);

	pnode_t p;
	while (!feof(input)) {

#ifdef PSC_USE_FLOAT
		int rc = fscanf(input, "%d %d %f %u ", &p.pid, &p.ppid, &p.cpu, &p.mem);
#else
		int rc = fscanf(input, "%d %d %lf %u ", &p.pid, &p.ppid, &p.cpu, &p.mem);
#endif
		if (rc == EOF)
			return NULL;

		read_word_and_skip_to_lf(input, pnode->name, PSC_MAX_NAME_LENGHT);

		pnode->pid = p.pid;
		pnode->ppid = p.ppid;
		pnode->cpu = p.cpu;
		pnode->mem = p.mem;

		return pnode;
	}

	return NULL;
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
