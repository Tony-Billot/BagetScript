#include "parser/parser.hpp"
#include "ast/expressions/identifier_expression.hpp"
#include "ast/expressions/number_expression.hpp"
#include "ast/expressions/text_expression.hpp"
#include "ast/expressions/boolean_expression.hpp"
#include "common/exceptions.hpp"
#include <iostream>
#include <memory>
#include "ast/expressions/binary_expression.hpp"
#include "ast/expressions/unary_expression.hpp"


Parser::Parser(std::vector<Token> tokens)
{
    this->tokens = tokens;
}

bool Parser::has_more_tokens() const
{
    return tokens[position].type != TokenType::FIN_DE_FICHIER;
}

Token Parser::current_token()
{
    return tokens[position];
}

Token Parser::consume(TokenType expected)
{
    if (current_token().type != expected)
    {
        throw SyntaxError(token_type_to_string(expected) + " attendu", current_token().line, current_token().column);
    }

    return tokens[position++];
}


Declaration Parser::parse_declaration()
{
    Declaration declaration;

    if (current_token().type != TokenType::TYPE_NOMBRE && current_token().type != TokenType::TYPE_TEXTE && current_token().type != TokenType::TYPE_BOOLEEN)
    {
        if (current_token().type == TokenType::IDENTIFIANT)
        {
            throw SyntaxError("type '" + current_token().valeur + "' non reconnu", current_token().line, current_token().column);
        }
        throw SyntaxError("type nombre, texte ou booléen attendu", current_token().line, current_token().column);
    }

    declaration.type = consume(current_token().type).type;
    declaration.name = consume(TokenType::IDENTIFIANT).valeur;
    consume(TokenType::A_POUR_VALEUR);
    declaration.value = parse_expression();
    consume(TokenType::POINT_VIRGULE);
    return declaration;
}

PrintStatement Parser::parse_print_statement()
{
    PrintStatement statement;

    consume(TokenType::AFFICHER);
    consume(TokenType::PARENTHESE_GAUCHE);
    statement.expression = parse_expression();
    consume(TokenType::PARENTHESE_DROITE);
    consume(TokenType::POINT_VIRGULE);

    return statement;
}

Assignment Parser::parse_assignment()
{
    Assignment assignment;

    assignment.name = consume(TokenType::IDENTIFIANT).valeur;
    consume(TokenType::A_POUR_VALEUR);
    assignment.value = parse_expression();
    consume(TokenType::POINT_VIRGULE);
    return assignment;
}

std::unique_ptr<Expression> Parser::operate(std::unique_ptr<Expression> left, Token operator_token, std::unique_ptr<Expression> right)
{
    auto operation = std::make_unique<BinaryExpression>();
    operation->left = std::move(left);
    operation->operation = operator_token.type;
    operation->right = std::move(right);
    operation->line = operator_token.line;
    operation->column = operator_token.column;
    return operation;
}

std::unique_ptr<Expression> Parser::parse_expression()
{
    return parse_logical_or();
}

std::unique_ptr<Expression> Parser::parse_logical_or()
{
    auto result = parse_logical_and();

    while (is_current(TokenType::OU))
    {
        Token operator_token = consume(TokenType::OU);
        auto right_side = parse_logical_and();
        result = operate(std::move(result), operator_token, std::move(right_side));
    }

    return result;
}

std::unique_ptr<Expression> Parser::parse_logical_and()
{
    auto result = parse_addition();

    while (is_current(TokenType::ET))
    {
        Token operator_token = consume(TokenType::ET);
        auto right_side = parse_addition();
        result = operate(std::move(result), operator_token, std::move(right_side));
    }

    return result;
}

std::unique_ptr<Expression> Parser::parse_addition()
{
    auto result = parse_multiplication();

    while (is_current(TokenType::PLUS) || is_current(TokenType::MOINS))
    {
        Token operator_token = consume(current_token().type);
        auto right_side = parse_multiplication();
        result = operate(std::move(result), operator_token, std::move(right_side));
    }

    return result;
}

