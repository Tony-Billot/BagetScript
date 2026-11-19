#pragma once

#include "ast/statements/declaration.hpp"
#include "ast/statements/print_statement.hpp"

class ASTPrinter
{
public:
    static void print(const Declaration& declaration);
    static void print(const PrintStatement& statement);
};