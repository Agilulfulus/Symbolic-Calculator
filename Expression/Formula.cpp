#include "Expression.h"

expPtr addition(const expPtr &a, const expPtr &b, const scpPtr &parent) {
	switch(a->type){
		case INTEGER: case CHARDEC:
		switch(b->type){
			case INTEGER: case CHARDEC:
			return newExp(a->value + b->value, parent);
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
				expPtr num_r1 = newExp(MULTIPLICATION, {num_a, den_b}, parent);
				expPtr num_r2 = newExp(MULTIPLICATION, {num_b, den_a}, parent);
				expPtr num = newExp(ADDITION, {num_r1, num_r2}, parent);
				expPtr den = newExp(MULTIPLICATION, {den_a, den_b}, parent);
				expPtr frac = newExp(DIVISION, {num, den}, parent);
				return frac->evaluate();
			}
			default:
			{
				auto num_a = a->data[0];
				auto den = a->data[1];
				auto num_b = b;
				expPtr num_r = newExp(MULTIPLICATION, {num_b, den}, parent);
				expPtr num = newExp(ADDITION, {num_a, num_r}, parent);
				expPtr frac = newExp(DIVISION, {num, den}, parent);
				return frac->evaluate();
			}
		}
	}

	return newExp(ADDITION, {a, b}, parent);
}

expPtr subtraction(const expPtr &a, const expPtr &b, const scpPtr &parent) {
	if (expressionEquals(a, b))
		return newExp(0, parent);

	switch(a->type){
		case INTEGER: case CHARDEC:
		switch(b->type){
			case INTEGER: case CHARDEC:
			return newExp(a->value - b->value, parent);
			case DIVISION:
			{
				auto num_b = b->data[0];
				auto den = b->data[1];
				auto num_a = a;
				expPtr num_r = newExp(MULTIPLICATION, {num_a, den}, parent);
				expPtr num = newExp(SUBTRACTION, {num_r, num_b}, parent);
				expPtr frac = newExp(DIVISION, {num, den}, parent);
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
			return newExp(SUBTRACTION, {a, b}, parent);
		}
		case DIVISION:
		switch(b->type){
			case DIVISION:
			{
				auto num_a = a->data[0];
				auto den_a = a->data[1];
				auto num_b = b->data[0];
				auto den_b = b->data[1];
				expPtr num_r1 = newExp(MULTIPLICATION, {num_a, den_b}, parent);
				expPtr num_r2 = newExp(MULTIPLICATION, {num_b, den_a}, parent);
				expPtr num = newExp(SUBTRACTION, {num_r1, num_r2}, parent);
				expPtr den = newExp(MULTIPLICATION, {den_a, den_b}, parent);
				expPtr frac = newExp(DIVISION, {num, den}, parent);
				return frac->evaluate();
			}
			default:
			{
				auto num_a = a->data[0];
				auto den = a->data[1];
				auto num_b = b;
				expPtr num_r = newExp(MULTIPLICATION, {num_b, den}, parent);
				expPtr num = newExp(SUBTRACTION, {num_a, num_r}, parent);
				expPtr frac = newExp(DIVISION, {num, den}, parent);
				return frac->evaluate();
			}
		}
		case LOG:
		switch(b->type){
			case LOG:
			{
				expPtr num_a = a->data[0];
				expPtr num_b = b->data[0];
				expPtr base_a = a->data[1];
				expPtr base_b = b->data[1];

				if (!expressionEquals(base_a, base_b))
					return newExp(SUBTRACTION, {a, b}, parent);

				expPtr div = newExp(DIVISION, {num_a, num_b}, parent);
				expPtr l = newExp(LOG, {div, base_a}, parent);
				return l->evaluate();
			}
		}
	}

	return newExp(SUBTRACTION, {a, b}, parent);
}

expPtr multiplication(const expPtr &a, const expPtr &b, const scpPtr &parent) {
	if (double_equals(a->approximate(), 0) || double_equals(b->approximate(), 0))
		return newExp(0, parent);

	switch(a->type){
		case INTEGER: case CHARDEC:
		switch(b->type){
			case INTEGER: case CHARDEC:
			return newExp(a->value * b->value, parent);
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

			expPtr n1 = newExp(MULTIPLICATION, {num_a, mul}, parent);
			expPtr n2 = newExp(MULTIPLICATION, {num_b, mul}, parent);
			expPtr res = newExp(ADDITION, {n1, n2}, parent);
			return res->evaluate();
		}
		case SUBTRACTION:
		{
			auto num_a = a->data[0];
			auto num_b = a->data[1];
			auto mul = b;

			expPtr n1 = newExp(MULTIPLICATION, {num_a, mul}, parent);
			expPtr n2 = newExp(MULTIPLICATION, {num_b, mul}, parent);
			expPtr res = newExp(SUBTRACTION, {n1, n2}, parent);
			return res->evaluate();
		}
		case MULTIPLICATION:
		{
			expPtr n1 = newExp(MULTIPLICATION, {a->data[1], b}, parent);
			expPtr n2 = newExp(MULTIPLICATION, {a->data[0], n1}, parent);
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
				expPtr num = newExp(MULTIPLICATION, {num_a, num_b}, parent);
				expPtr den = newExp(MULTIPLICATION, {den_a, den_b}, parent);
				expPtr frac = newExp(DIVISION, {num, den}, parent);
				return frac->evaluate();
			}
			default:
			{
				auto num_a = a->data[0];
				auto den = a->data[1];
				auto num_b = b;
				expPtr num = newExp(MULTIPLICATION, {num_a, num_b}, parent);
				expPtr frac = newExp(DIVISION, {num, den}, parent);
				return frac->evaluate();
			}
		}
		case ROOT:
		switch(b->type){
			case INTEGER: case CHARDEC:
				if (b->value == 1) return a;
				return newExp(MULTIPLICATION, {a, b}, parent);
			case DIVISION:
			{				
				auto num_a = a;
				auto den = b->data[1];
				auto num_b = b->data[0];
				expPtr num = newExp(MULTIPLICATION, {num_a, num_b}, parent);
				expPtr frac = newExp(DIVISION, {num, den}, parent);
				return frac->evaluate();
			}
		}
		break;
		case LOG:
		switch(b->type){
			default:
			{
				expPtr num = a->data[0];
				expPtr base = a->data[1];
				expPtr exp = b;
				expPtr new_exp = newExp(POWER, {num, exp}, parent);
				expPtr new_log = newExp(LOG, {new_exp, base}, parent);
				return new_log->evaluate();
			}
		}
	}

	return newExp(MULTIPLICATION, {a, b}, parent);
}

