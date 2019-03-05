/*
 * Copyright (c) 2019 xieqing. https://github.com/xieqing
 * May be freely redistributed, but copyright notice must be retained.
 */

#include <stdio.h>
#include <stdlib.h>
#include "rb.h"
#include "rb_data.h"

int main(int argc, char *argv[])
{
	rbtree *rbt;
	
	/* create a red-black tree */
	if ((rbt = rb_create(compare_func, destroy_func)) == NULL) {
		fprintf(stderr, "create red-black tree failed\n");
		return 1;
	}

	/* insert items */
	char a[] = {'R', 'E', 'D', 'S', 'O', 'X', 'C', 'U', 'B', 'T'};
	int i;
	mydata *data;
	for (i = 0; i < sizeof(a) / sizeof(a[0]); i++) {
		if ((data = makedata(a[i])) == NULL || rb_insert(rbt, data) == NULL) {
			fprintf(stderr, "insert %c: out of memory\n", a[i]);
			free(data);
			break;
		}
		printf("insert %c", a[i]);
		rb_print(rbt, print_char_func);
		printf("\n");
	}

	/* delete item */
	rbnode *node;
	mydata query;
	query.key = 'O';
	printf("delete %c", query.key);
	if ((node = rb_find(rbt, &query)) != NULL)
		rb_delete(rbt, node, 0);
	rb_print(rbt, print_char_func);

	#ifdef RB_MIN
	while ((node = RB_MINIMAL(rbt))) {
		printf("\ndelete ");
		print_char_func(node->data);
		rb_delete(rbt, node, 0);
		rb_print(rbt, print_char_func);
	}
	#endif

	rb_destroy(rbt);
	return 0;
}

/*
 * usage: gcc rb_example.c rb.c rb_data.c && ./a.out
 */