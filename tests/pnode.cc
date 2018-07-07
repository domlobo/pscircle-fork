#include "gtest/gtest.h"

extern "C" {
#include "pnode.h"
#include "cfg.h"
}

#define EPS PSC_EPS

using namespace std;
using namespace ::testing;

TEST(pnode_is_null, empty_pnode) {
	pnode_t n = {0};
	EXPECT_TRUE(pnode_is_null(&n));
}

TEST(pnode_is_null, not_empty_pnode) {
	pnode_t n = {0};
	n.pid = 1;
	EXPECT_FALSE(pnode_is_null(&n));
}

TEST(pnode_cpu_percentage, above_max) {
	config.max_cpu = 0.5;
	config.min_cpu = 0.2;

	pnode_t n = {0};
	n.cpu = 0.8;

	EXPECT_NEAR(pnode_cpu_percentage(&n), 1, EPS);
}

TEST(pnode_cpu_percentage, below_min) {
	config.max_cpu = 0.5;
	config.min_cpu = 0.2;

	pnode_t n = {0};
	n.cpu = 0.1;

	EXPECT_NEAR(pnode_cpu_percentage(&n), 0, EPS);
}

TEST(pnode_cpu_percentage, between_min_max) {
	config.max_cpu = 0.5;
	config.min_cpu = 0.2;

	pnode_t n = {0};
	n.cpu = 0.4;

	EXPECT_NEAR(pnode_cpu_percentage(&n), (0.4 - 0.2) / (0.5 - 0.2), EPS);
}

TEST(pnode_mem_percentage, above_max) {
	config.max_mem = 5;
	config.min_mem = 2;

	pnode_t n = {0};
	n.mem = 8;

	EXPECT_NEAR(pnode_mem_percentage(&n), 1, EPS);
}

TEST(pnode_mem_percentage, below_min) {
	config.max_mem = 5;
	config.min_mem = 2;

	pnode_t n = {0};
	n.mem = 1;

	EXPECT_NEAR(pnode_mem_percentage(&n), 0, EPS);
}

TEST(pnode_mem_percentage, between_min_max) {
	config.max_mem = 5;
	config.min_mem = 2;

	pnode_t n = {0};
	n.mem = 4;

	EXPECT_NEAR(pnode_mem_percentage(&n), (4.0 - 2.0) / (5.0 - 2.0), EPS);
}
