#include "Expression.h"

Expression * addition(Expression * a, Expression * b, Scope * parent) {
	switch(a->type){
		case INTEGER:
		switch(b->type){
			case INTEGER:
			return new Expression(a->value + b->value, parent);
			default:
			return addition(b, a, parent);
		}
		case DIVISION:
		switch(b->type){
			case DIVISION:
			{
				auto num_a = a->data[0];
				auto den_a = a->data[1];
				auto num_b = b->data[0];
				auto den_b = b->data[1];
				Expression * num_r1 = new Expression(MULTIPLICATION, {num_a, den_b}, parent);
				Expression * num_r2 = new Expression(MULTIPLICATION, {num_b, den_a}, parent);
				Expression * num = new Expression(ADDITION, {num_r1, num_r2}, parent);
				Expression * den = new Expression(MULTIPLICATION, {den_a, den_b}, parent);
				Expression * frac = new Expression(DIVISION, {num, den}, parent);
				return frac->evaluate();
			}
			default:
			{
				auto num_a = a->data[0];
				auto den = a->data[1];
				auto num_b = b;
				Expression * num_r = new Expression(MULTIPLICATION, {num_b, den}, parent);
				Expression * num = new Expression(ADDITION, {num_a, num_r}, parent);
				Expression * frac = new Expression(DIVISION, {num, den}, parent);
				return frac->evaluate();
			}
		}
	}

	return new Expression(ADDITION, {a, b}, parent);
}

Expression * subtraction(Expression * a, Expression * b, Scope * parent) {
	if (expressionEquals(a, b))
		return new Expression(0, parent);

	switch(a->type){
		case INTEGER:
		switch(b->type){
			case INTEGER:
			return new Expression(a->value - b->value, parent);
			case DIVISION:
			{
				auto num_b = b->data[0];
				auto den = b->data[1];
				auto num_a = a;
				Expression * num_r = new Expression(MULTIPLICATION, {num_a, den}, parent);
				Expression * num = new Expression(SUBTRACTION, {num_r, num_b}, parent);
				Expression * frac = new Expression(DIVISION, {num, den}, parent);
				return frac->evaluate();
			}
		}
		break;
		case ADDITION:
		{
			auto num_a = a->data[0];
			auto num_b = a->data[1];

			if (expressionEquals(num_a, b))
				return num_b;
			if (expressionEquals(num_b, b))
				return num_a;
			return new Expression(SUBTRACTION, {a, b}, parent);
		}
		case DIVISION:
		switch(b->type){
			case DIVISION:
			{
				auto num_a = a->data[0];
				auto den_a = a->data[1];
				auto num_b = b->data[0];
				auto den_b = b->data[1];
				Expression * num_r1 = new Expression(MULTIPLICATION, {num_a, den_b}, parent);
				Expression * num_r2 = new Expression(MULTIPLICATION, {num_b, den_a}, parent);
				Expression * num = new Expression(SUBTRACTION, {num_r1, num_r2}, parent);
				Expression * den = new Expression(MULTIPLICATION, {den_a, den_b}, parent);
				Expression * frac = new Expression(DIVISION, {num, den}, parent);
				return frac->evaluate();
			}
			default:
			{
				auto num_a = a->data[0];
				auto den = a->data[1];
				auto num_b = b;
				Expression * num_r = new Expression(MULTIPLICATION, {num_b, den}, parent);
				Expression * num = new Expression(SUBTRACTION, {num_a, num_r}, parent);
				Expression * frac = new Expression(DIVISION, {num, den}, parent);
				return frac->evaluate();
			}
		}
		case LOG:
		switch(b->type){
			case LOG:
			{
				Expression * num_a = a->data[0];
				Expression * num_b = b->data[0];
				Expression * base_a = a->data[1];
				Expression * base_b = b->data[1];

				if (!expressionEquals(base_a, base_b))
					return new Expression(SUBTRACTION, {a, b}, parent);

				Expression * div = new Expression(DIVISION, {num_a, num_b}, parent);
				Expression * l = new Expression(LOG, {div, base_a}, parent);
				return l->evaluate();
			}
		}
	}

	return new Expression(SUBTRACTION, {a, b}, parent);
}

