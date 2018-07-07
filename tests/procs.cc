#include "gtest/gtest.h"

extern "C" {
#include "procs.h"
#include "cfg.h"
}

#define EPS PSC_EPS

using namespace std;
using namespace ::testing;

class procs_test: public Test
{
public:
	procs_test() {};
	virtual ~procs_test() {};

	FILE *fp;

	procs_t *procs;

	virtual void SetUp() {
		ASSERT_EQ(PSC_TOPLIST_MAX_ROWS, 5);

		config.max_children = 90;
		config.root_pid = 0;
		config.memory_unit = 1;

		fp = tmpfile();

		procs = new procs_t();
	}

	virtual void TearDown(){
		fclose(fp);
		procs_dinit(procs);

		delete procs;
	}

	void create(const char *s) {
		fputs(s, fp);
		rewind(fp);

		procs_init(procs, fp);
	}
};

TEST_F(procs_test, read__pid_is_set) {
	create(
"1     0  3.14  4212 systemd\n"
	);

	ASSERT_NE(procs->root,  nullptr);
	auto p = (pnode_t *)procs->root->node.first;
	ASSERT_NE(p,  nullptr);

	EXPECT_EQ(p->pid, 1);
}

TEST_F(procs_test, read__ppid_is_set) {
	create(
"1     0  3.14  4212 systemd\n"
	);

	ASSERT_NE(procs->root,  nullptr);
	auto p = (pnode_t *)procs->root->node.first;
	ASSERT_NE(p,  nullptr);

	EXPECT_EQ(p->ppid, 0);
}

TEST_F(procs_test, read_cpu_is_set) {
	create(
"1     0  3.14  4212 systemd\n"
	);

	ASSERT_NE(procs->root,  nullptr);
	auto p = (pnode_t *)procs->root->node.first;
	ASSERT_NE(p,  nullptr);

	EXPECT_NEAR(p->cpu, 3.14, EPS);
}

TEST_F(procs_test, read__mem_in_mb) {
	config.memory_unit = 2;

	create(
"1     0  3.14  4212 systemd\n"
	);

	ASSERT_NE(procs->root,  nullptr);
	auto p = (pnode_t *)procs->root->node.first;
	ASSERT_NE(p,  nullptr);

	EXPECT_EQ(p->mem, 4212ul * 1024 * 1024);
}

TEST_F(procs_test, read__comm_is_set) {
	create(
"1     0  3.14  4212 systemd\n"
	);

	ASSERT_NE(procs->root,  nullptr);
	auto p = (pnode_t *)procs->root->node.first;
	ASSERT_NE(p,  nullptr);

	EXPECT_STREQ(p->name, "systemd");
}

TEST_F(procs_test, read__multiple_words) {
	create(
"1     0  3.14  4212 systemd abc\n"
	);

	ASSERT_NE(procs->root,  nullptr);
	auto p = (pnode_t *)procs->root->node.first;
	ASSERT_NE(p,  nullptr);

	EXPECT_STREQ(p->name, "systemd");
}

TEST_F(procs_test, read__multiple_words_longer_than_max) {
	string s = "1     0  3.14  4212 ";
	string n = "";
	for (size_t i = 0; i < PSC_MAX_NAME_LENGHT - 1; ++i)
		n += std::to_string(i % 10);
	s = s + n + " abc def\n";

	create(s.c_str());

	ASSERT_NE(procs->root,  nullptr);
	auto p = (pnode_t *)procs->root->node.first;
	ASSERT_NE(p,  nullptr);

	EXPECT_EQ(string(p->name), n);
}

TEST_F(procs_test, read__name_is_too_long) {
	string s = "1     0  3.14  4212 ";
	string n = "";
	for (size_t i = 0; i < PSC_MAX_NAME_LENGHT - 1; ++i)
		n += std::to_string(i % 10);
	s = s + n + "a\n";

	create(s.c_str());

	ASSERT_NE(procs->root,  nullptr);
	auto p = (pnode_t *)procs->root->node.first;
	ASSERT_NE(p,  nullptr);

	EXPECT_EQ(string(p->name), n);
}

