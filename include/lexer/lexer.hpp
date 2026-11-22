#pragma once

#include "common/token.hpp"
#include <string>
#include <vector>

class Lexer
{
    public:
        std::vector<Token> to_tokens(const std::string& source);
        std::string token_type_to_string(TokenType type);
};