expPtr division(const expPtr &a, const expPtr &b, const scpPtr &parent) {
	if (expressionEquals(a, b)) return newExp(1, parent);
	if (double_equals(b->approximate(), 0))
		throw std::runtime_error("ERROR: Division by zero is undefined.");

	switch(a->type){
		case INTEGER: case CHARDEC:
		switch(b->type){
			case INTEGER: case CHARDEC:
			{
				rawInt div = gcd(a->value, b->value);
				expPtr num = newExp(a->value / div, parent);
				expPtr den = newExp(b->value / div, parent);
				if (den->value == 1)
					return num;
				else
					return newExp(DIVISION, { num, den}, parent);
			}
			case MULTIPLICATION:
			{
				if (a->value == 1)
					return newExp(DIVISION, {a, b}, parent);

				if (b->data[0]->isIntegral() && gcd(a->value, b->data[0]->value) > 1){
					expPtr frac_a = newExp(DIVISION, {a, b->data[0]}, parent);
					expPtr frac_b = newExp(DIVISION, {newExp(1, parent), b->data[1]}, parent);
					expPtr res = newExp(MULTIPLICATION, {frac_a, frac_b}, parent);
					return res->evaluate();
				}else if (b->data[1]->isIntegral() && gcd(a->value, b->data[1]->value) > 1){
					expPtr frac_a = newExp(DIVISION, {newExp(1,parent), b->data[0]}, parent);
					expPtr frac_b = newExp(DIVISION, {a, b->data[1]}, parent);
					expPtr res = newExp(MULTIPLICATION, {frac_a, frac_b}, parent);
					return res->evaluate();
				}else
					return newExp(DIVISION, {a, b}, parent);
				break;
			}
			case DIVISION:
			{
				expPtr num = newExp(MULTIPLICATION, {a, b->data[1]}, parent);
				expPtr den = b->data[0];
				expPtr frac = newExp(DIVISION, {num, den}, parent);

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
					expPtr a_1 = a->data[0];
					expPtr a_2 = a->data[1];
					expPtr b_1 = b->data[0];
					expPtr b_2 = b->data[1];

					if ((a_1->isIntegral() && b_1->isIntegral() && gcd(a_1->value, b_1->value) > 1)
						|| expressionEquals(a_1, b_1)){
						expPtr frac_a = newExp(DIVISION, {a_1, b_1}, parent);
						expPtr frac_b = newExp(DIVISION, {a_2, b_2}, parent);
						expPtr res = newExp(MULTIPLICATION, {frac_a, frac_b}, parent);
						return res->evaluate();
					} else if ((a_1->isIntegral() && b_2->isIntegral() && gcd(a_1->value, b_2->value) > 1)
						|| expressionEquals(a_1, b_2)){
						expPtr frac_a = newExp(DIVISION, {a_1, b_2}, parent);
						expPtr frac_b = newExp(DIVISION, {a_2, b_1}, parent);
						expPtr res = newExp(MULTIPLICATION, {frac_a, frac_b}, parent);
						return res->evaluate();
					} else if ((a_2->isIntegral() && b_1->isIntegral() && gcd(a_2->value, b_1->value) > 1)
						|| expressionEquals(a_2, b_1)){
						expPtr frac_a = newExp(DIVISION, {a_2, b_1}, parent);
						expPtr frac_b = newExp(DIVISION, {a_1, b_2}, parent);
						expPtr res = newExp(MULTIPLICATION, {frac_a, frac_b}, parent);
						return res->evaluate();
					} else if ((a_2->isIntegral() && b_2->isIntegral() && gcd(a_2->value, b_2->value) > 1)
						|| expressionEquals(a_2, b_2)){
						expPtr frac_a = newExp(DIVISION, {a_2, b_2}, parent);
						expPtr frac_b = newExp(DIVISION, {a_1, b_1}, parent);
						expPtr res = newExp(MULTIPLICATION, {frac_a, frac_b}, parent);
						return res->evaluate();
					}
					break;
				}
				default:
				{
					expPtr a_1 = a->data[0];
					expPtr a_2 = a->data[1];

					if ((a_1->isIntegral() && b->isIntegral() && gcd(a_1->value, b->value) > 1)
						|| expressionEquals(a_1, b)){
						expPtr frac_a = newExp(DIVISION, {a_1, b}, parent);
						expPtr frac_b = a_2;
						expPtr res = newExp(MULTIPLICATION, {frac_a, frac_b}, parent);
						return res->evaluate();
					} else if ((a_2->isIntegral() && b->isIntegral() && gcd(a_2->value, b->value) > 1)
						|| expressionEquals(a_2, b)){
						expPtr frac_a = newExp(DIVISION, {a_2, b}, parent);
						expPtr frac_b = a_1;
						expPtr res = newExp(MULTIPLICATION, {frac_a, frac_b}, parent);
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
					expPtr num = newExp(MULTIPLICATION, {a->data[0], b->data[1]}, parent);
					expPtr den = newExp(MULTIPLICATION, {a->data[1], b->data[0]}, parent);
					expPtr frac = newExp(DIVISION, {num, den}, parent);

					return frac->evaluate();
				}
				default:
				{
					expPtr num = a->data[0];
					expPtr den = newExp(MULTIPLICATION, {a->data[1], b}, parent);
					expPtr frac = newExp(DIVISION, {num, den}, parent);

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
						auto new_log = newExp(LOG, {num_a, num_b}, parent);
						return new_log->evaluate();
					}else
					{
						return newExp(DIVISION, {a, b}, parent);
					}
				}
			}
		}
	}
	return newExp(DIVISION, {a, b}, parent);
}

