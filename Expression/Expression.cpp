#include "Expression.h"

long Expression::refCount = 0;
strVec (* Compiler::_send)(strVec&) = NULL;

expPtr newExp(){
	return std::make_shared<Expression>(
		Expression()
	);
}

expPtr newExp(lmbPtr lambda){
	//std::cout << "New Lambda" << std::endl;
	return std::make_shared<Expression>(
		Expression(lambda)
	);
}

expPtr newExp(rawInt value, scpPtr parent){
	//std::cout << "New Integer" << std::endl;
	return std::make_shared<Expression>(
		Expression(value, parent)
	);
}

expPtr newExp(Type type, expVec data, scpPtr parent){
	//std::cout << "New General" << std::endl;
	return std::make_shared<Expression>(
		Expression(type, data, parent)
	);
}

expPtr newExp(string varKey, scpPtr parent){
	//std::cout << "New Variable" << std::endl;
	return std::make_shared<Expression>(
		Expression(varKey, parent)
	);
}

Expression::Expression() {
	this->value = 0;
	this->type = INTEGER;
	this->parent = NULL;
	refCount+=2;
}

Expression::Expression(lmbPtr lambda){
	this->parent = lambda->parent;
	this->type = LAMBDA;
	this->lambda = lambda;
	refCount+=2;
}

Expression::Expression(rawInt value, scpPtr parent) {
	this->value = value;
	this->type = INTEGER;
	this->parent = parent;
	refCount+=2;
}

Expression::Expression(Type type, expVec data, scpPtr parent) {
	this->data = data;
	this->type = type;
	this->parent = parent;
	refCount+=2;
}

Expression::Expression(string varKey, scpPtr parent) {
	this->varKey = varKey;
	this->type = VARIABLE;
	this->parent = parent;
	refCount+=2;
}


Expression::~Expression(){
	data.clear();
	parent = NULL;
	lambda = NULL;
	refCount--;
}

double Expression::approximate(){
	switch(type){
		case VARIABLE:			return parent->getVariable(varKey, (int)value)->approximate();
		case INTEGER:			return value;
		case CHARDEC:			return value;
		case SET:				return evaluate()->approximate();
		case ADDITION:			return data[0]->approximate() + data[1]->approximate();
		case SUBTRACTION:		return data[0]->approximate() - data[1]->approximate();
		case MULTIPLICATION:	return data[0]->approximate() * data[1]->approximate();
		case DIVISION:			return data[0]->approximate() / data[1]->approximate();
		case MODULUS:			return (long long)data[0]->approximate() % (long long)data[1]->approximate();
		case POWER:				return std::pow(data[0]->approximate(), data[1]->approximate());
		case ROOT:				return std::pow(data[1]->approximate(), 1.0 / data[0]->approximate());
		case LESS:				return data[0]->approximate() < data[1]->approximate();
		case ELESS:				return data[0]->approximate() <= data[1]->approximate();
		case GREATER:			return data[0]->approximate() > data[1]->approximate();
		case EGREATER:			return data[0]->approximate() >= data[1]->approximate();
		case EQUAL:				return double_equals(data[0]->approximate(), data[1]->approximate());
		case NEQUAL:			return double_equals(data[0]->approximate(), data[1]->approximate());
		case LOG:				return std::log(data[0]->approximate()) / std::log(data[1]->approximate());
		case IMMUTABLE:			return data[0]->approximate();
		default:				return 0;
	}
}

