#pragma once

#include <stdbool.h>

struct ppoint_t {
	double nx;
	double ny;
	double r;
};

struct ppoint_t
ppoint_from_radial(double phi, double rho);

bool
ppoint_codirectinal(struct ppoint_t a, struct ppoint_t b);

struct ppoint_t
ppoint_normal(struct ppoint_t a, bool up);

struct ppoint_t
ppoint_add(struct ppoint_t a, struct ppoint_t b);
