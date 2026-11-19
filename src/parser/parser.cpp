#include "parser/parser.hpp"
#include "ast/expressions/identifier_expression.hpp"
#include "ast/expressions/number_expression.hpp"
#include "ast/expressions/text_expression.hpp"
#include "common/exceptions.hpp"
#include <iostream>
#include <memory>


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

    if (current_token().type != TokenType::TYPE_NOMBRE && current_token().type != TokenType::TYPE_TEXTE)
    {
        throw SyntaxError("type nombre ou type texte attendu", current_token().line, current_token().column);
    }

    declaration.type = consume(current_token().type).type;
    declaration.name = consume(TokenType::IDENTIFIANT).valeur;
    consume(TokenType::A_POUR_VALEUR);

    Token value_token = current_token();
    TokenType expected_value_type = TokenType::TEXTE;

    if (declaration.type == TokenType::TYPE_NOMBRE)
    {
        expected_value_type = TokenType::NOMBRE;
    }

    if (value_token.type != expected_value_type)
    {
        throw SyntaxError("valeur de type " + token_type_to_string(value_token.type) + " incompatible avec " + token_type_to_string(declaration.type), value_token.line, value_token.column);
    }

    if (declaration.type == TokenType::TYPE_NOMBRE)
    {
        auto value = std::make_unique<NumberExpression>();
        Token token = consume(TokenType::NOMBRE);
        value->value = std::stod(token.valeur);
        value->line = token.line;
        value->column = token.column;
        declaration.value = std::move(value);
    }
    else
    {
        auto value = std::make_unique<TextExpression>();
        Token token = consume(TokenType::TEXTE);
        value->value = token.valeur;
        value->line = token.line;
        value->column = token.column;
        declaration.value = std::move(value);
    }

    consume(TokenType::POINT_VIRGULE);

    return declaration;
}

PrintStatement Parser::parse_print_statement()
{
    PrintStatement statement;

    consume(TokenType::AFFICHER);
    consume(TokenType::PARENTHESE_GAUCHE);

    if (current_token().type == TokenType::TEXTE)
    {
        auto value = std::make_unique<TextExpression>();
        Token token = consume(TokenType::TEXTE);
        value->value = token.valeur;
        value->line = token.line;
        value->column = token.column;
        statement.expression = std::move(value);
    }
    else if (current_token().type == TokenType::NOMBRE)
    {
        auto value = std::make_unique<NumberExpression>();
        Token token = consume(TokenType::NOMBRE);
        value->value = std::stod(token.valeur);
        value->line = token.line;
        value->column = token.column;
        statement.expression = std::move(value);
    }
    else if (current_token().type == TokenType::IDENTIFIANT)
    {
        auto value = std::make_unique<IdentifierExpression>();
        Token token = consume(TokenType::IDENTIFIANT);
        value->name = token.valeur;
        value->line = token.line;
        value->column = token.column;
        statement.expression = std::move(value);
    }
    else
    {
        throw SyntaxError("texte, nombre ou identifiant attendu", current_token().line, current_token().column);
    }

    consume(TokenType::PARENTHESE_DROITE);
    consume(TokenType::POINT_VIRGULE);

    return statement;
}

std::string Parser::token_type_to_string(TokenType type)
{
    switch (type)
    {
        case TokenType::TYPE_NOMBRE: return "type nombre";
        case TokenType::TYPE_TEXTE: return "type texte";
        case TokenType::NOMBRE: return "nombre";
        case TokenType::TEXTE: return "texte";
        case TokenType::IDENTIFIANT: return "identifiant";
        case TokenType::A_POUR_VALEUR: return "'='";
        case TokenType::PLUS: return "'+'";
        case TokenType::MOINS: return "'-'";
        case TokenType::FOIS: return "'*'";
        case TokenType::DIVISE: return "'/'";
        case TokenType::PARENTHESE_GAUCHE: return "'('";
        case TokenType::PARENTHESE_DROITE: return "')'";
        case TokenType::POINT_VIRGULE: return "';'";
        case TokenType::AFFICHER: return "'afficher'";
        case TokenType::FIN_DE_FICHIER: return "fin du fichier";
    }

    return "token inconnu";
}