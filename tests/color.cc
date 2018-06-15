#include "gtest/gtest.h"

extern "C" {
#include "color.h"
}

#define EPS 1e-15

using namespace std;
using namespace ::testing;

TEST(color_from_hex, hex_8_digits) {
	color_t c = color_from_hex("0A141E28");
	EXPECT_NEAR(c.r, 10./255, EPS);
	EXPECT_NEAR(c.g, 20./255, EPS);
	EXPECT_NEAR(c.b, 30./255, EPS);
	EXPECT_NEAR(c.a, 40./255, EPS);
}

TEST(color_from_hex, hex_6_digits) {
	color_t c = color_from_hex("0A141E");
	EXPECT_NEAR(c.r, 10./255, EPS);
	EXPECT_NEAR(c.g, 20./255, EPS);
	EXPECT_NEAR(c.b, 30./255, EPS);
	EXPECT_NEAR(c.a, 1, EPS);
}

TEST(color_from_hex, hex_3_digits) {
	color_t c = color_from_hex("ABC");
	EXPECT_NEAR(c.r, 170./255, EPS);
	EXPECT_NEAR(c.g, 187./255, EPS);
	EXPECT_NEAR(c.b, 204./255, EPS);
	EXPECT_NEAR(c.a, 1, EPS);
}
