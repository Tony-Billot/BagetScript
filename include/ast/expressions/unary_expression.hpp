#pragma once

#include "../expression.hpp"
#include "common/token.hpp"
#include <memory>

struct UnaryExpression : Expression
{
    TokenType operation;
    std::unique_ptr<Expression> value;
};