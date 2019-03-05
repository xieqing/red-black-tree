#!/bin/bash

# gcc rb.c rb_data.c rb_test.c && (time valgrind --leak-check=yes ./a.out || time ./a.out)

gcc rb.c rb_data.c rb_test.c && time ./a.out
