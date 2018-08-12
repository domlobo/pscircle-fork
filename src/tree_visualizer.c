#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include <ppoint.h>

#include "cfg.h"
#include "node.h"
#include "tree_visualizer.h"

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
calc_rotation(visualizer_t *vis, procs_t *procs);

void
calc_sector(visualizer_t *vis, procs_t *procs);

void
calc_offsets(visualizer_t *vis);

void
init_tree_painter(painter_t *painter);

real_t
radius_inc_at_depth(size_t depth);

void
dinit_tree_painter(painter_t *painter);

void
draw_tree_recurcive(visualizer_t *vis, painter_t *painter, pnode_t *procs, size_t depth);

void
draw_dot(painter_t *painter, pnode_t *pnode);

void
draw_link(visualizer_t *vis, painter_t *painter, pnode_t *parent, pnode_t *child, real_t lenght);

void
draw_label(visualizer_t *vis, painter_t *painter, pnode_t *child, real_t angle);

void
draw_tree(painter_t *painter, procs_t *procs)
{
	visualizer_t vis = {0};

	calc_sector(&vis, procs);

	calc_rotation(&vis, procs);

	calc_offsets(&vis);

	init_tree_painter(painter);

	draw_tree_recurcive(&vis, painter, procs->root, 0);

	dinit_tree_painter(painter);
}

void
calc_sector(visualizer_t *vis, procs_t *procs)
{
	assert(procs);

	if (config.tree.sector) {
		vis->sector = config.tree.sector;
		return;
	}

	vis->sector = R(2.) * M_PI;
}

void
calc_rotation(visualizer_t *vis, procs_t *procs)
{
	if (config.tree.rotate) {
		vis->rotation = -config.tree.rotation;
		return;
	}

	if (config.tree.anchor_proc_name) {
		pnode_t *found = procs_child_by_name(procs, config.tree.anchor_proc_name);

		if (found) {
			vis->rotation = -vis->sector * found->node.x + config.tree.anchor_proc_angle;
			return;
		}
	}

	assert(procs->root);
	node_t *w = node_widest_child((node_t *)procs->root);

	if (w && (pnode_t *)w != procs->root) {
		assert(vis->sector > 0);
		vis->rotation = -vis->sector * w->x;
	}
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

real_t
radius_inc_at_depth(size_t depth)
{
	assert(config.tree.radius_inc.size > 0);
	if (depth < config.tree.radius_inc.size)
		return config.tree.radius_inc.data[depth];

	return config.tree.radius_inc.data[config.tree.radius_inc.size - 1];
}

void
draw_tree_recurcive(visualizer_t *vis, painter_t *painter, pnode_t *parent, size_t depth)
{
	assert(painter);
	assert(parent);
	assert(config.tree.radius_inc.size > 0);

	real_t radius_inc = 0;
	real_t radius = 0;

	for (size_t d = 0; d <= depth; ++d) {
		radius_inc = radius_inc_at_depth(d);
		radius += radius_inc;
	}

	for (node_t *n = parent->node.first; n != NULL; n = n->next) {
		pnode_t *child = (pnode_t *) n;

		real_t angle = vis->sector * n->x + vis->rotation;
		child->position = ppoint_from_radial(angle, radius);

		draw_tree_recurcive(vis, painter, child, depth + 1);

		if (depth < config.tree.hide_levels)
			continue;

		draw_dot(painter, child);

		if (depth > 0 && depth > config.tree.hide_levels)
			draw_link(vis, painter, parent, child, radius_inc);

		draw_label(vis, painter, child, angle);
	}
}

void
draw_dot(painter_t *painter, pnode_t *pnode)
{
	real_t mem = pnode_mem_percentage(pnode);
	real_t cpu = pnode_cpu_percentage(pnode);

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
draw_link(visualizer_t *vis, painter_t *painter, pnode_t *parent, pnode_t *child, real_t lenght)
{
	ppoint_t a = parent->position;
	a.r += vis->link_offset;

	ppoint_t b = child->position;
	b.r -= vis->link_offset;

	real_t mem = pnode_mem_percentage(child);
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

	real_t conv = config.link.convexity * lenght;
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

