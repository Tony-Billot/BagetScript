#include "common/token.hpp"
#include <iostream>

std::string token_type_to_string(TokenType type)
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
        case TokenType::EST_EGAL_A: return "'=='";
        case TokenType::EST_INFERIEUR_A: return "'<'";
        case TokenType::EST_SUPERIEUR_A: return "'>'";
        case TokenType::NON: return "'non'";
        case TokenType::ET: return "'et'";
        case TokenType::OU: return "'ou'";
        case TokenType::PARENTHESE_GAUCHE: return "'('";
        case TokenType::PARENTHESE_DROITE: return "')'";
        case TokenType::ACCOLADE_GAUCHE: return "'{'";
        case TokenType::ACCOLADE_DROITE: return "'}'";
        case TokenType::POINT_VIRGULE: return "';'";
        case TokenType::AFFICHER: return "'afficher'";
        case TokenType::FONCTION: return "'fonction'";
        case TokenType::VIRGULE: return "','";
        case TokenType::RETOURNER: return "'retourner'";
        case TokenType::SI: return "'si'";
        case TokenType::SINON: return "'sinon'";
        case TokenType::FIN_DE_FICHIER: return "fin du fichier";
    }

    return "token inconnu";
}

void print_tokens(const std::vector<Token>& tokens)
{
    for (const Token& token : tokens)
    {
        std::cout << token.line << ':' << token.column << ' ' << token_type_to_string(token.type);

        if (!token.valeur.empty())
        {
            std::cout << " \"" << token.valeur << '"';
        }

        std::cout << '\n';
    }
}