expPtr power(const expPtr &a, const expPtr &b, const scpPtr &parent) {
	if (double_equals(b->approximate(), 0))
		return newExp(1, parent);
	if (double_equals(b->approximate(), 1))
		return a;
	if (double_equals(a->approximate(), 1))
		return newExp(1, parent);

	switch(a->type){
		case INTEGER: case CHARDEC:
		switch(b->type){
			case INTEGER: case CHARDEC: return newExp(std::pow(a->value, b->value), parent);
			case DIVISION:
			{			
				expPtr power = newExp(POWER, {a, b->data[0]}, parent);
				expPtr root = newExp(ROOT, {b->data[1], power}, parent);

				return root->evaluate();
			}
			case LOG:
			{
				expPtr num_a = a;
				expPtr base = b->data[1];
				expPtr num_b = b->data[0];

				if (!expressionEquals(num_a, base))
					return newExp(POWER, {a, b}, parent);

				return num_b;
			}
		}
		break;
		case DIVISION:
		{			
			expPtr num = newExp(POWER, {a->data[0], b}, parent);
			expPtr den = newExp(POWER, {a->data[1], b}, parent);
			expPtr frac = newExp(DIVISION, {num, den}, parent);

			return frac->evaluate();
		}
		break;
		default:
		switch(b->type){
			case DIVISION:
			{			
				expPtr power = newExp(POWER, {a, b->data[0]}, parent);
				expPtr root = newExp(ROOT, {b->data[1], power}, parent);

				return root->evaluate();
			}
		}
		break;
	}
	return newExp(POWER, {a, b}, parent);
}

