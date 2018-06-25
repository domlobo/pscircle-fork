#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <strings.h>

#include "cfg.h"
#include "ptree.h"
#include "timing.h"
#include "painter.h"
#include "tree_visualizer.h"
#include "toplist_visualizer.h"

#define CHECK(x) do { \
	if (x) break; \
	fprintf(stderr, "%s:%d error: %s\n", \
			__FILE__, __LINE__, strerror(errno)); \
	exit(EXIT_FAILURE); \
} while (0)

int main(int argc, const char *argv[])
{
	timing_t tm = {0};
	tm_start(&tm);

	parse_cmdline(argc, argv);

	ptree_t *ptree = calloc(1, sizeof(ptree_t));
	CHECK(ptree);

	FILE *input = NULL;
	if (config.read_stdin)
		input = stdin;

	ptree_init(ptree, input);

	tm_tick(&tm, "init");

	node_reorder_by_leaves((node_t *)ptree->root);

	node_arrange((node_t *)ptree->root);

	tm_tick(&tm, "arrange");

	painter_t *painter = calloc(1, sizeof(painter_t));
	CHECK(painter);

	painter_init(painter);

	draw_tree(painter, ptree);

	tm_tick(&tm, "draw tree");

	if (config.toplists.visible) {
		draw_toplists(painter, ptree);
		tm_tick(&tm, "draw lists");
	}

	painter_write(painter);

	tm_tick(&tm, "write");

	painter_dinit(painter);

	tm_total(&tm);

	return 0;
}
