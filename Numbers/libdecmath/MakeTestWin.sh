#!/bin/bash -x

rm -f dm_muldiv.o
rm -f MulDivTests.exe

if [ "$1" == "clean" ]; then
   exit
fi

x86_64-w64-mingw32-gcc.exe -c -Wall -Wextra -Wpedantic -g -O0 -DDM_NO_128_BIT_TYPE dm_muldiv.c
x86_64-w64-mingw32-g++.exe -o MulDivTests -Wall -Wextra -Wpedantic -g -O0 -DDM_NO_128_BIT_TYPE -I../External/googletest/include MulDivTests.cpp dm_muldiv.o ../External/googletest/lib-w64/libgtest.a ../External/googletest/lib-w64/libgtest_main.a

./MulDivTests.exe

x86_64-w64-mingw32-gcc.exe -c -Wall -Wextra -Wpedantic -s -O3 -DDM_NO_128_BIT_TYPE dm_muldiv.c
x86_64-w64-mingw32-g++.exe -o MulDivTests -Wall -Wextra -Wpedantic -s -O3 -DDM_NO_128_BIT_TYPE -I../External/googletest/include MulDivTests.cpp dm_muldiv.o ../External/googletest/lib-w64/libgtest.a ../External/googletest/lib-w64/libgtest_main.a

./MulDivTests.exe
