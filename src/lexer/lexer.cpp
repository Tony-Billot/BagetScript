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

    for (char c : source)
    {
        if (c == '"')
        {
            if (in_a_string)
            {
                tokens.push_back({TokenType::TEXTE, current_token});
                current_token.clear();
            }

            in_a_string = !in_a_string;
        }
        else if (in_a_string)
        {
            current_token += c;
        }
        else if (std::isspace(c))
        {
            if (!current_token.empty())
                tokens.push_back(lecture_token(current_token));

            current_token.clear();
        }
        else if (symboles.find(c) != symboles.end())
        {
            if (!current_token.empty())
                tokens.push_back(lecture_token(current_token));

            current_token.clear();
            tokens.push_back({symboles.at(c), std::string(1, c)});
        }
        else
        {
            current_token += c;
        }
    }

    if (!current_token.empty())
        tokens.push_back(lecture_token(current_token));

    tokens.push_back({TokenType::FIN_DE_FICHIER, "code is dead"});

    for (const auto& token : tokens)
    {
        std::cout << token.valeur << std::endl;
    }

    return tokens;
}

Token Lexer::lecture_token(const std::string& token)
{
    static const std::unordered_map<std::string, TokenType> mots_cles = {
        {"nombre", TokenType::TYPE_NOMBRE},
        {"texte", TokenType::TYPE_TEXTE},
        {"afficher", TokenType::AFFICHER}
    };

    std::unordered_map<std::string, TokenType>::const_iterator it = mots_cles.find(token);
    
    if (it != mots_cles.end())
        return {it->second, token};

    for (char c : token)
    {
        if (!std::isdigit(c))
            return {TokenType::IDENTIFIANT, token};
    }

    return {TokenType::NOMBRE, token};
}