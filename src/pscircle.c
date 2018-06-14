#include <stdio.h>
#include "color.h"

int main(int argc, char *argv[])
{
	/* fprintf(stderr, "%d\n", fun()); */
	struct color_t c = color_from_hex("0A141E28");
	/* struct color_t c = fun(); */

	fprintf(stderr, "r = \t %lf\n", c.r);
	fprintf(stderr, "g = \t %lf\n", c.g);
	fprintf(stderr, "b = \t %lf\n", c.b);
	fprintf(stderr, "a = \t %lf\n", c.a);
	return 0;
}
