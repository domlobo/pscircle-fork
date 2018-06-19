#pragma once

#include <stdbool.h>

#include "types.h"
#include "color.h"
#include "point.h"

typedef struct {
	const char *font_face;
	double font_size;
	color_t font_color;

	real_t radius_inc;
	real_t sector;
	bool rotate;
	real_t rotation;

	point_t center;

} tree_t;

typedef struct {
	real_t radius;
	real_t border;
	color_t bg_min;
	color_t bg_max;
	color_t fg_min;
	color_t fg_max;
} dot_t;

typedef struct {
	real_t width;
	real_t convexity;
	color_t color_min;
	color_t color_max;
} link_t;

typedef struct {
	bool show_header;
	char *name;
	char *label;
	char *value_format;
	real_t value;
} toplist_t;

typedef struct {
	real_t width;
	real_t height;
	color_t background;
	color_t color;
} bar_t;

typedef struct {
	point_t center;
	real_t width;
	real_t row_height;
	real_t font_size;
	color_t font_color;
	color_t pid_font_color;
	char *font_face;
	real_t column_padding;

	bar_t bar;
	toplist_t cpulist;
	toplist_t memlist;
} toplists_t;

typedef struct {
	size_t output_width;
	size_t output_height;
	char *output;
	char *output_display;

	uint_fast8_t memory_unit;
	pid_t root_pid;
	pid_t zero_angle_pid;
	nnodes_t max_children;

	real_t max_mem;
	real_t min_mem;
	real_t max_cpu;
	real_t min_cpu;

	color_t background;

	tree_t tree;
	dot_t dot;
	link_t link;
	toplists_t toplists;

} cfg_t;

extern cfg_t config;
