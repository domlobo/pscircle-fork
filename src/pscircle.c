#include <stdio.h>
#include <stdlib.h>

#include "ptree.h"
#include "painter.h"
#include "visualizer.h"

int main()
{
	ptree_t *ptree = calloc(1, sizeof(ptree_t));
	ptree_init(ptree, stdin);

	node_reorder_by_leaves((node_t *)ptree->root);

	node_arrange((node_t *)ptree->root);

	painter_t *painter = calloc(1, sizeof(painter_t));

	painter_init(painter);

	draw_tree(painter, ptree);

	painter_write(painter);

	painter_dinit(painter);

	free(painter);
	free(ptree);

	return 0;
}
