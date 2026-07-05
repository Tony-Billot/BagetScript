#include "lexer.hpp"
#include <cctype>
#include <stdexcept>

Lexer::Lexer(const std::string& source)
{
    src = source;
}

char Lexer::current()
{
    if (isAtEnd())
        return '\0';

    return src[pos];
}

char Lexer::peek(int offset)
{
    if (pos + offset >= src.size())
        return '\0';

    return src[pos + offset];
}

char Lexer::advance()
{
    char c = src[pos++];

    if (c == '\n')
        line++;

    return c;
}

bool Lexer::isAtEnd()
{
    return pos >= src.size();
}

void Lexer::skipWhitespace()
{
    while (!isAtEnd())
    {
        char c = current();

        if (std::isspace((unsigned char)c))
        {
            advance();
            continue;
        }

        if (c == '/' && peek() == '/')
        {
            while (!isAtEnd() && current() != '\n')
                advance();

            continue;
        }

        if (c == '/' && peek() == '*')
        {
            advance();
            advance();

            while (!isAtEnd())
            {
                if (current() == '*' && peek() == '/')
                {
                    advance();
                    advance();
                    break;
                }

                advance();
            }

            continue;
        }

        break;
    }
}

Token Lexer::makeToken(TokenType type, std::string value)
{
    return Token{type, value, line};
}

Token Lexer::readNumber()
{
    std::string value;

    while (!isAtEnd() && std::isdigit((unsigned char)current()))
        value += advance();

    return makeToken(NUMBER, value);
}

Token Lexer::readIdentifier()
{
    std::string value;

    while (!isAtEnd() &&
           (std::isalnum((unsigned char)current()) || current() == '_'))
    {
        value += advance();
    }

    if (value == "entier") return makeToken(ENTIER, value);
    if (value == "texte") return makeToken(TEXTE, value);
    if (value == "booleen") return makeToken(BOOLEEN, value);
    if (value == "liste") return makeToken(LISTE, value);
    if (value == "si") return makeToken(SI, value);
    if (value == "sinon") return makeToken(SINON, value);
    if (value == "tantque") return makeToken(TANTQUE, value);
    if (value == "pour") return makeToken(POUR, value);
    if (value == "allant") return makeToken(ALLANT, value);
    if (value == "de") return makeToken(DE, value);
    if (value == "a") return makeToken(A, value);
    if (value == "dans") return makeToken(DANS, value);
    if (value == "afficher") return makeToken(AFFICHER, value);
    if (value == "fonction") return makeToken(FONCTION, value);

    return makeToken(IDENTIFIER, value);
}

Token Lexer::readString()
{
    advance();

    std::string value;

    while (!isAtEnd() && current() != '"')
        value += advance();

    if (isAtEnd())
        throw std::runtime_error("String non fermee ligne " + std::to_string(line));

    advance();

    return makeToken(STRING, value);
}

std::vector<Token> Lexer::tokenize()
{
    std::vector<Token> tokens;

    while (!isAtEnd())
    {
        skipWhitespace();

        if (isAtEnd())
            break;

        char c = current();

        if (std::isdigit((unsigned char)c))
        {
            tokens.push_back(readNumber());
            continue;
        }

        if (std::isalpha((unsigned char)c) || c == '_')
        {
            tokens.push_back(readIdentifier());
            continue;
        }

        if (c == '"')
        {
            tokens.push_back(readString());
            continue;
        }

        switch (c)
        {
            case '+':
                advance();
                tokens.push_back(makeToken(PLUS, "+"));
                break;

            case '-':
                advance();
                tokens.push_back(makeToken(MINUS, "-"));
                break;

            case '*':
                advance();
                tokens.push_back(makeToken(STAR, "*"));
                break;

            case '/':
                advance();
                tokens.push_back(makeToken(SLASH, "/"));
                break;

            case '%':
                advance();
                tokens.push_back(makeToken(PERCENT, "%"));
                break;

            case '=':
                advance();

                if (current() == '=')
                {
                    advance();
                    tokens.push_back(makeToken(EQUAL_EQUAL, "=="));
                }
                else
                {
                    tokens.push_back(makeToken(EQUAL, "="));
                }

                break;

            case '!':
                advance();

                if (current() == '=')
                {
                    advance();
                    tokens.push_back(makeToken(NOT_EQUAL, "!="));
                }
                else
                {
                    tokens.push_back(makeToken(NOT, "!"));
                }

                break;

            case '>':
                advance();

                if (current() == '=')
                {
                    advance();
                    tokens.push_back(makeToken(GREATER_EQUAL, ">="));
                }
                else
                {
                    tokens.push_back(makeToken(GREATER, ">"));
                }

                break;

            case '<':
                advance();

                if (current() == '=')
                {
                    advance();
                    tokens.push_back(makeToken(LESS_EQUAL, "<="));
                }
                else
                {
                    tokens.push_back(makeToken(LESS, "<"));
                }

                break;

            case '&':
                advance();

                if (current() == '&')
                {
                    advance();
                    tokens.push_back(makeToken(AND, "&&"));
                }
                else
                {
                    throw std::runtime_error("Caractere inattendu '&' ligne " + std::to_string(line));
                }

                break;

            case '|':
                advance();

                if (current() == '|')
                {
                    advance();
                    tokens.push_back(makeToken(OR, "||"));
                }
                else
                {
                    throw std::runtime_error("Caractere inattendu '|' ligne " + std::to_string(line));
                }

                break;

            case ';':
                advance();
                tokens.push_back(makeToken(SEMICOLON, ";"));
                break;

            case '(':
                advance();
                tokens.push_back(makeToken(LEFT_PAREN, "("));
                break;

            case ')':
                advance();
                tokens.push_back(makeToken(RIGHT_PAREN, ")"));
                break;

            case '{':
                advance();
                tokens.push_back(makeToken(LEFT_BRACE, "{"));
                break;

            case '}':
                advance();
                tokens.push_back(makeToken(RIGHT_BRACE, "}"));
                break;

            case '[':
                advance();
                tokens.push_back(makeToken(LEFT_BRACKET, "["));
                break;

            case ']':
                advance();
                tokens.push_back(makeToken(RIGHT_BRACKET, "]"));
                break;

            case ',':
                advance();
                tokens.push_back(makeToken(COMMA, ","));
                break;

            default:
                throw std::runtime_error(
                    "Caractere inattendu '" +
                    std::string(1, current()) +
                    "' ligne " +
                    std::to_string(line));
        }
    }

    tokens.push_back(makeToken(END_OF_FILE, "<eof>"));

    return tokens;
}