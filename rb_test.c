/*
 * Copyright (c) 2019 xieqing. https://github.com/xieqing
 * May be freely redistributed, but copyright notice must be retained.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "rb.h"
#include "rb_data.h"
#include "minunit.h"

#define MIN -99999
#define MAX 99999

int mu_tests= 0, mu_fails = 0;

static rbtree *tr_create();
static rbnode *tr_find(rbtree *rbt, int key);
static void tr_print(rbtree *rbt);
static int tr_check(rbtree *rbt);
static rbnode *tr_insert(rbtree *rbt, int key);
static int tr_delete(rbtree *rbt, int key);

static int unit_test_rb_create();
static int unit_test_rb_find();
static int unit_test_rb_successor();
static int unit_test_rb_insert();
static int unit_test_rb_insert_overflow();
static int unit_test_rb_delete();
static int unit_test_rb_delete_underflow();
static int unit_test_insert_delete_rand();

void all_tests()
{
	mu_test("unit_test_rb_create", unit_test_rb_create());

	mu_test("unit_test_rb_find", unit_test_rb_find());

	mu_test("unit_test_rb_successor", unit_test_rb_successor());

	mu_test("unit_test_rb_insert", unit_test_rb_insert());
	mu_test("unit_test_rb_insert_overflow", unit_test_rb_insert_overflow());

	mu_test("unit_test_rb_delete", unit_test_rb_delete());
	mu_test("unit_test_rb_delete_underflow", unit_test_rb_delete_underflow());	

	mu_test("unit_test_insert_delete_rand", unit_test_insert_delete_rand());
}

int main(int argc, char **argv)
{
	all_tests();

	if (mu_fails) {
		printf("*** %d/%d TESTS FAILED ***\n", mu_fails, mu_tests);
		return 1;
	} else {
		printf("ALL TESTS PASSED\n");
		return 0;
	}
}

rbtree *tr_create()
{
	return rb_create(compare_func, destroy_func);
}

rbnode *tr_find(rbtree *rbt, int key)
{
	mydata query;
	query.key = key;
	return rb_find(rbt, &query);
}

void tr_print(rbtree *rbt)
{
	rb_print(rbt, print_func);
}

int tr_check(rbtree *rbt)
{
	mydata min, max;
	int rc;

	min.key = MIN;
	max.key = MAX;
	rc = 1;

	if (rb_check_order(rbt, &min, &max) == 0) {
		fprintf(stdout, "tr_check: invalid order\n");
		rc = 0;
	}

	if (rb_check_black_height(rbt) == 0) {
		fprintf(stdout, "tr_check: invalid black height\n");
		rc = 0;
	}

	return rc;
}

rbnode *tr_insert(rbtree *rbt, int key)
{
	rbnode *node;
	mydata *data;

	if (key < MIN || key > MAX) {
		fprintf(stdout, "tr_insert: invalid key %d\n", key);
		return NULL;
	}
	
	if ((data = makedata(key)) == NULL || (node = rb_insert(rbt, data)) == NULL) {
		fprintf(stdout, "tr_insert: insert %d failed\n", key);
		free(data);
		return NULL;
	}

	return node;
}

int tr_delete(rbtree *rbt, int key)
{
	rbnode *node;

	if ((node = tr_find(rbt, key)) == NULL) {
		fprintf(stdout, "tr_delete: %d not found\n", key);
		return 0;
	}

	rb_delete(rbt, node, 0);

	if (tr_find(rbt, key) == node) {
		fprintf(stdout, "tr_delete: delete %d failed\n", key);
		return 0;
	}

	return 1;
}

int unit_test_rb_create()
{
	rbtree *rbt;

	if ((rbt = tr_create()) == NULL) {
		fprintf(stdout, "create red-black tree failed\n");
		return 0;
	}

	if (rbt->compare != compare_func || \
		rbt->destroy != destroy_func || \
		rbt->nil.left != RB_NIL(rbt) || \
		rbt->nil.right != RB_NIL(rbt) || \
		rbt->nil.parent != RB_NIL(rbt) || \
		rbt->nil.color != BLACK || \
		rbt->nil.data != NULL || \
		rbt->root.left != RB_NIL(rbt) || \
		rbt->root.right != RB_NIL(rbt) || \
		rbt->root.parent != RB_NIL(rbt) || \
		rbt->root.color != BLACK || \
		rbt->root.data != NULL) {
		fprintf(stdout, "init1 failed\n");
		rb_destroy(rbt);
		return 0;
	}

	#ifdef RB_MIN
	if (rbt->min != NULL) {
		fprintf(stdout, "init2 failed\n");
		rb_destroy(rbt);
		return 0;
	}
	#endif

	rb_destroy(rbt);
	return 1;
}

int unit_test_rb_find()
{
	rbtree *rbt;
	rbnode *r, *e, *d, *s, *o, *x, *c, *u, *b, *t;
	
	if ((rbt = tr_create()) == NULL) {
		fprintf(stdout, "create red-black tree failed\n");
		goto err0;
	}

	if ((r = tr_insert(rbt, 'R')) == NULL || \
		(e = tr_insert(rbt, 'E')) == NULL || \
		(d = tr_insert(rbt, 'D')) == NULL || \
		(s = tr_insert(rbt, 'S')) == NULL || \
		(o = tr_insert(rbt, 'O')) == NULL || \
		(x = tr_insert(rbt, 'X')) == NULL || \
		(c = tr_insert(rbt, 'C')) == NULL || \
		(u = tr_insert(rbt, 'U')) == NULL || \
		(b = tr_insert(rbt, 'B')) == NULL || \
		(t = tr_insert(rbt, 'T')) == NULL || \
		tr_check(rbt) != 1) {
		fprintf(stdout, "init failed\n");
		goto err;
	}

	if (rb_find(rbt, r->data) != r || \
		rb_find(rbt, e->data) != e || \
		rb_find(rbt, d->data) != d || \
		rb_find(rbt, s->data) != s || \
		rb_find(rbt, o->data) != o || \
		rb_find(rbt, x->data) != x || \
		rb_find(rbt, c->data) != c || \
		rb_find(rbt, u->data) != u || \
		rb_find(rbt, b->data) != b || \
		rb_find(rbt, t->data) != t) {
		fprintf(stdout, "find failed\n");
		goto err;
	}

	rb_destroy(rbt);
	return 1;

err:
	rb_destroy(rbt);
err0:
	return 0;
}

int unit_test_rb_successor()
{
	rbtree *rbt;
	rbnode *r, *e, *d, *s, *o, *x, *c, *u, *b, *t;
	
	if ((rbt = tr_create()) == NULL) {
		fprintf(stdout, "create red-black tree failed\n");
		goto err0;
	}

	if ((r = tr_insert(rbt, 'R')) == NULL || \
		(e = tr_insert(rbt, 'E')) == NULL || \
		(d = tr_insert(rbt, 'D')) == NULL || \
		(s = tr_insert(rbt, 'S')) == NULL || \
		(o = tr_insert(rbt, 'O')) == NULL || \
		(x = tr_insert(rbt, 'X')) == NULL || \
		(c = tr_insert(rbt, 'C')) == NULL || \
		(u = tr_insert(rbt, 'U')) == NULL || \
		(b = tr_insert(rbt, 'B')) == NULL || \
		(t = tr_insert(rbt, 'T')) == NULL || \
		tr_delete(rbt, 'O') != 1 || \
		tr_check(rbt) != 1) {
		fprintf(stdout, "init failed\n");
		goto err;
	}

	if (rb_successor(rbt, b) != c || \
		rb_successor(rbt, c) != d || \
		rb_successor(rbt, d) != e || \
		rb_successor(rbt, e) != r || \
		rb_successor(rbt, r) != s || \
		rb_successor(rbt, s) != t || \
		rb_successor(rbt, t) != u || \
		rb_successor(rbt, u) != x || \
		rb_successor(rbt, x) != NULL) {
		fprintf(stdout, "successor failed\n");
		goto err;
	}

	rb_destroy(rbt);
	return 1;

err:
	rb_destroy(rbt);
err0:
	return 0;
}

int unit_test_rb_insert()
{
	rbtree *rbt;
	int i, j;

	char cs[][30] = {
		/* empty node becomes 2-children node */
		{'D'}, /* (balanced) */

		/* 2-children node becomes 3-children node */
		{'D', 'B'}, /* d.left (balanced) */
		{'D', 'F'}, /* d.right (balanced) */

		/* 3-children node becomes 4-children node */
		{'D', 'F', 'B'}, /* d.left (balanced) */
		{'D', 'B', 'F'}, /* d.right (balanced) */
		{'D', 'B', 'A'}, /* d.left.left */
		{'D', 'B', 'C'}, /* d.left.right */
		{'D', 'F', 'E'}, /* d.right.left */
		{'D', 'F', 'G'}, /* d.right.right */

		/* 4-children node splits into 2-children node and 3-children node */
		{'D', 'B', 'F', 'A'}, /* d.left.left */
		{'D', 'B', 'F', 'C'}, /* d.left.right */
		{'D', 'B', 'F', 'E'}, /* d.right.left */
		{'D', 'B', 'F', 'G'}, /* d.right.right */
	};

	char *name[] = {
		"empty node becomes 2-children node: insert d",
		"2-children node becomes 3-children nodes: insert b",
		"2-children node becomes 3-children nodes: insert f",
		"3-children node becomes 4-children node: insert b",
		"3-children node becomes 4-children node: insert f",
		"3-children node becomes 4-children node: insert a",
		"3-children node becomes 4-children node: insert c",
		"3-children node becomes 4-children node: insert e",
		"3-children node becomes 4-children node: insert g",
		"4-children node splits: insert a",
		"4-children node splits: insert c",
		"4-children node splits: insert e",
		"4-children node splits: insert g",
	};
	
	for (i = 0; i < sizeof(cs) / sizeof(cs[0]); i++) {
		if ((rbt = tr_create()) == NULL) {
			fprintf(stdout, "%s - create red-black tree failed\n", name[i]);
			goto err0;
		}

		for (j = 0; j < sizeof(cs[0]) / sizeof(cs[0][0]) && cs[i][j]; j++) {
			if (tr_insert(rbt, cs[i][j]) == NULL || tr_check(rbt) != 1) {
				fprintf(stdout, "%s - insert %c failed\n", name[i], cs[i][j]);
				goto err;
			}
		}

		rb_destroy(rbt);
	}
	
	return 1;

