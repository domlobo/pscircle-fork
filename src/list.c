#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "list.h"

#define ITEM_LENGHT 16

char *
list_real_to_string(list_t items)
{
	char *buf = (char *) calloc(PSC_ARGLIST_LENGHT * ITEM_LENGHT, sizeof(char));
	char *end = buf;
	bool comma = false;

	for (size_t i = 0; i < PSC_ARGLIST_LENGHT; ++i) {
		real_t item = items.d[i];
		if (item == 0) {
			if (i == 0)
				end += snprintf(end, ITEM_LENGHT, "0");
			break;
		}

		if (comma)
			end += snprintf(end, ITEM_LENGHT, ",%.0f", item);
		else
			end += snprintf(end, ITEM_LENGHT, "%.0f", item);

		comma = true;
	}

	return buf;
}
