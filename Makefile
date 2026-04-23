#Merkfile for SmolCalc

CC  := gcc
CCP := g++

CFLAGS += -Wall -Wextra -Wpedantic

B_INCLUDE := -INumbers -IBackwards/include
F_INCLUDE := $(B_INCLUDE) -IForwards/include

LIBMPDEC_FLAGS := -Wall -Wextra -Wno-unknown-pragmas -std=c99 -pedantic -DCONFIG_64 -DASM -DNDEBUG -O2

ifeq "$(MAKECMDGOALS)" "release"
   CFLAGS += -O2
   BFLAGS += -s
endif

ifeq "$(MAKECMDGOALS)" "debug"
   CFLAGS += -O0 -g
endif

.PHONY: all clean release debug
all: bin/SmolCalc.exe


clean:
	rm bin/*.exe | true
	rm lib/* | true
	rm -rf obj/* | true

release: all


debug: all


bin/SmolCalc.exe: lib/libbcnum.a lib/libdecmath.a lib/libmpdec.a lib/NumLib.a lib/backwards.a lib/Forwards.a obj/main.o obj/Screen.o obj/BatchMode.o obj/GetAndSet.o obj/LibraryLoader.o obj/SaveFile.o obj/StdLib.o | bin
	$(CCP) $(CFLAGS) $(BFLAGS) -o bin/SmolCalc.exe obj/*.o lib/*.a -lncurses -lmpfr -lgmp

obj/main.o: Curses/main.cpp
	$(CCP) $(CFLAGS) $(F_INCLUDE) -IOddsAndEnds -c -o obj/main.o Curses/main.cpp

obj/Screen.o: Curses/Screen.cpp
	$(CCP) $(CFLAGS) $(F_INCLUDE) -IOddsAndEnds -c -o obj/Screen.o Curses/Screen.cpp

obj/BatchMode.o: OddsAndEnds/BatchMode.cpp
	$(CCP) $(CFLAGS) $(F_INCLUDE) -c -o obj/BatchMode.o OddsAndEnds/BatchMode.cpp

obj/GetAndSet.o: OddsAndEnds/GetAndSet.cpp
	$(CCP) $(CFLAGS) $(F_INCLUDE) -c -o obj/GetAndSet.o OddsAndEnds/GetAndSet.cpp

obj/LibraryLoader.o: OddsAndEnds/LibraryLoader.cpp
	$(CCP) $(CFLAGS) $(F_INCLUDE) -c -o obj/LibraryLoader.o OddsAndEnds/LibraryLoader.cpp

obj/SaveFile.o: OddsAndEnds/SaveFile.cpp
	$(CCP) $(CFLAGS) $(F_INCLUDE) -c -o obj/SaveFile.o OddsAndEnds/SaveFile.cpp

obj/StdLib.o: OddsAndEnds/StdLib.cpp
	$(CCP) $(CFLAGS) $(F_INCLUDE) -c -o obj/StdLib.o OddsAndEnds/StdLib.cpp


lib/libbcnum.a: obj/libbcnum/Integer.o obj/libbcnum/Fixed.o | lib
	ar -rsc lib/libbcnum.a obj/libbcnum/*.o

obj/libbcnum/Integer.o: Numbers/BCNum/Integer.cpp | obj/libbcnum
	$(CCP) $(CFLAGS) -c -o obj/libbcnum/Integer.o Numbers/BCNum/Integer.cpp

obj/libbcnum/Fixed.o: Numbers/BCNum/Fixed.cpp | obj/libbcnum
	$(CCP) $(CFLAGS) -c -o obj/libbcnum/Fixed.o Numbers/BCNum/Fixed.cpp


lib/libdecmath.a: obj/libdecmath/dm_double.o obj/libdecmath/dm_muldiv.o obj/libdecmath/dm_double_pretty.o | lib
	ar -rsc lib/libdecmath.a obj/libdecmath/*.o

obj/libdecmath/dm_double.o: Numbers/libdecmath/dm_double.c | obj/libdecmath
	$(CC) $(CFLAGS) -c -o obj/libdecmath/dm_double.o Numbers/libdecmath/dm_double.c

obj/libdecmath/dm_muldiv.o: Numbers/libdecmath/dm_muldiv.c | obj/libdecmath
	$(CC) $(CFLAGS) -c -o obj/libdecmath/dm_muldiv.o Numbers/libdecmath/dm_muldiv.c

obj/libdecmath/dm_double_pretty.o: Numbers/libdecmath/dm_double_pretty.c | obj/libdecmath
	$(CC) $(CFLAGS) -c -o obj/libdecmath/dm_double_pretty.o Numbers/libdecmath/dm_double_pretty.c


lib/libmpdec.a: obj/libmpdec/basearith.o obj/libmpdec/constants.o obj/libmpdec/context.o obj/libmpdec/convolute.o obj/libmpdec/crt.o obj/libmpdec/difradix2.o obj/libmpdec/fnt.o obj/libmpdec/fourstep.o obj/libmpdec/io.o obj/libmpdec/mpalloc.o obj/libmpdec/mpdecimal.o obj/libmpdec/mpsignal.o obj/libmpdec/numbertheory.o obj/libmpdec/sixstep.o obj/libmpdec/transpose.o
	ar -rcs lib/libmpdec.a obj/libmpdec/*.o

obj/libmpdec/basearith.o: external/libmpdec/basearith.c | obj/libmpdec
	$(CC) $(LIBMPDEC_FLAGS) -c -o obj/libmpdec/basearith.o external/libmpdec/basearith.c

obj/libmpdec/constants.o: external/libmpdec/constants.c | obj/libmpdec
	$(CC) $(LIBMPDEC_FLAGS) -c -o obj/libmpdec/constants.o external/libmpdec/constants.c

obj/libmpdec/context.o: external/libmpdec/context.c | obj/libmpdec
	$(CC) $(LIBMPDEC_FLAGS) -c -o obj/libmpdec/context.o external/libmpdec/context.c

obj/libmpdec/convolute.o: external/libmpdec/convolute.c | obj/libmpdec
	$(CC) $(LIBMPDEC_FLAGS) -c -o obj/libmpdec/convolute.o external/libmpdec/convolute.c

obj/libmpdec/crt.o: external/libmpdec/crt.c | obj/libmpdec
	$(CC) $(LIBMPDEC_FLAGS) -c -o obj/libmpdec/crt.o external/libmpdec/crt.c

obj/libmpdec/difradix2.o: external/libmpdec/difradix2.c | obj/libmpdec
	$(CC) $(LIBMPDEC_FLAGS) -c -o obj/libmpdec/difradix2.o external/libmpdec/difradix2.c

obj/libmpdec/fnt.o: external/libmpdec/fnt.c | obj/libmpdec
	$(CC) $(LIBMPDEC_FLAGS) -c -o obj/libmpdec/fnt.o external/libmpdec/fnt.c

obj/libmpdec/fourstep.o: external/libmpdec/fourstep.c | obj/libmpdec
	$(CC) $(LIBMPDEC_FLAGS) -c -o obj/libmpdec/fourstep.o external/libmpdec/fourstep.c

obj/libmpdec/io.o: external/libmpdec/io.c | obj/libmpdec
	$(CC) $(LIBMPDEC_FLAGS) -c -o obj/libmpdec/io.o external/libmpdec/io.c

obj/libmpdec/mpalloc.o: external/libmpdec/mpalloc.c | obj/libmpdec
	$(CC) $(LIBMPDEC_FLAGS) -c -o obj/libmpdec/mpalloc.o external/libmpdec/mpalloc.c

obj/libmpdec/mpdecimal.o: external/libmpdec/mpdecimal.c | obj/libmpdec
	$(CC) $(LIBMPDEC_FLAGS) -c -o obj/libmpdec/mpdecimal.o external/libmpdec/mpdecimal.c

obj/libmpdec/mpsignal.o: external/libmpdec/mpsignal.c | obj/libmpdec
	$(CC) $(LIBMPDEC_FLAGS) -c -o obj/libmpdec/mpsignal.o external/libmpdec/mpsignal.c

obj/libmpdec/numbertheory.o: external/libmpdec/numbertheory.c | obj/libmpdec
	$(CC) $(LIBMPDEC_FLAGS) -c -o obj/libmpdec/numbertheory.o external/libmpdec/numbertheory.c

obj/libmpdec/sixstep.o: external/libmpdec/sixstep.c | obj/libmpdec
	$(CC) $(LIBMPDEC_FLAGS) -c -o obj/libmpdec/sixstep.o external/libmpdec/sixstep.c

obj/libmpdec/transpose.o: external/libmpdec/transpose.c | obj/libmpdec
	$(CC) $(LIBMPDEC_FLAGS) -c -o obj/libmpdec/transpose.o external/libmpdec/transpose.c


lib/NumLib.a: obj/NumLib/NumberHolder.o obj/NumLib/NumberSystem.o obj/NumLib/BCNum_NumberSystem.o obj/NumLib/libdecmath_NumberSystem.o obj/NumLib/SlowFloat_NumberSystem.o obj/NumLib/SlowFloat.o obj/NumLib/double_NumberSystem.o obj/NumLib/libmpdec_NumberSystem.o obj/NumLib/mpfr_NumberSystem.o obj/NumLib/Float.o obj/NumLib/FloatFixed.o obj/NumLib/DAPFP_NumberSystem.o | lib
	ar -rsc lib/NumLib.a obj/NumLib/*.o

obj/NumLib/NumberHolder.o: Numbers/NumberHolder.cpp | obj/NumLib
	$(CCP) $(CFLAGS) -c -o obj/NumLib/NumberHolder.o Numbers/NumberHolder.cpp

obj/NumLib/NumberSystem.o: Numbers/NumberSystem.cpp | obj/NumLib
	$(CCP) $(CFLAGS) -c -o obj/NumLib/NumberSystem.o Numbers/NumberSystem.cpp

obj/NumLib/BCNum_NumberSystem.o: Numbers/BCNum_NumberSystem.cpp | obj/NumLib
	$(CCP) $(CFLAGS) -c -o obj/NumLib/BCNum_NumberSystem.o Numbers/BCNum_NumberSystem.cpp

obj/NumLib/libdecmath_NumberSystem.o: Numbers/libdecmath_NumberSystem.cpp | obj/NumLib
	$(CCP) $(CFLAGS) -c -o obj/NumLib/libdecmath_NumberSystem.o Numbers/libdecmath_NumberSystem.cpp

obj/NumLib/SlowFloat_NumberSystem.o: Numbers/SlowFloat_NumberSystem.cpp | obj/NumLib
	$(CCP) $(CFLAGS) -c -o obj/NumLib/SlowFloat_NumberSystem.o Numbers/SlowFloat_NumberSystem.cpp

obj/NumLib/SlowFloat.o: Numbers/SlowFloat/SlowFloat.cpp | obj/NumLib
	$(CCP) $(CFLAGS) -c -o obj/NumLib/SlowFloat.o Numbers/SlowFloat/SlowFloat.cpp

obj/NumLib/double_NumberSystem.o: Numbers/double_NumberSystem.cpp | obj/NumLib
	$(CCP) $(CFLAGS) -c -o obj/NumLib/double_NumberSystem.o Numbers/double_NumberSystem.cpp

obj/NumLib/libmpdec_NumberSystem.o: Numbers/libmpdec_NumberSystem.cpp | obj/NumLib
	$(CCP) $(CFLAGS) -c -o obj/NumLib/libmpdec_NumberSystem.o Numbers/libmpdec_NumberSystem.cpp

obj/NumLib/mpfr_NumberSystem.o: Numbers/mpfr_NumberSystem.cpp | obj/NumLib
	$(CCP) $(CFLAGS) -c -o obj/NumLib/mpfr_NumberSystem.o Numbers/mpfr_NumberSystem.cpp

obj/NumLib/Float.o: Numbers/DAPFP/Float.cpp | obj/NumLib
	$(CCP) $(CFLAGS) -c -o obj/NumLib/Float.o Numbers/DAPFP/Float.cpp

obj/NumLib/FloatFixed.o: Numbers/DAPFP/FloatFixed.cpp | obj/NumLib
	$(CCP) $(CFLAGS) -c -o obj/NumLib/FloatFixed.o Numbers/DAPFP/FloatFixed.cpp

obj/NumLib/DAPFP_NumberSystem.o: Numbers/DAPFP_NumberSystem.cpp | obj/NumLib
	$(CCP) $(CFLAGS) -c -o obj/NumLib/DAPFP_NumberSystem.o Numbers/DAPFP_NumberSystem.cpp


lib/backwards.a: obj/Backwards/CallingContext.o obj/Backwards/ConstantsSingleton.o obj/Backwards/Expression.o obj/Backwards/Statement.o obj/Backwards/StdLib.o obj/Backwards/BufferedGenericInput.o obj/Backwards/Lexer.o obj/Backwards/LineBufferedStreamInput.o obj/Backwards/StringInput.o obj/Backwards/ContextBuilder.o obj/Backwards/DebuggerHook.o obj/Backwards/Eval.o obj/Backwards/Parser.o obj/Backwards/SymbolTable.o obj/Backwards/ArrayValue.o obj/Backwards/CellRangeValue.o obj/Backwards/CellRefValue.o obj/Backwards/DictionaryValue.o obj/Backwards/FloatValue.o obj/Backwards/FunctionValue.o obj/Backwards/NilValue.o obj/Backwards/StringValue.o obj/Backwards/ValueType.o | lib
	ar -rsc lib/backwards.a obj/Backwards/*.o

obj/Backwards/CallingContext.o: Backwards/src/Engine/CallingContext.cpp | obj/Backwards
	$(CCP) $(CFLAGS) $(B_INCLUDE) -c -o obj/Backwards/CallingContext.o Backwards/src/Engine/CallingContext.cpp

obj/Backwards/ConstantsSingleton.o: Backwards/src/Engine/ConstantsSingleton.cpp | obj/Backwards
	$(CCP) $(CFLAGS) $(B_INCLUDE) -c -o obj/Backwards/ConstantsSingleton.o Backwards/src/Engine/ConstantsSingleton.cpp

obj/Backwards/Expression.o: Backwards/src/Engine/Expression.cpp | obj/Backwards
	$(CCP) $(CFLAGS) $(B_INCLUDE) -c -o obj/Backwards/Expression.o Backwards/src/Engine/Expression.cpp

obj/Backwards/Statement.o: Backwards/src/Engine/Statement.cpp | obj/Backwards
	$(CCP) $(CFLAGS) $(B_INCLUDE) -c -o obj/Backwards/Statement.o Backwards/src/Engine/Statement.cpp

obj/Backwards/StdLib.o: Backwards/src/Engine/StdLib.cpp | obj/Backwards
	$(CCP) $(CFLAGS) $(B_INCLUDE) -c -o obj/Backwards/StdLib.o Backwards/src/Engine/StdLib.cpp

obj/Backwards/BufferedGenericInput.o: Backwards/src/Input/BufferedGenericInput.cpp | obj/Backwards
	$(CCP) $(CFLAGS) $(B_INCLUDE) -c -o obj/Backwards/BufferedGenericInput.o Backwards/src/Input/BufferedGenericInput.cpp

obj/Backwards/Lexer.o: Backwards/src/Input/Lexer.cpp | obj/Backwards
	$(CCP) $(CFLAGS) $(B_INCLUDE) -c -o obj/Backwards/Lexer.o Backwards/src/Input/Lexer.cpp

obj/Backwards/LineBufferedStreamInput.o: Backwards/src/Input/LineBufferedStreamInput.cpp | obj/Backwards
	$(CCP) $(CFLAGS) $(B_INCLUDE) -c -o obj/Backwards/LineBufferedStreamInput.o Backwards/src/Input/LineBufferedStreamInput.cpp

obj/Backwards/StringInput.o: Backwards/src/Input/StringInput.cpp | obj/Backwards
	$(CCP) $(CFLAGS) $(B_INCLUDE) -c -o obj/Backwards/StringInput.o Backwards/src/Input/StringInput.cpp

obj/Backwards/ContextBuilder.o: Backwards/src/Parser/ContextBuilder.cpp | obj/Backwards
	$(CCP) $(CFLAGS) $(B_INCLUDE) -c -o obj/Backwards/ContextBuilder.o Backwards/src/Parser/ContextBuilder.cpp

obj/Backwards/DebuggerHook.o: Backwards/src/Parser/DebuggerHook.cpp | obj/Backwards
	$(CCP) $(CFLAGS) $(B_INCLUDE) -c -o obj/Backwards/DebuggerHook.o Backwards/src/Parser/DebuggerHook.cpp

obj/Backwards/Eval.o: Backwards/src/Parser/Eval.cpp | obj/Backwards
	$(CCP) $(CFLAGS) $(B_INCLUDE) -c -o obj/Backwards/Eval.o Backwards/src/Parser/Eval.cpp

obj/Backwards/Parser.o: Backwards/src/Parser/Parser.cpp | obj/Backwards
	$(CCP) $(CFLAGS) $(B_INCLUDE) -c -o obj/Backwards/Parser.o Backwards/src/Parser/Parser.cpp

obj/Backwards/SymbolTable.o: Backwards/src/Parser/SymbolTable.cpp | obj/Backwards
	$(CCP) $(CFLAGS) $(B_INCLUDE) -c -o obj/Backwards/SymbolTable.o Backwards/src/Parser/SymbolTable.cpp

obj/Backwards/ArrayValue.o: Backwards/src/Types/ArrayValue.cpp | obj/Backwards
	$(CCP) $(CFLAGS) $(B_INCLUDE) -c -o obj/Backwards/ArrayValue.o Backwards/src/Types/ArrayValue.cpp

obj/Backwards/CellRangeValue.o: Backwards/src/Types/CellRangeValue.cpp | obj/Backwards
	$(CCP) $(CFLAGS) $(B_INCLUDE) -c -o obj/Backwards/CellRangeValue.o Backwards/src/Types/CellRangeValue.cpp

obj/Backwards/CellRefValue.o: Backwards/src/Types/CellRefValue.cpp | obj/Backwards
	$(CCP) $(CFLAGS) $(B_INCLUDE) -c -o obj/Backwards/CellRefValue.o Backwards/src/Types/CellRefValue.cpp

obj/Backwards/DictionaryValue.o: Backwards/src/Types/DictionaryValue.cpp | obj/Backwards
	$(CCP) $(CFLAGS) $(B_INCLUDE) -c -o obj/Backwards/DictionaryValue.o Backwards/src/Types/DictionaryValue.cpp

obj/Backwards/FloatValue.o: Backwards/src/Types/FloatValue.cpp | obj/Backwards
	$(CCP) $(CFLAGS) $(B_INCLUDE) -c -o obj/Backwards/FloatValue.o Backwards/src/Types/FloatValue.cpp

obj/Backwards/FunctionValue.o: Backwards/src/Types/FunctionValue.cpp | obj/Backwards
	$(CCP) $(CFLAGS) $(B_INCLUDE) -c -o obj/Backwards/FunctionValue.o Backwards/src/Types/FunctionValue.cpp

obj/Backwards/NilValue.o: Backwards/src/Types/NilValue.cpp | obj/Backwards
	$(CCP) $(CFLAGS) $(B_INCLUDE) -c -o obj/Backwards/NilValue.o Backwards/src/Types/NilValue.cpp

obj/Backwards/StringValue.o: Backwards/src/Types/StringValue.cpp | obj/Backwards
	$(CCP) $(CFLAGS) $(B_INCLUDE) -c -o obj/Backwards/StringValue.o Backwards/src/Types/StringValue.cpp

obj/Backwards/ValueType.o: Backwards/src/Types/ValueType.cpp | obj/Backwards
	$(CCP) $(CFLAGS) $(B_INCLUDE) -c -o obj/Backwards/ValueType.o Backwards/src/Types/ValueType.cpp


lib/Forwards.a: obj/Forwards/CallingContext.o obj/Forwards/CellRangeExpand.o obj/Forwards/CellRefEval.o obj/Forwards/Expression.o obj/Forwards/ShuntingYard.o obj/Forwards/StdLib.o obj/Forwards/Lexer.o obj/Forwards/CellEval.o obj/Forwards/ContextBuilder.o obj/Forwards/SpreadSheet.o obj/Forwards/CellRangeValue.o obj/Forwards/CellRefValue.o obj/Forwards/FloatValue.o obj/Forwards/NilValue.o obj/Forwards/StringValue.o | lib
	ar -rsc lib/Forwards.a obj/Forwards/*.o

obj/Forwards/CallingContext.o: Forwards/src/Engine/CallingContext.cpp | obj/Forwards
	$(CCP) $(CFLAGS) $(F_INCLUDE) -c -o obj/Forwards/CallingContext.o Forwards/src/Engine/CallingContext.cpp

obj/Forwards/CellRangeExpand.o: Forwards/src/Engine/CellRangeExpand.cpp | obj/Forwards
	$(CCP) $(CFLAGS) $(F_INCLUDE) -c -o obj/Forwards/CellRangeExpand.o Forwards/src/Engine/CellRangeExpand.cpp

obj/Forwards/CellRefEval.o: Forwards/src/Engine/CellRefEval.cpp | obj/Forwards
	$(CCP) $(CFLAGS) $(F_INCLUDE) -c -o obj/Forwards/CellRefEval.o Forwards/src/Engine/CellRefEval.cpp

obj/Forwards/Expression.o: Forwards/src/Engine/Expression.cpp | obj/Forwards
	$(CCP) $(CFLAGS) $(F_INCLUDE) -c -o obj/Forwards/Expression.o Forwards/src/Engine/Expression.cpp

obj/Forwards/ShuntingYard.o: Forwards/src/Engine/ShuntingYard.cpp | obj/Forwards
	$(CCP) $(CFLAGS) $(F_INCLUDE) -c -o obj/Forwards/ShuntingYard.o Forwards/src/Engine/ShuntingYard.cpp

obj/Forwards/StdLib.o: Forwards/src/Engine/StdLib.cpp | obj/Forwards
	$(CCP) $(CFLAGS) $(F_INCLUDE) -c -o obj/Forwards/StdLib.o Forwards/src/Engine/StdLib.cpp

obj/Forwards/Lexer.o: Forwards/src/Input/Lexer.cpp | obj/Forwards
	$(CCP) $(CFLAGS) $(F_INCLUDE) -c -o obj/Forwards/Lexer.o Forwards/src/Input/Lexer.cpp

obj/Forwards/CellEval.o: Forwards/src/Parser/CellEval.cpp | obj/Forwards
	$(CCP) $(CFLAGS) $(F_INCLUDE) -c -o obj/Forwards/CellEval.o Forwards/src/Parser/CellEval.cpp

obj/Forwards/ContextBuilder.o: Forwards/src/Parser/ContextBuilder.cpp | obj/Forwards
	$(CCP) $(CFLAGS) $(F_INCLUDE) -c -o obj/Forwards/ContextBuilder.o Forwards/src/Parser/ContextBuilder.cpp

obj/Forwards/SpreadSheet.o: Forwards/src/Parser/SpreadSheet.cpp | obj/Forwards
	$(CCP) $(CFLAGS) $(F_INCLUDE) -c -o obj/Forwards/SpreadSheet.o Forwards/src/Parser/SpreadSheet.cpp

obj/Forwards/CellRangeValue.o: Forwards/src/Types/CellRangeValue.cpp | obj/Forwards
	$(CCP) $(CFLAGS) $(F_INCLUDE) -c -o obj/Forwards/CellRangeValue.o Forwards/src/Types/CellRangeValue.cpp

obj/Forwards/CellRefValue.o: Forwards/src/Types/CellRefValue.cpp | obj/Forwards
	$(CCP) $(CFLAGS) $(F_INCLUDE) -c -o obj/Forwards/CellRefValue.o Forwards/src/Types/CellRefValue.cpp

obj/Forwards/FloatValue.o: Forwards/src/Types/FloatValue.cpp | obj/Forwards
	$(CCP) $(CFLAGS) $(F_INCLUDE) -c -o obj/Forwards/FloatValue.o Forwards/src/Types/FloatValue.cpp

obj/Forwards/NilValue.o: Forwards/src/Types/NilValue.cpp | obj/Forwards
	$(CCP) $(CFLAGS) $(F_INCLUDE) -c -o obj/Forwards/NilValue.o Forwards/src/Types/NilValue.cpp

obj/Forwards/StringValue.o: Forwards/src/Types/StringValue.cpp | obj/Forwards
	$(CCP) $(CFLAGS) $(F_INCLUDE) -c -o obj/Forwards/StringValue.o Forwards/src/Types/StringValue.cpp


bin:
	mkdir bin

lib:
	mkdir lib

obj/libbcnum:
	mkdir -p obj/libbcnum

obj/libdecmath:
	mkdir -p obj/libdecmath

obj/libmpdec:
	mkdir -p obj/libmpdec

obj/NumLib:
	mkdir -p obj/NumLib

obj/Backwards:
	mkdir -p obj/Backwards

obj/Forwards:
	mkdir -p obj/Forwards
