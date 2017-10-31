CC=g++

all: Calculator.exe TokenTesting.exe

Calculator.exe: Tokenizer.o Expression.o Formula.o Scope.o Main.o
	${CC} -g -std=gnu++11 Tokenizer.o Expression.o Formula.o Scope.o Main.o -o Calculator.exe

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

Formula.o: Expression/Formula.cpp
	${CC} -c -std=gnu++11 Expression/Formula.cpp