
# red-black tree implementation

## overview

A red-black tree is a binary search tree where each node has a color attribute, the value of which is either red or black. Essentially, it is just a convenient way to express a 2-3-4 binary search tree where the color indicates whether the node is part of a 3-node or a 4-node. 2-3-4 trees and red-black trees are equivalent data structures, red-black trees are simpler to implement, so tend to be used instead.

## files

* rb.h - red-black tree header
* rb.c - red-black tree library
* rb_data.h - red-black tree data header
* rb_data.c - red-black tree data library
* rb_example.c - red-black tree example
* rb_test.c - red-black tree unit test
* rb_test.sh - red-black tree unit test shell script
* rb_note.txt - red-black tree implementation note
