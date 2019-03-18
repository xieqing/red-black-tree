/*
 * Copyright (c) 2019 xieqing. https://github.com/xieqing
 * May be freely redistributed, but copyright notice must be retained.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include "rb.h"
#include "rb_data.h"
#include "minunit.h"

#define MIN INT_MIN
#define MAX INT_MAX
#define CHARS "ABCDEFGHIJ"

int mu_tests= 0, mu_fails = 0;

int permutation_error = 0;

static rbtree *tree_create();
static rbnode *tree_find(rbtree *rbt, int key);
static void tree_print(rbtree *rbt);
static int tree_check(rbtree *rbt);
static rbnode *tree_insert(rbtree *rbt, int key);
static int tree_delete(rbtree *rbt, int key);

static rbtree *make_black_tree();

static void swap(char *x, char *y);
static void permute(char *a, int start, int end, void func(char *));
static void permutation_insert(char *a);
static void permutation_delete(char *a);

static int unit_test_create();
static int unit_test_find();
static int unit_test_successor();
static int unit_test_atomic_insertion();
static int unit_test_chain_insertion();
static int unit_test_atomic_deletion();
static int unit_test_chain_deletion();
static int unit_test_permutation_insertion();
static int unit_test_permutation_deletion();
static int unit_test_random_insertion_deletion();

static int unit_test_dup();
#ifdef RB_MIN
static int unit_test_min();
#endif

void all_tests()
{
	mu_test("unit_test_create", unit_test_create());

	mu_test("unit_test_find", unit_test_find());

	mu_test("unit_test_successor", unit_test_successor());

	mu_test("unit_test_atomic_insertion", unit_test_atomic_insertion());
	mu_test("unit_test_chain_insertion", unit_test_chain_insertion());

	mu_test("unit_test_atomic_deletion", unit_test_atomic_deletion());
	mu_test("unit_test_chain_deletion", unit_test_chain_deletion());

	mu_test("unit_test_permutation_insertion", unit_test_permutation_insertion());
	mu_test("unit_test_permutation_deletion", unit_test_permutation_deletion());

	mu_test("unit_test_random_insertion_deletion", unit_test_random_insertion_deletion());

	mu_test("unit_test_dup", unit_test_dup());

	#ifdef RB_MIN
	mu_test("unit_test_min", unit_test_min());
	#endif
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

rbtree *tree_create()
{
	return rb_create(compare_func, destroy_func);
}

rbnode *tree_find(rbtree *rbt, int key)
{
	mydata query;
	query.key = key;
	return rb_find(rbt, &query);
}

void tree_print(rbtree *rbt)
{
	rb_print(rbt, print_func);
}

int tree_check(rbtree *rbt)
{
	mydata min, max;
	int rc;

	min.key = MIN;
	max.key = MAX;
	rc = 1;

	if (rb_check_order(rbt, &min, &max) == 0) {
		fprintf(stdout, "tree_check: invalid order\n");
		rc = 0;
	}

	if (rb_check_black_height(rbt) == 0) {
		fprintf(stdout, "tree_check: invalid black height\n");
		rc = 0;
	}

	return rc;
}

rbnode *tree_insert(rbtree *rbt, int key)
{
	rbnode *node;
	mydata *data;

	if (key < MIN || key > MAX) {
		fprintf(stdout, "tree_insert: invalid key %d\n", key);
		return NULL;
	}
	
	if ((data = makedata(key)) == NULL || (node = rb_insert(rbt, data)) == NULL) {
		fprintf(stdout, "tree_insert: insert %d failed\n", key);
		free(data);
		return NULL;
	}

	return node;
}

int tree_delete(rbtree *rbt, int key)
{
	rbnode *node;

	if ((node = tree_find(rbt, key)) == NULL) {
		fprintf(stdout, "tree_delete: %d not found\n", key);
		return 0;
	}

	rb_delete(rbt, node, 0);

	if (tree_find(rbt, key) == node) {
		fprintf(stdout, "tree_delete: delete %d failed\n", key);
		return 0;
	}

	return 1;
}

void swap(char *x, char *y)
{
	char temp;
	temp = *x;
	*x = *y;
	*y = temp;
}

void permute(char *a, int start, int end, void func(char *))
{
	if (start == end) {
		func(a);
		return;
	}

	int i;
	for (i = start; i <= end; i++) {
		swap(a + start, a + i);
		permute(a, start + 1, end, func);
		swap(a + start, a + i);
	}
}

void permutation_insert(char *a)
{
	rbtree *rbt;
	rbnode *node;
	int i;
	
	if ((rbt = tree_create()) == NULL) {
		fprintf(stdout, "create red-black tree failed\n");
		permutation_error++;
		return;
	}

	for (i = 0; i < strlen(a); i++) {
		if ((node = tree_insert(rbt, a[i])) == NULL || tree_find(rbt, a[i]) != node || tree_check(rbt) != 1) {
			fprintf(stdout, "insert %c failed\n", a[i]);
			permutation_error++;
			return;
		}
	}

	rb_destroy(rbt);
}

void permutation_delete(char *a)
{
	rbtree *rbt;
	rbnode *node;
	int i;
	
	if ((rbt = tree_create()) == NULL) {
		fprintf(stdout, "create red-black tree failed\n");
		permutation_error++;
		return;
	}

	char b[] = CHARS;

	for (i = 0; i < strlen(b); i++) {
		if ((node = tree_insert(rbt, b[i])) == NULL || tree_find(rbt, b[i]) != node || tree_check(rbt) != 1) {
			fprintf(stdout, "insert %c failed\n", b[i]);
			permutation_error++;
			return;
		}
	}

	for (i = 0; i < strlen(a); i++) {
		if (tree_delete(rbt, a[i]) != 1 || tree_check(rbt) != 1) {
			fprintf(stdout, "delete %c failed\n", a[i]);
			permutation_error++;
			return;
		}
	}

	rb_destroy(rbt);
}


rbtree *make_black_tree()
{
	rbtree *rbt;
	rbnode *node;
	char a[] = "ABCDEFGHIJ";
	char b[] = "ACJ";
	char c[] = "BDEFGHI";
	int i, n;

	if ((rbt = tree_create()) == NULL)
		goto err0;

	n = strlen(a);
	for (i = 0; i < n; i++) {
		if (tree_insert(rbt, a[i]) == NULL || tree_check(rbt) != 1)
			goto err;
	}

	n = strlen(b);
	for (i = 0; i < n; i++) {
		if (tree_delete(rbt, b[i]) != 1 || tree_check(rbt) != 1)
			goto err;
	}

	n = strlen(c);
	for (i = 0; i < n; i++) {
		if ((node = tree_find(rbt, c[i])) == NULL || node->color != BLACK)
			goto err;
	}

	rbnode *nb, *nd, *ne, *nf, *ng, *nh, *ni;
	nb = tree_find(rbt, 'B');
	nd = tree_find(rbt, 'D');
	ne = tree_find(rbt, 'E');
	nf = tree_find(rbt, 'F');
	ng = tree_find(rbt, 'G');
	nh = tree_find(rbt, 'H');
	ni = tree_find(rbt, 'I');
	if (nf->left != nd || nf->right != nh || \
		nd->left != nb || nd->right != ne || \
		nh->left != ng || nh->right != ni) {
		goto err;
	}

	return rbt;

err:
	rb_destroy(rbt);
err0:
	return NULL;
}

int unit_test_create()
{
	rbtree *rbt;

	if ((rbt = tree_create()) == NULL) {
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
		fprintf(stdout, "init failed\n");
		rb_destroy(rbt);
		return 0;
	}

	rb_destroy(rbt);
	return 1;
}

int unit_test_find()
{
	rbtree *rbt;
	rbnode *r, *e, *d, *s, *o, *x, *c, *u, *b, *t;
	
	if ((rbt = tree_create()) == NULL) {
		fprintf(stdout, "create red-black tree failed\n");
		goto err0;
	}

	if ((r = tree_insert(rbt, 'R')) == NULL || \
		(e = tree_insert(rbt, 'E')) == NULL || \
		(d = tree_insert(rbt, 'D')) == NULL || \
		(s = tree_insert(rbt, 'S')) == NULL || \
		(o = tree_insert(rbt, 'O')) == NULL || \
		(x = tree_insert(rbt, 'X')) == NULL || \
		(c = tree_insert(rbt, 'C')) == NULL || \
		(u = tree_insert(rbt, 'U')) == NULL || \
		(b = tree_insert(rbt, 'B')) == NULL || \
		(t = tree_insert(rbt, 'T')) == NULL || \
		tree_check(rbt) != 1) {
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

int unit_test_successor()
{
	rbtree *rbt;
	rbnode *r, *e, *d, *s, *o, *x, *c, *u, *b, *t;
	
	if ((rbt = tree_create()) == NULL) {
		fprintf(stdout, "create red-black tree failed\n");
		goto err0;
	}

	if ((r = tree_insert(rbt, 'R')) == NULL || \
		(e = tree_insert(rbt, 'E')) == NULL || \
		(d = tree_insert(rbt, 'D')) == NULL || \
		(s = tree_insert(rbt, 'S')) == NULL || \
		(o = tree_insert(rbt, 'O')) == NULL || \
		(x = tree_insert(rbt, 'X')) == NULL || \
		(c = tree_insert(rbt, 'C')) == NULL || \
		(u = tree_insert(rbt, 'U')) == NULL || \
		(b = tree_insert(rbt, 'B')) == NULL || \
		(t = tree_insert(rbt, 'T')) == NULL || \
		tree_delete(rbt, 'O') != 1 || \
		tree_check(rbt) != 1) {
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

int unit_test_atomic_insertion()
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
		if ((rbt = tree_create()) == NULL) {
			fprintf(stdout, "%s - create red-black tree failed\n", name[i]);
			goto err0;
		}

		for (j = 0; j < sizeof(cs[0]) / sizeof(cs[0][0]) && cs[i][j]; j++) {
			if (tree_insert(rbt, cs[i][j]) == NULL || tree_check(rbt) != 1) {
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

int unit_test_chain_insertion()
{
	rbtree *rbt;
	int i, j;

	int a1[] = {1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 27, 29, 39, 41};
	int a2[] = {16, 8, 24, 4, 12, 20, 32, 2, 6, 10, 14, 18, 22, 28, 40};
	
	for (i = 0; i < sizeof(a1) / sizeof(a1[0]); i++) {
		if ((rbt = tree_create()) == NULL) {
			fprintf(stdout, "create red-black tree failed\n", i);
			goto err0;
		}

		for (j = 0; j < sizeof(a2) / sizeof(a2[0]); j++) {
			if (tree_insert(rbt, a2[j]) == NULL || tree_check(rbt) != 1) {
				fprintf(stdout, "insert %d failed\n", a2[j]);
				goto err;
			}
		}

		if (tree_insert(rbt, a1[i]) == NULL || tree_check(rbt) != 1) {
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

int unit_test_atomic_deletion()
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
		if ((rbt = tree_create()) == NULL) {
			fprintf(stdout, "%s - create red-black tree failed\n", name[i]);
			goto err0;
		}

		for (j = 0; j < sizeof(cs[0][0]) / sizeof(cs[0][0][0]) && cs[i][0][j]; j++) {
			if (tree_insert(rbt, cs[i][0][j]) == NULL || tree_check(rbt) == 0) {
				fprintf(stdout, "%s - insert %c failed\n", name[i], cs[i][0][j]);
				goto err;
			}
		}

		for (j = 0; j < sizeof(cs[0][0]) / sizeof(cs[0][0][0]) && cs[i][0][j]; j++) {
			if (tree_delete(rbt, cs[i][0][j]) == 0 || tree_check(rbt) == 0) {
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

int unit_test_chain_deletion()
{
	rbtree *rbt;
	char a[] = "BEGI";
	int i, n;

	n = strlen(a);
	for (i = 0; i < n; i++) {
		if ((rbt = make_black_tree()) == NULL) {
			fprintf(stdout, "make black tree failed\n");
			goto err;
		}

		if (tree_delete(rbt, a[i]) != 1 || tree_check(rbt) != 1) {
			fprintf(stdout, "delete %c failed\n", a[i]);
			goto err;
		}

		rb_destroy(rbt);
	}
	
	return 1;

err:
	if (rbt)
		rb_destroy(rbt);
	return 0;
}

int unit_test_permutation_insertion()
{
	char a[] = CHARS;
	
	permutation_error = 0;
	permute(a, 0, strlen(a) - 1, permutation_insert);
	return (permutation_error == 0);
}

int unit_test_permutation_deletion()
{
	char a[] = CHARS;
	
	permutation_error = 0;
	permute(a, 0, strlen(a) - 1, permutation_delete);
	return (permutation_error == 0);
}

int unit_test_random_insertion_deletion()
{
	rbtree *rbt;
	int ninsert, ndelete;
	int i, key, max;
	
	if ((rbt = tree_create()) == NULL) {
		fprintf(stdout, "create red-black tree failed\n");
		goto err0;
	}
	
	ninsert = 0;
	ndelete = 0;
	max = 9999;

	srand((unsigned int) time(NULL));

	for (i = 1; i <= 1999; i++) {
		key = rand() % max;
		if (tree_find(rbt, key) != NULL)
			continue;
		ninsert++;
		if (tree_insert(rbt, key) == NULL || tree_check(rbt) != 1) {
			fprintf(stdout, "insert %d failed\n", key);
			goto err;
		}
	}

	for (i = 1; i < max; i++) {
		key = rand() % max;
		if (tree_find(rbt, key) == NULL)
			continue;
		ndelete++;
		if (tree_delete(rbt, key) != 1 || tree_check(rbt) != 1) {
			fprintf(stdout, "delete %d failed\n", key);
			goto err;
		}
	}

	printf("\tstat: ninsert=%d, ndelete=%d\n", ninsert, ndelete);

	rb_destroy(rbt);
	return 1;

err:
	rb_destroy(rbt);
err0:
	return 0;
}

#ifdef RB_MIN
int unit_test_min()
{
	rbtree *rbt;

	if ((rbt = tree_create()) == NULL) {
		fprintf(stdout, "create red-black tree failed\n");
		goto err0;
	}

	if (RB_MINIMAL(rbt) != NULL || \
		tree_insert(rbt, 'B') == NULL || RB_MINIMAL(rbt) != tree_find(rbt, 'B') || \
		tree_insert(rbt, 'A') == NULL || RB_MINIMAL(rbt) != tree_find(rbt, 'A') || \
		tree_insert(rbt, 'C') == NULL || RB_MINIMAL(rbt) != tree_find(rbt, 'A') || \
		tree_delete(rbt, 'B') != 1 || RB_MINIMAL(rbt) != tree_find(rbt, 'A') || \
		tree_delete(rbt, 'A') != 1 || RB_MINIMAL(rbt) != tree_find(rbt, 'C') || \
		tree_delete(rbt, 'C') != 1 || RB_MINIMAL(rbt) != NULL) {
		fprintf(stdout, "invalid min\n");
		goto err;
	}

	rb_destroy(rbt);

	return 1;

err:
	rb_destroy(rbt);
err0:
	return 0;
}
#endif

int unit_test_dup()
{
	rbtree *rbt;
	rbnode *n1, *n2;

	if ((rbt = tree_create()) == NULL) {
		fprintf(stdout, "create red-black tree failed\n");
		goto err0;
	}

	if ((n1 = tree_insert(rbt, 'N')) == NULL || (n2 = tree_insert(rbt, 'N')) == NULL) {
		fprintf(stdout, "insert failed\n");
		goto err;
	}

	#ifdef RB_DUP
	if (n1 == n2 || n1->right != n2) {
	#else
	if (n1 != n2) {
	#endif
		fprintf(stdout, "invalid dup\n");
		goto err;
	}

	rb_destroy(rbt);

	return 1;

err:
	rb_destroy(rbt);
err0:
	return 0;
}