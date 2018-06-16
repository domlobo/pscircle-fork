#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <assert.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include "painter.h"

/*
#define CHECK(x) do { \
	int rc = (x); \
	if (rc == 0) break; \
	fprintf(stderr, "%s:%d %s returned %d: %s:", \
			__FILE__, __LINE__, #x, rc, strerror(errno)); \
	exit(EXIT_FAILURE); \
} while (0)
*/

#define CHECK(x) do { \
	if (!(x)) break; \
	fprintf(stderr, "%s:%d %s failed: %s:", \
			__FILE__, __LINE__, #x, strerror(errno)); \
	exit(EXIT_FAILURE); \
} while (0)

painter_t *
painter_create(size_t width, size_t height)
{
	painter_t *painter = calloc(1, sizeof(painter_t));
	CHECK(painter);

	painter->width = width;
	painter->height = height;

	painter->_surface = cairo_image_surface_create(
		CAIRO_FORMAT_ARGB32, painter->width, painter->height); 

	painter->_cr = cairo_create(painter->_surface);

	cairo_translate(painter->_cr,
			R(.5) * painter->width, R(.5) * painter->height);

	return painter;
}

void
painter_destroy(painter_t *painter)
{
	assert(painter);
	assert(painter->_cr);
	assert(painter->_surface);

	cairo_destroy(painter->_cr);
	cairo_surface_destroy(painter->_surface);
	free(painter);
}

void
painter_save(painter_t *painter)
{
	assert(painter);

	cairo_save(painter->_cr);
}

void
painter_restore(painter_t *painter)
{
	assert(painter);

	cairo_restore(painter->_cr);
}

void
painter_write(painter_t *painter, const char *filename)
{
	assert(painter);
	assert(filename);

	cairo_surface_write_to_png(painter->_surface, filename);
}

point_t
painter_text_size(painter_t *painter, const char *str)
{
	assert(painter);
	assert(str);

	cairo_text_extents_t extents;
	cairo_text_extents(painter->_cr, str, &extents);

	point_t p = {
		.x = extents.width + extents.x_bearing,
		.y = -extents.y_bearing
	};

	return p;
}

void
painter_translate(painter_t *painter, point_t position)
{
	assert(painter);

	cairo_translate(painter->_cr, position.x, position.y);
}

void
fill_backgound(painter_t *painter, color_t background)
{
	cairo_rectangle(painter->_cr, 0, 0, painter->width, painter->height);
	cairo_set_source_rgba(painter->_cr,
		background.r,
		background.g,
		background.b,
		background.a
	);

	cairo_fill(painter->_cr);
}

void
set_font_face(painter_t *painter, const char *fontface)
{
	cairo_select_font_face(
		painter->_cr, 
		fontface,
		CAIRO_FONT_SLANT_NORMAL,
		CAIRO_FONT_WEIGHT_NORMAL
	);
}

void
set_font_size(painter_t *painter, real_t fontsize)
{
	cairo_set_font_size(painter->_cr, fontsize);
}

void
draw_circle(painter_t *painter, circle_t circle)
{
	cairo_arc(
		painter->_cr,
		circle.center.x,
		circle.center.y,
		circle.radius,
		0, 2 * M_PI
	);

	if (circle.border > 0) {
		cairo_set_line_width(painter->_cr, circle.border);  

		cairo_set_source_rgba(
			painter->_cr,
			circle.foreground.r,
			circle.foreground.g,
			circle.foreground.b,
			circle.foreground.a
		);

		cairo_stroke_preserve(painter->_cr);
	}

	cairo_set_source_rgba(
		painter->_cr,
		circle.background.r,
		circle.background.g,
		circle.background.b,
		circle.background.a
	);

	cairo_fill(painter->_cr);

	cairo_stroke(painter->_cr);

}

void
draw_line(painter_t *painter, line_t line)
{
	cairo_move_to(painter->_cr, line.a.x, line.a.y);
	cairo_line_to(painter->_cr, line.b.x, line.b.y);

	cairo_set_source_rgba(
		painter->_cr,
		line.background.r,
		line.background.g,
		line.background.b,
		line.background.a
	);

	if (line.width > 0)
		cairo_set_line_width(painter->_cr, line.width);

	cairo_stroke(painter->_cr);
}

void
draw_curve(painter_t *painter, curve_t curve)
{
	cairo_move_to(painter->_cr, curve.a.x, curve.a.y);

	cairo_curve_to(
		painter->_cr,
		curve.ac.x, curve.ac.y,
		curve.bc.x, curve.bc.y,
		curve.b.x, curve.b.y
	);

	cairo_set_source_rgba(
		painter->_cr,
		curve.background.r,
		curve.background.g,
		curve.background.b,
		curve.background.a
	);

	if (curve.width > 0)
		cairo_set_line_width(painter->_cr, curve.width);

	cairo_stroke(painter->_cr);
}

void
draw_text(painter_t *painter, text_t text)
{
	cairo_save(painter->_cr);

	cairo_move_to(painter->_cr, text.refpoint.x, text.refpoint.y);

	cairo_rotate(painter->_cr, text.angle);
	
	cairo_set_source_rgba(
		painter->_cr, 
		text.foreground.r,
		text.foreground.g,
		text.foreground.b,
		text.foreground.a
	);

	cairo_show_text(painter->_cr, text.str);

	cairo_stroke(painter->_cr);

	cairo_restore(painter->_cr);
}
