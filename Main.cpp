#include <iostream>
#include "Expression/Expression.h"
#include "Tokenizer/Tokenizer.h"

/*

sum = (a) -> s from {
	s = 0,
	i in a do s = s + i
}

prod = (a) -> p from {
	p = 1,
	i in a do p = p * i
}

reverse(a) -> a from {
	a[1:#a] = a[#a:1]
}

*/

int main() {
	const std::vector<std::string> operators = {
        "+","-","*","/","%","^","rt",
        "++","--",",",
        "+=","-=","*=","/=","%=","^=",
        "=","==","!=","<",">","<=",">=",
        "||","&&", ":","?",
        "<<", ">>", "**", "->",
        "::", "##", "!!",
        "~~", "do", "from", "in"
    };

	const std::map<std::string, int> precedence =
    {
        //Grouping
        { "!!", 11 },
        { "##", 11 },
        { "::", 11 },
        { "->", 1 },
    
        //Arithmetic
        { "~~", 10 },
        { "++", 6 },
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
        { "^=", -1 },
        { "*=", -1 },
        { "/=", -1 },
        { "%=", -1 },
        { "+=", -1 },
        { "-=", -1 },
    
        //Seperation
		{ "?", -2 },
        { "do", -2 },
        { ":", -2 },
        { ",", -3 }
	};
	
	Scope * m = new Scope(NULL);
	std::string line;
	while(true) {
		std::cout << "> ";
		std::getline(std::cin, line);
		if (line == "") break;

		try {
			auto tokens = tokenize(line, operators);
			auto postfix = infixToPostfix(tokens, operators, precedence);
			for (auto &t : postfix)
				std::cout << t << " ";
			std::cout << std::endl;

			auto e = convertTokens(m, postfix, operators);
			std::cout << e->getString() << std::endl;
			std::cout << e->evaluate()->getString() << std::endl;
		} catch(std::runtime_error &e){
			std::cout << e.what() << std::endl;
		}
    }
	return 0;
}