string Expression::getString() {
	switch(type){
		case CHARDEC:			return string(1, (char)value);
		case ADDITION:			return "( " + data[0]->getString() + " + " + data[1]->getString() + " )";
		case BREAK_MARKER:		return "break";
		case RETURN_MARKER:		return "return";
		case CONCAT:			return "( " + data[0]->getString() + " ++ " + data[1]->getString() + " )";
		case STR_CAT:			return "( " + data[0]->getString() + " .. " + data[1]->getString() + " )";
		case DIVISION:			return "( " + data[0]->getString() + " / " + data[1]->getString() + " )";
		case MODULUS:			return "( " + data[0]->getString() + " % " + data[1]->getString() + " )";
		case DO_LOOP:			return "( " + data[0]->getString() + " do " + data[1]->getString() + " )";
		case EGREATER:			return "( " + data[0]->getString() + " >= " + data[1]->getString() + " )";
		case ELESS:				return "( " + data[0]->getString() + " <= " + data[1]->getString() + " )";
		case END_MARKER:		return "~";
		case EQUAL:				return "( " + data[0]->getString() + " == " + data[1]->getString() + " )";
		case EXTERNAL:			return "( " + data[0]->getString() + " >> " + data[1]->getString() + " )";
		case FUNCTION_INIT:		return "( " + data[0]->getString() + " => " + data[1]->getString() + " )";
		case GREATER:			return "( " + data[0]->getString() + " > " + data[1]->getString() + " )";
		case IF_ELSE:			return "( " + data[0]->getString() + " then " + data[1]->getString() + (data.size() > 2 ? " else " + data[2]->getString() : "") + " )";
		case IMMUTABLE:			return varKey;
		case INDEX:				return "( " + data[0]->getString() + " ## " + data[1]->getString() + " )";
		case INTEGER:			return std::to_string(value);
		case ITERATOR:			return "( " + data[0]->getString() + " << " + data[1]->getString() + " )";
		case LAMBDA:			return lambda->getString();
		case LAMBDA_INIT:		return "( " + data[0]->getString() + " -> " + data[1]->getString() + " )";
		case LAMBDA_RUN:		return "( " + data[0]->getString() + " !! " + data[1]->getString() + " )";
		case LESS:				return "( " + data[0]->getString() + " < " + data[1]->getString() + " )";
		case LOG:				return "( " + data[0]->getString() + " log " + data[1]->getString() + " )";
		case MULTIPLICATION:	return "( " + data[0]->getString() + " * " + data[1]->getString() + " )";
		case NEQUAL:			return "( " + data[0]->getString() + " != " + data[1]->getString() + " )";
		case POWER:				return "( " + data[0]->getString() + " ^ " + data[1]->getString() + " )";
		case RANGE:				return "( " + data[0]->getString() + " : " + data[1]->getString() + (data.size() > 2 ? " : " + data[2]->getString() : "") + " )";
		case ROOT:				return "( " + data[0]->getString() + " rt " + data[1]->getString() + " )";
		case SCOPE:				return "{ " + data[0]->getString() + " }";
		case SET:				return "( " + data[0]->getString() + " = " + data[1]->getString() + " )";
		case SIZE:				return "( #" + data[0]->getString() + " )";
		case SUBTRACTION:		return "( " + data[0]->getString() + " - " + data[1]->getString() + " )";
		case VARIABLE:			return varKey;
		case SEND_FUNC:			return "__send(" + data[0]->getString() + ")";
		case TO_NUM:			return "num(" + data[0]->getString() + ")";
		case SEQUENCE:
		{
			if (value == 2){
				string r = "";
				for (auto &d : data)
					r += (char)d->value;
				return r;
			}else{
				if (data.empty()) return "[]";
				string r = "[ " + data[0]->getString();

				for (size_t i = 1; i < data.size(); i++)
				{
					expPtr d = data[i];
					r += ", " + d->getString();
				}

				return r + " ]";
			}

		}
		default: return "NULL";
	}
}

expPtr Expression::clone(scpPtr newParent) {
	if (type == INTEGER) 	return newExp(value, newParent);
	if (type == VARIABLE)	{
		auto e = newExp(varKey, newParent);
		e->value = value;
		return e;
	}
	if (type == LAMBDA)		return newExp(lambda);
	if (type == SCOPE)		return newExp(SCOPE, {data[0]->clone(newScp(newParent))}, newParent);

	expVec newData;
	for (expPtr &e : data)	newData.push_back(e->clone(newParent));
	expPtr r = newExp(type, newData, newParent);
	r->value = value;
	return r;
}

void Expression::set(const expPtr &e){
	expPtr c = e->clone(e->parent);
	shared_from_this()->type = c->type;
	shared_from_this()->data = c->data;
	shared_from_this()->value = c->value;
	shared_from_this()->varKey = c->varKey;
	shared_from_this()->lambda = c->lambda;
}

