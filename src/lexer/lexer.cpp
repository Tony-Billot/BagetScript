#include "lexer/lexer.hpp"
#include <iostream>
#include <cctype>
#include <unordered_map>

static const std::unordered_map<char, TokenType> symboles = {
    {'=', TokenType::A_POUR_VALEUR},
    {'+', TokenType::PLUS},
    {'-', TokenType::MOINS},
    {'*', TokenType::FOIS},
    {'/', TokenType::DIVISE},
    {'(', TokenType::PARENTHESE_GAUCHE},
    {')', TokenType::PARENTHESE_DROITE},
    {';', TokenType::POINT_VIRGULE}
};

std::vector<Token> Lexer::to_tokens(const std::string& source)
{
    std::vector<Token> tokens;
    std::string current_token;

    bool in_a_string = false;
    int line = 1;
    int column = 1;
    int token_line = 1;
    int token_column = 1;


    for (char c : source)
    {
        if (current_token.empty())
        {
            token_line = line;
            token_column = column;
        }
        if (c == '"')
        {
            if (in_a_string)
            {
                tokens.push_back({TokenType::TEXTE, current_token, token_line, token_column});
                current_token.clear();
            }

            in_a_string = !in_a_string;
        }
        else if (in_a_string)
        {
            current_token += c;
        }
        else if (c == '\n')
        {
            if (!current_token.empty())
            {
                Token token = lecture_token(current_token);
                token.line = token_line;
                token.column = token_column;
                tokens.push_back(token);
                current_token.clear();
            }

            line++;
            column = 0;
        }
        else if (std::isspace(c))
        {
            if (!current_token.empty())
            {
                Token token = lecture_token(current_token);
                token.line = token_line;
                token.column = token_column;
                tokens.push_back(token);
                current_token.clear();
            }
        }
        else if (symboles.find(c) != symboles.end())
        {
            if (!current_token.empty())
            {
                Token token = lecture_token(current_token);
                token.line = token_line;
                token.column = token_column;
                tokens.push_back(token);
                current_token.clear();
            }

            tokens.push_back({symboles.at(c), std::string(1, c), line, column});
        }
        else
        {
            current_token += c;
        }

        column++;
    }

    if (!current_token.empty())
    {
        Token token = lecture_token(current_token);
        token.line = token_line;
        token.column = token_column;
        tokens.push_back(token);
        current_token.clear();
    }
    return tokens;
}

Token Lexer::lecture_token(const std::string& token)
{
    static const std::unordered_map<std::string, TokenType> keywords = {
        {"nombre", TokenType::TYPE_NOMBRE},
        {"texte", TokenType::TYPE_TEXTE},
        {"afficher", TokenType::AFFICHER}
    };

    std::unordered_map<std::string, TokenType>::const_iterator it = keywords.find(token);

    if (it != keywords.end())
    {
        return {it->second, token, 0, 0};
    }

    bool has_dot = false;
    bool has_digit = false;

    for (char c : token)
    {
        if (c == '.')
        {
            if (has_dot)
            {
                return {TokenType::IDENTIFIANT, token, 0, 0};
            }

            has_dot = true;
        }
        else if (!std::isdigit(c))
        {
            return {TokenType::IDENTIFIANT, token, 0, 0};
        }
        else
        {
            has_digit = true;
        }
    }

    if (has_digit)
    {
        return {TokenType::NOMBRE, token, 0, 0};
    }

    return {TokenType::IDENTIFIANT, token, 0, 0};
}

// DEBUG
std::string Lexer::token_type_to_string(TokenType type)
{
    switch (type)
    {
        case TokenType::TYPE_NOMBRE: return "TYPE_NOMBRE";
        case TokenType::TYPE_TEXTE: return "TYPE_TEXTE";
        case TokenType::NOMBRE: return "NOMBRE";
        case TokenType::TEXTE: return "TEXTE";
        case TokenType::IDENTIFIANT: return "IDENTIFIANT";
        case TokenType::A_POUR_VALEUR: return "A_POUR_VALEUR";
        case TokenType::PLUS: return "PLUS";
        case TokenType::MOINS: return "MOINS";
        case TokenType::FOIS: return "FOIS";
        case TokenType::DIVISE: return "DIVISE";
        case TokenType::PARENTHESE_GAUCHE: return "PARENTHESE_GAUCHE";
        case TokenType::PARENTHESE_DROITE: return "PARENTHESE_DROITE";
        case TokenType::POINT_VIRGULE: return "POINT_VIRGULE";
        case TokenType::AFFICHER: return "AFFICHER";
        case TokenType::FIN_DE_FICHIER: return "FIN_DE_FICHIER";
    }

    return "INCONNU";
}