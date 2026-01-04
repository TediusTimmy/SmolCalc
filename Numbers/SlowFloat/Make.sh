#!/bin/sh -x

g++ -Wall -Wextra -Wpedantic -O3 -c SlowFloat.cpp
ar -rcs SlowFloat.a SlowFloat.o
