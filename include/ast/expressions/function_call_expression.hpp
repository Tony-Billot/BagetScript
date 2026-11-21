#pragma once

#include "../expression.hpp"

#include <memory>
#include <string>
#include <vector>

struct FunctionCallExpression : Expression
{
    std::string name;
    std::vector<std::unique_ptr<Expression>> arguments;
};
