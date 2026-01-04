#!/bin/bash -x

if [ "$FILE_TO_TEST" == "" ]
then
   export FILE_TO_TEST=dm_double
   export OPTION=""
else
   export OPTION="-DMISRAbleC"
fi

rm -f $FILE_TO_TEST.o
rm -f dm_muldiv.o
rm -f dm_double_pretty.o

rm -f $FILE_TO_TEST.gcda
rm -f $FILE_TO_TEST.gcno
rm -f dm_muldiv.gcda
rm -f dm_muldiv.gcno
rm -f dm_double_pretty.gcda
rm -f dm_double_pretty.gcno

rm -f Base.info
rm -f Run.info
rm -f Full.info

rm -f DoubleTests-DoubleTests.gcda
rm -f DoubleTests-DoubleTests.gcno

rm -f DoubleTests.exe

rm -rf cov

if [ "$1" == "clean" ]; then
   exit
fi

gcc -c -Wall -Wextra -Wpedantic -g --coverage -O0 $FILE_TO_TEST.c dm_muldiv.c dm_double_pretty.c
g++ -o DoubleTests -Wall -Wextra -Wpedantic -g --coverage -O0 -I../External/googletest/include $OPTION DoubleTests.cpp $FILE_TO_TEST.o dm_muldiv.o dm_double_pretty.o ../External/googletest/lib/libgtest.a ../External/googletest/lib/libgtest_main.a


if [ "$1" == "nocov" ]; then
   exit
fi

../External/lcov/bin/lcov --rc lcov_branch_coverage=1 --no-external --capture --initial --directory . --output-file Base.info
./DoubleTests.exe
../External/lcov/bin/lcov --rc lcov_branch_coverage=1 --no-external --capture --directory . --output-file Run.info
../External/lcov/bin/lcov --rc lcov_branch_coverage=1 --add-tracefile Base.info --add-tracefile Run.info --output-file Full.info
mkdir cov
../External/lcov/bin/genhtml --rc lcov_branch_coverage=1 --branch-coverage --output-directory cov Full.info
