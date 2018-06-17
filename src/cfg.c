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
	}
};
