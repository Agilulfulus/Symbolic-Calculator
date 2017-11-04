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
    x > 1 then x * fact(x - 1) else x
}

size(array) => s from {
    s = 0,
    i in array do s = s + 1
}

sort(array) => array from {
    n in array do {
        i in [1:(#array - 1)] do {
            array[i] > array[i + 1] then {
                array[i, i + 1] = array[i + 1, i]
            }
        }
    }
}

quickSort(array, comparator) => array from {
    #array <= 1 then return,
    comparator == nil then comparator(x, y) => {x < y},
    pivot = array[~],
    lower = upper = [],
    n in array[1:(#array - 1)] do {
        comparator(n, pivot) then
            lower = lower ++ [n]
        else
            upper = upper ++ [n]
    },
    array = quickSort(lower, comparator) ++ [pivot] ++ quickSort(upper, comparator)
}

fib(limit) => array from {
    array = [0, 1],
    i in [3:limit] do {
        array = array ++ [array[i - 1] + array[i - 2]]
    }
}

*/

int main() {
	std::vector<std::string> operators = {
        "+","-","*","/","%","^","rt","log",
        "++","--",",",
        "+=","-=","*=","/=","%=","^=",
        "=","==","!=","<",">","<=",">=",
        "||","&&", ":","?",
        "<<", ">>", "**", "->", "=>",
        "::", "##", "!!", "#",
        "~~", "do", "from", "in","then","else"
    };

	std::map<std::string, int> precedence =
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
        { "else", -3},
        { ",", -4 }
	};
	
    scpPtr m = newScp();
	std::string line;
	while(true) {
		std::cout << "\n>>> ";
		std::getline(std::cin, line);
		if (line == "") break;

		try {
			auto tokens = tokenize(line, operators);
			auto postfix = infixToPostfix(tokens, operators, precedence);

            //for (auto &s : postfix)
                //std::cout << s << " ";
            //std::cout << std::endl;

			auto e = convertTokens(m, postfix, operators);
			std::cout << "\n\t" << e->getString() << std::endl;
			std::cout << "\n\t" << e->evaluate()->getString() << std::endl;
            std::cout << "\n\tDAT:" << Expression::refCount << "\tLAM: " << Lambda::refCount << "\tSCO: " << Scope::refCount << std::endl;
		} catch(std::runtime_error &e){
			std::cout << "\n\t" << e.what() << std::endl;
		}
    }
	return 0;
}