Expression * multiplication(Expression * a, Expression * b, Scope * parent) {
	if (double_equals(a->approximate(), 0) || double_equals(b->approximate(), 0))
		return new Expression(0, parent);

	switch(a->type){
		case INTEGER:
		switch(b->type){
			case INTEGER:
			return new Expression(a->value * b->value, parent);
			default:
			{
				if (a->value == 1) return b;
				return multiplication(b, a, parent);
			}
		}
		case ADDITION:
		{
			auto num_a = a->data[0];
			auto num_b = a->data[1];
			auto mul = b;

			Expression * n1 = new Expression(MULTIPLICATION, {num_a, mul}, parent);
			Expression * n2 = new Expression(MULTIPLICATION, {num_b, mul}, parent);
			Expression * res = new Expression(ADDITION, {n1, n2}, parent);
			return res->evaluate();
		}
		case SUBTRACTION:
		{
			auto num_a = a->data[0];
			auto num_b = a->data[1];
			auto mul = b;

			Expression * n1 = new Expression(MULTIPLICATION, {num_a, mul}, parent);
			Expression * n2 = new Expression(MULTIPLICATION, {num_b, mul}, parent);
			Expression * res = new Expression(SUBTRACTION, {n1, n2}, parent);
			return res->evaluate();
		}
		case MULTIPLICATION:
		{
			Expression * n1 = new Expression(MULTIPLICATION, {a->data[1], b}, parent);
			Expression * n2 = new Expression(MULTIPLICATION, {a->data[0], n1}, parent);
			return n2->evaluate();
		}
		case DIVISION:
		switch(b->type){
			case DIVISION:
			{
				auto num_a = a->data[0];
				auto den_a = a->data[1];
				auto num_b = b->data[0];
				auto den_b = b->data[1];
				Expression * num = new Expression(MULTIPLICATION, {num_a, num_b}, parent);
				Expression * den = new Expression(MULTIPLICATION, {den_a, den_b}, parent);
				Expression * frac = new Expression(DIVISION, {num, den}, parent);
				return frac->evaluate();
			}
			default:
			{
				auto num_a = a->data[0];
				auto den = a->data[1];
				auto num_b = b;
				Expression * num = new Expression(MULTIPLICATION, {num_a, num_b}, parent);
				Expression * frac = new Expression(DIVISION, {num, den}, parent);
				return frac->evaluate();
			}
		}
		case ROOT:
		switch(b->type){
			case INTEGER:
				if (b->value == 1) return a;
				return new Expression(MULTIPLICATION, {a, b}, parent);
			case DIVISION:
			{				
				auto num_a = a;
				auto den = b->data[1];
				auto num_b = b->data[0];
				Expression * num = new Expression(MULTIPLICATION, {num_a, num_b}, parent);
				Expression * frac = new Expression(DIVISION, {num, den}, parent);
				return frac->evaluate();
			}
		}
		break;
		case LOG:
		switch(b->type){
			default:
			{
				Expression * num = a->data[0];
				Expression * base = a->data[1];
				Expression * exp = b;
				Expression * new_exp = new Expression(POWER, {num, exp}, parent);
				Expression * new_log = new Expression(LOG, {new_exp, base}, parent);
				return new_log->evaluate();
			}
		}
	}

	return new Expression(MULTIPLICATION, {a, b}, parent);
}

