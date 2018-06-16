#include "gtest/gtest.h"

extern "C" {
#include "ptree.h"
#include "cfg.h"
}

#define EPS PSC_EPS

using namespace std;
using namespace ::testing;

TEST(ptree_create, processes_pid_ppid_mem_comm_are_set) {
	config.max_name_lenght = 20;
	config.max_children = 100;
	config.root_pid = 0;
	config.memory_unit = 1;

	FILE *fptr = tmpfile();

	fputs(
"1     0  3.14  4212 systemd\n"
	, fptr);
	rewind(fptr);

	auto ptree = ptree_create(fptr);

	fclose(fptr);

	auto r = ptree->root;
	EXPECT_EQ(r->pid, 0);
	process_t *p = (process_t *)r->node.first;
	EXPECT_EQ(p->pid, 1);
	EXPECT_EQ(p->ppid, 0);
	EXPECT_NEAR(p->pcpu, 3.14, EPS);
	EXPECT_EQ(p->mem, 4212u*1024);
	EXPECT_EQ(string(p->comm), "systemd");

	ptree_destroy(ptree);
}

TEST(ptree_create, comm_name_is_too_long) {
	config.max_name_lenght = 3;
	config.max_children = 100;
	config.root_pid = 0;
	config.memory_unit = 1;

	FILE *fptr = tmpfile();

	fputs(
"1     0  3.14  4212 systemd\n"
	, fptr);
	rewind(fptr);

	auto ptree = ptree_create(fptr);

	fclose(fptr);

	process_t *p = (process_t *)ptree->root->node.first;
	EXPECT_EQ(string(p->comm), "sy");

	ptree_destroy(ptree);
}

TEST(ptree_create, comm_multipel_words) {
	config.max_name_lenght = 3;
	config.max_children = 100;
	config.root_pid = 0;
	config.memory_unit = 1;

	FILE *fptr = tmpfile();

	fputs(
"1     0  3.14  4212 systemd abc ab\n"
	, fptr);
	rewind(fptr);

	auto ptree = ptree_create(fptr);

	fclose(fptr);

	process_t *p = (process_t *)ptree->root->node.first;
	EXPECT_EQ(string(p->comm), "sy");

	ptree_destroy(ptree);
}

TEST(ptree_create, root_pid_found) {
	config.max_name_lenght = 20;
	config.max_children = 100;
	config.root_pid = 1;
	config.memory_unit = 1;

	FILE *fptr = tmpfile();

	fputs(
"1     0  0.0  0 n1\n"
"2     1  0.0  0 n12\n"
"3     1  0.0  0 n13\n"
"4     2  0.0  0 n121\n"
	, fptr);
	rewind(fptr);

	auto ptree = ptree_create(fptr);

	fclose(fptr);

	auto r = ptree->root;
	EXPECT_EQ(string(r->comm), "n1");
	process_t *p1 = (process_t *)r->node.first;
	EXPECT_EQ(string(p1->comm), "n12");
	process_t *p2 = (process_t *)p1->node.next;
	EXPECT_EQ(string(p2->comm), "n13");
	process_t *p21 = (process_t *)p1->node.first;
	EXPECT_EQ(string(p21->comm), "n121");

	ptree_destroy(ptree);
}

TEST(ptree_create, mem_toplists_empty_rows) {
	config.max_name_lenght = 20;
	config.max_children = 100;
	config.root_pid = 1;
	config.memory_unit = 1;
	config.toplist_max_rows = 10;

	FILE *fptr = tmpfile();

	fputs(
"1     0  0.0  1 n1\n"
"2     1  0.0  4 n12\n"
"3     1  0.0  3 n13\n"
"4     2  0.0  2 n121\n"
	, fptr);
	rewind(fptr);

	auto ptree = ptree_create(fptr);

	fclose(fptr);

	auto l = ptree->mem_toplist;
	EXPECT_EQ(string(l[0]->comm), "n12");
	EXPECT_EQ(string(l[1]->comm), "n13");
	EXPECT_EQ(string(l[2]->comm), "n121");
	EXPECT_EQ(string(l[3]->comm), "n1");
	for (size_t i = 4; i < config.toplist_max_rows; ++i)
		EXPECT_EQ(l[i],  nullptr);

	ptree_destroy(ptree);
}

