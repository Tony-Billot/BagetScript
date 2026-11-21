#include "lexer/lexer.hpp"
#include <iostream>
#include <cctype>
#include <map>
#include "common/exceptions.hpp"

static const std::map<char, TokenType> symboles = {
    {'=', TokenType::A_POUR_VALEUR},
    {'+', TokenType::PLUS},
    {'-', TokenType::MOINS},
    {'*', TokenType::FOIS},
    {'/', TokenType::DIVISE},
    {'(', TokenType::PARENTHESE_GAUCHE},
    {')', TokenType::PARENTHESE_DROITE},
    {'{', TokenType::ACCOLADE_GAUCHE},
    {'}', TokenType::ACCOLADE_DROITE},
    {';', TokenType::POINT_VIRGULE},
    {',', TokenType::VIRGULE}

};

static const std::map<std::string, TokenType> keywords = {
    {"nombre", TokenType::TYPE_NOMBRE},
    {"texte", TokenType::TYPE_TEXTE},
    {"booléen", TokenType::TYPE_BOOLEEN},
    {"vrai", TokenType::VRAI},
    {"faux", TokenType::FAUX},
    {"non", TokenType::NON},
    {"et", TokenType::ET},
    {"ou", TokenType::OU},
    {"afficher", TokenType::AFFICHER},
    {"si", TokenType::SI},
    {"sinon", TokenType::SINON},
    {"fonction", TokenType::FONCTION},
    {"retourner", TokenType::RETOURNER}
};

std::vector<Token> Lexer::to_tokens(const std::string& source)
{
    std::vector<Token> tokens;
    int position = 0;
    int line = 1;
    int column = 1;

    /* 
    =========================================================================
    Functions used in the main loop
    ========================================================================= 
    */
    auto is_end = [&]() -> bool
    {
        return position >= static_cast<int>(source.size());
    };

    auto peek = [&]() -> unsigned char
    {
        return static_cast<unsigned char>(source[position]);
    };

    auto advance = [&]() -> char
    {
        char current_char = source[position];
        position++;

        if (current_char == '\n')
        {
            line++;
            column = 1;
        }
        else
        {
            column++;
        }

        return current_char;
    };

    auto is_letter = [](unsigned char c) -> bool
    {
        return std::isalpha(c) || c == '_' || c >= 128;
    };


    // Loop which creates the tokens
    while (!is_end())
    {
        unsigned char current_char = peek();

        int start_line = line;
        int start_column = column;

        if (std::isspace(current_char))
        {
            advance();
        }

        else if (current_char == '='
            && position + 1 < static_cast<int>(source.size())
            && source[position + 1] == '=')
        {
            advance();
            advance();
            tokens.push_back({TokenType::EST_EGAL_A, "==", start_line, start_column});
        }

        else if (current_char == '<')
        {
            advance();
            tokens.push_back({TokenType::EST_INFERIEUR_A, "<", start_line, start_column});
        }

        else if (current_char == '>')
        {
            advance();
            tokens.push_back({TokenType::EST_SUPERIEUR_A, ">", start_line, start_column});
        }

        else if (symboles.find(current_char) != symboles.end())
        {
            advance();
            tokens.push_back({symboles.at(current_char), std::string(1, current_char), start_line, start_column});
        }

        else if (is_letter(current_char))
        {
            std::string word;

            while (!is_end() && (is_letter(peek()) || std::isdigit(peek())))
            {
                word += advance();
            }

            auto keyword = keywords.find(word);
            TokenType type = (keyword != keywords.end()) ? keyword->second : TokenType::IDENTIFIANT;

            tokens.push_back({type, word, start_line, start_column});
        }

        else if (std::isdigit(current_char))
        {
            std::string nombre;
            bool has_dot = false;

            while (!is_end() && (std::isdigit(peek()) || (peek() == '.' && !has_dot)))
            {
                if (peek() == '.')
                {
                    has_dot = true;
                }

                nombre += advance();
            }

            tokens.push_back({TokenType::NOMBRE, nombre, start_line, start_column});
        }

        else if (current_char == '"')
        {
            advance();
            std::string texte;

            while (!is_end() && peek() != '"')
            {
                texte += advance();
            }

            if (is_end())
            {
                throw LexicalError("texte non fermé (guillemet manquant)", start_line, start_column);
            }

            advance();
            tokens.push_back({TokenType::TEXTE, texte, start_line, start_column});
        }

        else
        {
            throw LexicalError("caractère inattendu '" + std::string(1, current_char) + "'", start_line, start_column);
        }
    }

    tokens.push_back({TokenType::FIN_DE_FICHIER, "", line, column});
    return tokens;
}



/* 
=========================================================================
DEBUG PART
=========================================================================
*/

//Debug function to print the tokens
std::string Lexer::token_type_to_string(TokenType type)
{
    switch (type)
    {
        case TokenType::TYPE_NOMBRE: return "TYPE_NOMBRE";
        case TokenType::TYPE_TEXTE: return "TYPE_TEXTE";
        case TokenType::TYPE_BOOLEEN: return "TYPE_BOOLEEN";
        case TokenType::NOMBRE: return "NOMBRE";
        case TokenType::TEXTE: return "TEXTE";
        case TokenType::VRAI: return "VRAI";
        case TokenType::FAUX: return "FAUX";
        case TokenType::IDENTIFIANT: return "IDENTIFIANT";
        case TokenType::A_POUR_VALEUR: return "A_POUR_VALEUR";
        case TokenType::PLUS: return "PLUS";
        case TokenType::MOINS: return "MOINS";
        case TokenType::FOIS: return "FOIS";
        case TokenType::DIVISE: return "DIVISE";
        case TokenType::PARENTHESE_GAUCHE: return "PARENTHESE_GAUCHE";
        case TokenType::PARENTHESE_DROITE: return "PARENTHESE_DROITE";
        case TokenType::ACCOLADE_GAUCHE: return "ACCOLADE_GAUCHE";
        case TokenType::ACCOLADE_DROITE: return "ACCOLADE_DROITE";
        case TokenType::POINT_VIRGULE: return "POINT_VIRGULE";
        case TokenType::AFFICHER: return "AFFICHER";
        case TokenType::SI: return "SI";
        case TokenType::SINON: return "SINON";
        case TokenType::SINON_SI: return "SINON_SI";
        case TokenType::EST_EGAL_A: return "EST_EGAL_A";
        case TokenType::EST_INFERIEUR_A: return "EST_INFERIEUR_A";
        case TokenType::EST_SUPERIEUR_A: return "EST_SUPERIEUR_A";
        case TokenType::NON: return "NON";
        case TokenType::ET: return "ET";
        case TokenType::OU: return "OU";
        case TokenType::FONCTION: return "FONCTION";
        case TokenType::RETOURNER: return "RETOURNER";
        case TokenType::VIRGULE: return "VIRGULE";
        case TokenType::FIN_DE_FICHIER: return "FIN_DE_FICHIER";
    }

    return "INCONNU";
}