#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <vector>
#include <string>
#include <cmath>
#include <map>
#include <iostream>
#include <algorithm>
#include <memory>

using std::string;

#define newLmb	std::make_shared<Lambda>
#define newScp	std::make_shared<Scope>

struct Expression;
struct Scope;
struct Lambda;

typedef std::vector<string> 		strVec;
typedef std::shared_ptr<Expression>	expPtr;
typedef std::shared_ptr<Scope>		scpPtr;
typedef std::shared_ptr<Lambda>		lmbPtr;
typedef std::vector<expPtr> 		expVec;
typedef long long 					rawInt;

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
	MODULUS,
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
	CONCAT,

//MARKERS
	P_MARKER,
	B_MARKER,
	C_MARKER,
	END_MARKER,
	IMMUTABLE,
	BREAK_MARKER,
	RETURN_MARKER
};

struct Expression : std::enable_shared_from_this<Expression> 
{
	Type type;
	rawInt value = 0;
	lmbPtr lambda = NULL;
	string varKey;
	expVec data;
	scpPtr parent;
	Expression();
	Expression(lmbPtr);
	Expression(rawInt, scpPtr);
	Expression(Type, expVec, scpPtr);
	Expression(string, scpPtr);
	expPtr evaluate();
	expPtr clone(scpPtr);
	void set(expPtr);
	string getString();
	double approximate();
	static long refCount;
	~Expression();
};

struct Scope : std::enable_shared_from_this<Scope> 
{
	scpPtr parent;
	std::map<string, expPtr> variables;
	Scope();
	Scope(scpPtr);
	expPtr& getVariable(string);
	static long refCount;
	~Scope();
};

struct Lambda : std::enable_shared_from_this<Lambda> 
{
	expPtr exp;
	scpPtr parent;
	strVec arg_names;
	Lambda(strVec, scpPtr);
	expPtr evaluate(expVec);
	string getString();
	static long refCount;
	~Lambda();
};

expPtr convertTokens(scpPtr&, strVec&, strVec&);
expPtr addition(expPtr, expPtr, scpPtr);
expPtr subtraction(expPtr, expPtr, scpPtr);
expPtr multiplication(expPtr, expPtr, scpPtr);
expPtr division(expPtr, expPtr, scpPtr);
expPtr power(expPtr, expPtr, scpPtr);
expPtr root(expPtr, expPtr, scpPtr);
expPtr logar(expPtr, expPtr, scpPtr);

rawInt gcd(rawInt a, rawInt b);
std::vector<rawInt> factor(rawInt n, rawInt fac);
bool expressionEquals(expPtr a, expPtr b);
bool double_equals(double a, double b, double epsilon = 0.00000001);
std::vector<rawInt> primeFactors(rawInt n);

expPtr newExp();
expPtr newExp(lmbPtr);
expPtr newExp(rawInt, scpPtr);
expPtr newExp(Type, expVec, scpPtr);
expPtr newExp(string, scpPtr);

#endif