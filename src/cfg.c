#include "cfg.h"

#ifndef M_PI
#define M_PI R(3.14159265358979323846)
#endif

#define rgb(r,g,b) {(real_t)(r)/255, (real_t)(g)/255, (real_t)(b)/255, 1}

#define rgba(r,g,b,a) {(real_t)(r)/255, (real_t)(g)/255, (real_t)(b)/255, (real_t)(a)/255}

#include "config.h"

cfg_t config = {
	.output         = PSC_OUTPUT,
	.output_width   = PSC_OUTPUT_WIDTH,
	.output_height  = PSC_OUTPUT_HEIGHT,
	.output_display = PSC_OUTPUT_DISPLAY,
	.memory_unit    = PSC_MEMORY_UNIT,
	.root_pid       = PSC_ROOT_PID,
	.max_children   = PSC_MAX_CHILDREN,
	.background     = PSC_BACKGROUND_COLOR,
	.zero_angle_pid = PSC_ZERO_ANGLE_PID,

	.max_mem = PSC_MAX_MEM_MAX,
	.min_mem = PSC_MAX_MEM_MIN,
	.max_cpu = PSC_MAX_CPU_MAX,
	.min_cpu = PSC_MAX_CPU_MIN,

	.tree = {
		.font_face  = PSC_TREE_FONT_FACE,
		.font_size  = PSC_TREE_FONT_SIZE,
		.font_color = PSC_TREE_FONT_COLOR,
		.radius_inc = PSC_TREE_RADIUS_INCREMENT,
		.sector     = PSC_TREE_SECTOR,
		.rotate     = PSC_TREE_ROTATE,
		.rotation   = PSC_TREE_ROTATION,
		.center     = PSC_TREE_CENTER,
	},

	.dot = {
		.radius = PSC_DOT_RADIUS,
		.border = PSC_DOT_BORDER,
		.fg_min = PSC_DOT_BORDER_COLOR_MIN,
		.fg_max = PSC_DOT_BORDER_COLOR_MAX,
		.bg_min = PSC_DOT_BACKGROUND_COLOR_MIN,
		.bg_max = PSC_DOT_BACKGROUND_COLOR_MAX,
	},

	.link = {
		.width     = PSC_LINK_WIDTH,
		.convexity = PSC_LINK_CONVIXITY,
		.color_min = PSC_LINK_COLOR_MIN,
		.color_max = PSC_LINK_COLOR_MAX
	},

	.toplists = {
		.center         = PSC_TOPLISTS_CENTER,
		.width          = PSC_TOPLISTS_WIDTH,
		.row_height     = PSC_TOPLISTS_ROW_HEIGHT,
		.font_size      = PSC_TOPLISTS_FONT_SIZE,
		.font_color     = PSC_TOPLISTS_FONT_COLOR,
		.font_face      = PSC_TOPLISTS_FONT_FACE,
		.pid_font_color = PSC_TOPLISTS_PID_FONT_COLOR,
		.column_padding = PSC_TOPLISTS_COLUMN_PADDING,

		.bar = {
			.width      = PSC_TOPLISTS_BAR_WIDTH,
			.height     = PSC_TOPLISTS_BAR_HEIGHT,
			.background = PSC_TOPLISTS_BAR_BG,
			.color      = PSC_TOPLISTS_BAR_COLOR,
		},

		.cpulist = {
			.show_header  = PSC_CPULIST_SHOW_HEADER,
			.name         = PSC_CPULIST_NAME,
			.label        = PSC_CPULIST_LABEL,
			.value_format = PSC_CPULIST_VALUE_FMT,
			.value        = PSC_CPULIST_BAR_VALUE,
		},

		.memlist = {
			.show_header  = PSC_MEMLIST_SHOW_HEADER,
			.name         = PSC_MEMLIST_NAME,
			.label        = PSC_MEMLIST_LABEL,
			.value_format = PSC_MEMLIST_VALUE_FMT,
			.value        = PSC_MEMLIST_BAR_VALUE,
		},

	}
};
