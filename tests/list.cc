#include <cmath>

#include "gtest/gtest.h"

extern "C" {
#include "list.h"
}

#define EPS PSC_EPS

using namespace std;
using namespace ::testing;

TEST(list_real_to_string, single_value) {
	list_t items = {0};
	items.d[0] = 30;

	char *buf = list_real_to_string(items);
	ASSERT_NE(buf, nullptr);

	EXPECT_STREQ(buf, "30");

	free(buf);
}

TEST(list_real_to_string, multiple_values) {
	list_t items = {0};
	items.d[0] = 30;
	items.d[1] = 40;
	items.d[2] = 50;

	char *buf = list_real_to_string(items);
	ASSERT_NE(buf, nullptr);

	EXPECT_STREQ(buf, "30,40,50");

	free(buf);
}

TEST(list_real_to_string, no_values) {
	list_t items = {0};

	char *buf = list_real_to_string(items);
	ASSERT_NE(buf, nullptr);

	EXPECT_STREQ(buf, "0");

	free(buf);
}
