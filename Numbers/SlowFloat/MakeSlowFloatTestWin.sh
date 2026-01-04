#!/bin/sh -x

rm -f SlowFloat.o
rm -f SlowFloatTest.exe

if [ "$1" == "clean" ]; then
   exit
fi

x86_64-w64-mingw32-g++.exe -Wall -Wextra -Wpedantic -g -O0 -c SlowFloat.cpp

x86_64-w64-mingw32-g++.exe -o SlowFloatTest -Wall -Wextra -Wpedantic -g -O0 -I../../../External/googletest/include SlowFloatTest.cpp SlowFloat.o ../../../External/googletest/lib-w64/libgtest.a ../../../External/googletest/lib-w64/libgtest_main.a
./SlowFloatTest.exe

x86_64-w64-mingw32-g++.exe -Wall -Wextra -Wpedantic -s -O3 -c SlowFloat.cpp

x86_64-w64-mingw32-g++.exe -o SlowFloatTest -Wall -Wextra -Wpedantic -s -O3 -I../../../External/googletest/include SlowFloatTest.cpp SlowFloat.o ../../../External/googletest/lib-w64/libgtest.a ../../../External/googletest/lib-w64/libgtest_main.a
./SlowFloatTest.exe
