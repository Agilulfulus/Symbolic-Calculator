#include "Expression.h"

Expression::Expression() {
	this->value = 0;
	this->type = INTEGER;
	this->parent = NULL;
}

Expression::Expression(Lambda * lambda){
	this->parent = lambda->parent;
	this->type = LAMBDA;
	this->lambda = lambda;
}

Expression::Expression(long long value, Scope * parent) {
	this->value = value;
	this->type = INTEGER;
	this->parent = parent;
}

Expression::Expression(Type type, std::vector<Expression *> data, Scope * parent) {
	this->data = data;
	this->type = type;
	this->parent = parent;
}

Expression::Expression(std::string var_key, Scope * parent) {
	this->var_key = var_key;
	this->type = VARIABLE;
	this->parent = parent;
}

double Expression::approximate(){
	switch(type){
		case VARIABLE:			return parent->getVariable(var_key)->approximate();
		case INTEGER:			return value;
		case SET:				return evaluate()->approximate();
		case ADDITION:			return data[0]->approximate() + data[1]->approximate();
		case SUBTRACTION:		return data[0]->approximate() - data[1]->approximate();
		case MULTIPLICATION:	return data[0]->approximate() * data[1]->approximate();
		case DIVISION:			return data[0]->approximate() / data[1]->approximate();
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

std::string Expression::getString() {
	switch(type){
		case IMMUTABLE:
		case VARIABLE:			return var_key;
		case INTEGER:			return std::to_string(value);
		case END_MARKER:		return "~";
		case LAMBDA:			return lambda->getString();
		case SIZE:				return "( #" + data[0]->getString() + " )";
		case FUNCTION_INIT:		return "( " + data[0]->getString() + " => " + data[1]->getString() + " )";
		case LAMBDA_INIT:		return "( " + data[0]->getString() + " -> " + data[1]->getString() + " )";
		case LAMBDA_RUN:		return "( " + data[0]->getString() + " !! " + data[1]->getString() + " )";
		case SCOPE:				return "{ " + data[0]->getString() + " }";
		case ITERATOR:			return "( " + data[0]->getString() + " << " + data[1]->getString() + " )";
		case DO_LOOP:			return "( " + data[0]->getString() + " do " + data[1]->getString() + " )";
		case EXTERNAL:			return "( " + data[0]->getString() + " >> " + data[1]->getString() + " )";
		case SET:				return "( " + data[0]->getString() + " = " + data[1]->getString() + " )";
		case ADDITION:			return "( " + data[0]->getString() + " + " + data[1]->getString() + " )";
		case SUBTRACTION:		return "( " + data[0]->getString() + " - " + data[1]->getString() + " )";
		case MULTIPLICATION:	return "( " + data[0]->getString() + " * " + data[1]->getString() + " )";
		case DIVISION:			return "( " + data[0]->getString() + " / " + data[1]->getString() + " )";
		case POWER:				return "( " + data[0]->getString() + " ^ " + data[1]->getString() + " )";
		case ROOT:				return "( " + data[0]->getString() + " rt " + data[1]->getString() + " )";
		case LOG:				return "( " + data[0]->getString() + " log " + data[1]->getString() + " )";
		case INDEX:				return "( " + data[0]->getString() + " ## " + data[1]->getString() + " )";
		case RANGE:				return "( " + data[0]->getString() + " : " + data[1]->getString() + (data.size() > 2 ? " : " + data[2]->getString() : "") + " )";
		case LESS:				return "( " + data[0]->getString() + " < " + data[1]->getString() + " )";
		case ELESS:				return "( " + data[0]->getString() + " <= " + data[1]->getString() + " )";
		case GREATER:			return "( " + data[0]->getString() + " > " + data[1]->getString() + " )";
		case EGREATER:			return "( " + data[0]->getString() + " >= " + data[1]->getString() + " )";
		case EQUAL:				return "( " + data[0]->getString() + " == " + data[1]->getString() + " )";
		case NEQUAL:			return "( " + data[0]->getString() + " != " + data[1]->getString() + " )";
		case IF_ELSE:			return "( " + data[0]->getString() + " then " + data[1]->getString() + (data.size() > 2 ? " else " + data[2]->getString() : "") + " )";
		case SEQUENCE:
		{
			if (data.empty()) return "[]";
			std::string r = "[ " + data[0]->getString();

			for (int i = 1; i < data.size(); i++)
			{
				auto d = data[i];
				r += ", " + d->getString();
			}

			return r + " ]";
		}
		default: return "NULL";
	}
}

Expression * Expression::clone(Scope * parent) {
	if (type == INTEGER)
		return new Expression(value, parent);

	if (type == VARIABLE)
		return new Expression(var_key, parent);

	if (type == LAMBDA)
		return new Expression(lambda);

	if (type == SCOPE)
	{
		Scope * ns = new Scope(parent);
		Expression * c = data[0]->clone(ns);
		return new Expression(SCOPE, {c}, parent);
	}

	std::vector<Expression *> newdata;
	for (auto &e : data)
		newdata.push_back(e->clone(parent));

	auto r = new Expression(type, newdata, parent);
	r->value = value;

	return r;
}

void Expression::set(Expression * e){
	auto c = e->clone(e->parent);

	this->type = c->type;
	this->data = c->data;
	this->value = c->value;
	this->var_key = c->var_key;
	this->lambda = c->lambda;
}

Expression * Expression::evaluate() {
	//std::cout << " >> " << getString() << std::endl;

	if (type == LAMBDA_INIT){
		Expression * exp = data[1];
		Expression * arg_array = data[0];
		std::vector<std::string> arg_names;

		for (auto &arg : arg_array->data)
			arg_names.push_back(arg->var_key);

		Lambda * l = new Lambda(arg_names, parent);
		l->exp = exp;

		return new Expression(l);
	}

	if (type == FUNCTION_INIT){
		Expression * exp = data[1];
		Expression * var = data[0]->data[0]->evaluate();
		Expression * arg_array = data[0]->data[1];
		std::vector<std::string> arg_names;

		for (auto &arg : arg_array->data)
			arg_names.push_back(arg->var_key);

		Lambda * l = new Lambda(arg_names, parent);
		l->exp = exp;

		var->set(new Expression(l));

		return var;
	}

	if (type == IF_ELSE){
		Expression * statement = data[0]->evaluate();
		if (statement->approximate() != 0)
			return data[1]->evaluate();
		else if (data.size() > 2)
			return data[2]->evaluate();
		else
			return new Expression(0, parent);
	}

	if (type == VARIABLE) return parent->getVariable(var_key);

	std::vector<Expression *> newdata;
	if (type == DO_LOOP)
	{
		newdata.push_back(data[0]->evaluate());
		newdata.push_back(data[1]);
	}else{
		for (auto &e : data)
		{
			Expression * d1 = e->evaluate();
			if (type == SEQUENCE && (value == 0 || e->type == RANGE) && d1->type == SEQUENCE && d1->value == 0)
				newdata.insert(newdata.end(), d1->data.begin(), d1->data.end());
			else
				newdata.push_back(d1);
		}
	}
	if (type == SEQUENCE)
	{
		if (newdata.size() == 1 && value == 0)
			return newdata[0];
		auto r = new Expression(SEQUENCE, newdata, parent);
		r->value = value;
		return r;
	}

	if (type == SET){
		if (newdata[0]->type == SEQUENCE && newdata[1]->type == SEQUENCE
			&& newdata[0]->data.size() == newdata[1]->data.size())
		{
			Expression * temp = newdata[1]->clone(newdata[1]->parent);
			for (int i = 0; i < newdata[0]->data.size(); i++)
			{
				Expression * eval = new Expression(SET, {newdata[0]->data[i], temp->data[i]}, parent);
				eval->evaluate();
			}
			return newdata[0];
		}else
		{
			newdata[0]->set(newdata[1]);
			return newdata[0];
		}
	}

	if (newdata.size() == 2 
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
		&& type != SIZE)
	{
		if (newdata[0]->type == SEQUENCE && newdata[1]->type != SEQUENCE){
			std::vector<Expression *> seq;
			for (auto &d : newdata[0]->data){
				Expression * r = new Expression(type, {d, newdata[1]}, parent);
				seq.push_back(r->evaluate());
			}
			auto r = new Expression(SEQUENCE, seq, parent);
			r->value = value;
			return r;
		} else if (newdata[1]->type == SEQUENCE) {
			std::vector<Expression *> seq;
			for (auto &d : newdata[1]->data){
				Expression * r = new Expression(type, {newdata[0], d}, parent);
				seq.push_back(r->evaluate());
			}
			auto r = new Expression(SEQUENCE, seq, parent);
			r->value = value;
			return r;
		}
	}

	switch(type){
		case SCOPE:				return newdata[0];
		case EXTERNAL:			return data[0]->data[0]->parent->getVariable(data[1]->var_key);
		case ITERATOR:			return new Expression(ITERATOR, {newdata[0], newdata[1]}, parent);
		case LAMBDA_RUN:		return newdata[0]->lambda->evaluate(newdata[1]->data);
		case SIZE:				return new Expression(newdata[0]->data.size(), parent);

		case ADDITION: 			return addition(newdata[0], newdata[1], parent); 
		case SUBTRACTION: 		return subtraction(newdata[0], newdata[1], parent);
		case MULTIPLICATION: 	return multiplication(newdata[0], newdata[1], parent);
		case DIVISION:		 	return division(newdata[0], newdata[1], parent);
		case POWER:			 	return power(newdata[0], newdata[1], parent);
		case ROOT:			 	return root(newdata[0], newdata[1], parent);
		case LOG:			 	return logar(newdata[0], newdata[1], parent);

		case LESS:				return new Expression(newdata[0]->approximate() < newdata[1]->approximate(),parent);
		case ELESS:				return new Expression(newdata[0]->approximate() <= newdata[1]->approximate(),parent);
		case GREATER:			return new Expression(newdata[0]->approximate() > newdata[1]->approximate(),parent);
		case EGREATER:			return new Expression(newdata[0]->approximate() >= newdata[1]->approximate(),parent);
		case EQUAL:				return new Expression(expressionEquals(newdata[0], newdata[1]), parent);
		case NEQUAL:			return new Expression(!expressionEquals(newdata[0], newdata[1]), parent);

		case INDEX:
		{
			std::vector<Expression*> ls;
			for (auto &d : newdata[1]->data)
			{
				if (d->type == END_MARKER)
					ls.push_back(newdata[0]->data.back());
				else
				{
					size_t index_value = d->approximate() - 1;

					if (index_value < 0 || index_value >= newdata[0]->data.size())
						throw std::runtime_error("ERROR: Index out of range [" + std::to_string(index_value + 1) + "]");

					ls.push_back(newdata[0]->data[index_value]);
				}
			}
			if (ls.size() == 1)
				return ls[0];
			else
				return new Expression(SEQUENCE, ls, parent);
		}
		case RANGE:
		{
			long long start = newdata[0]->approximate();
			long long end = newdata[1]->approximate();

			Expression * interval = NULL;
			if (newdata.size() > 2) interval = newdata[2];

			std::vector<Expression*> ls;
			if (start < end){
				for (long long i = start; i <= end; i+=(interval!=NULL ? interval->approximate() : 1))
					ls.push_back(new Expression(i, parent));
			}else{
				for (long long i = start; i >= end; i+=(interval!=NULL ? interval->approximate() : -1))
					ls.push_back(new Expression(i, parent));
			}

			return new Expression(SEQUENCE, ls, parent);
		}
		case DO_LOOP:
		{
			std::string varname = data[0]->data[0]->var_key;
			Expression * array = newdata[0]->data[1];
			Expression * scope = newdata[1];

			for (auto &d : array->data)
			{
				parent->getVariable(varname) = d;

				scope->evaluate();
			}

			return new Expression(0, parent);
		}
	}

	return this;
}

Expression * convertTokens(Scope * prim, const std::vector<std::string> &tokens, const std::vector<std::string> &operators) {
	std::vector<Expression*> stack;

	Scope * m = prim;

	for (auto &token : tokens) {
		if (token == ",")
			continue;

		if (isdigit(token[0]))
			stack.push_back(new Expression(stoll(token), m));
		else if (token == "true")
		{
			stack.push_back(new Expression(1, m));
		}
		else if (token == "false")
		{
			stack.push_back(new Expression(0, m));
		}
		else if (token == "PI" || token == "pi"){
			Expression * pi_raw = new Expression(DIVISION, {new Expression(355, m), new Expression(113, m)}, m);
			Expression * pi = new Expression(IMMUTABLE, {pi_raw}, m);
			pi->var_key = "PI";
			stack.push_back(pi);
		}
		else if (token == "e"){
			Expression * e_raw = new Expression(DIVISION, {new Expression(1457, m), new Expression(536, m)}, m);
			Expression * e = new Expression(IMMUTABLE, {e_raw}, m);
			e->var_key = "e";
			stack.push_back(e);
		}
		else if (token == "~")
		{
			stack.push_back(new Expression(END_MARKER, {}, m));			
		}
		else if (token == "~~")
		{
			auto num1 = stack.back();
			stack.pop_back();

			stack.push_back(new Expression(SUBTRACTION, {new Expression(0, m), num1}, m));			
		}
		else if (token == "(")
		{
			stack.push_back(new Expression(P_MARKER, {}, m));
		}
		else if (token == "[")
		{
			stack.push_back(new Expression(B_MARKER, {}, m));
		}
		else if (token == "{")
		{
			stack.push_back(new Expression(C_MARKER, {}, m));
			m = new Scope(m);
		}
		else if (token == ")")
		{
			std::vector<Expression*> ls;
			while(stack.back()->type != P_MARKER)
			{
				ls.push_back(stack.back());
				stack.pop_back();
			}
			reverse(ls.begin(), ls.end());
			stack.pop_back();
			stack.push_back(new Expression(SEQUENCE, ls, m));
		}
		else if (token == "]")
		{
			std::vector<Expression*> ls;
			while(stack.back()->type != B_MARKER)
			{
				ls.push_back(stack.back());
				stack.pop_back();
			}
			reverse(ls.begin(), ls.end());
			stack.pop_back();
			stack.push_back(new Expression(SEQUENCE, ls, m));
			stack.back()->value = 1;
		}
		else if (token == "}")
		{
			std::vector<Expression*> ls;
			while(stack.back()->type != C_MARKER)
			{
				ls.push_back(stack.back());
				stack.pop_back();
			}
			reverse(ls.begin(), ls.end());
			stack.pop_back();

			Expression * slist = new Expression(SEQUENCE, ls, m);

			m = m->parent;
			stack.push_back(new Expression(SCOPE, {slist}, m));
		}
		else if (token == "#")
		{
			auto num1 = stack.back();
			stack.pop_back();

			stack.push_back(new Expression(SIZE, {num1}, m));
		}
		else if (find(operators.begin(), operators.end(), token) != operators.end())
		{
			auto num2 = stack.back();
			stack.pop_back();
			auto num1 = stack.back();
			stack.pop_back();

			if (token == "+")		stack.push_back(new Expression(ADDITION, {num1, num2}, m));
			else if (token == "-")	stack.push_back(new Expression(SUBTRACTION, {num1, num2}, m));
			else if (token == "*") 	stack.push_back(new Expression(MULTIPLICATION, {num1, num2}, m));
			else if (token == "/") 	stack.push_back(new Expression(DIVISION, {num1, num2}, m));
			else if (token == "^")	stack.push_back(new Expression(POWER, {num1, num2}, m));
			else if (token == "rt")	stack.push_back(new Expression(ROOT, {num1, num2}, m));
			else if (token == "log")	stack.push_back(new Expression(LOG, {num1, num2}, m));
			else if (token == "=")	stack.push_back(new Expression(SET, {num1, num2}, m));
			else if (token == ":")	{
				if (num1->type == RANGE){
					num1->data.push_back(num2);
					stack.push_back(num1);
				}else
					stack.push_back(new Expression(RANGE, {num1, num2}, m));
			}
			else if (token == "else") {
				num1->data.push_back(num2);
				stack.push_back(num1);
			}
			else if (token == "##")	stack.push_back(new Expression(INDEX, {num1, num2}, m));
			else if (token == "<")	stack.push_back(new Expression(LESS, {num1, num2}, m));
			else if (token == "<=")	stack.push_back(new Expression(ELESS, {num1, num2}, m));
			else if (token == ">")	stack.push_back(new Expression(GREATER, {num1, num2}, m));
			else if (token == ">=")	stack.push_back(new Expression(EGREATER, {num1, num2}, m));
			else if (token == "==")	stack.push_back(new Expression(EQUAL, {num1, num2}, m));
			else if (token == "!=")	stack.push_back(new Expression(NEQUAL, {num1, num2}, m));
			else if (token == ">>")	stack.push_back(new Expression(EXTERNAL, {num1, num2}, m));
			else if (token == "from")	stack.push_back(new Expression(EXTERNAL, {num2, num1}, m));
			else if (token == "<<")	stack.push_back(new Expression(ITERATOR, {num1, num2}, m));
			else if (token == "in")	stack.push_back(new Expression(ITERATOR, {num1, num2}, m));
			else if (token == "do")	stack.push_back(new Expression(DO_LOOP, {num1, num2}, m));
			else if (token == "=>")	stack.push_back(new Expression(FUNCTION_INIT, {num1, num2}, m));
			else if (token == "->")	stack.push_back(new Expression(LAMBDA_INIT, {num1, num2}, m));
			else if (token == "then")	stack.push_back(new Expression(IF_ELSE, {num1, num2}, m));
			else if (token == "!!")	{
				num2->value = 1;
				stack.push_back(new Expression(LAMBDA_RUN, {num1, num2}, m));
			}
		}else
		{
			stack.push_back(new Expression(token,m));
		}
	}

	return new Expression(SEQUENCE, stack, m);
}

bool expressionEquals(Expression * a, Expression * b){
	return a->getString() == b->getString();
}

long long gcd(long long a, long long b)
{
	if (b == 0) return a;
	return gcd(b, a % b);
}

std::vector<long long> factor(long long n, long long fac)
{
	std::vector<long long> factors;

	for (long long i = n; i > 1; i--)
	{
		if (double_equals(std::pow(i, 1.0 / (double)fac), std::round(std::pow(i, 1.0 / (double)fac))) && (n % i == 0))
		{
			factors.push_back(i);
			n = n / i;
			i = n;
		}
	}

	return factors;
}

bool double_equals(double a, double b, double epsilon)
{
	return std::abs(a - b) < epsilon;
}

std::vector<long long> primeFactors(long long n)
{
	std::vector<long long> output;

	while (n % 2 == 0)
	{
		output.push_back(2);
		n = n / 2;
	}

	for (int i = 3; i <= sqrt(n); i = i + 2)
	{
		while (n%i == 0)
		{
			output.push_back(i);
			n = n / i;
		}
	}

	if (n > 2)
		output.push_back(n);

	return output;
}