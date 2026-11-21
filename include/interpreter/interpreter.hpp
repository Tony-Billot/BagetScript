#pragma once

#include "ast/statements/declaration.hpp"
#include "ast/statements/print_statement.hpp"
#include "ast/statements/assignment.hpp"
#include "ast/statements/if_statement.hpp"
#include "ast/statements/function.hpp"
#include "ast/expressions/function_call_expression.hpp"

#include <string>
#include <unordered_map>
#include <variant>
#include <memory>
#include <optional>
#include <vector>

class Interpreter
{
    public:
        void execute(std::unique_ptr<Statement> statement);
        void execute(const Declaration& declaration);
        void execute(const PrintStatement& statement);
        void execute(const Assignment& assignment);
        void execute(const IfStatement& statement);
        void execute(std::unique_ptr<FunctionStatement> statement);
        void execute(const FunctionCallExpression& statement);

    private:
        using Value = std::variant<double, std::string, bool>;
        std::unordered_map<std::string, Value> variables;
        std::unordered_map<std::string, std::unique_ptr<FunctionStatement>> functions;

        Value evaluate(const Expression& expression);
        std::optional<Value> execute_block(const std::vector<std::unique_ptr<Statement>>& statements);
        Value execute_function(const FunctionCallExpression& call);
};