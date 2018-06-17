#include "gtest/gtest.h"

extern "C" {
#include "ptree.h"
#include "cfg.h"
}

#define EPS PSC_EPS

using namespace std;
using namespace ::testing;

class ptree_test: public Test
{
public:
	ptree_test() {};
	virtual ~ptree_test() {};

	FILE *input;

	ptree_t *ptree;

	virtual void SetUp() {
		ASSERT_EQ(PSC_TOPLIST_MAX_ROWS, 4);

		config.max_children = 90;
		config.root_pid = 0;
		config.memory_unit = 1;

		input = tmpfile();

		ptree = new ptree_t();
	}

	virtual void TearDown(){
		fclose(input);
		ptree_dinit(ptree);

		delete ptree;
	}

	void create(const char *s) {
		fputs(s, input);
		rewind(input);

		ptree_init(ptree, input);
	}
};

TEST_F(ptree_test, read__pid_is_set) {
	create(
"1     0  3.14  4212 systemd\n"
	);

	ASSERT_NE(ptree->root,  nullptr);
	auto p = (pnode_t *)ptree->root->node.first;
	ASSERT_NE(p,  nullptr);

	EXPECT_EQ(p->pid, 1);
}

TEST_F(ptree_test, read__ppid_is_set) {
	create(
"1     0  3.14  4212 systemd\n"
	);

	ASSERT_NE(ptree->root,  nullptr);
	auto p = (pnode_t *)ptree->root->node.first;
	ASSERT_NE(p,  nullptr);

	EXPECT_EQ(p->ppid, 0);
}

TEST_F(ptree_test, read_cpu_is_set) {
	create(
"1     0  3.14  4212 systemd\n"
	);

	ASSERT_NE(ptree->root,  nullptr);
	auto p = (pnode_t *)ptree->root->node.first;
	ASSERT_NE(p,  nullptr);

	EXPECT_NEAR(p->cpu, 3.14, EPS);
}

TEST_F(ptree_test, read__mem_in_mb) {
	config.memory_unit = 2;

	create(
"1     0  3.14  4212 systemd\n"
	);

	ASSERT_NE(ptree->root,  nullptr);
	auto p = (pnode_t *)ptree->root->node.first;
	ASSERT_NE(p,  nullptr);

	EXPECT_EQ(p->mem, 4212ul * 1024 * 1024);
}

TEST_F(ptree_test, read__comm_is_set) {
	create(
"1     0  3.14  4212 systemd\n"
	);

	ASSERT_NE(ptree->root,  nullptr);
	auto p = (pnode_t *)ptree->root->node.first;
	ASSERT_NE(p,  nullptr);

	EXPECT_STREQ(p->comm, "systemd");
}

TEST_F(ptree_test, read__multiple_words) {
	create(
"1     0  3.14  4212 systemd abc\n"
	);

	ASSERT_NE(ptree->root,  nullptr);
	auto p = (pnode_t *)ptree->root->node.first;
	ASSERT_NE(p,  nullptr);

	EXPECT_STREQ(p->comm, "systemd");
}

TEST_F(ptree_test, read__multiple_words_longer_than_max) {
	string s = "1     0  3.14  4212 ";
	string n = "";
	for (size_t i = 0; i < PSC_MAX_NAME_LENGHT - 1; ++i)
		n += std::to_string(i % 10);
	s = s + n + " abc def\n";

	create(s.c_str());

	ASSERT_NE(ptree->root,  nullptr);
	auto p = (pnode_t *)ptree->root->node.first;
	ASSERT_NE(p,  nullptr);

	EXPECT_EQ(string(p->comm), n);
}

TEST_F(ptree_test, read__name_is_too_long) {
	string s = "1     0  3.14  4212 ";
	string n = "";
	for (size_t i = 0; i < PSC_MAX_NAME_LENGHT - 1; ++i)
		n += std::to_string(i % 10);
	s = s + n + "a\n";

	create(s.c_str());

	ASSERT_NE(ptree->root,  nullptr);
	auto p = (pnode_t *)ptree->root->node.first;
	ASSERT_NE(p,  nullptr);

	EXPECT_EQ(string(p->comm), n);
}

TEST_F(ptree_test, links__root_pid_found) {
	config.root_pid = 1;

	create(
"1     0  0.0  0 p1\n"
"2     1  0.0  0 p2\n"
"3     1  0.0  0 p3\n"
"4     2  0.0  0 p4\n"
	);

	auto r = ptree->root;
	ASSERT_NE(r,  nullptr);
	auto p = (pnode_t *)r->node.first;
	ASSERT_NE(p,  nullptr);

	EXPECT_STREQ(r->comm, "p1");
	EXPECT_STREQ(p->comm, "p2");
}

