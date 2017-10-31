#include<iostream>
#include "Tokenizer/Tokenizer.h"

int main(void)
{
    const std::vector<std::string> operators = {
        "+","-","*","/","%","^","rt",
        "++","--",
        "+=","-=","*=","/=","%=","^=",
        "=","==","!=","<",">","<=",">=",
        "||","&&", ":","?",
        "<<", ">>", "**",
        "::", "##",
        "~~"
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
        { "<<", 2 },
    
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
        { "!", -2 },
        { ":", -2 },
        { ",", -3 }
	};
	
	std::string line;
	while(true) {
		std::cout << "> ";
		std::getline(std::cin, line);
		if (line == "") break;
		try {
			auto tokens = tokenize(line, operators);
			auto postfix = infixToPostfix(tokens, operators, precedence);
			for (auto &t : postfix)
				if (t != "(" && t != ")")
					std::cout << t << " ";
			std::cout << std::endl;
		} catch(std::runtime_error &e){
			std::cout << e.what() << std::endl;
		}
    }
	return 0;
}