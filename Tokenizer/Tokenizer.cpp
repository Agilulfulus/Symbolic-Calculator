#include "Tokenizer.h"

std::vector<std::string> tokenize(
    const std::string &str,
    const std::vector<std::string> &operators) 
{
    // Building stack - seperating operators from variables/numbers/strings
    std::vector<std::string> stack = {""};
    bool mode = true;
    char inQuotes = 0;
    int newMode;
    for (const char &c : str) {
        // First sections checks whether the current character is a quotation 
        // mark, either " or '
        if (inQuotes != 0) {
            if (c == inQuotes) {
                stack.push_back("");
                inQuotes = 0;
            } else
                stack.back().push_back(c);
            continue;
        } else {
            if (c == '\"' || c == '\'') {
                if (c == '\"')
                    stack.push_back("$");
                else
                    stack.push_back("`");
                inQuotes = c;
                continue;
            }
        }
        // If the char is not a quotation mark, it begins the process of 
        // seperating the values from the operators
        newMode = 0;
        if (isspace(c))
            newMode = -1;
        else if (isalnum(c) || c == '_')
            newMode = 1;
        else if (!vector_contains(operators, stack.back() + std::string(1, c)))
            stack.push_back("");

        if (mode != newMode)
            stack.push_back("");
        if (newMode >= 0)
            stack.back().push_back(c);

        mode = newMode;
    }

    // Erase all empty items
    stack.erase(std::remove(stack.begin(), stack.end(), ""), stack.end());

    // See if there are any negative numbers - not very pretty
    std::string last = "(";
    for (auto &s : stack){
        if (s == "-" && (
            std::find(operators.begin(), operators.end(), last) != operators.end()
                || last == "("
                || last == "["
                || last == "{"
                || last[0] == '$'
                || last[0] == '`')
            )
        {
            last = "~~";
            s = "~~";
        }else{
            last = s;
        }
    }

    // Building final tokens by declaring functional and array operators
    std::vector<std::string> tokens = {stack[0]};
    for (size_t i = 1; i < stack.size(); i++) {
        if ((isalpha(stack[i-1][0]) && !vector_contains(operators, stack[i-1])) 
            || stack[i-1][0] == '_' 
            || stack[i-1] == ")" 
            || stack[i-1] == "]" 
            || stack[i-1] == "}" 
            || stack[i-1][0] == '$'
            || stack[i-1][0] == '`') 
        {
            if (stack[i] == "(")
                tokens.push_back("!!");
            else if (stack[i] == "[")
                tokens.push_back("##");
        }

        tokens.push_back(stack[i]);
    }

    return tokens;
}

std::vector<std::string> infixToPostfix(
    const std::vector<std::string> &tokens,
    const std::vector<std::string> &operators,
    const std::map<std::string, int> &precedence) 
{
    std::vector<std::string> stack;
    std::vector<std::string> output;

    int pCount = 0;
    int bCount = 0;
    int cCount = 0;

    // Small lambda to simplify the process of getting precedence
    auto getPrecedence = [&](std::string token) {
        if (precedence.find(token) != precedence.end())
            return precedence.at(token);
        if (token == "("
            || token == ")"
            || token == "["
            || token == "]"
            || token == "{"
            || token == "}")
            return -999;
        return 999;
    };

    for (const std::string token : tokens){
        if (vector_contains(operators, token)){
            if (!stack.empty()){
                while (getPrecedence(stack.back()) > getPrecedence(token) 
                    || (getPrecedence(stack.back()) == getPrecedence(token) 
                        && token != "^"
                        && token != "else"))
                {
                    output.push_back(stack.back());
                    stack.pop_back();
                    if (stack.empty()) break;
                }
            }
            stack.push_back(token);
        }else if (token == "(" || token == "[" || token == "{"){
            // Bracket initialization
            switch(token[0]){
            case '(': pCount++; break;
            case '[': bCount++; break;
            case '{': cCount++; break;
            }
            stack.push_back(token);
            output.push_back(token);
        }else if (token == ")" || token == "]" || token == "}"){
            // Bracket finalization
            switch(token[0]){
            case ')':
                pCount--;
                while (!stack.empty()) {
                    if (stack.back() == "(") break;
                    output.push_back(stack.back());
                    stack.pop_back();
                }
                break;
            case ']':
                bCount--;
                while (!stack.empty()) {
                    if (stack.back() == "[") break;
                    output.push_back(stack.back());
                    stack.pop_back();
                }
                break;
            case '}':
                cCount--;
                while (!stack.empty()) {
                    if (stack.back() == "{") break;
                    output.push_back(stack.back());
                    stack.pop_back();
                }
                break;
            }
            // Error checking - if the stack is empty that means the loops above
            // failed to find their corresponding bracket
            if (stack.empty()){
                switch(token[0]){
                case ')': throw(std::runtime_error("ERROR: Paranthesis mismatch!"));
                case ']': throw(std::runtime_error("ERROR: List bracket mismatch!"));
                case '}': throw(std::runtime_error("ERROR: Scope bracket mismatch!"));
                }
            }
            stack.pop_back();
            output.push_back(token);
        }else
            output.push_back(token);
    }

    // Error checking - if for whatever reason the count of brackets is uneven,
    // the input was unable to be evaluated
    if (pCount != 0) throw(std::runtime_error("ERROR: Paranthesis mismatch!"));
    if (bCount != 0) throw(std::runtime_error("ERROR: List bracket mismatch!"));
    if (cCount != 0) throw(std::runtime_error("ERROR: Scope bracket mismatch!"));

    reverse(stack.begin(), stack.end());
    output.insert(output.end(), stack.begin(), stack.end());

    return output;
}