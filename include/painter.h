#pragma once

#include <stddef.h>

#include "types.h"
#include "point.h"
#include "color.h"

#include <cairo.h>

typedef struct {
	cairo_t *_cr;
	cairo_surface_t *_surface;
} painter_t;

typedef struct {
	point_t center;
	real_t radius;
	real_t border;
	color_t background;
	color_t foreground;
} circle_t;

typedef struct {
	point_t a;
	point_t b;
	real_t width;
	color_t background;
} line_t;

typedef struct {
	point_t a;
	point_t b;
	point_t ac;
	point_t bc;
	real_t width;
	color_t background;
} curve_t;

typedef struct {
	point_t refpoint;
	real_t angle;
	color_t foreground;
	const char *str;
} text_t;

void
painter_init(painter_t *painter);

void
painter_dinit(painter_t *painter);

void
painter_save(painter_t *painter);

void
painter_restore(painter_t *painter);

void
painter_write(painter_t *painter);

point_t
painter_text_size(painter_t *painter, const char *str);

void
painter_center(painter_t *painter);

void
painter_translate(painter_t *painter, point_t position);

void
painter_fill_backgound(painter_t *painter, color_t background);

void
painter_set_font_face(painter_t *painter, const char *fontface);

void
painter_set_font_size(painter_t *painter, real_t fontsize);

void
painter_draw_circle(painter_t *painter, circle_t circle);

void
painter_draw_line(painter_t *painter, line_t line);

void
painter_draw_curve(painter_t *painter, curve_t curve);

void
painter_draw_text(painter_t *painter, text_t text);