expPtr Expression::evaluate() {
	//std::cout << getString() << std::endl;

	if (type == LAMBDA_INIT){
		expPtr exp = data[1];
		expPtr argArray = data[0];
		strVec argNames;

		for (expPtr &arg : argArray->data)
			argNames.push_back(arg->varKey);

		lmbPtr l = newLmb(argNames, parent);
		l->exp = exp;

		return newExp(l);
	}

	if (type == FUNCTION_INIT){
		expPtr exp = data[1];
		expPtr var = data[0]->data[0]->evaluate();
		expPtr argArray = data[0]->data[1];
		strVec argNames;

		for (expPtr &arg : argArray->data)
			argNames.push_back(arg->varKey);

		lmbPtr l = newLmb(argNames, parent);
		l->exp = exp;

		var->set(newExp(l));

		return var;
	}

	if (type == IF_ELSE){
		expPtr statement = data[0]->evaluate();
		if (statement->approximate() != 0)
			return data[1]->evaluate();
		else if (data.size() > 2)
			return data[2]->evaluate();
		else
			return newExp(0, parent);
	}

	if (type == VARIABLE) return parent->getVariable(varKey, (int)value);

	expVec newData;
	if (type == DO_LOOP){
		newData.push_back(data[0]->evaluate());
		newData.push_back(data[1]);
	}else{
		for (expPtr &e : data){
			expPtr d1 = e->evaluate();
			if (d1->type == RETURN_MARKER && type == EXTERNAL) break;
			if (d1->type == BREAK_MARKER || d1->type == RETURN_MARKER) return d1;
			if (type == SEQUENCE && (value == 0 || e->type == RANGE) && d1->type == SEQUENCE && d1->value == 0)
				newData.insert(newData.end(), d1->data.begin(), d1->data.end());
			else
				newData.push_back(d1);
		}
	}

	if (type == SEQUENCE){
		if (newData.size() == 1 && value == 0)
			return newData[0];
		expPtr r = newExp(SEQUENCE, newData, parent);
		r->value = value;
		return r;
	}

	if (type == SET){
		if (newData[0]->type == SEQUENCE && newData[1]->type == SEQUENCE
			&& newData[0]->data.size() == newData[1]->data.size())
		{
			expPtr temp = newData[1]->clone(newData[1]->parent);
			for (size_t i = 0; i < newData[0]->data.size(); i++){
				expPtr eval = newExp(SET, {newData[0]->data[i], temp->data[i]}, parent);
				eval->evaluate();
			}
			return newData[0];
		}else{
			newData[0]->set(newData[1]);
			return newData[0];
		}
	}

	if (newData.size() == 2 
		&& type != INDEX
		&& type != RANGE
		&& type != EQUAL
		&& type != NEQUAL
		&& type != SCOPE
		&& type != EXTERNAL
		&& type != ITERATOR
		&& type != DO_LOOP
		&& type != LAMBDA_INIT
		&& type != LAMBDA_RUN
		&& type != SIZE
		&& type != CONCAT
		&& type != STR_CAT)
	{
		if (newData[0]->type == SEQUENCE && newData[1]->type != SEQUENCE){
			expVec seq;
			for (expPtr &d : newData[0]->data){
				expPtr r = newExp(type, {d, newData[1]}, parent);
				seq.push_back(r->evaluate());
			}
			expPtr r = newExp(SEQUENCE, seq, parent);
			r->value = value;
			return r;
		} else if (newData[1]->type == SEQUENCE) {
			expVec seq;
			for (expPtr &d : newData[1]->data){
				expPtr r = newExp(type, {newData[0], d}, parent);
				seq.push_back(r->evaluate());
			}
			expPtr r = newExp(SEQUENCE, seq, parent);
			r->value = value;
			return r;
		}
	}

	switch(type){
		case SCOPE:				return newData[0];
		case EXTERNAL:			return data[0]->data[0]->parent->getVariable(data[1]->varKey, 0);
		case ITERATOR:			return newExp(ITERATOR, {newData[0], newData[1]}, parent);
		case LAMBDA_RUN:{
			if (newData[0]->lambda == NULL)
				throw std::runtime_error("ERROR: Function requested is either undeclared or not lambda type.");
			return newData[0]->lambda->evaluate(newData[1]->data);
		}		
		case SIZE:				return newExp(newData[0]->data.size(), parent);
		case TO_NUM:			return newExp(std::stoi(newData[0]->data[0]->getString()), parent);

		case CONCAT:{
			expVec newseq;
			for (expPtr &d : newData[0]->data) newseq.push_back(d->clone(parent));
			for (expPtr &d : newData[1]->data) newseq.push_back(d->clone(parent));
			expPtr ret = newExp(SEQUENCE, newseq, parent);
			ret->value = 1;
			return ret;
		}

		case STR_CAT:{
			string s1 = newData[0]->getString();
			string s2 = newData[1]->getString();

			expVec ls;
			for (auto &c : s1){
				ls.push_back(
					newExp((rawInt)c, parent)
				);
				ls.back()->type = CHARDEC;
			}
			for (auto &c : s2){
				ls.push_back(
					newExp((rawInt)c, parent)
				);
				ls.back()->type = CHARDEC;
			}
			expPtr nd = newExp(SEQUENCE, ls, parent);
			nd->value = 2;
			return nd;
		}

		case ADDITION: 			return addition(newData[0], newData[1], parent); 
		case SUBTRACTION: 		return subtraction(newData[0], newData[1], parent);
		case MULTIPLICATION: 	return multiplication(newData[0], newData[1], parent);
		case DIVISION:		 	return division(newData[0], newData[1], parent);
		case POWER:			 	return power(newData[0], newData[1], parent);
		case ROOT:			 	return root(newData[0], newData[1], parent);
		case LOG:			 	return logar(newData[0], newData[1], parent);
		case MODULUS:			return newExp((long long)newData[0]->approximate() % (long long)newData[1]->approximate(), parent);

		case LESS:				return newExp(newData[0]->approximate() < newData[1]->approximate(),parent);
		case ELESS:				return newExp(newData[0]->approximate() <= newData[1]->approximate(),parent);
		case GREATER:			return newExp(newData[0]->approximate() > newData[1]->approximate(),parent);
		case EGREATER:			return newExp(newData[0]->approximate() >= newData[1]->approximate(),parent);
		case EQUAL:				return newExp(expressionEquals(newData[0], newData[1]), parent);
		case NEQUAL:			return newExp(!expressionEquals(newData[0], newData[1]), parent);
		case RANDOM:			return newData[0]->data[0]->data[(int)(((double)rand() / (double)RAND_MAX) * (double)newData[0]->data[0]->data.size())];

		case SEND_FUNC:{
			if (Compiler::_send == NULL)
				throw std::runtime_error("ERROR: No reference to a _send function. This is a compiler error.");

			strVec input;
			for (auto &d : newData[0]->data)
				input.push_back(d->getString());
			
			auto output = Compiler::_send(input);
			expVec ls1;
			for (auto &s : output){
				expVec ls;
				for (auto &c : s){
					ls.push_back(
						newExp((rawInt)c, parent)
					);
					ls.back()->type = CHARDEC;
				}
				ls1.push_back(newExp(SEQUENCE, ls, parent));
				ls1.back()->value = 2;
			}
			return newExp(SEQUENCE, ls1, parent);
		}
		case INDEX:{
			expVec ls;
			for (expPtr &d : newData[1]->data){
				if (d->type == END_MARKER)
					ls.push_back(newData[0]->data.back());
				else{
					size_t index_value = d->approximate() - 1;
					if (index_value < 0 || index_value >= newData[0]->data.size())
						throw std::runtime_error("ERROR: Index out of range [" + std::to_string(index_value + 1) + "]");
					ls.push_back(newData[0]->data[index_value]);
				}
			}
			if (ls.size() == 1) return ls[0];
			return newExp(SEQUENCE, ls, parent);
		}
		case RANGE:{
			rawInt start = newData[0]->approximate();
			rawInt end = newData[1]->approximate();

			expPtr interval = NULL;
			if (newData.size() > 2) interval = newData[2];

			expVec ls;
			if (start < end){
				if (interval == NULL)
					interval=newExp(1, parent);
				expPtr s_e = newData[0]->clone(parent);
				for (double i = start; i <= end; i+=interval->approximate()){
					ls.push_back(s_e);
					expPtr temp = newExp(ADDITION, {s_e, interval}, parent);
					s_e = temp->evaluate();
				}
			}else{
				if (interval == NULL)
					interval=newExp(-1, parent);
				expPtr s_e = newData[0]->clone(parent);
				for (double i = start; i >= end; i+=interval->approximate()){
					ls.push_back(s_e);
					expPtr temp = newExp(ADDITION, {s_e, interval}, parent);
					s_e = temp->evaluate();
				}
			}

			return newExp(SEQUENCE, ls, parent);
		}
		case DO_LOOP:{
			if (newData[0]->type == ITERATOR){
				string varname = data[0]->data[0]->varKey;
				expPtr array = newData[0]->data[1];
				expPtr scope = newData[1];

				if (array->type != SEQUENCE){
					parent->getVariable(varname, 1) = array;
					expPtr s = scope->evaluate();
					if (s->type == BREAK_MARKER)
						break;				
				}else{
					for (expPtr &d : array->data){
						parent->getVariable(varname, 1) = d;
						expPtr s = scope->evaluate();
						if (s->type == BREAK_MARKER)
							break;
					}
				}
			}else{
				expPtr cond = data[0];
				expPtr scope = newData[1];

				while(!double_equals(cond->evaluate()->approximate(), 0)){
					expPtr s = scope->evaluate();
					if (s->type == BREAK_MARKER)
						break;
				}
			}
			return newExp(0, parent);
		}
	}

	return shared_from_this();
}

