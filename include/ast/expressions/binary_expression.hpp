#pragma once

#include "../expression.hpp"
#include "../../lexer/token.hpp"
#include <memory>

struct BinaryExpression : Expression
{
    std::unique_ptr<Expression> left;
    TokenType operation;
    std::unique_ptr<Expression> right;
};