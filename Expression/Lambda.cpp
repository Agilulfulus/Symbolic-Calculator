#include "Expression.h"

Lambda::Lambda(std::vector<std::string> arg_names, Scope * parent){
	this->arg_names = arg_names;
	this->parent = parent;
}

Expression * Lambda::evaluate(std::vector<Expression *> params){
	Scope * ns = new Scope(parent);
	Expression * eval = exp->clone(ns);

	for (size_t i = 0; i < arg_names.size(); i++){
		Expression * n = new Expression(0, ns);
		n->set(params[i]);
		ns->getVariable(arg_names[i]) = n;
	}

	return eval->evaluate();
}