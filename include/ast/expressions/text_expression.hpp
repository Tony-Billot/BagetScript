#pragma once

#include "../expression.hpp"
#include <string>

struct TextExpression : Expression
{
    std::string value;
};