expPtr Compiler::convertTokens(scpPtr &prim, strVec &tokens, strVec &operators) {
	expVec stack;
	scpPtr m = prim;

	for (string &token : tokens) {
		if (token == ",")
			continue;
		else if (isdigit(token[0]))
			stack.push_back(newExp(stoll(token), m));
		else if (token == "load"){
			string filename = stack.back()->getString();
			string line;
			string totalCode = "";
			std::ifstream newCode (filename);
			if (newCode.is_open()){
				while ( getline (newCode,line) )
					totalCode+=line;
				newCode.close();
			}
			if (std::find(loadedFiles.begin(), loadedFiles.end(), totalCode) == loadedFiles.end()){
				execute(totalCode);
				loadedFiles.push_back(totalCode);
			}
			
			stack.pop_back();
		}
		else if (token == "true")	stack.push_back(newExp(1, m));
		else if (token == "false" || token == "nil")	stack.push_back(newExp(0, m));
		else if (token == "break")	stack.push_back(newExp(BREAK_MARKER, {}, m));
		else if (token == "return")	stack.push_back(newExp(RETURN_MARKER, {}, m));
		else if (token == "~")		stack.push_back(newExp(END_MARKER, {}, m));	
		else if (token == "(")		stack.push_back(newExp(P_MARKER, {}, m));
		else if (token == "[")		stack.push_back(newExp(B_MARKER, {}, m));
		else if (token == "PI" || token == "pi"){
			expPtr pi_raw = newExp(DIVISION, {newExp(355, m), newExp(113, m)}, m);
			expPtr pi = newExp(IMMUTABLE, {pi_raw}, m);
			pi->varKey = "PI";
			stack.push_back(pi);
		}else if (token == "e"){
			expPtr e_raw = newExp(DIVISION, {newExp(1457, m), newExp(536, m)}, m);
			expPtr e = newExp(IMMUTABLE, {e_raw}, m);
			e->varKey = "e";
			stack.push_back(e);
		}else if (token == "~~"){
			expPtr num1 = stack.back();
			stack.pop_back();
			stack.push_back(newExp(SUBTRACTION, {newExp(0, m), num1}, m));			
		}else if (token == "{"){
			stack.push_back(newExp(C_MARKER, {}, m));
			m = newScp(m);
		}else if (token == ")"){
			expVec ls;
			while(stack.back()->type != P_MARKER){
				ls.push_back(stack.back());
				stack.pop_back();
			}
			reverse(ls.begin(), ls.end());
			stack.pop_back();
			stack.push_back(newExp(SEQUENCE, ls, m));
		}else if (token == "]"){
			expVec ls;
			while(stack.back()->type != B_MARKER){
				ls.push_back(stack.back());
				stack.pop_back();
			}
			reverse(ls.begin(), ls.end());
			stack.pop_back();
			stack.push_back(newExp(SEQUENCE, ls, m));
			stack.back()->value = 1;
		}else if (token == "}"){
			expVec ls;
			while(stack.back()->type != C_MARKER){
				ls.push_back(stack.back());
				stack.pop_back();
			}
			reverse(ls.begin(), ls.end());
			stack.pop_back();
			expPtr slist = newExp(SEQUENCE, ls, m);
			m = m->parent;
			stack.push_back(newExp(SCOPE, {slist}, m));
		}else if (token == "#"){
			expPtr num1 = stack.back();
			stack.pop_back();
			stack.push_back(newExp(SIZE, {num1}, m));
		}else if (token == "local"){
			expPtr num1 = stack.back();
			num1->value = 1;
		}else if (find(operators.begin(), operators.end(), token) != operators.end()){
			expPtr num2 = stack.back();
			stack.pop_back();
			expPtr num1 = stack.back();
			stack.pop_back();

			if (token == "+")			stack.push_back(newExp(ADDITION, {num1, num2}, m));
			else if (token == "-")		stack.push_back(newExp(SUBTRACTION, {num1, num2}, m));
			else if (token == "*") 		stack.push_back(newExp(MULTIPLICATION, {num1, num2}, m));
			else if (token == "/") 		stack.push_back(newExp(DIVISION, {num1, num2}, m));
			else if (token == "%") 		stack.push_back(newExp(MODULUS, {num1, num2}, m));
			else if (token == "^")		stack.push_back(newExp(POWER, {num1, num2}, m));
			else if (token == "rt")		stack.push_back(newExp(ROOT, {num1, num2}, m));
			else if (token == "log")	stack.push_back(newExp(LOG, {num1, num2}, m));
			else if (token == "=")		stack.push_back(newExp(SET, {num1, num2}, m));
			else if (token == "++")		stack.push_back(newExp(CONCAT, {num1, num2}, m));
			else if (token == "..")		stack.push_back(newExp(STR_CAT, {num1, num2}, m));
			else if (token == "##")		stack.push_back(newExp(INDEX, {num1, num2}, m));
			else if (token == "<")		stack.push_back(newExp(LESS, {num1, num2}, m));
			else if (token == "<=")		stack.push_back(newExp(ELESS, {num1, num2}, m));
			else if (token == ">")		stack.push_back(newExp(GREATER, {num1, num2}, m));
			else if (token == ">=")		stack.push_back(newExp(EGREATER, {num1, num2}, m));
			else if (token == "==")		stack.push_back(newExp(EQUAL, {num1, num2}, m));
			else if (token == "!=")		stack.push_back(newExp(NEQUAL, {num1, num2}, m));
			else if (token == ">>")		stack.push_back(newExp(EXTERNAL, {num1, num2}, m));
			else if (token == "from")	stack.push_back(newExp(EXTERNAL, {num2, num1}, m));
			else if (token == "<<")		stack.push_back(newExp(ITERATOR, {num1, num2}, m));
			else if (token == "in")		stack.push_back(newExp(ITERATOR, {num1, num2}, m));
			else if (token == "do")		stack.push_back(newExp(DO_LOOP, {num1, num2}, m));
			else if (token == "=>")		stack.push_back(newExp(FUNCTION_INIT, {num1, num2}, m));
			else if (token == "->")		stack.push_back(newExp(LAMBDA_INIT, {num1, num2}, m));
			else if (token == "then")	stack.push_back(newExp(IF_ELSE, {num1, num2}, m));
			else if (token == ":")	{
				if (num1->type == RANGE){
					num1->data.push_back(num2);
					stack.push_back(num1);
				}else
					stack.push_back(newExp(RANGE, {num1, num2}, m));
			}else if (token == "else") {
				num1->data.push_back(num2);
				stack.push_back(num1);
			}else if (token == "!!")	{
				num2->value = 1;
				if (num1->varKey == "__send")
					stack.push_back(newExp(SEND_FUNC, {num2}, m));
				else if (num1->varKey == "num")
					stack.push_back(newExp(TO_NUM, {num2}, m));
				else if (num1->varKey == "rand")
					stack.push_back(newExp(RANDOM, {num2}, m));
				else
					stack.push_back(newExp(LAMBDA_RUN, {num1, num2}, m));
			}
		}else if (token[0] == '$'){
			expVec ls;
			for (auto &c : token.substr(1)){
				ls.push_back(
					newExp((rawInt)c, m)
				);
				ls.back()->type = CHARDEC;
			}
			stack.push_back(newExp(SEQUENCE, ls, m));
			stack.back()->value = 2;
		}else
			stack.push_back(newExp(token,m));
	}

	return newExp(SEQUENCE, stack, m);
}

