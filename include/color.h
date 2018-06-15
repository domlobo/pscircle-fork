#pragma once

#include "config.h"

typedef struct {
	double r;
	double g;
	double b;
	double a;
} color_t;

color_t
color_from_hex(const char *hstr);

color_t
color_between(color_t a, color_t b, double k);
