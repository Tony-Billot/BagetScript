#pragma once

#include "../statement.hpp"
#include "../expression.hpp"
#include <memory>

struct PrintStatement : Statement
{
    std::unique_ptr<Expression> expression;
};