#include "parser/parser.hpp"
#include "ast/expressions/number_expression.hpp"
#include <iostream>
#include <memory>


Parser::Parser(std::vector<Token> tokens)
{
    this->tokens = tokens;
}

Token Parser::current_token()
{
    return tokens[position];
}

Token Parser::consume(TokenType expected)
{
    if (current_token().type != expected)
    {
        throw std::runtime_error("Erreur : " + token_type_to_string(expected) + " attendu ligne " + std::to_string(current_token().line) + ", colonne " + std::to_string(current_token().column) + ".");
    }

    return tokens[position++];
}


Declaration Parser::parse_declaration()
{
    Declaration declaration;

    declaration.type = consume(TokenType::TYPE_NOMBRE).type;
    declaration.name = consume(TokenType::IDENTIFIANT).valeur;
    consume(TokenType::A_POUR_VALEUR);

    auto value = std::make_unique<NumberExpression>();
    value->value = std::stod(consume(TokenType::NOMBRE).valeur);
    declaration.value = std::move(value);

    consume(TokenType::POINT_VIRGULE);

    return declaration;
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