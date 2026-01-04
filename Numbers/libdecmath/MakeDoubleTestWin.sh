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

x86_64-w64-mingw32-gcc.exe -c -Wall -Wextra -Wpedantic -g -O0 $FILE_TO_TEST.c dm_muldiv.c dm_double_pretty.c
x86_64-w64-mingw32-g++.exe -o DoubleTests -Wall -Wextra -Wpedantic -g -O0 -I../External/googletest/include $OPTION DoubleTests.cpp $FILE_TO_TEST.o dm_muldiv.o dm_double_pretty.o ../External/googletest/lib-w64/libgtest.a ../External/googletest/lib-w64/libgtest_main.a

./DoubleTests.exe

x86_64-w64-mingw32-gcc.exe -c -Wall -Wextra -Wpedantic -s -O3 $FILE_TO_TEST.c dm_muldiv.c dm_double_pretty.c
x86_64-w64-mingw32-g++.exe -o DoubleTests -Wall -Wextra -Wpedantic -s -O3 -I../External/googletest/include $OPTION DoubleTests.cpp $FILE_TO_TEST.o dm_muldiv.o dm_double_pretty.o ../External/googletest/lib-w64/libgtest.a ../External/googletest/lib-w64/libgtest_main.a

./DoubleTests.exe

x86_64-w64-mingw32-gcc.exe -c -Wall -Wextra -Wpedantic -g -O0 -DDM_NO_128_BIT_TYPE $FILE_TO_TEST.c dm_muldiv.c dm_double_pretty.c
x86_64-w64-mingw32-g++.exe -o DoubleTests -Wall -Wextra -Wpedantic -g -O0 -I../External/googletest/include $OPTION DoubleTests.cpp $FILE_TO_TEST.o dm_muldiv.o dm_double_pretty.o ../External/googletest/lib-w64/libgtest.a ../External/googletest/lib-w64/libgtest_main.a

./DoubleTests.exe

x86_64-w64-mingw32-gcc.exe -c -Wall -Wextra -Wpedantic -s -O3 -DDM_NO_128_BIT_TYPE $FILE_TO_TEST.c dm_muldiv.c dm_double_pretty.c
x86_64-w64-mingw32-g++.exe -o DoubleTests -Wall -Wextra -Wpedantic -s -O3 -I../External/googletest/include $OPTION DoubleTests.cpp $FILE_TO_TEST.o dm_muldiv.o dm_double_pretty.o ../External/googletest/lib-w64/libgtest.a ../External/googletest/lib-w64/libgtest_main.a

./DoubleTests.exe
