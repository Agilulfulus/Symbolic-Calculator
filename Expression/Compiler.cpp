#include "Expression.h"

Compiler::Compiler(strVec (*_send)(strVec&)){
	origin = newScp();
	this->_send = _send;
}

expPtr Compiler::execute(string line){
	std::vector<std::string> operators = {
        "+","-","*","/","%","^","rt","log",
        "++","--",",","..",".",
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
        { ".", 12 },
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
            else if (token == "."){
                if (num1->type != INTEGER || num2->type != INTEGER)
                    throw std::runtime_error("ERROR: Inapropriate decimal placement.");

                auto den = newExp(std::pow(10, (std::to_string(num2->value)).length()), m);
                stack.push_back(newExp(ADDITION, {num1, newExp(DIVISION, {num2, den}, m)}, m));
            }else if (token == "=")		stack.push_back(newExp(SET, {num1, num2}, m));
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