TEST(ptree_create, mem_toplists_updated) {
	config.max_name_lenght = 20;
	config.max_children = 100;
	config.root_pid = 1;
	config.memory_unit = 1;
	config.toplist_max_rows = 2;

	FILE *fptr = tmpfile();

	fputs(
"1     0  0.0  1 n1\n"
"2     1  0.0  4 n12\n"
"3     1  0.0  3 n13\n"
"4     2  0.0  2 n121\n"
	, fptr);
	rewind(fptr);

	auto ptree = ptree_create(fptr);

	fclose(fptr);

	auto l = ptree->mem_toplist;

	EXPECT_EQ(string(l[0]->comm), "n12");
	EXPECT_EQ(string(l[1]->comm), "n13");

	ptree_destroy(ptree);
}

TEST(ptree_create, pcpu_toplists_empty_rows) {
	config.max_name_lenght = 20;
	config.max_children = 100;
	config.root_pid = 1;
	config.memory_unit = 1;
	config.toplist_max_rows = 10;

	FILE *fptr = tmpfile();

	fputs(
"1     0  1.0  1 n1\n"
"2     1  4.0  4 n12\n"
"3     1  3.0  3 n13\n"
"4     2  2.0  2 n121\n"
	, fptr);
	rewind(fptr);

	auto ptree = ptree_create(fptr);

	fclose(fptr);

	auto l = ptree->cpu_toplist;
	EXPECT_EQ(string(l[0]->comm), "n12");
	EXPECT_EQ(string(l[1]->comm), "n13");
	EXPECT_EQ(string(l[2]->comm), "n121");
	EXPECT_EQ(string(l[3]->comm), "n1");
	for (size_t i = 4; i < config.toplist_max_rows; ++i)
		EXPECT_EQ(l[i],  nullptr);

	ptree_destroy(ptree);
}

TEST(ptree_create, pcpu_toplists_updated) {
	config.max_name_lenght = 20;
	config.max_children = 100;
	config.root_pid = 1;
	config.memory_unit = 1;
	config.toplist_max_rows = 2;

	FILE *fptr = tmpfile();

	fputs(
"1     0  1.0  1 n1\n"
"2     1  4.0  4 n12\n"
"3     1  3.0  3 n13\n"
"4     2  2.0  2 n121\n"
	, fptr);
	rewind(fptr);

	auto ptree = ptree_create(fptr);

	fclose(fptr);

	auto l = ptree->cpu_toplist;

	EXPECT_EQ(string(l[0]->comm), "n12");
	EXPECT_EQ(string(l[1]->comm), "n13");

	ptree_destroy(ptree);
}

TEST(ptree_create, stubs_are_added) {
	config.max_name_lenght = 20;
	config.max_children = 1;
	config.root_pid = 1;
	config.memory_unit = 1;
	config.toplist_max_rows = 2;

	FILE *fptr = tmpfile();

	fputs(
"1     0  1.0  1 n1\n"
"2     1  4.0  4 n11\n"
"3     1  3.0  3 n12\n"
"4     1  5.0  2 n13\n"
	, fptr);
	
	rewind(fptr);

	auto ptree = ptree_create(fptr);

	fclose(fptr);

	auto r = ptree->root;
	process_t *p1 = (process_t *)r->node.first;
	process_t *p2 = (process_t *)p1->node.next;

	EXPECT_EQ(string(p1->comm), "n11");
	EXPECT_EQ(string(p2->comm), "<2 omitted>");
	EXPECT_EQ(p2->mem, 3u*1024);
	EXPECT_NEAR(p2->pcpu, 5, EPS);

	ptree_destroy(ptree);
}
