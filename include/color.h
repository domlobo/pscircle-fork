#pragma once

#include "config.h"

struct color_t {
	double r;
	double g;
	double b;
	double a;
};

struct color_t color_from_hex(const char *hstr);

struct color_t fun();