bool expressionEquals(expPtr a, expPtr b){
	return a->getString() == b->getString();
}

rawInt gcd(rawInt a, rawInt b){
	if (b == 0) return a;
	return gcd(b, a % b);
}

std::vector<rawInt> factor(rawInt n, rawInt fac){
	std::vector<rawInt> factors;

	for (rawInt i = n; i > 1; i--){
		if (double_equals(std::pow(i, 1.0 / (double)fac), std::round(std::pow(i, 1.0 / (double)fac))) && (n % i == 0)){
			factors.push_back(i);
			n = n / i;
			i = n;
		}
	}

	return factors;
}

bool Expression::isIntegral(){
	return type == INTEGER
	|| type == CHARDEC;
}

bool double_equals(double a, double b, double epsilon){
	return std::abs(a - b) < epsilon;
}

std::vector<rawInt> primeFactors(rawInt n){
	std::vector<rawInt> output;

	while (n % 2 == 0){
		output.push_back(2);
		n = n / 2;
	}

	for (rawInt i = 3; i <= sqrt(n); i = i + 2){
		while (n%i == 0){
			output.push_back(i);
			n = n / i;
		}
	}

	if (n > 2)
		output.push_back(n);

	return output;
}

Compiler::Compiler(strVec (*_send)(strVec&)){
	origin = newScp();
	this->_send = _send;
}

