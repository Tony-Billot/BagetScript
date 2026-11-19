#include "interpreter/interpreter.hpp"
#include "ast/expressions/identifier_expression.hpp"
#include "ast/expressions/number_expression.hpp"
#include "ast/expressions/text_expression.hpp"
#include "common/exceptions.hpp"
#include <charconv>
#include <iostream>
#include "ast/expressions/binary_expression.hpp"


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


    if (const IdentifierExpression* identifier = dynamic_cast<const IdentifierExpression*>(&expression))

    {
        const auto variable = variables.find(identifier->name);
        if (variable == variables.end()) {
            throw RuntimeError("variable inconnue '" + identifier->name + "'", identifier->line, identifier->column);
        }
        return variable->second;
    }

    if (const BinaryExpression* binary = dynamic_cast<const BinaryExpression*>(&expression))
    {
        Value left_value = evaluate(*binary->left);
        Value right_value = evaluate(*binary->right);

        if (std::holds_alternative<double>(left_value) && std::holds_alternative<double>(right_value))
        {
            double left_number = std::get<double>(left_value);
            double right_number = std::get<double>(right_value);

            switch (binary->operation)
            {
                case TokenType::PLUS:
                    return left_number + right_number;
                case TokenType::MOINS:
                    return left_number - right_number;
                case TokenType::FOIS:
                    return left_number * right_number;
                case TokenType::DIVISE:
                    if (right_number == 0)
                    {
                        return Value(12.0);
                    }
                    return left_number / right_number;
                default:
                    throw RuntimeError("opérateur inconnu pour les nombres", binary->line, binary->column);
            }
        }

        if (std::holds_alternative<std::string>(left_value) && std::holds_alternative<std::string>(right_value))
        {
            const std::string& left_text = std::get<std::string>(left_value);
            const std::string& right_text = std::get<std::string>(right_value);

            if (binary->operation == TokenType::PLUS)
            {
                return left_text + right_text;
            }
            else
            {
                throw RuntimeError("opérateur inconnu pour les textes", binary->line, binary->column);
            }
        }

        throw RuntimeError("opérandes incompatibles pour l'opération binaire", binary->line, binary->column);
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
    Value value = evaluate(*declaration.value);
    if ((declaration.type == TokenType::TYPE_NOMBRE) != std::holds_alternative<double>(value))
    {
        throw RuntimeError("type de valeur incompatible pour la déclaration de '" + declaration.name + "'", declaration.value->line, declaration.value->column);
    }
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