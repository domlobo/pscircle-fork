#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "color.h"

struct color_t color_from_hex(const char *hstr)
{
	uint32_t r = 0;
	uint32_t g = 0;
	uint32_t b = 0;
	uint32_t a = 255;

	switch (strlen(hstr)) {
		case 3:
			if (sscanf(hstr, "%01x%01x%01x", &r, &g, &b) != 3)
				goto error;

			r *= 0x11;
			g *= 0x11;
			b *= 0x11;

			break;

		case 6:
			if (sscanf(hstr, "%02x%02x%02x", &r, &g, &b) != 3)
				goto error;
			break;
		case 8:
			if (sscanf(hstr, "%02x%02x%02x%02x", &r, &g, &b, &a) != 4)
				goto error;
			break;
		default:
			goto error;
	}

	struct color_t col = {
		.r = (double) r / 255,
		.g = (double) g / 255,
		.b = (double) b / 255,
		.a = (double) a / 255
	};

	return col;

error:
	fprintf(stderr, "Can not parse HEX color: %s\n", hstr);
	exit(EXIT_FAILURE);
}