err:
	rb_destroy(rbt);
err0:
	return 0;
}

int unit_test_rb_insert_overflow()
{
	rbtree *rbt;
	int i, j;

	int a1[] = {1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 27, 29, 39, 41};
	int a2[] = {16, 8, 24, 4, 12, 20, 32, 2, 6, 10, 14, 18, 22, 28, 40};
	
	for (i = 0; i < sizeof(a1) / sizeof(a1[0]); i++) {
		if ((rbt = tr_create()) == NULL) {
			fprintf(stdout, "create red-black tree failed\n", i);
			goto err0;
		}

		for (j = 0; j < sizeof(a2) / sizeof(a2[0]); j++) {
			if (tr_insert(rbt, a2[j]) == NULL || tr_check(rbt) != 1) {
				fprintf(stdout, "insert %d failed\n", a2[j]);
				goto err;
			}
		}

		if (tr_insert(rbt, a1[i]) == NULL || tr_check(rbt) != 1) {
			fprintf(stdout, "insert %d failed\n", a1[i]);
			goto err;
		}

		rb_destroy(rbt);
	}
	
	return 1;

err:
	rb_destroy(rbt);
err0:
	return 0;
}

int unit_test_rb_delete()
{
	rbtree *rbt;
	int i, j;

	char cs[][2][30] = {
		/* 4-children node becomes 3-children node */
		{{'D', 'B', 'F'}, {'B'}}, /* d.left (balanced) */
		{{'D', 'B', 'F'}, {'D'}}, /* d (balanced) */
		{{'D', 'B', 'F'}, {'F'}}, /* d.right (balanced) */

		/* 3-children node becomes 2-children node */
		{{'D', 'B'}, {'B'}}, /* d.left (balanced) */
		{{'D', 'B'}, {'D'}}, /* d (balanced) */
		{{'D', 'F'}, {'D'}}, /* d (balanced) */
		{{'D', 'F'}, {'F'}}, /* d.right (balanced) */

		/* 2-children node becomes empty node */
		{{'D'}, {'D'}}, /* d (balanced) */

		/* 2-children node becomes 3-children node (transfer) */
		{{'D', 'B', 'F', 'E', 'G'}, {'B'}}, /* d->left */
		{{'D', 'B', 'F', 'A', 'C'}, {'F'}}, /* d->right */

		/* 2-children node becomes 2-children node (transfer) */
		{{'D', 'B', 'F', 'A'}, {'F'}}, /* d->right */
		{{'D', 'B', 'F', 'C'}, {'F'}}, /* d->right */
		{{'D', 'B', 'F', 'E'}, {'B'}}, /* d->left */
		{{'D', 'B', 'F', 'G'}, {'B'}}, /* d->left */

		/* 2-children node becomes 3-children node (fuse) */
		{{'D', 'B', 'F', 'A'}, {'A', 'B'}}, /* d->left */
		{{'D', 'B', 'F', 'A'}, {'A', 'F'}} /* d->right */
	};

	char *name[] = {
		"4-children node becomes 3-children node: delete b",
		"4-children node becomes 3-children node: delete d",
		"4-children node becomes 3-children node: delete f",
		"3-children node becomes 2-children node: delete b",
		"3-children node becomes 2-children node: delete d",
		"3-children node becomes 2-children node: delete d",
		"3-children node becomes 2-children node: delete f",
		"2-children node becomes empty node: delete d",
		"2-children node becomes 3-children node (transfer): delete b",
		"2-children node becomes 3-children node (transfer): delete f",
		"2-children node becomes 2-children node (transfer): delete f",
		"2-children node becomes 2-children node (transfer): delete f",
		"2-children node becomes 2-children node (transfer): delete b",
		"2-children node becomes 2-children node (transfer): delete b",
		"2-children node becomes 3-children node (fuse): delete b",
		"2-children node becomes 3-children node (fuse): delete f",
		"2-children node becomes 3-children node (fuse): delete f"
	};

	for (i = 0; i < sizeof(cs) / sizeof(cs[0]); i++) {
		if ((rbt = tr_create()) == NULL) {
			fprintf(stdout, "%s - create red-black tree failed\n", name[i]);
			goto err0;
		}

		for (j = 0; j < sizeof(cs[0][0]) / sizeof(cs[0][0][0]) && cs[i][0][j]; j++) {
			if (tr_insert(rbt, cs[i][0][j]) == NULL || tr_check(rbt) == 0) {
				fprintf(stdout, "%s - insert %c failed\n", name[i], cs[i][0][j]);
				goto err;
			}
		}

		for (j = 0; j < sizeof(cs[0][0]) / sizeof(cs[0][0][0]) && cs[i][0][j]; j++) {
			if (tr_delete(rbt, cs[i][0][j]) == 0 || tr_check(rbt) == 0) {
				fprintf(stdout, "%s - delete %c failed\n", name[i], cs[i][0][j]);
				goto err;
			}
		}

		rb_destroy(rbt);
	}
	
	return 1;

err:
	rb_destroy(rbt);
err0:
	return 0;
}

