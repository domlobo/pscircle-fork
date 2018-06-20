#include <assert.h>
#include "argparser.h"

#include <stdio.h>
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

	.max_mem = PSC_MEM_MAX,
	.min_mem = PSC_MEM_MIN,
	.max_cpu = PSC_CPU_MAX,
	.min_cpu = PSC_CPU_MIN,

	.tree = {
		.font_face  = PSC_TREE_FONT_FACE,
		.font_size  = PSC_TREE_FONT_SIZE,
		.font_color = PSC_TREE_FONT_COLOR,
		.radius_inc = PSC_TREE_RADIUS_INCREMENT,
		.sector     = PSC_TREE_SECTOR,
		.rotate     = PSC_TREE_ROTATE,
		.rotation   = PSC_TREE_ROTATION,
		.center     = PSC_TREE_CENTER,
		.zero_pid   = PSC_ZERO_ANGLE_PID,
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
		.visible        = PSC_TOPLISTS_VISIBLE,
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

void
parse_cmdline(int argc, char const * argv[])
{
	assert(argv);
	assert(argc >= 1);

	if (argc == 1)
		return;

	argparser_t argp = {0};
	argparser_init(&argp);

	ARG(&argp, "--output", config.output, parser_string, PSC_OUTPUT,
		"Path to output image. If it's not set, X11 root window is used");
	ARG(&argp, "--output-display", config.output_display, parser_string, PSC_OUTPUT_DISPLAY,
		"Name of X11 display to draw the image to");
	ARGQ(&argp, "--output-width", config.output_width, parser_ulong, PSC_OUTPUT_WIDTH,
		"Width(px) of output image or X11 root window");
	ARGQ(&argp, "--output-height", config.output_height, parser_ulong, PSC_OUTPUT_HEIGHT,
		"Height(px) of output image or X11 root window");

	ARGQ(&argp, "--root-pid", config.root_pid, parser_long, PSC_ROOT_PID,
		"PID of the root process");
	ARGQ(&argp, "--max-children", config.max_children, parser_long, PSC_MAX_CHILDREN,
		"Maximum number of child proceceses.");

	ARGQ(&argp, "--memory-unit", config.memory_unit, parser_memory_unit, PSC_MEMORY_UNIT,
		"Unit of memeory (B, K, M, G, T) used in RSS memory column");
	ARGQ(&argp, "--memory-min-value", config.min_mem, parser_memory, PSC_MEM_MIN,
			"Processes with RSS below specified value will have --dot-border-color-min "
			"and --link-color-min color");
	ARGQ(&argp, "--memory-max-value", config.max_mem, parser_memory, PSC_MEM_MAX,
			"Processes with RSS above specified value will have --dot-border-color-max "
			"and --link-color-max color");
	ARGQ(&argp, "--cpu-min-value", config.min_cpu, parser_real, PSC_CPU_MIN,
			"Processes with PCPU below specified value will have --dot-color-min color");
	ARGQ(&argp, "--cpu-max-value", config.max_cpu, parser_real, PSC_CPU_MAX,
			"Processes with PCPU above specified value will have --dot-color-max color");

	ARG(&argp, "--background-color", config.background, parser_color, color_to_hex((color_t) PSC_BACKGROUND_COLOR),
			"Image backgound color");

	ARG(&argp, "--tree-center", config.tree.center, parser_point, point_to_str((point_t) PSC_TREE_CENTER),
			"X:Y Position of a tree center from the center of image");
	ARGQ(&argp, "--tree-radius-increment", config.tree.radius_inc, parser_real, PSC_TREE_RADIUS_INCREMENT,
			"The diffrence between radii of concentric circiles of the tree");
	ARGQ(&argp, "--tree-sector-angle", config.tree.sector, parser_real, PSC_TREE_SECTOR,
			"Tree vertices will be displayed inside the sector with specified angle");
	ARGQ(&argp, "--tree-rotate", config.tree.rotate, parser_bool, PSC_TREE_ROTATE,
			"Tree will be rotated to the angle specified in --tree-rotation-angle");
	ARGQ(&argp, "--tree-rotation-angle", config.tree.rotation, parser_real, PSC_TREE_ROTATION,
			"Rotation angle of a tree (radians). If --tree-rotate is not set, the tree will "
			"be rotated to --tree-zero-angle-pid (if it differs from --root-pid) or to "
			"the angle of the widest child");
	ARGQ(&argp, "--tree-zero-anlge-pid", config.tree.zero_pid, parser_long, PSC_ZERO_ANGLE_PID,
			"Tree will be rotated so that specified process is positioned at 0 rad, "
			"unless --tree-rotate is set. If --tree-zero-angle-pid equals to --root-pid "
			"the tree is rotated to so that its widest node is 0 rad");
	ARGQ(&argp, "--tree-font-size", config.tree.font_size, parser_real, PSC_TREE_FONT_SIZE,
			"Font size of the tree process names");
	ARG(&argp, "--tree-font-face", config.tree.font_face, parser_string, PSC_TREE_FONT_FACE,
			"Font face of the tree process names");
	ARG(&argp, "--tree-font-color", config.tree.font_color, parser_color, color_to_hex((color_t) PSC_TREE_FONT_COLOR),
			"The color of the tree process names");

	ARGQ(&argp, "--dot-radius", config.dot.radius, parser_real, PSC_DOT_RADIUS,
			"Radius of the dots (px)");
	ARGQ(&argp, "--dot-border-width", config.dot.border, parser_real, PSC_DOT_BORDER,
			"Width of dots borsers (px)");
	ARG(&argp, "--dot-color-min", config.dot.bg_min, parser_color, color_to_hex((color_t) PSC_DOT_BACKGROUND_COLOR_MIN),
			"Backgound color of the dots. This value corresponds to --cpu-max-value");
	ARG(&argp, "--dot-color-max", config.dot.bg_max, parser_color, color_to_hex((color_t) PSC_DOT_BACKGROUND_COLOR_MAX),
			"Backgound color of the dots. This value corresponds to --cpu-max-value");
	ARG(&argp, "--dot-border-color-min", config.dot.fg_min, parser_color, color_to_hex((color_t) PSC_DOT_BORDER_COLOR_MIN),
			"Border color of the dots. This value corresponds to --memory-min-value");
	ARG(&argp, "--dot-border-color-max", config.dot.fg_max, parser_color, color_to_hex((color_t) PSC_DOT_BORDER_COLOR_MAX),
			"Border color of the dots. This value corresponds to --memory-max-value");

	ARGQ(&argp, "--link-width", config.link.width, parser_real, PSC_LINK_WIDTH,
			"Width of the curves between dots");
	ARGQ(&argp, "--link-convexity", config.link.convexity, parser_real, PSC_LINK_CONVIXITY,
			"Convexity of links curves. Curve control points are offset from the "
			"centers of the dots to the distance of --tree-radius-increment times "
			"this value");
	ARG(&argp, "--link-color-min", config.link.color_min, parser_color, color_to_hex((color_t) PSC_LINK_COLOR_MIN),
			"Color of the curves betwwen dots. this value corresponds to --memory-min-value");
	ARG(&argp, "--link-color-max", config.link.color_max, parser_color, color_to_hex((color_t) PSC_LINK_COLOR_MAX),
			"Color of the curves betwwen dots. this value corresponds to --memory-max-value");

	ARGQ(&argp, "--toplists-show", config.toplists.visible, parser_bool, PSC_TOPLISTS_VISIBLE,
			"Show two lists of the processes with highest PCPU and RSS values");
	ARG(&argp, "--toplists-center", config.toplists.center, parser_point, point_to_str((point_t) PSC_TOPLISTS_CENTER),
			"X:Y position of the center of lists from the center of the image");
	ARGQ(&argp, "--toplists-width", config.toplists.width, parser_real, PSC_TOPLISTS_WIDTH,
			"Horizontal distance between the dots in two lists");
	ARGQ(&argp, "--toplists-row-height", config.toplists.row_height, parser_real, PSC_TOPLISTS_ROW_HEIGHT,
			"Hight of each row in toplist (px)");
	ARGQ(&argp, "--toplists-font-size", config.toplists.font_size, parser_real, PSC_TOPLISTS_FONT_SIZE,
			"Font size of the text in toplists");
	ARG(&argp, "--toplists-font-color", config.toplists.font_color, parser_color, color_to_hex((color_t) PSC_TOPLISTS_FONT_COLOR),
			"Font color of the text in toplists");
	ARG(&argp, "--toplists-pid-font-color", config.toplists.pid_font_color, parser_color, color_to_hex((color_t) PSC_TOPLISTS_PID_FONT_COLOR),
			"Font of processes PIDs in toplists");
	ARG(&argp, "--toplists-font-face", config.toplists.font_face, parser_string, PSC_TOPLISTS_FONT_FACE,
			"Font of the text in toplists");
	ARGQ(&argp, "--toplists-column-padding", config.toplists.column_padding, parser_real, PSC_TOPLISTS_COLUMN_PADDING,
			"Font of the text in toplists");

	ARGQ(&argp, "--toplists-bar-width", config.toplists.bar.width, parser_real, PSC_TOPLISTS_BAR_WIDTH,
			"Width of the percentage bar and legend markers in each row");
	ARGQ(&argp, "--toplists-bar-height", config.toplists.bar.height, parser_real, PSC_TOPLISTS_BAR_HEIGHT,
			"Height of the percentage bar");
	ARG(&argp, "--toplists-bar-background", config.toplists.bar.background, parser_color, color_to_hex((color_t) PSC_TOPLISTS_BAR_BG),
			"Backgound color of the percentage bar");
	ARG(&argp, "--toplists-bar-colot", config.toplists.bar.color, parser_color, color_to_hex((color_t) PSC_TOPLISTS_BAR_COLOR),
			"Foreground color of the percentage bar");

	ARGQ(&argp, "--cpulist-show-header", config.toplists.cpulist.show_header, parser_bool, PSC_CPULIST_SHOW_HEADER,
			"Show the header of the CPU toplist");
	ARG(&argp, "--cpulist-name", config.toplists.cpulist.name, parser_string, PSC_CPULIST_NAME,
			"The string to the left of the progress bar of CPU toplist");
	ARG(&argp, "--cpulist-label", config.toplists.cpulist.label, parser_string, PSC_CPULIST_LABEL,
			"The string to the right of the progress bar of CPU toplist");
	ARGQ(&argp, "--cpulist-bar-value", config.toplists.cpulist.value, parser_real, PSC_CPULIST_BAR_VALUE,
			"The value (0<=x<=1) of percentage bar of CPU toplist");

	ARGQ(&argp, "--memlist-show-header", config.toplists.memlist.show_header, parser_bool, PSC_MEMLIST_SHOW_HEADER,
			"Show the header of the MEM toplist");
	ARG(&argp, "--memlist-name", config.toplists.memlist.name, parser_string, PSC_MEMLIST_NAME,
			"The string to the left of the progress bar of MEM toplist");
	ARG(&argp, "--memlist-label", config.toplists.memlist.label, parser_string, PSC_MEMLIST_LABEL,
			"The string to the right of the progress bar of MEM toplist");
	ARGQ(&argp, "--memlist-bar-value", config.toplists.memlist.value, parser_real, PSC_MEMLIST_BAR_VALUE,
			"The value (0<=x<=1) of percentage bar of MEM toplist");

	argparser_parse(&argp, argc, argv);
}
