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
struct Lambda;

enum Type {
	INTEGER,
	VARIABLE,
	SEQUENCE,
	LAMBDA,
	SET,
	ADDITION,
	SUBTRACTION,
	MULTIPLICATION,
	DIVISION,
	POWER,
	ROOT,
	LOG,

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
	FUNCTION_INIT,
	LAMBDA_INIT,
	LAMBDA_RUN,
	IF_ELSE,
	SIZE,

//MARKERS
	P_MARKER,
	B_MARKER,
	C_MARKER,
	END_MARKER,
	IMMUTABLE
};

struct Expression{
	Type type;
	long long value = 0;
	Lambda * lambda = NULL;
	std::string var_key;
	std::vector<Expression *> data;
	Scope * parent;
	Expression();
	Expression(Lambda * lambda);
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

struct Lambda {
	Expression * exp;
	Scope * parent;
	std::vector<std::string> arg_names;
	Lambda(std::vector<std::string> arg_names, Scope * parent);
	Expression * evaluate(std::vector<Expression *> params);
	std::string getString();
};

Expression * convertTokens(Scope * prim, const std::vector<std::string> &tokens, const std::vector<std::string> &operators);
Expression * addition(Expression * a, Expression * b, Scope * parent);
Expression * subtraction(Expression * a, Expression * b, Scope * parent);
Expression * multiplication(Expression * a, Expression * b, Scope * parent);
Expression * division(Expression * a, Expression * b, Scope * parent);
Expression * power(Expression * a, Expression * b, Scope * parent);
Expression * root(Expression * a, Expression * b, Scope * parent);
Expression * logar(Expression * a, Expression * b, Scope * parent);

long long gcd(long long a, long long b);
std::vector<long long> factor(long long n, long long fac);
bool expressionEquals(Expression * a, Expression * b);
bool double_equals(double a, double b, double epsilon = 0.00000001);
std::vector<long long> primeFactors(long long n);

#endif