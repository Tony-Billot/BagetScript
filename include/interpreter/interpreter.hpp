#pragma once

#include "ast/statements/declaration.hpp"
#include "ast/statements/print_statement.hpp"
#include "ast/statements/assignment.hpp"
#include "ast/statements/if_statement.hpp"

#include <string>
#include <unordered_map>
#include <variant>

class Interpreter
{
    public:
        void execute(const Declaration& declaration);
        void execute(const PrintStatement& statement);
        void execute(const Assignment& assignment);
        void execute(const IfStatement& statement);

    private:
        using Value = std::variant<double, std::string, bool>;
        std::unordered_map<std::string, Value> variables;

        Value evaluate(const Expression& expression);
};