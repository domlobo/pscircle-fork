#include "config.h"

#include "cfg.h"

cfg_t config = {
	.max_name_lenght = PSC_MAX_CHILDREN,
	.memory_unit = PSC_MEMORY_UNIT,
	.initial_process_count = PSC_INITIAL_PROCESS_COUNT,
	.root_pid = PSC_ROOT_PID,
	.max_children = PSC_MAX_CHILDREN,
	.toplist_max_rows = PSC_TOPLIST_MAX_ROWS
};
