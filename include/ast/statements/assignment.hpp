#pragma once

#include "../statement.hpp"
#include "../expression.hpp"
#include <memory>
#include <string>

struct Assignment : Statement
{
    std::string name;
    std::unique_ptr<Expression> value;
};