TEST_F(procs_test, links__root_pid_found) {
	config.root_pid = 1;

	create(
"1     0  0.0  0 p1\n"
"2     1  0.0  0 p2\n"
"3     1  0.0  0 p3\n"
"4     2  0.0  0 p4\n"
	);

	auto r = procs->root;
	ASSERT_NE(r,  nullptr);
	auto p = (pnode_t *)r->node.first;
	ASSERT_NE(p,  nullptr);

	EXPECT_STREQ(r->name, "p1");
	EXPECT_STREQ(p->name, "p2");
}

TEST_F(procs_test, links__root_pid_not_found__empty_tree) {
	config.root_pid = 10;

	create(
"1     0  0.0  0 p1\n"
"2     1  0.0  0 p2\n"
"3     1  0.0  0 p3\n"
"4     2  0.0  0 p4\n"
	);

	auto r = procs->root;
	ASSERT_NE(r,  nullptr);
	auto p1 = (pnode_t *)r->node.first;
	EXPECT_EQ(p1,  nullptr);
}

TEST_F(procs_test, links__too_much_processes__array_resized) {
	string s = "1     0  0.0  0 p1\n";
	size_t N = PSC_MAX_PROCS_COUNT;

	config.max_children = N + 10;
	for (size_t i = 2; i < N + 1; ++i) {
		s += std::to_string(i) + "  1 0.0 0 p" + std::to_string(i) + "\n";
	}

	create(s.c_str());

	auto r = procs->root;
	ASSERT_NE(r,  nullptr);
	auto p1 = (pnode_t *)r->node.first;
	ASSERT_NE(p1,  nullptr);
	auto pf = (pnode_t *)p1->node.first;
	ASSERT_NE(pf,  nullptr);
	auto pl = (pnode_t *)p1->node.last;
	ASSERT_NE(pl,  nullptr);

	EXPECT_STREQ(p1->name, "p1");
	EXPECT_STREQ(pf->name, "p2");
	string last = "p" + std::to_string(N-1);
	EXPECT_STREQ(pl->name, last.c_str());
}

TEST_F(procs_test, memlist__empty_rows) {
	create(
"1     0  1.0  1 p1\n"
"2     1  4.0  4 p2\n"
"3     1  3.0  3 p3\n"
	);

	auto l = procs->memlist;
	size_t i = 0;
	for (auto &name : {"p2", "p3", "p1"}) {
		ASSERT_FALSE(pnode_is_null(l + i));
		EXPECT_STREQ(l[i].name, name);
		i++;
	}

	for (size_t i = 4; i < PSC_TOPLIST_MAX_ROWS; ++i)
		ASSERT_TRUE(pnode_is_null(l + i));
}

TEST_F(procs_test, memlist__toplist_smaller_than_max_rows) {
	create(
"1     0  1.0  10 p1\n"
"2     1  4.0  40 p2\n"
"3     1  3.0  30 p3\n"
"4     2  2.0  20 p4\n"
"5     4  3.2  32 p5\n"
"6     4  1.2  12 p6\n"
"7     4  2.2  22 p7\n"
	);

	auto l = procs->memlist;
	size_t i = 0;
	for (auto &name : {"p2", "p5"}) {
		ASSERT_FALSE(pnode_is_null(l + i));
		EXPECT_STREQ(l[i].name, name);
		i++;
	}
}

TEST_F(procs_test, cpu_toplist__empty_rows) {
	create(
"1     0  1.0  1 p1\n"
"2     1  4.0  4 p2\n"
"3     1  3.0  3 p3\n"
	);

	auto l = procs->cpulist;
	size_t i = 0;
	for (auto &name : {"p2", "p3", "p1"}) {
		ASSERT_FALSE(pnode_is_null(l + i));
		EXPECT_STREQ(l[i].name, name);
		i++;
	}

	for (size_t i = 4; i < PSC_TOPLIST_MAX_ROWS; ++i)
		ASSERT_TRUE(pnode_is_null(l + i));
}

TEST_F(procs_test, cpu_toplist__toplist_smaller_than_max_rows) {
	create(
"1     0  1.0  10 p1\n"
"2     1  4.0  40 p2\n"
"3     1  3.0  30 p3\n"
"4     2  2.0  20 p4\n"
"5     4  3.2  32 p5\n"
"6     4  1.2  12 p6\n"
"7     4  2.2  22 p7\n"
	);

	auto l = procs->cpulist;
	size_t i = 0;
	for (auto &name : {"p2", "p5"}) {
		ASSERT_FALSE(pnode_is_null(l + i));
		EXPECT_STREQ(l[i].name, name);
		i++;
	}
}

