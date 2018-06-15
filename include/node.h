#pragma once

#include <stdint.h>
#include <stddef.h>

typedef struct {
	struct node_t *first;
	struct node_t *next;
	struct node_t *_last;
	struct node_t *_prev;
	struct node_t *_parent;
	struct node_t *_ancesstor;
	struct node_t *_link;

	uint_fast16_t _id;
	uint_fast16_t _nleaves;

	double x;
	double _mod;
	double _shift;
	double _change;
} node_t;

void
node_arrange(node_t *root);

void
node_add(node_t *child, node_t *parent);
