#include "config.h"
#include <stdint.h>


#ifdef PSC_USE_FLOAT
#define R(x) x##f
typedef float real_t;
#else
#define R(x) x
typedef double real_t;
#endif

typedef PSC_NODE_COUNT_TYPE nnodes_t;