Expression * division(Expression * a, Expression * b, Scope * parent) {
	if (expressionEquals(a, b)) return new Expression(1, parent);

	switch(a->type){
		case INTEGER:
		switch(b->type){
			case INTEGER:
			{
				long long div = gcd(a->value, b->value);
				Expression * num = new Expression(a->value / div, parent);
				Expression * den = new Expression(b->value / div, parent);
				if (den->value == 1)
					return num;
				else
					return new Expression(DIVISION, { num, den}, parent);
			}
			case MULTIPLICATION:
			{
				if (a->value == 1)
					return new Expression(DIVISION, {a, b}, parent);

				if (b->data[0]->type == INTEGER && gcd(a->value, b->data[0]->value) > 1){
					Expression * frac_a = new Expression(DIVISION, {a, b->data[0]}, parent);
					Expression * frac_b = new Expression(DIVISION, {new Expression(1, parent), b->data[1]}, parent);
					Expression * res = new Expression(MULTIPLICATION, {frac_a, frac_b}, parent);
					return res->evaluate();
				}else if (b->data[1]->type == INTEGER && gcd(a->value, b->data[1]->value) > 1){
					Expression * frac_a = new Expression(DIVISION, {new Expression(1,parent), b->data[0]}, parent);
					Expression * frac_b = new Expression(DIVISION, {a, b->data[1]}, parent);
					Expression * res = new Expression(MULTIPLICATION, {frac_a, frac_b}, parent);
					return res->evaluate();
				}else
					return new Expression(DIVISION, {a, b}, parent);
				break;
			}
			case DIVISION:
			{
				Expression * num = new Expression(MULTIPLICATION, {a, b->data[1]}, parent);
				Expression * den = b->data[0];
				Expression * frac = new Expression(DIVISION, {num, den}, parent);

				return frac->evaluate();
			}
		}
		break;		
		case MULTIPLICATION:
		{
			switch(b->type)
			{
				case MULTIPLICATION:
				{
					Expression * a_1 = a->data[0];
					Expression * a_2 = a->data[1];
					Expression * b_1 = b->data[0];
					Expression * b_2 = b->data[1];

					if ((a_1->type == INTEGER && b_1->type == INTEGER && gcd(a_1->value, b_1->value) > 1)
						|| expressionEquals(a_1, b_1)){
						Expression * frac_a = new Expression(DIVISION, {a_1, b_1}, parent);
						Expression * frac_b = new Expression(DIVISION, {a_2, b_2}, parent);
						Expression * res = new Expression(MULTIPLICATION, {frac_a, frac_b}, parent);
						return res->evaluate();
					} else if ((a_1->type == INTEGER && b_2->type == INTEGER && gcd(a_1->value, b_2->value) > 1)
						|| expressionEquals(a_1, b_2)){
						Expression * frac_a = new Expression(DIVISION, {a_1, b_2}, parent);
						Expression * frac_b = new Expression(DIVISION, {a_2, b_1}, parent);
						Expression * res = new Expression(MULTIPLICATION, {frac_a, frac_b}, parent);
						return res->evaluate();
					} else if ((a_2->type == INTEGER && b_1->type == INTEGER && gcd(a_2->value, b_1->value) > 1)
						|| expressionEquals(a_2, b_1)){
						Expression * frac_a = new Expression(DIVISION, {a_2, b_1}, parent);
						Expression * frac_b = new Expression(DIVISION, {a_1, b_2}, parent);
						Expression * res = new Expression(MULTIPLICATION, {frac_a, frac_b}, parent);
						return res->evaluate();
					} else if ((a_2->type == INTEGER && b_2->type == INTEGER && gcd(a_2->value, b_2->value) > 1)
						|| expressionEquals(a_2, b_2)){
						Expression * frac_a = new Expression(DIVISION, {a_2, b_2}, parent);
						Expression * frac_b = new Expression(DIVISION, {a_1, b_1}, parent);
						Expression * res = new Expression(MULTIPLICATION, {frac_a, frac_b}, parent);
						return res->evaluate();
					}
					break;
				}
				default:
				{
					Expression * a_1 = a->data[0];
					Expression * a_2 = a->data[1];

					if ((a_1->type == INTEGER && b->type == INTEGER && gcd(a_1->value, b->value) > 1)
						|| expressionEquals(a_1, b)){
						Expression * frac_a = new Expression(DIVISION, {a_1, b}, parent);
						Expression * frac_b = a_2;
						Expression * res = new Expression(MULTIPLICATION, {frac_a, frac_b}, parent);
						return res->evaluate();
					} else if ((a_2->type == INTEGER && b->type == INTEGER && gcd(a_2->value, b->value) > 1)
						|| expressionEquals(a_2, b)){
						Expression * frac_a = new Expression(DIVISION, {a_2, b}, parent);
						Expression * frac_b = a_1;
						Expression * res = new Expression(MULTIPLICATION, {frac_a, frac_b}, parent);
						return res->evaluate();
					} 
					break;					
				}
			}
			break;
		}
		case DIVISION:
		{
			switch(b->type)
			{
				case DIVISION:
				{
					Expression * num = new Expression(MULTIPLICATION, {a->data[0], b->data[1]}, parent);
					Expression * den = new Expression(MULTIPLICATION, {a->data[1], b->data[0]}, parent);
					Expression * frac = new Expression(DIVISION, {num, den}, parent);

					return frac->evaluate();
				}
				default:
				{
					Expression * num = a->data[0];
					Expression * den = new Expression(MULTIPLICATION, {a->data[1], b}, parent);
					Expression * frac = new Expression(DIVISION, {num, den}, parent);

					return frac->evaluate();
				}
			}
		}
		case LOG:
		{
			switch(b->type){
				case LOG:
				{
					auto num_a = a->data[0];
					auto base_a = a->data[1];
					auto num_b = b->data[0];
					auto base_b = b->data[1];

					if (expressionEquals(base_a, base_b)){
						auto new_log = new Expression(LOG, {num_a, num_b}, parent);
						return new_log->evaluate();
					}else
					{
						return new Expression(DIVISION, {a, b}, parent);
					}
				}
			}
		}
	}
	return new Expression(DIVISION, {a, b}, parent);
}

