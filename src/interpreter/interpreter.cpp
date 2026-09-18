#include "interpreter/interpreter.hpp"
#include "ast/expressions/identifier_expression.hpp"
#include "ast/expressions/number_expression.hpp"
#include "ast/expressions/text_expression.hpp"

#include <iostream>
#include <stdexcept>

Interpreter::Value Interpreter::evaluate(const Expression& expression)
{
    if (auto* number = dynamic_cast<const NumberExpression*>(&expression))
        return number->value;

    if (auto* text = dynamic_cast<const TextExpression*>(&expression))
        return text->value;

    auto* identifier = dynamic_cast<const IdentifierExpression*>(&expression);
    if (identifier)
    {
        auto variable = variables.find(identifier->name);
        if (variable == variables.end())
            throw std::runtime_error("Erreur : variable inconnue '" + identifier->name + "'.");
        return variable->second;
    }

    throw std::runtime_error("Erreur : expression inconnue.");
}

void Interpreter::execute(const Declaration& declaration)
{
    variables[declaration.name] = evaluate(*declaration.value);
}

void Interpreter::execute(const PrintStatement& statement)
{
    std::visit([](const auto& value) { std::cout << value << '\n'; }, evaluate(*statement.expression));
}