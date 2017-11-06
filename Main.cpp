#include <iostream>
#include "Expression/Expression.h"
#include "Tokenizer/Tokenizer.h"

/*

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

fib(limit) => array from {
    array = [0, 1],
    i in [3:limit] do {
        array = array ++ [array[i - 1] + array[i - 2]]
    }
}

*/

int main() {
    Compiler * c = new Compiler();
	std::string line;
	while(true) {
		std::cout << "\n>>> ";
		std::getline(std::cin, line);
		if (line == "") break;

		try {
            expPtr e = c->execute(line);
			std::cout << "\n\t" << e->getString() << std::endl;
            std::cout << "\n\tDAT:" << Expression::refCount << "\tLAM: " << Lambda::refCount << "\tSCO: " << Scope::refCount << std::endl;
		} catch(std::runtime_error &e){
			std::cout << "\n\t" << e.what() << std::endl;
		}
    }
    delete c;
	return 0;
}