expPtr root(const expPtr &a, const expPtr &b, const scpPtr &parent) {
	if (double_equals(b->approximate(), 0))
		return newExp(0, parent);
	if (double_equals(b->approximate(), 1))
		return newExp(1, parent);
	if (b->approximate() < 0)
		throw std::runtime_error("ERROR: You cannot take the root of a negative number.");

	switch(a->type){
		case INTEGER: case CHARDEC:
		switch(b->type){
			case INTEGER: case CHARDEC:
			{
				auto num = b->value;
				auto root = a->value;
				auto list = factor(num, root);

				if (!list.empty())
				{
					rawInt newNumber = 1;

					for (auto &i : list)
					{
						newNumber *= (rawInt)std::round(std::pow(i, 1.0 / (double)root));
						num /= i;
					}

					if (num == 1)
						return newExp(newNumber, parent);

					expPtr r = newExp(ROOT, {newExp(root, parent), newExp(num, parent)}, parent);
					expPtr t = newExp(MULTIPLICATION, {newExp(newNumber, parent), r}, parent);
					return t->evaluate();
				}
				break;
			}
			case DIVISION:
			{			
				expPtr num = newExp(ROOT, {a, b->data[0]}, parent);
				expPtr den = newExp(ROOT, {a, b->data[1]}, parent);
				expPtr frac = newExp(DIVISION, {num, den}, parent);

				return frac->evaluate();
			}
		}
		break;
		case DIVISION:
		{			
			expPtr power = newExp(POWER, {b, a->data[1]}, parent);
			expPtr root = newExp(ROOT, {a->data[0], power}, parent);

			return root->evaluate();
		}
		break;
		default:
		switch(b->type){
			case DIVISION:
			{			
				expPtr num = newExp(ROOT, {a, b->data[0]}, parent);
				expPtr den = newExp(ROOT, {a, b->data[1]}, parent);
				expPtr frac = newExp(DIVISION, {num, den}, parent);

				return frac->evaluate();
			}
		}
		break;
	}
	return newExp(ROOT, {a, b}, parent);
}

expPtr logar(const expPtr &a, const expPtr &b, const scpPtr &parent) {
	if (b->approximate() <= 0)
		throw std::runtime_error("ERROR: You cannot take the log of zero or a negative number.");
	switch(a->type){
		case INTEGER: case CHARDEC:
		switch(b->type){
			case INTEGER: case CHARDEC:
			{
				rawInt num = a->value;
				rawInt base = b->value;

				if (double_equals(log(num) / log(base), round(log(num) / log(base))))
				{
					rawInt base_value_n = round(log(num) / log(base));
					return newExp(base_value_n, parent);
				}

				auto factors = primeFactors(num);

				if (factors[0] == num)
					return newExp(LOG, {a, b}, parent);
				else
				{
					expPtr num_a = newExp(LOG, {newExp(factors[1], parent), newExp(base, parent)}, parent);
					expPtr num_b = newExp(LOG, {newExp(num / factors[1], parent), newExp(base, parent)}, parent);
					expPtr t = newExp(ADDITION, {num_b, num_a}, parent);

					return t->evaluate();
				}
			}
		}
	}
	return newExp(LOG, {a, b}, parent);
}