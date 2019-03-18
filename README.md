
# A Red-black Tree Implementation In C

There are several choices when implementing red-black trees:
- store parent reference or not
- recursive or non-recursive (iterative)
- do top-down splits or bottom-up splits (only when needed)
- do top-down fusion or top-bottom fusion (only when needed)

This implementation's choice:
- store parent reference
- non-recursive (iterative)
- do bottom-up splits (only when needed)
- do top-bottom fusion (only when needed)

Files
* rb.h - red-black tree header
* rb.c - red-black tree library
* rb_data.h - data header
* rb_data.c - data library
* rb_example.c - example code for red-black tree application
* rb_test.c - unit test program
* rb_test.sh - unit test shell script
* README.md - implementation note

If you have suggestions, corrections, or comments, please get in touch with [xieqing](https://github.com/xieqing).

## DEFINITION

A red-black tree is a binary search tree where each node has a color attribute, the value of which is either red or black. Essentially, it is just a convenient way to express a 2-3-4 binary search tree where the color indicates whether the node is part of a 3-node or a 4-node. 2-3-4 trees and red-black trees are equivalent data structures, red-black trees are simpler to implement, so tend to be used instead.

Binary search property or order property: the key in each node must be greater than or equal to any key stored in the left sub-tree, and less than or equal to any key stored in the right sub-tree.

In addition to the ordinary requirements imposed on binary search trees, we make the following additional requirements of any valid red-black tree.

Red-black properties:
1. Every node is either red or black.
2. The root and leaves (NIL's) are black.
3. Parent of each red node is black.
4. Both children of each red node are black.
5. Every path from a given node to any of its descendant NIL nodes contains the same number of black nodes.

## WHY 2-3-4 TREE?

We could only keep binary search tree almostly balanced instead of completely balanced (consider AVL tree as an example). We need at least 1-3 nodes (2-4 children) to keep tree completely balanced.

```
    Binary search tree

             1
            / \
           /   \
          4     9
         / \   / \
        3   5 6  nil
    
    2-3-4 tree
                        (1)
                       /   \
                      /     \
           (3   4   5)      (6   9)
           /  |   |  \      /  |  \
        nil nil nil  nil nil  nil  nil

    2-children: (1)
    3-children: (6 9)
    4-children: (3 4 5)
```

Why not 2-5 tree, 2-6 tree...?

2-4 tree will guarantee O(log n) using 2, 3 or 4 subtrees per node, while implementation could be trivial (red-black tree). 2-N (N>4) tree still guarantee O(logn), while implementation could be much complicated.

## INSERTION

**Insertion into a 2-3-4 tree**

split, and insert grandparent node into parent cluster

**Insertion into a red-black tree**

Insert as in simple binary search tree

- 0-children root cluster (parent node is BLACK) becomes 2-children root cluster: paint root node BLACK, and done
- 2-children cluster (parent node is BLACK) becomes 3-children cluster: done
- 3-children cluster (parent node is BLACK) becomes 4-children cluster: done
- 3-children cluster (parent node is RED) becomes 4-children cluster: rotate, and done
- 4-children cluster (parent node is RED) splits into 2-children cluster and 3-children cluster: split, and insert grandparent node into parent cluster

## DELETION

**Deletion from 2-3-4 tree**

- transfer, and done
- fuse, and done or delete parent node from parent cluster

**Deletion from red-black tree**

Delete as in simple binary search tree

- 4-children cluster (RED target node) becomes 3-children cluster: done
- 3-children cluster (RED target node) becomes 2-children cluster: done
- 3-children cluster (BLACK target node, RED child node) becomes 2-children cluster: paint child node BLACK, and done
- 2-children root cluster (BLACK target node, BLACK child node) becomes 0-children root cluster: done
- 2-children cluster (BLACK target node, 4-children sibling cluster) becomes 3-children cluster: transfer, and done
- 2-children cluster (BLACK target node, 3-children sibling cluster) becomes 2-children cluster: transfer, and done
- 2-children cluster (BLACK target node, 2-children sibling cluster, 3/4-children parent cluster) becomes 3-children cluster: fuse, paint parent node BLACK, and done
- 2-children cluster (BLACK target node, 2-children sibling cluster, 2-children parent cluster) becomes 3-children cluster: fuse, and delete parent node from parent cluster

## References

1. [https://en.wikipedia.org/wiki/Red-black_tree](https://en.wikipedia.org/wiki/Red-black_tree)
2. [https://en.wikipedia.org/wiki/2-3-4_tree](https://en.wikipedia.org/wiki/2-3-4_tree)
3. [https://www.cs.usfca.edu/~galles/visualization/RedBlack.html](https://www.cs.usfca.edu/~galles/visualization/RedBlack.html)

## License

Copyright (c) 2019 xieqing. https://github.com/xieqing

May be freely redistributed, but copyright notice must be retained.
