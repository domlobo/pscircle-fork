#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "timing.h"
#include "config.h"

#define TIME_FMT "%15s: %.4lf seconds\n"

void
tm_start(timing_t *tm)
{
#ifdef PSC_PRINT_TIME
	tm->t = clock();
	tm->total = 0;
#endif
}

void
tm_tick(timing_t *tm, const char *message)
{
#ifdef PSC_PRINT_TIME
	clock_t t2 = clock();

	assert(tm);
	assert(message);
	double dt = (double) (t2 - tm->t) / CLOCKS_PER_SEC;
	if (dt < 0)
		dt = 0;

	fprintf(stderr, TIME_FMT, message, dt);

	tm->total += dt;
	tm->t += clock();
#endif
}

void
tm_total(timing_t *tm)
{
#ifdef PSC_PRINT_TIME
	fprintf(stderr, TIME_FMT, "total", tm->total);
#endif
}
