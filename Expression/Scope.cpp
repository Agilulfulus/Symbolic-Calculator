#include "Expression.h"

long Scope::refCount = 0;

Scope::Scope(){
	this->parent = NULL;
	refCount+=2;
}

Scope::Scope(scpPtr parent){
	this->parent = parent;
	refCount+=2;
}

Scope::~Scope(){
	variables.clear();
	refCount--;
}

expPtr& Scope::getVariable(string key, int forceLocal){
	scpPtr cur = shared_from_this();
	if (forceLocal == 0)
	{
		while (cur != NULL){
			if (cur->variables.find(key) != cur->variables.end())
				break;
			cur = cur->parent;
		}
	}

	if (forceLocal == 1 || cur == NULL){
		variables[key] = newExp(0, shared_from_this());
		return variables[key];
	}else
		return cur->variables[key];
}