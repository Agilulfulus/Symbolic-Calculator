#include "Expression.h"

Scope::Scope(Scope * parent){
	this->parent = parent;
}

Expression *& Scope::getVariable(std::string key){
	Scope * cur = this;
	while (cur != NULL){
		if (cur->variables.find(key) != cur->variables.end())
			break;
		cur = cur->parent;
	}

	if (cur == NULL){
		variables[key] = new Expression(0, this);
		return variables[key];
	}else
		return cur->variables[key];
}