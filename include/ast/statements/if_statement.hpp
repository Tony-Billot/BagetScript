#pragma once

#include "../statement.hpp"
#include "../expression.hpp"
#include <memory>
#include <vector>

struct ElseIfBranch
{
    std::unique_ptr<Expression> condition;
    std::vector<std::unique_ptr<Statement>> statements;
};

struct IfStatement : Statement
{
    std::unique_ptr<Expression> condition;
    std::vector<std::unique_ptr<Statement>> statements;
    std::vector<ElseIfBranch> else_if_branches;
    std::vector<std::unique_ptr<Statement>> else_statements;
};
