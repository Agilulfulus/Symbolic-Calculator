#include <iostream>
#include <ctime>
#include "Expression/Expression.h"
#include "Tokenizer/Tokenizer.h"

/*

reverse(a) => a from {
	a[1:#a] = a[#a:1]
}

fact(x) => {
    x > 1 then x * fact(x - 1) else x
}

fib(limit) => array from {
    array = [0, 1],
    i in [3:limit] do {
        array = array ++ [array[i - 1] + array[i - 2]]
    }
}

*/

enum COMMANDS {
    SYSTEM = 0,
    PRINT,
    INPUT
};

std::vector<std::string> _send(std::vector<std::string> &input){
    std::vector<std::string> output;
    switch(std::stoi(input[0]))
    {
        case SYSTEM:
        {
            system(input[1].c_str());
            break;
        }
        case PRINT:
        {
            for (int i = 1; i < input.size(); i++)
            {
                std::cout << input[i];

                if (i < input.size() - 1)
                    std::cout << "\t";
            }
            break;
        }
        case INPUT:
        {
            std::string line;
            std::getline(std::cin, line);
            output.push_back(line);
            break;
        }
    }

    return output;
}

int main(int argc, char *argv[]) {
    srand(time(NULL));
    Compiler * c = new Compiler(&_send);
    if ( argc != 2 ){
        std::cout << "Symbolic Calculator (Terminal Interface - term.sy)" << std::endl;
        std::cout << "To execute a file, do 'symc <filename>'" << std::endl;
        c->execute("load \"term.sy\"");
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
    }else{
        expPtr e = c->execute("load \"" + std::string(argv[1]) + "\"");
    }
    c->_send = NULL;
    delete c;
    return 0;
}