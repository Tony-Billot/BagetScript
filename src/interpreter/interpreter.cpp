#include "interpreter/interpreter.hpp"
#include "ast/expressions/identifier_expression.hpp"
#include "ast/expressions/number_expression.hpp"
#include "ast/expressions/text_expression.hpp"
#include "common/exceptions.hpp"
#include <charconv>
#include <iostream>

Interpreter::Value Interpreter::evaluate(const Expression& expression)
{
    if (const NumberExpression* number = dynamic_cast<const NumberExpression*>(&expression))
    {
        return number->value;
    }

    if (const TextExpression* text = dynamic_cast<const TextExpression*>(&expression))
    {
        return text->value;
    }

    const IdentifierExpression* identifier = dynamic_cast<const IdentifierExpression*>(&expression);

    if (identifier)
    {
        const auto variable = variables.find(identifier->name);
        if (variable == variables.end()) {
            throw RuntimeError("variable inconnue '" + identifier->name + "'", identifier->line, identifier->column);
        }
        return variable->second;
    }

    throw RuntimeError("expression inconnue", expression.line, expression.column);
}

static std::string format_number(double value)
{
    char buffer[400];
    auto result = std::to_chars(buffer, buffer + sizeof(buffer), value, std::chars_format::fixed);
    return std::string(buffer, result.ptr);
}

void Interpreter::execute(const Declaration& declaration)
{
    variables[declaration.name] = evaluate(*declaration.value);
}

void Interpreter::execute(const PrintStatement& statement)
{
    Value value = evaluate(*statement.expression);

    if (std::holds_alternative<double>(value))
    {
        std::cout << format_number(std::get<double>(value)) << '\n';
    }
    else if (std::holds_alternative<std::string>(value))
    {
        std::cout << std::get<std::string>(value) << '\n';
    }

    else
    {
        throw RuntimeError("type de valeur inconnu pour l'affichage", statement.expression->line, statement.expression->column);
    }
}