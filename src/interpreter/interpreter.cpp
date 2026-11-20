#include "interpreter/interpreter.hpp"
#include "ast/expressions/identifier_expression.hpp"
#include "ast/expressions/number_expression.hpp"
#include "ast/expressions/text_expression.hpp"
#include "ast/expressions/boolean_expression.hpp"
#include "common/exceptions.hpp"
#include <charconv>
#include <iostream>
#include "ast/expressions/binary_expression.hpp"
#include "ast/expressions/unary_expression.hpp"
#include <algorithm>


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

    if (const BooleanExpression* boolean = dynamic_cast<const BooleanExpression*>(&expression))
    {
        return boolean->value;
    }

    if (const UnaryExpression* unary = dynamic_cast<const UnaryExpression*>(&expression))
    {
        Value value = evaluate(*unary->value);

        if (unary->operation == TokenType::MOINS)
        {
            // Reverse the string : to change if not smart
            if (!std::holds_alternative<double>(value))
            {
                std::string text = std::get<std::string>(value);
                std::reverse(text.begin(), text.end());
                return text;
            }
            return -std::get<double>(value);
        }

        if (unary->operation == TokenType::NON)
        {
            if (!std::holds_alternative<bool>(value))
            {
                throw RuntimeError(
                    "l'opérateur 'non' attend un booléen",
                    unary->line,
                    unary->column
                );
            }

            return !std::get<bool>(value);
        }

        throw RuntimeError("opérateur unaire inconnu", unary->line, unary->column);
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

        if (std::holds_alternative<bool>(left_value) && std::holds_alternative<bool>(right_value))
        {
            bool left_bool = std::get<bool>(left_value);
            bool right_bool = std::get<bool>(right_value);

            if (binary->operation == TokenType::ET)
            {
                return left_bool && right_bool;
            }
            if (binary->operation == TokenType::OU)
            {
                return left_bool || right_bool;
            }

            throw RuntimeError("opérateur inconnu pour les booléens", binary->line, binary->column);
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
    bool compatible = (declaration.type == TokenType::TYPE_NOMBRE && std::holds_alternative<double>(value)) || (declaration.type == TokenType::TYPE_TEXTE && std::holds_alternative<std::string>(value)) || (declaration.type == TokenType::TYPE_BOOLEEN && std::holds_alternative<bool>(value));

    if (!compatible)
    {
        throw RuntimeError("type de valeur incompatible pour la déclaration de '" + declaration.name + "'", declaration.value->line, declaration.value->column);
    }
    variables[declaration.name] = evaluate(*declaration.value);
}

void Interpreter::execute(const Assignment& assignment)
{
    auto variable = variables.find(assignment.name);
    if (variable == variables.end())
    {
        throw RuntimeError("variable inconnue '" + assignment.name + "'", assignment.value->line, assignment.value->column);
    }

    Value value = evaluate(*assignment.value);
    if (variable->second.index() != value.index())
    {
        throw RuntimeError("type de valeur incompatible pour la variable '" + assignment.name + "'", assignment.value->line, assignment.value->column);
    }

    variable->second = value;
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
    else if (std::holds_alternative<bool>(value))
    {
        std::cout << (std::get<bool>(value) ? "vrai" : "faux") << '\n';
    }

    else
    {
        throw RuntimeError("type de valeur inconnu pour l'affichage", statement.expression->line, statement.expression->column);
    }
}
