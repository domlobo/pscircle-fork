#define _GNU_SOURCE

#include <assert.h>
#include <math.h>
#include <float.h>

#include "ppoint.h"

#include "config.h"

bool is_valid(struct ppoint_t p)
{
	if (p.r < 0)
		return false;

	if (p.r == 0)
		return p.nx == 0 && p.ny == 0;

	double d = sqrt(p.nx*p.nx + p.ny*p.ny);
	return fabs(d - 1.) < PSC_EPS;
}

struct ppoint_t
ppoint_from_radial(double phi, double rho)
{
	struct ppoint_t pp = {0};
	pp.r = rho;

	if (rho != 0)
		sincos(phi, &pp.ny, &pp.nx);

	assert(is_valid(pp));

	return pp;
}

bool
ppoint_codirectinal(struct ppoint_t a, struct ppoint_t b)
{
	assert(is_valid(a));
	assert(is_valid(b));

	if (fabs(a.r) < PSC_EPS || fabs(b.r) < PSC_EPS)
		return true;

	return (fabs(a.nx - b.nx) < PSC_EPS) && (fabs(a.ny - b.ny) < PSC_EPS);
}

struct ppoint_t
ppoint_normal(struct ppoint_t a, bool up)
{
	assert(is_valid(a));

	struct ppoint_t pp = {0};
	if (fabs(a.r) < PSC_EPS)
		return pp;

	pp.r = 1;

	if (up) {
		pp.nx = -a.ny;
		pp.ny = a.nx;
	} else {
		pp.nx = a.ny;
		pp.ny = -a.nx;
	}

	assert(is_valid(pp));

	return pp;
}

struct ppoint_t
ppoint_add(struct ppoint_t a, struct ppoint_t b)
{
	assert(is_valid(a));
	assert(is_valid(b));

	double x = a.r*a.nx + b.r*b.nx;
	double y = a.r*a.ny + b.r*b.ny;
	double r = sqrt(x*x + y*y);

	struct ppoint_t pp = {0};

	if (fabs(r) > PSC_EPS) {
		pp.r = r;
		pp.nx = x / r;
		pp.ny = y / r;
	} else {
		pp.r = 0;
		pp.nx = 0;
		pp.ny = 0;
	}

	assert(is_valid(pp));
	
	return pp;
}
