#include "debug/ast_printer.hpp"
#include "ast/expressions/number_expression.hpp"
#include "ast/expressions/text_expression.hpp"

#include <iostream>

void ASTPrinter::print(const Declaration& declaration)
{
    std::cout << "Declaration\n";
    std::cout << "├── type: " << (declaration.type == TokenType::TYPE_NOMBRE ? "nombre" : "texte") << '\n';
    std::cout << "├── name: " << declaration.name << '\n';
    std::cout << "└── value:\n";

    if (declaration.type == TokenType::TYPE_NOMBRE)
    {
        NumberExpression* number = static_cast<NumberExpression*>(declaration.value.get());
        std::cout << "    └── NumberExpression\n";
        std::cout << "        └── value: " << number->value << '\n';
    }
    else
    {
        TextExpression* text = static_cast<TextExpression*>(declaration.value.get());
        std::cout << "    └── TextExpression\n";
        std::cout << "        └── value: " << text->value << '\n';
    }
}