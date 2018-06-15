#pragma once

#include <stdbool.h>

typedef struct {
	double nx;
	double ny;
	double r;
} ppoint_t;

ppoint_t
ppoint_from_radial(double phi, double rho);

bool
ppoint_codirectinal(ppoint_t a, ppoint_t b);

ppoint_t
ppoint_normal(ppoint_t a, bool up);

ppoint_t
ppoint_add(ppoint_t a, ppoint_t b);
