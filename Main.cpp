#include <iostream>
#include "Expression/Expression.h"
#include "Tokenizer/Tokenizer.h"

/*

sum(a) => s from {
	s = 0,
	i in a do s = s + i
}

prod(a) => p from {
	p = 1,
	i in a do p = p * i
}

reverse(a) => a from {
	a[1:#a] = a[#a:1]
}

fact(x) => {
    x > 1 then 
        x * fact(x - 1)
    else
        x
}

size(array) => s from {
    s = 0,
    i in array do s = s + 1
}

sort(array) => array from {
    n in [1:#array] do {
        i in [1:(#array - 1)] do {
            first = array[i],
            second = array[i + 1],
            first > second then {
                array[i] = second,
                array[i + 1] = first
            }
        }
    }
}

*/

int main() {
	const std::vector<std::string> operators = {
        "+","-","*","/","%","^","rt","log",
        "++","--",",",
        "+=","-=","*=","/=","%=","^=",
        "=","==","!=","<",">","<=",">=",
        "||","&&", ":","?",
        "<<", ">>", "**", "->", "=>",
        "::", "##", "!!", "#",
        "~~", "do", "from", "in","then","else"
    };

	const std::map<std::string, int> precedence =
    {
        //Grouping
        { "!!", 11 },
        { "##", 11 },
        { "::", 11 },
        { "->", 1 },
        { "#", 10 },
        { ":", 9 },
    
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
        { "else", -2},
        { ",", -4 }
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