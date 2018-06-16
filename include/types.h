#include "config.h"
#include <stdint.h>

#include <sys/types.h>

#ifdef PSC_USE_FLOAT
#define R(x) x##f
typedef float real_t;
#define PSC_EPS FLT_EPSILON
#else
#define R(x) x
typedef double real_t;
#define PSC_EPS DBL_EPSILON
#endif

typedef PSC_NODE_COUNT_TYPE nnodes_t;

