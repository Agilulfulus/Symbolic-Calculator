CC=g++

all: symc.exe TokenTesting.exe

symc.exe: Tokenizer.o Expression.o Formula.o Scope.o Lambda.o Main.o Compiler.o
	${CC} -g -std=gnu++11 Tokenizer.o Expression.o Formula.o Scope.o Lambda.o Compiler.o Main.o -o symc.exe

Main.o: Main.cpp
	${CC} -c -std=gnu++11 Main.cpp

TokenTesting.exe: Tokenizer.o TokenTesting.cpp
	${CC} -g -std=gnu++11 Tokenizer.o TokenTesting.cpp -o TokenTesting.exe

Tokenizer.o: Tokenizer/Tokenizer.cpp Tokenizer/Tokenizer.h
	${CC} -c -std=gnu++11 Tokenizer/Tokenizer.cpp

Expression.o: Expression/Expression.cpp Expression/Expression.h
	${CC} -c -std=gnu++11 Expression/Expression.cpp

Scope.o: Expression/Scope.cpp Expression/Expression.h
	${CC} -c -std=gnu++11 Expression/Scope.cpp

Compiler.o: Expression/Compiler.cpp Expression/Expression.h
	${CC} -c -std=gnu++11 Expression/Compiler.cpp

Formula.o: Expression/Formula.cpp Expression/Expression.h
	${CC} -c -std=gnu++11 Expression/Formula.cpp

Lambda.o: Expression/Lambda.cpp Expression/Expression.h
	${CC} -c -std=gnu++11 Expression/Lambda.cpp