Expression * power(Expression * a, Expression * b, Scope * parent) {
	if (double_equals(b->approximate(), 0))
		return new Expression(1, parent);
	if (double_equals(b->approximate(), 1))
		return a;
	if (double_equals(a->approximate(), 1))
		return new Expression(1, parent);

	switch(a->type){
		case INTEGER:
		switch(b->type){
			case INTEGER: return new Expression(std::pow(a->value, b->value), parent);
			case DIVISION:
			{			
				Expression * power = new Expression(POWER, {a, b->data[0]}, parent);
				Expression * root = new Expression(ROOT, {b->data[1], power}, parent);

				return root->evaluate();
			}
			case LOG:
			{
				Expression * num_a = a;
				Expression * base = b->data[1];
				Expression * num_b = b->data[0];

				if (!expressionEquals(num_a, base))
					return new Expression(POWER, {a, b}, parent);

				return num_b;
			}
		}
		break;
		case DIVISION:
		{			
			Expression * num = new Expression(POWER, {a->data[0], b}, parent);
			Expression * den = new Expression(POWER, {a->data[1], b}, parent);
			Expression * frac = new Expression(DIVISION, {num, den}, parent);

			return frac->evaluate();
		}
		break;
		default:
		switch(b->type){
			case DIVISION:
			{			
				Expression * power = new Expression(POWER, {a, b->data[0]}, parent);
				Expression * root = new Expression(ROOT, {b->data[1], power}, parent);

				return root->evaluate();
			}
		}
		break;
	}
	return new Expression(POWER, {a, b}, parent);
}

Expression * root(Expression * a, Expression * b, Scope * parent) {
	switch(a->type){
		case INTEGER:
		switch(b->type){
			case INTEGER:
			{
				auto num = b->value;
				auto root = a->value;
				auto list = factor(num, root);

				if (!list.empty())
				{
					long long newn = 1;

					for (auto &i : list)
					{
						newn *= (long long)std::round(std::pow(i, 1.0 / (double)root));
						num /= i;
					}

					if (num == 1)
						return new Expression(newn, parent);

					Expression * r = new Expression(ROOT, {new Expression(root, parent), new Expression(num, parent)}, parent);
					Expression * t = new Expression(MULTIPLICATION, {new Expression(newn, parent), r}, parent);
					return t->evaluate();
				}
				break;
			}
			case DIVISION:
			{			
				Expression * num = new Expression(ROOT, {a, b->data[0]}, parent);
				Expression * den = new Expression(ROOT, {a, b->data[1]}, parent);
				Expression * frac = new Expression(DIVISION, {num, den}, parent);

				return frac->evaluate();
			}
		}
		break;
		case DIVISION:
		{			
			Expression * power = new Expression(POWER, {b, a->data[1]}, parent);
			Expression * root = new Expression(ROOT, {a->data[0], power}, parent);

			return root->evaluate();
		}
		break;
		default:
		switch(b->type){
			case DIVISION:
			{			
				Expression * num = new Expression(ROOT, {a, b->data[0]}, parent);
				Expression * den = new Expression(ROOT, {a, b->data[1]}, parent);
				Expression * frac = new Expression(DIVISION, {num, den}, parent);

				return frac->evaluate();
			}
		}
		break;
	}
	return new Expression(ROOT, {a, b}, parent);
}

Expression * logar(Expression * a, Expression * b, Scope * parent) {
	switch(a->type){
		case INTEGER:
		switch(b->type){
			case INTEGER:
			{
				long long num = a->value;
				long long base = b->value;

				if (double_equals(log(num) / log(base), round(log(num) / log(base))))
				{
					long long base_value_n = round(log(num) / log(base));
					return new Expression(base_value_n, parent);
				}

				auto factors = primeFactors(num);

				if (factors[0] == num)
					return new Expression(LOG, {a, b}, parent);
				else
				{
					Expression * num_a = new Expression(LOG, {new Expression(factors[1], parent), new Expression(base, parent)}, parent);
					Expression * num_b = new Expression(LOG, {new Expression(num / factors[1], parent), new Expression(base, parent)}, parent);
					Expression * t = new Expression(ADDITION, {num_b, num_a}, parent);

					return t->evaluate();
				}
			}
		}
	}
	return new Expression(LOG, {a, b}, parent);
}