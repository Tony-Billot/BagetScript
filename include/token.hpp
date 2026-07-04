#pragma once

#include <string>

enum TokenType
{
    NUMBER,
    IDENTIFIER,
    STRING,

    ENTIER,
    TEXTE,
    BOOLEEN,
    SI,
    SINON,
    TANTQUE,
    AFFICHER,

    PLUS,
    MINUS,
    STAR,
    SLASH,
    PERCENT,

    EQUAL,
    EQUAL_EQUAL,

    NOT,
    NOT_EQUAL,

    LESS,
    LESS_EQUAL,

    GREATER,
    GREATER_EQUAL,

    AND,
    OR,

    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACE,
    RIGHT_BRACE,
    SEMICOLON,

    END_OF_FILE
};

struct Token
{
    TokenType type;
    std::string value;
    int line;
};

inline std::string tokenTypeToString(TokenType t)
{
    switch (t)
    {
        case NUMBER: return "NUMBER";
        case IDENTIFIER: return "IDENTIFIER";
        case STRING: return "STRING";

        case ENTIER: return "ENTIER";
        case TEXTE: return "TEXTE";
        case BOOLEEN: return "BOOLEEN";
        case SI: return "SI";
        case SINON: return "SINON";
        case TANTQUE: return "TANTQUE";
        case AFFICHER: return "AFFICHER";

        case PLUS: return "PLUS";
        case MINUS: return "MINUS";
        case STAR: return "STAR";
        case SLASH: return "SLASH";
        case PERCENT: return "PERCENT";

        case EQUAL: return "EQUAL";
        case EQUAL_EQUAL: return "EQUAL_EQUAL";

        case NOT: return "NOT";
        case NOT_EQUAL: return "NOT_EQUAL";

        case LESS: return "LESS";
        case LESS_EQUAL: return "LESS_EQUAL";

        case GREATER: return "GREATER";
        case GREATER_EQUAL: return "GREATER_EQUAL";

        case AND: return "AND";
        case OR: return "OR";

        case LEFT_PAREN: return "LEFT_PAREN";
        case RIGHT_PAREN: return "RIGHT_PAREN";
        case LEFT_BRACE: return "LEFT_BRACE";
        case RIGHT_BRACE: return "RIGHT_BRACE";
        case SEMICOLON: return "SEMICOLON";

        case END_OF_FILE: return "EOF";
    }

    return "UNKNOWN";
}