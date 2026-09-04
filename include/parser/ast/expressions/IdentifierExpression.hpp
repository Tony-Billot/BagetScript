#pragma once

#include "../Expression.hpp"
#include <string>

struct IdentifierExpression : Expression
{
    std::string name;
};