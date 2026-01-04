#!/bin/sh -x

gcc -Wall -Wextra -Wpedantic -O3 -c dm_double.c dm_muldiv.c dm_double_pretty.c
ar -rcs libdecmath.a *.o
rm *.o
