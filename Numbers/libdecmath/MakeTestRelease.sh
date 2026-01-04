#!/bin/bash -x

rm -f dm_muldiv.o
rm -f MulDivTests.exe

if [ "$1" == "clean" ]; then
   exit
fi

gcc -c -Wall -Wextra -Wpedantic -s -O3 -DDM_NO_128_BIT_TYPE dm_muldiv.c
g++ -o MulDivTests -Wall -Wextra -Wpedantic -s -O3 -DDM_NO_128_BIT_TYPE -I../External/googletest/include MulDivTests.cpp dm_muldiv.o ../External/googletest/lib/libgtest.a ../External/googletest/lib/libgtest_main.a
./MulDivTests.exe
