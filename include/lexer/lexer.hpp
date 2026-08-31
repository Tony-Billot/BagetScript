#pragma once

#include "common/token.hpp"
#include <string>
#include <vector>

class Lexer
{
public:
    std::vector<Token> to_tokens(const std::string& source);
    Token lecture_token(const std::string& token);
};