TEST_F(ptree_test, links__root_pid_not_found__empty_tree) {
	config.root_pid = 10;

	create(
"1     0  0.0  0 p1\n"
"2     1  0.0  0 p2\n"
"3     1  0.0  0 p3\n"
"4     2  0.0  0 p4\n"
	);

	auto r = ptree->root;
	ASSERT_NE(r,  nullptr);
	auto p1 = (pnode_t *)r->node.first;
	EXPECT_EQ(p1,  nullptr);
}

TEST_F(ptree_test, links__too_much_processes__array_resized) {
	string s = "1     0  0.0  0 p1\n";
	size_t N = PSC_MAX_PROCS_COUNT;

	config.max_children = N + 10;
	for (size_t i = 2; i < N + 1; ++i) {
		s += std::to_string(i) + "  1 0.0 0 p" + std::to_string(i) + "\n";
	}

	create(s.c_str());

	auto r = ptree->root;
	ASSERT_NE(r,  nullptr);
	auto p1 = (pnode_t *)r->node.first;
	ASSERT_NE(p1,  nullptr);
	auto pf = (pnode_t *)p1->node.first;
	ASSERT_NE(pf,  nullptr);
	auto pl = (pnode_t *)p1->node.last;
	ASSERT_NE(pl,  nullptr);

	EXPECT_STREQ(p1->comm, "p1");
	EXPECT_STREQ(pf->comm, "p2");
	string last = "p" + std::to_string(N-1);
	EXPECT_STREQ(pl->comm, last.c_str());
}

TEST_F(ptree_test, mem_toplist__empty_rows) {
	create(
"1     0  1.0  1 p1\n"
"2     1  4.0  4 p2\n"
"3     1  3.0  3 p3\n"
	);

	auto l = ptree->mem_toplist;
	size_t i = 0;
	for (auto &name : {"p2", "p3", "p1"}) {
		ASSERT_NE(l[i],  nullptr);
		EXPECT_STREQ(l[i]->comm, name);
		i++;
	}

	for (size_t i = 4; i < PSC_TOPLIST_MAX_ROWS; ++i)
		ASSERT_EQ(l[i],  nullptr);
}

TEST_F(ptree_test, mem_toplist__toplist_smaller_than_max_rows) {
	create(
"1     0  1.0  10 p1\n"
"2     1  4.0  40 p2\n"
"3     1  3.0  30 p3\n"
"4     2  2.0  20 p4\n"
"5     4  3.2  32 p5\n"
"6     4  1.2  12 p6\n"
"7     4  2.2  22 p7\n"
	);

	auto l = ptree->mem_toplist;
	size_t i = 0;
	for (auto &name : {"p2", "p5"}) {
		ASSERT_NE(l[i],  nullptr);
		EXPECT_STREQ(l[i]->comm, name);
		i++;
	}
}

TEST_F(ptree_test, cpu_toplist__empty_rows) {
	create(
"1     0  1.0  1 p1\n"
"2     1  4.0  4 p2\n"
"3     1  3.0  3 p3\n"
	);

	auto l = ptree->cpu_toplist;
	size_t i = 0;
	for (auto &name : {"p2", "p3", "p1"}) {
		ASSERT_NE(l[i],  nullptr);
		EXPECT_STREQ(l[i]->comm, name);
		i++;
	}

	for (size_t i = 4; i < PSC_TOPLIST_MAX_ROWS; ++i)
		ASSERT_EQ(l[i],  nullptr);
}

TEST_F(ptree_test, cpu_toplist__toplist_smaller_than_max_rows) {
	create(
"1     0  1.0  10 p1\n"
"2     1  4.0  40 p2\n"
"3     1  3.0  30 p3\n"
"4     2  2.0  20 p4\n"
"5     4  3.2  32 p5\n"
"6     4  1.2  12 p6\n"
"7     4  2.2  22 p7\n"
	);

	auto l = ptree->cpu_toplist;
	size_t i = 0;
	for (auto &name : {"p2", "p5"}) {
		ASSERT_NE(l[i],  nullptr);
		EXPECT_STREQ(l[i]->comm, name);
		i++;
	}
}



TEST_F(ptree_test, stubs__too_much_procs) {
	config.max_children = 1;

	create(
"1     0  1.0  1 p1\n"
"2     1  4.0  4 p2\n"
"3     1  3.0  3 p3\n"
"4     1  5.0  2 p4\n"
	);

	ASSERT_NE(ptree, nullptr);
	auto r = ptree->root;
	ASSERT_NE(r, nullptr);
	auto p1 = (pnode_t *)r->node.first;
	ASSERT_NE(p1, nullptr);
	auto p2 = (pnode_t *)p1->node.first;
	ASSERT_NE(p2, nullptr);
	auto p3 = (pnode_t *)p2->node.next;
	ASSERT_NE(p3, nullptr);

	EXPECT_STREQ(p1->comm, "p1");
	EXPECT_STREQ(p2->comm, "p2");
	EXPECT_STREQ(p3->comm, "<2 omitted>");
	EXPECT_EQ(p3->mem, 3u*1024);
	EXPECT_NEAR(p3->cpu, 5, EPS);
}

