#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include <ppoint.h>

#include "cfg.h"
#include "node.h"
#include "visualizer.h"

#ifndef M_PI
#define M_PI R(3.14159265358979323846)
#endif

typedef struct {
	real_t rotation;
	real_t sector;
	real_t lable_offset;
	real_t link_offset;
} visualizer_t ;

void
calc_rotation(visualizer_t *vis, ptree_t *ptree);

void
calc_sector(visualizer_t *vis, ptree_t *ptree);

void
calc_offsets(visualizer_t *vis);

void
init_tree_painter(painter_t *painter);

void
dinit_tree_painter(painter_t *painter);

void
draw_tree_recurcive(visualizer_t *vis, painter_t *painter, pnode_t *ptree, int depth);

void
draw_dot(painter_t *painter, pnode_t *pnode);

void
draw_link(visualizer_t *vis, painter_t *painter, pnode_t *parent, pnode_t *child);

void
draw_label(visualizer_t *vis, painter_t *painter, pnode_t *child, real_t angle);

real_t
calc_cpu_percentage(pnode_t *node);

real_t
calc_mem_percentage(pnode_t *node);

void
draw_tree(painter_t *painter, ptree_t *ptree)
{
	visualizer_t vis = {0};

	calc_sector(&vis, ptree);

	calc_rotation(&vis, ptree);

	calc_offsets(&vis);

	init_tree_painter(painter);

	draw_tree_recurcive(&vis, painter, ptree->root, 0);

	dinit_tree_painter(painter);
}

void
calc_sector(visualizer_t *vis, ptree_t *ptree)
{
	assert(ptree);

	if (config.tree.sector) {
		vis->sector = config.tree.sector;
		return;
	}

	real_t cat = config.dot.radius + config.dot.border;
	real_t hyp = config.tree.radius_inc;
	vis->sector = R(2.) * M_PI - R(atan)(cat / hyp);
}

void
calc_rotation(visualizer_t *vis, ptree_t *ptree)
{
	if (config.tree.rotate) {
		vis->rotation = -config.tree.rotation;
		return;
	}

	if (config.zero_angle_pid != config.root_pid) {
		pnode_t *zp = ptree_child_by_pid(ptree, config.zero_angle_pid);
		if (!zp) {
			fprintf(stderr, "PID %d (from --zero-angle-pid option) is not found\n",
					config.zero_angle_pid);
			exit(EXIT_FAILURE);
		}
		vis->rotation = -vis->sector * zp->node.x;
		return;
	}

	assert(ptree->root);
	node_t *w = node_widest_child((node_t *)ptree->root);
	assert(w);

	assert(vis->sector > 0);
	vis->rotation = -vis->sector * w->x;
}

void
calc_offsets(visualizer_t *vis)
{
	vis->lable_offset = R(2.) * config.dot.radius + config.dot.border / 2;
	vis->link_offset = config.dot.radius + config.dot.border / 2;
}

void
init_tree_painter(painter_t *painter)
{
	painter_save(painter);

	painter_set_font_face(painter, config.tree.font_face);
	painter_set_font_size(painter, config.tree.font_size);
	painter_translate(painter, config.tree.center);
}

void
dinit_tree_painter(painter_t *painter)
{
	painter_restore(painter);
}

void
draw_tree_recurcive(visualizer_t *vis, painter_t *painter, pnode_t *parent, int depth)
{
	assert(painter);
	assert(parent);

	for (node_t *n = parent->node.first; n != NULL; n = n->next) {
		pnode_t *child = (pnode_t *) n;

		real_t cr = config.tree.radius_inc * (depth + 1);
		real_t ca = vis->sector * n->x + vis->rotation;
		ppoint_t c = ppoint_from_radial(ca, cr);
		child->position = c;

		draw_tree_recurcive(vis, painter, child, depth + 1);

		draw_dot(painter, child);

		if (depth > 0)
			draw_link(vis, painter, parent, child);

		draw_label(vis, painter, child, ca);
	}
}

void
draw_dot(painter_t *painter, pnode_t *pnode)
{
	real_t mem = calc_mem_percentage(pnode);
	real_t cpu = calc_cpu_percentage(pnode);

	point_t center = ppoint_to_point(pnode->position);

	color_t bg = color_between(config.dot.bg_min, config.dot.bg_max, cpu);

	color_t fg = color_between(config.dot.fg_min, config.dot.fg_max, mem);

	circle_t c = {
		.center = center,
		.radius = config.dot.radius,
		.border = config.dot.border,
		.background = bg,
		.foreground = fg
	};

	painter_draw_circle(painter, c);
}

void
draw_link(visualizer_t *vis, painter_t *painter, pnode_t *parent, pnode_t *child)
{
	ppoint_t a = parent->position;
	a.r += vis->link_offset;

	ppoint_t b = child->position;
	b.r -= vis->link_offset;

	real_t mem = calc_mem_percentage(child);
	color_t col = color_between(config.link.color_min, config.link.color_max, mem);

	if (ppoint_codirectinal(a, b)) {
		line_t line = {
			.a = ppoint_to_point(a),
			.b = ppoint_to_point(b),
			.width = config.link.width,
			.color = col
		};

		painter_draw_line(painter, line);
		return;
	}

	real_t conv = config.link.convexity * config.tree.radius_inc;
	ppoint_t ac = parent->position;
	ac.r += conv;
	ppoint_t bc = child->position;
	bc.r -= conv;

	curve_t curve = {
		.a = ppoint_to_point(a),
		.b = ppoint_to_point(b),
		.ac = ppoint_to_point(ac),
		.bc = ppoint_to_point(bc),
		.width = config.link.width,
		.color = col
	};

	painter_draw_curve(painter, curve);
}

void
draw_label(visualizer_t *vis, painter_t *painter, pnode_t *child, real_t angle)
{
	point_t dim = painter_text_size(painter, child->name);

	ppoint_t p = child->position;

	if (p.nx < 0) {
		angle += M_PI;
		p.r += dim.x;
	}

	ppoint_t np = ppoint_normal(p, p.nx > 0);
	np.r = dim.y / 2;


	p = ppoint_add(p, np);

	p.r += vis->lable_offset;

	text_t text = {
		.refpoint = ppoint_to_point(p),
		.angle = angle,
		.foreground = config.tree.font_color,
		.str = child->name
	};

	painter_draw_text(painter, text);
}

real_t
calc_mem_percentage(pnode_t *pnode)
{
	assert(config.max_mem >= config.min_mem);

	real_t m = pnode->mem;
	if (m < config.min_mem)
		return 0;
	if (m > config.max_mem)
		return 1;

	return (m - config.min_mem) / (config.max_mem - config.min_mem);
}

real_t
calc_cpu_percentage(pnode_t *pnode)
{
	assert(config.max_cpu >= config.min_cpu);

	real_t m = pnode->cpu;
	if (m < config.min_cpu)
		return 0;
	if (m > config.max_cpu)
		return 1;

	return (m - config.min_cpu) / (config.max_cpu - config.min_cpu);
}