int unit_test_rb_delete_underflow()
{
	rbtree *rbt;
	int i, j;
	const int min = 1;
	const int max = 99;

	for (i = min; i <= max; i++) {
		if ((rbt = tr_create()) == NULL) {
			fprintf(stdout, "create red-black tree failed\n", i);
			goto err0;
		}

		for (j = min; j <= max; j++) {
			if (tr_insert(rbt, j) == NULL || tr_check(rbt) == 0) {
				fprintf(stdout, "insert %d failed\n", j);
				goto err;
			}
		}

		if (tr_delete(rbt, i) == 0 || tr_check(rbt) == 0) {
			fprintf(stdout, "delete %d failed\n", i);
			goto err;
		}

		rb_destroy(rbt);
	}
	
	return 1;

err:
	rb_destroy(rbt);
err0:
	return 0;
}

int unit_test_insert_delete_rand()
{
	rbtree *rbt;
	rbnode *node;
	int i, key, ninsert, ndelete;
	
	if ((rbt = tr_create()) == NULL) {
		fprintf(stdout, "create red-black tree failed\n");
		goto err0;
	}

	srand((unsigned int) time(NULL));
	
	for (i = 1, ninsert = 0; i <= 1999; i++) {
		key = rand() % MAX;
		if (tr_find(rbt, key) != NULL)
			continue;
		ninsert++;
		if (tr_insert(rbt, key) == NULL || tr_check(rbt) != 1) {
			fprintf(stdout, "insert %d failed\n", key);
			goto err;
		}
	}

	for (i = 1, ndelete = 0; i < MAX; i++) {
		key = rand() % MAX;
		if ((node = tr_find(rbt, key)) == NULL)
			continue;
		ndelete++;
		rb_delete(rbt, node, 0);
		if (tr_find(rbt, key) != NULL || tr_check(rbt) != 1) {
			fprintf(stdout, "delete %d failed\n", key);
			goto err;
		}
	}

	printf("\tstat: ninsert=%d ndelete=%d\n", ninsert, ndelete);

	rb_destroy(rbt);
	return 1;

err:
	rb_destroy(rbt);
err0:
	return 0;
}