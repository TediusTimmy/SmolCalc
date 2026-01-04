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
rm -f DoubleTests.exe

if [ "$1" == "clean" ]; then
   exit
fi

gcc -c -Wall -Wextra -Wpedantic -s -O3 $FILE_TO_TEST.c dm_muldiv.c dm_double_pretty.c
g++ -o DoubleTests -Wall -Wextra -Wpedantic -s -O3 -I../External/googletest/include $OPTION DoubleTests.cpp $FILE_TO_TEST.o dm_muldiv.o dm_double_pretty.o ../External/googletest/lib/libgtest.a ../External/googletest/lib/libgtest_main.a
./DoubleTests.exe
