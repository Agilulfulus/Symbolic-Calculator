#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <vector>
#include <string>
#include <cmath>
#include <map>
#include <iostream>
#include <algorithm>

struct Expression;
struct Scope;

enum Type {
	INTEGER,
	VARIABLE,
	SEQUENCE,
	SET,
	ADDITION,
	SUBTRACTION,
	MULTIPLICATION,
	DIVISION,
	POWER,
	ROOT,

//NON-ARITHMETIC
	SCOPE,
	INDEX,
	RANGE,
	LESS,
	ELESS,
	GREATER,
	EGREATER,
	EQUAL,
	NEQUAL,
	EXTERNAL,
	ITERATOR,
	DO_LOOP,

//MARKERS
	P_MARKER,
	B_MARKER,
	C_MARKER,
	END_MARKER
};

struct Expression{
	Type type;
	long long value = 0;
	std::string var_key;
	std::vector<Expression *> data;
	Scope * parent;
	Expression();
	Expression(long long value, Scope * parent);
	Expression(Type type, std::vector<Expression *> data, Scope * parent);
	Expression(std::string var_key, Scope * parent);
	Expression * evaluate();
	Expression * clone(Scope * parent);
	void set(Expression * e);
	std::string getString();
	double approximate();
};

struct Scope{
	Scope * parent;
	std::map<std::string, Expression *> variables;
	Scope(Scope * parent);
	Expression *& getVariable(std::string key);
};

Expression * convertTokens(Scope * prim, const std::vector<std::string> &tokens, const std::vector<std::string> &operators);
Expression * addition(Expression * a, Expression * b, Scope * parent);
Expression * subtraction(Expression * a, Expression * b, Scope * parent);
Expression * multiplication(Expression * a, Expression * b, Scope * parent);
Expression * division(Expression * a, Expression * b, Scope * parent);
Expression * power(Expression * a, Expression * b, Scope * parent);
Expression * root(Expression * a, Expression * b, Scope * parent);

long long gcd(long long a, long long b);
std::vector<long long> factor(long long n, long long fac);
bool expressionEquals(Expression * a, Expression * b);
bool double_equals(double a, double b, double epsilon = 0.0001);

#endif