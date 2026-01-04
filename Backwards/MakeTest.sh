#!/bin/sh

g++ -I../../include -I../../../Numbers -g --coverage -O0 -c -Wall -Wextra -Wpedantic *.cpp
