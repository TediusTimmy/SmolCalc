#!/bin/sh -x

x86_64-w64-mingw32-g++.exe -Wall -Wextra -Wpedantic -O3 -std=c++17 -c SlowFloat.cpp
ar -rcs SlowFloat.a SlowFloat.o
