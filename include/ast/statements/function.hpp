#pragma once

#include "../statement.hpp"
#include "../expression.hpp"
#include "common/token.hpp"
#include "../expressions/function_call_expression.hpp"

#include <string>
#include <vector>
#include <memory>

struct Parameter
{
    TokenType type;
    std::string name;
};

struct FunctionStatement : Statement
{
    std::string name;
    std::vector<Parameter> parameters;
    std::vector<std::unique_ptr<Statement>> statements;
};

struct ReturnStatement : Statement
{
    std::unique_ptr<Expression> value;
};

struct FunctionCallStatement : Statement
{
    std::unique_ptr<FunctionCallExpression> call;
};