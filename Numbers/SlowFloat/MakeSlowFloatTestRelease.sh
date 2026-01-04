#!/bin/sh -x

rm -f SlowFloat.o
rm -f SlowFloatTest.exe

if [ "$1" == "clean" ]; then
   exit
fi

g++ -Wall -Wextra -Wpedantic -s -O3 -c SlowFloat.cpp

g++ -o SlowFloatTest -Wall -Wextra -Wpedantic -s -O3 -I../../../External/googletest/include SlowFloatTest.cpp SlowFloat.o ../../../External/googletest/lib/libgtest.a ../../../External/googletest/lib/libgtest_main.a
./SlowFloatTest.exe