TEST_F(procs_test, collapse__same_proccesses) {
	config.collapse_threads = true;

	create(
"1     0  1.0  1 p1\n"
"2     1  4.0  4 p2\n"
"3     1  3.0  3 p2\n"
"4     1  5.0  9 p3\n"
"5     1  8.0  3 p2\n"
	);

	ASSERT_NE(procs, nullptr);
	auto r = procs->root;
	ASSERT_NE(r, nullptr);
	auto p1 = (pnode_t *)r->node.first;
	ASSERT_NE(p1, nullptr);
	auto p2 = (pnode_t *)p1->node.first;
	ASSERT_NE(p2, nullptr);
	auto p3 = (pnode_t *)p2->node.next;
	ASSERT_NE(p3, nullptr);
	auto p4 = (pnode_t *)p3->node.next;
	EXPECT_EQ(p4, nullptr);

	EXPECT_STREQ(p1->name, "p1");
	EXPECT_STREQ(p2->name, "3 * p2");
	EXPECT_STREQ(p3->name, "p3");
	EXPECT_EQ(p2->mem, 4u*1024);
	EXPECT_NEAR(p2->cpu, 8, EPS);
}

TEST_F(procs_test, collapse__process_with_child) {
	config.collapse_threads = true;

	create(
"1     0  1.0  1 p1\n"
"2     1  4.0  4 p2\n"
"3     1  3.0  3 p2\n"
"4     1  5.0  9 p3\n"
"5     1  8.0  3 p2\n"
"6     3  3.0  3 p6\n"
	);

	ASSERT_NE(procs, nullptr);
	auto r = procs->root;
	ASSERT_NE(r, nullptr);
	auto p1 = (pnode_t *)r->node.first;
	ASSERT_NE(p1, nullptr);
	auto p2 = (pnode_t *)p1->node.first;
	ASSERT_NE(p2, nullptr);
	auto p3 = (pnode_t *)p2->node.next;
	ASSERT_NE(p3, nullptr);
	auto p31 = (pnode_t *)p3->node.first;
	ASSERT_NE(p31, nullptr);
	auto p4 = (pnode_t *)p3->node.next;
	EXPECT_NE(p4, nullptr);

	EXPECT_STREQ(p1->name, "p1");
	EXPECT_STREQ(p2->name, "2 * p2");
	EXPECT_STREQ(p3->name, "p2");
	EXPECT_STREQ(p31->name, "p6");
	EXPECT_STREQ(p4->name, "p3");
}

TEST_F(procs_test, stubs__cpu_and_mem_are_max) {
	config.max_children = 2;

	create(
"1     0  1.0  1 p1\n"
"2     1  4.0  4 p2\n"
"3     1  3.0  3 p3\n"
"4     1  5.0  9 p4\n"
"5     1  8.0  3 p5\n"
	);

	ASSERT_NE(procs, nullptr);
	auto r = procs->root;
	ASSERT_NE(r, nullptr);
	auto p1 = (pnode_t *)r->node.first;
	ASSERT_NE(p1, nullptr);
	auto p2 = (pnode_t *)p1->node.first;
	ASSERT_NE(p2, nullptr);
	auto p3 = (pnode_t *)p2->node.next;
	ASSERT_NE(p3, nullptr);
	auto p4 = (pnode_t *)p3->node.next;
	ASSERT_NE(p4, nullptr);

	EXPECT_STREQ(p1->name, "p1");
	EXPECT_STREQ(p2->name, "p2");
	EXPECT_STREQ(p3->name, "p3");
	EXPECT_STREQ(p4->name, "<2 omitted>");
	EXPECT_EQ(p4->mem, 9u*1024);
	EXPECT_NEAR(p4->cpu, 8, EPS);
}

