#ifndef TOKENIZER_H
#define TOKENIZER_H

#include<string>
#include<vector>
#include<map>
#include<ctype.h>
#include<algorithm>
#include<stdexcept>

#define vector_contains(v, a)	(std::find(v.begin(), v.end(), a) != v.end())

std::vector<std::string> tokenize(
	const std::string &str,
	const std::vector<std::string> &operators);

std::vector<std::string> infixToPostfix(
    const std::vector<std::string> &tokens,
    const std::vector<std::string> &operators,
    const std::map<std::string, int> &precedence);

#endif