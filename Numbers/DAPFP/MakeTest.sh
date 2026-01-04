#!/bin/bash -x

rm -f Fixed.o
rm -f Integer.o
rm -f FloatFixed.o
rm -f Float.o

rm -f FloatFixed.gcda
rm -f FloatFixed.gcno
rm -f Float.gcda
rm -f Float.gcno

rm -f Base.info
rm -f Run.info
rm -f Full.info

rm -f Test-Test.gcda
rm -f Test-Test.gcno

rm -f Test.exe

rm -rf cov

if [ "$1" == "clean" ]; then
   exit
fi

g++ -c -Wall -Wextra -Wpedantic -O2 ../BCNum/Fixed.cpp ../BCNum/Integer.cpp
g++ -c -Wall -Wextra -Wpedantic -g --coverage -O0 FloatFixed.cpp Float.cpp
g++ -o Test -Wall -Wextra -Wpedantic -g --coverage -O0 -I../../../External/googletest/include Test.cpp Float.o FloatFixed.o Fixed.o Integer.o ../../../External/googletest/lib/libgtest.a ../../../External/googletest/lib/libgtest_main.a -lgmp


if [ "$1" == "nocov" ]; then
   exit
fi

../../../External/lcov/bin/lcov --rc lcov_branch_coverage=1 --no-external --capture --initial --directory . --output-file Base.info
./Test.exe
../../../External/lcov/bin/lcov --rc lcov_branch_coverage=1 --no-external --capture --directory . --output-file Run.info
../../../External/lcov/bin/lcov --rc lcov_branch_coverage=1 --add-tracefile Base.info --add-tracefile Run.info --output-file Full.info
mkdir cov
../../../External/lcov/bin/genhtml --rc lcov_branch_coverage=1 --branch-coverage --output-directory cov Full.info
