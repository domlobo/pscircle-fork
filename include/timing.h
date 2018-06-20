#pragma once

#include <stdbool.h>
#include <time.h>

typedef struct {
	clock_t t;
	double total;
} timing_t;

void
tm_start(timing_t *tm);

void
tm_tick(timing_t *tm, const char *message);

void
tm_total(timing_t *tm);
