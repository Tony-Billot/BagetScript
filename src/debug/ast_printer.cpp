#include "debug/ast_printer.hpp"
#include "ast/expressions/number_expression.hpp"

#include <iostream>

void ASTPrinter::print(const Declaration& declaration)
{
    std::cout << "Declaration\n";
    std::cout << "├── type: nombre\n";
    std::cout << "├── name: " << declaration.name << '\n';
    std::cout << "└── value:\n";

    NumberExpression* number = static_cast<NumberExpression*>(declaration.value.get());

    std::cout << "    └── NumberExpression\n";
    std::cout << "        └── value: " << number->value << '\n';
}