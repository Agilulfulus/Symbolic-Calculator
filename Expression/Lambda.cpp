#include "Expression.h"

long Lambda::refCount = 0;

Lambda::Lambda(strVec arg_names, scpPtr parent){
	this->arg_names = arg_names;
	this->parent = parent;
	refCount+=2;
}

Lambda::~Lambda(){
	exp = NULL;
	parent = NULL;
	refCount--;
}

expPtr Lambda::evaluate(expVec params){
	scpPtr ns = newScp(parent);
	expPtr eval = exp->clone(ns);

	for (size_t i = 0; i < arg_names.size(); i++){
		expPtr n = newExp(0, ns);
		if (params.size() > i)
			n->set(params[i]);
		ns->variables[arg_names[i]] = n;

		//std::cout << "VAR: " << arg_names[i] << " = " << n->getString() << std::endl;
	}

	expPtr temp = eval->evaluate();
	if (temp->type == RETURN_MARKER)
		return newExp(0, ns);
	return temp;
}

string Lambda::getString(){
	string ret = "( ";

	for (auto &s : arg_names)
		ret += s + " ";
	ret += ") -> " + exp->getString();

	return ret;
}