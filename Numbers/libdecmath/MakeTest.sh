#!/bin/bash -x

rm -f dm_muldiv.o

rm -f dm_muldiv.gcda
rm -f dm_muldiv.gcno

rm -f Base.info
rm -f Run.info
rm -f Full.info

rm -f MulDivTests-MulDivTests.gcda
rm -f MulDivTests-MulDivTests.gcno

rm -f MulDivTests.exe

rm -rf cov

if [ "$1" == "clean" ]; then
   exit
fi

gcc -c -Wall -Wextra -Wpedantic -g --coverage -O0 -DDM_NO_128_BIT_TYPE dm_muldiv.c
g++ -o MulDivTests -Wall -Wextra -Wpedantic -g --coverage -O0 -DDM_NO_128_BIT_TYPE -I../External/googletest/include MulDivTests.cpp dm_muldiv.o ../External/googletest/lib/libgtest.a ../External/googletest/lib/libgtest_main.a


if [ "$1" == "nocov" ]; then
   exit
fi

../External/lcov/bin/lcov --rc lcov_branch_coverage=1 --no-external --capture --initial --directory . --output-file Base.info
./MulDivTests.exe
../External/lcov/bin/lcov --rc lcov_branch_coverage=1 --no-external --capture --directory . --output-file Run.info
../External/lcov/bin/lcov --rc lcov_branch_coverage=1 --add-tracefile Base.info --add-tracefile Run.info --output-file Full.info
mkdir cov
../External/lcov/bin/genhtml --rc lcov_branch_coverage=1 --branch-coverage --output-directory cov Full.info
