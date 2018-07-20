#include <cmath>

#include "gtest/gtest.h"

extern "C" {
#include "reals.h"
}

#define EPS PSC_EPS

using namespace std;
using namespace ::testing;

TEST(reals_macro, no_values) {
	reals_t items = REALS();
	EXPECT_EQ(items.size, 0u);
}

TEST(reals_macro, single_values) {
	reals_t items = REALS(30);
	EXPECT_EQ(items.size, 1u);
	EXPECT_NEAR(items.data[0], 30, EPS);
}

TEST(reals_macro, multiple_values) {
	reals_t items = REALS(30,40,50);
	EXPECT_EQ(items.size, 3u);
	EXPECT_NEAR(items.data[0], 30, EPS);
	EXPECT_NEAR(items.data[1], 40, EPS);
	EXPECT_NEAR(items.data[2], 50, EPS);
}

TEST(reals_add, below_max_cout) {
	reals_t items = REALS();
	reals_add(&items, 30);
	reals_add(&items, 40);
	reals_add(&items, 50);

	EXPECT_EQ(items.size, 3u);
	EXPECT_NEAR(items.data[0], 30, EPS);
	EXPECT_NEAR(items.data[1], 40, EPS);
	EXPECT_NEAR(items.data[2], 50, EPS);
}

TEST(reals_add, above_max_cout) {
	reals_t items = REALS();
	for (size_t i = 0; i < PSC_REALS_COUNT + 2; ++i)
		reals_add(&items, i);

	EXPECT_EQ(items.size, (size_t)PSC_REALS_COUNT);
	for (size_t i = 0; i < PSC_REALS_COUNT; ++i)
		EXPECT_NEAR(items.data[i], i, EPS);
}

TEST(reals_to_string, single_value) {
	reals_t items = REALS(30);

	char *buf = reals_to_string(items);
	ASSERT_NE(buf, nullptr);

	EXPECT_STREQ(buf, "30");

	free(buf);
}

TEST(reals_to_string, multiple_values) {
	reals_t items = REALS(30,40,50);

	char *buf = reals_to_string(items);
	ASSERT_NE(buf, nullptr);

	EXPECT_STREQ(buf, "30,40,50");

	free(buf);
}

