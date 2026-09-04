#pragma once

#include "../Expression.hpp"
#include <string>

struct TextExpression : Expression
{
    std::string value;
};