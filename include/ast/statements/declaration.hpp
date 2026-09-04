#pragma once

#include "../statement.hpp"
#include "../expression.hpp"
#include "common/token.hpp"
#include <memory>
#include <string>

struct Declaration : Statement
{
    TokenType type;
    std::string name;
    std::unique_ptr<Expression> value;
};