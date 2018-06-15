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
	struct ppoint_t p = {0};
	p.r = rho;

	if (rho != 0)
		sincos(phi, &p.ny, &p.nx);

	assert(is_valid(p));

	return p;
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

	struct ppoint_t p = {0};
	if (fabs(a.r) < PSC_EPS)
		return p;

	p.r = 1;

	if (up) {
		p.nx = -a.ny;
		p.ny = a.nx;
	} else {
		p.nx = a.ny;
		p.ny = -a.nx;
	}

	assert(is_valid(p));

	return p;
}

struct ppoint_t
ppoint_add(struct ppoint_t a, struct ppoint_t b)
{
	assert(is_valid(a));
	assert(is_valid(b));

	double x = a.r*a.nx + b.r*b.nx;
	double y = a.r*a.ny + b.r*b.ny;
	double r = sqrt(x*x + y*y);

	struct ppoint_t p = {0};

	if (fabs(r) > PSC_EPS) {
		p.r = r;
		p.nx = x / r;
		p.ny = y / r;
	} else {
		p.r = 0;
		p.nx = 0;
		p.ny = 0;
	}

	assert(is_valid(p));
	
	return p;
}