TEST_F(procs_test, stubs__single_node_with_children) {
	config.max_children = 2;

	create(
"1     0  1.0  1 p1\n"
"2     1  4.0  4 p2\n"
"3     1  3.0  3 p3\n"
"4     1  5.0  9 p4\n"
"5     1  8.0  3 p5\n"
"6     5  3.0  3 p6\n"
	);

	ASSERT_NE(procs, nullptr);
	auto r = procs->root;
	ASSERT_NE(r, nullptr);
	auto p1 = (pnode_t *)r->node.first;
	ASSERT_NE(p1, nullptr);
	auto p2 = (pnode_t *)p1->node.first;
	ASSERT_NE(p2, nullptr);
	auto p3 = (pnode_t *)p2->node.next;
	ASSERT_NE(p3, nullptr);
	auto p4 = (pnode_t *)p3->node.next;
	ASSERT_NE(p4, nullptr);

	EXPECT_STREQ(p1->name, "p1");
	EXPECT_STREQ(p2->name, "p2");
	EXPECT_STREQ(p3->name, "p5");
	EXPECT_STREQ(p4->name, "<2 omitted>");
}

TEST_F(procs_test, stubs__some_nodes_with_children) {
	config.max_children = 2;

	create(
"1     0  1.0  1 p1\n"
"2     1  4.0  4 p2\n"
"3     1  3.0  3 p3\n"
"4     1  5.0  9 p4\n"
"7     4  3.0  3 p7\n"
"5     1  8.0  3 p5\n"
"6     5  3.0  3 p6\n"
	);

	ASSERT_NE(procs, nullptr);
	auto r = procs->root;
	ASSERT_NE(r, nullptr);
	auto p1 = (pnode_t *)r->node.first;
	ASSERT_NE(p1, nullptr);
	auto p2 = (pnode_t *)p1->node.first;
	ASSERT_NE(p2, nullptr);
	auto p3 = (pnode_t *)p2->node.next;
	ASSERT_NE(p3, nullptr);
	auto p4 = (pnode_t *)p3->node.next;
	ASSERT_NE(p4, nullptr);

	EXPECT_STREQ(p1->name, "p1");
	EXPECT_STREQ(p2->name, "p4");
	EXPECT_STREQ(p3->name, "p5");
	EXPECT_STREQ(p4->name, "<2 omitted>");
}

TEST_F(procs_test, stubs__all_with_children) {
	config.max_children = 2;

	create(
"1     0  1.0  1 p1\n"
"2     1  4.0  4 p2\n"
"9     2  3.0  3 p9\n"
"3     1  3.0  3 p3\n"
"8     3  3.0  3 p8\n"
"4     1  5.0  9 p4\n"
"7     4  3.0  3 p7\n"
"5     1  8.0  3 p5\n"
"6     5  3.0  3 p6\n"
	);

	ASSERT_NE(procs, nullptr);
	auto r = procs->root;
	ASSERT_NE(r, nullptr);
	auto p1 = (pnode_t *)r->node.first;
	ASSERT_NE(p1, nullptr);
	auto p2 = (pnode_t *)p1->node.first;
	ASSERT_NE(p2, nullptr);
	auto p3 = (pnode_t *)p2->node.next;
	ASSERT_NE(p3, nullptr);
	auto p4 = (pnode_t *)p3->node.next;
	ASSERT_NE(p4, nullptr);

	EXPECT_STREQ(p1->name, "p1");
	EXPECT_STREQ(p2->name, "p2");
	EXPECT_STREQ(p3->name, "p3");
	EXPECT_STREQ(p4->name, "<2 omitted>");
}

TEST_F(procs_test, stubs__single_proc_is_not_omited) {
	config.max_children = 2;

	create(
"1     0  1.0  1 p1\n"
"2     1  4.0  4 p2\n"
"3     1  3.0  3 p3\n"
"4     1  5.0  9 p4\n"
	);

	ASSERT_NE(procs, nullptr);
	auto r = procs->root;
	ASSERT_NE(r, nullptr);
	auto p1 = (pnode_t *)r->node.first;
	ASSERT_NE(p1, nullptr);
	auto p2 = (pnode_t *)p1->node.first;
	ASSERT_NE(p2, nullptr);
	auto p3 = (pnode_t *)p2->node.next;
	ASSERT_NE(p3, nullptr);
	auto p4 = (pnode_t *)p3->node.next;
	ASSERT_NE(p4, nullptr);

	EXPECT_STREQ(p1->name, "p1");
	EXPECT_STREQ(p2->name, "p2");
	EXPECT_STREQ(p3->name, "p3");
	EXPECT_STREQ(p4->name, "p4");
}