std::unique_ptr<Expression> Parser::parse_multiplication()
{
    auto result = parse_unary();

    while (is_current(TokenType::FOIS) || is_current(TokenType::DIVISE))
    {
        Token operator_token = consume(current_token().type);
        auto right_side = parse_unary();
        result = operate(std::move(result), operator_token, std::move(right_side));
    }

    return result;
}

std::unique_ptr<Expression> Parser::parse_value()
{
    Token token = current_token();

    if (is_current(TokenType::NOMBRE))
    {
        consume(TokenType::NOMBRE);
        auto number = std::make_unique<NumberExpression>();
        number->value = std::stod(token.valeur);
        number->line = token.line;
        number->column = token.column;
        return number;
    }

    if (is_current(TokenType::TEXTE))
    {
        consume(TokenType::TEXTE);
        auto text = std::make_unique<TextExpression>();
        text->value = token.valeur;
        text->line = token.line;
        text->column = token.column;
        return text;
    }

    if (is_current(TokenType::VRAI) || is_current(TokenType::FAUX))
    {
        Token token = consume(current_token().type);
        auto boolean = std::make_unique<BooleanExpression>();
        boolean->value = token.type == TokenType::VRAI;
        boolean->line = token.line;
        boolean->column = token.column;
        return boolean;
    }

    if (is_current(TokenType::NON))
    {
        Token operator_token = consume(TokenType::NON);

        auto non = std::make_unique<UnaryExpression>();
        non->operation = operator_token.type;
        non->value = parse_unary();
        non->line = operator_token.line;
        non->column = operator_token.column;

        return non;
    }

    if (is_current(TokenType::IDENTIFIANT))
    {
        consume(TokenType::IDENTIFIANT);
        auto variable = std::make_unique<IdentifierExpression>();
        variable->name = token.valeur;
        variable->line = token.line;
        variable->column = token.column;
        return variable;
    }

    if (is_current(TokenType::PARENTHESE_GAUCHE))
    {
        consume(TokenType::PARENTHESE_GAUCHE);
        auto inside = parse_expression();
        consume(TokenType::PARENTHESE_DROITE);
        return inside;
    }

    throw SyntaxError("nombre, texte, variable ou parenthèse attendu", token.line, token.column);
}


std::unique_ptr<Expression> Parser::parse_unary()
{
    if (is_current(TokenType::MOINS))
    {
        Token operator_token = consume(TokenType::MOINS);
        auto negative = std::make_unique<UnaryExpression>();
        negative->operation = operator_token.type;
        negative->value = parse_unary();
        negative->line = operator_token.line;
        negative->column = operator_token.column;
        return negative;
    }

    return parse_value();
}

bool Parser::is_current(TokenType type)
{
    return current_token().type == type;
}

bool Parser::has_assignment_syntax()
{
    return position + 1 < static_cast<int>(tokens.size())
        && tokens[position].type == TokenType::IDENTIFIANT
        && tokens[position + 1].type == TokenType::A_POUR_VALEUR;
}


std::string Parser::token_type_to_string(TokenType type)
{
    switch (type)
    {
        case TokenType::TYPE_NOMBRE: return "type nombre";
        case TokenType::TYPE_TEXTE: return "type texte";
        case TokenType::TYPE_BOOLEEN: return "type booléen";
        case TokenType::NOMBRE: return "nombre";
        case TokenType::TEXTE: return "texte";
        case TokenType::VRAI: return "vrai";
        case TokenType::FAUX: return "faux";
        case TokenType::IDENTIFIANT: return "identifiant";
        case TokenType::A_POUR_VALEUR: return "'='";
        case TokenType::PLUS: return "'+'";
        case TokenType::MOINS: return "'-'";
        case TokenType::FOIS: return "'*'";
        case TokenType::DIVISE: return "'/'";
        case TokenType::NON: return "'non'";
        case TokenType::ET: return "'et'";
        case TokenType::OU: return "'ou'";
        case TokenType::PARENTHESE_GAUCHE: return "'('";
        case TokenType::PARENTHESE_DROITE: return "')'";
        case TokenType::POINT_VIRGULE: return "';'";
        case TokenType::AFFICHER: return "'afficher'";
        case TokenType::FIN_DE_FICHIER: return "fin du fichier";
    }

    return "token inconnu";
}