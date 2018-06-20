#include <stdlib.h>
#include <stdio.h>

#include "point.h"

#define POINT_BUFSIZE 25

char *
point_to_str(point_t p)
{
	char *buf = calloc(POINT_BUFSIZE, sizeof(char));
	snprintf(buf, POINT_BUFSIZE, "%.1f:%.1f", p.x, p.y);
	return buf;
}
