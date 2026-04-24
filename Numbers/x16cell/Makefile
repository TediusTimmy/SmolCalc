CC := gcc
LCOV := ../../../External/lcov/bin

CFLAGS += -Wall -Wextra -Wpedantic

T_INCLUDE := -I.

ifeq "$(MAKECMDGOALS)" "release"
   CFLAGS += -O2
endif

ifeq "$(MAKECMDGOALS)" "debug"
   CFLAGS += -O0 -g
endif

ifeq "$(MAKECMDGOALS)" "test"
   CFLAGS += -O0 -g
endif

ifeq "$(MAKECMDGOALS)" "coverage"
   CFLAGS += -O0 -g --coverage
endif

.PHONY: all clean release debug test coverage
all:
	echo There is no all there is only Zuul.

clean:
	rm *.exe *.gcda *.gcno *.info *.o | true
	rm -rf cov

release: floats.o


debug: floats.o


test: AddTest.exe MiscTest.exe MulTest.exe
	./AddTest.exe
	./MiscTest.exe
	./MulTest.exe

coverage: AddTest.exe MiscTest.exe MulTest.exe
	$(LCOV)/lcov --rc lcov_branch_coverage=1 --capture --initial --directory . --output-file Base.info
	echo Start Test
	./AddTest.exe
	./MiscTest.exe
	./MulTest.exe
	echo Test Done
	$(LCOV)/lcov --rc lcov_branch_coverage=1 --capture --directory . --output-file Run.info
	$(LCOV)/lcov --rc lcov_branch_coverage=1 --add-tracefile Base.info --add-tracefile Run.info --output-file Full.info
	mkdir cov
	$(LCOV)/genhtml --rc lcov_branch_coverage=1 --branch-coverage --output-directory ./cov Full.info

AddTest.exe: floats.o AddTest.c
	$(CC) $(CFLAGS) $(T_INCLUDE) -o AddTest.exe AddTest.c floats.o

MiscTest.exe: floats.o MiscTest.c
	$(CC) $(CFLAGS) $(T_INCLUDE) -o MiscTest.exe MiscTest.c floats.o

MulTest.exe: floats.o MulTest.c
	$(CC) $(CFLAGS) $(T_INCLUDE) -o MulTest.exe MulTest.c floats.o

floats.o: floats.c
	$(CC) $(CFLAGS) -c -o floats.o floats.c