expPtr Compiler::execute(string line){
	std::vector<std::string> operators = {
        "+","-","*","/","%","^","rt","log",
        "++","--",",","..",
        "+=","-=","*=","/=","%=","^=",
        "=","==","!=","<",">","<=",">=",
        "||","&&", ":","?",
        "<<", ">>", "**", "->", "=>",
        "::", "##", "!!", "#",
        "~~", "do", "from", "in","then","else",
        "load", "local"
    };

	std::map<std::string, int> precedence =
    {
        { "load", -1},
        
		//Grouping
		{ "local", 12},
        { "!!", 11 },
        { "##", 11 },
        { "::", 11 },
        { "->", 1 },
        { "#", 10 },
        { ":", 9 },
    
        //Arithmetic
        { "~~", 10 },
		{ "++", 6 },
		{ "..", 3 },
        { "rt", 5 },
        { "log", 5 },
        { "`", 5 },
        { "^", 5 },
        { "*", 4 },
        { "/", 4 },
        { "%", 4 },
        { "+", 3 },
        { "-", 3 },
        { ">>", 2 },
        { "from", 2},
        { "<<", 2 },
        { "in", 2 },
    
        //Boolean
        { "<=", 2 },
        { ">=", 2 },
        { "<", 2 },
        { ">", 2 },
        { "==", 1 },
        { "!=", 1 },
        { "||", 0 },
        { "&&", 0 },
    
        //Allocation
        { "=", -1 },
        { "=>", -1 },
        { "^=", -1 },
        { "*=", -1 },
        { "/=", -1 },
        { "%=", -1 },
        { "+=", -1 },
        { "-=", -1 },
    
        //Seperation
        { "then", -2},
        { "do", -2 },
        { "else", -3},
        { ",", -4 }
	};

	auto tokens = tokenize(line, operators);
	auto postfix = infixToPostfix(tokens, operators, precedence);
	auto e = convertTokens(origin, postfix, operators);

	return e->evaluate();
}