TEST_F(procs_test, collapse_stubs__lists_are_not_affected) {
	config.max_children = 1;
	config.collapse_threads = true;

	create(
"1     0  1.0  1 p1\n"
"2     1  4.0  4 p2\n"
"3     1  3.0  3 p3\n"
"4     1  5.0  9 p4\n"
"5     1  8.0  5 p4\n"
	);

	size_t i = 0;
	for (auto &name : {"p4", "p4", "p2", "p3", "p1"}) {
		ASSERT_FALSE(pnode_is_null(procs->cpulist + i));
		EXPECT_STREQ(procs->cpulist[i].name, name);
		i++;
	}

	i = 0;
	for (auto &name : {"p4", "p4", "p2", "p3", "p1"}) {
		ASSERT_FALSE(pnode_is_null(procs->memlist + i));
		EXPECT_STREQ(procs->memlist[i].name, name);
		i++;
	}

}

TEST_F(procs_test, find_by_name__existing_processes) {
	create(
"1     0  1.0  1 p1\n"
"2     1  4.0  4 p2\n"
"3     1  3.0  3 p3\n"
"4     1  5.0  2 p4\n"
"5     3  3.0  3 p5\n"
"6     3  3.0  3 p6\n"
"7     3  3.0  3 p7\n"
"8     4  5.0  2 p8\n"
	);

	auto c = procs_child_by_name(procs, "p6");
	ASSERT_NE(c, nullptr);
	EXPECT_STREQ(c->name, "p6");
}

TEST_F(procs_test, find_by_name__not_in_tree__returns_nullptr) {
	config.root_pid = 3;

	create(
"1     0  1.0  1 p1\n"
"2     1  4.0  4 p2\n"
"3     1  3.0  3 p3\n"
"4     1  5.0  2 p4\n"
"5     3  3.0  3 p5\n"
"6     3  3.0  3 p6\n"
"7     3  3.0  3 p7\n"
"8     4  5.0  2 p8\n"
	);

	auto c = procs_child_by_name(procs, "p8");
	EXPECT_EQ(c, nullptr);
}

TEST_F(procs_test, find_by_name__unknown_process__returns_nullptr) {
	create(
"1     0  1.0  1 p1\n"
"2     1  4.0  4 p2\n"
"3     2  3.0  3 p3\n"
"4     2  5.0  2 p4\n"
	);

	auto c = procs_child_by_name(procs, "p10");
	EXPECT_EQ(c, nullptr);
}

TEST_F(procs_test, find_by_pid__existing_processes) {
	create(
"1     0  1.0  1 p1\n"
"2     1  4.0  4 p2\n"
"3     1  3.0  3 p3\n"
"4     1  5.0  2 p4\n"
"5     3  3.0  3 p5\n"
"6     3  3.0  3 p6\n"
"7     3  3.0  3 p7\n"
"8     4  5.0  2 p8\n"
	);

	auto c = procs_child_by_pid(procs, 6);
	ASSERT_NE(c, nullptr);
	EXPECT_STREQ(c->name, "p6");
}

TEST_F(procs_test, find_by_pid__not_in_tree__returns_nullptr) {
	config.root_pid = 3;

	create(
"1     0  1.0  1 p1\n"
"2     1  4.0  4 p2\n"
"3     1  3.0  3 p3\n"
"4     1  5.0  2 p4\n"
"5     3  3.0  3 p5\n"
"6     3  3.0  3 p6\n"
"7     3  3.0  3 p7\n"
"8     4  5.0  2 p8\n"
	);

	auto c = procs_child_by_pid(procs, 8);
	EXPECT_EQ(c, nullptr);
}

TEST_F(procs_test, find_by_pid__unknown_process__returns_nullptr) {
	create(
"1     0  1.0  1 p1\n"
"2     1  4.0  4 p2\n"
"3     2  3.0  3 p3\n"
"4     2  5.0  2 p4\n"
	);

	auto c = procs_child_by_pid(procs, 10);
	EXPECT_EQ(c, nullptr);
}
