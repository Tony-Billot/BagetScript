#pragma once

#include "../expression.hpp"
#include <string>

struct IdentifierExpression : Expression
{
    std::string name;
};