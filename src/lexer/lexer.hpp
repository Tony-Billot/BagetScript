#include <string>
#include <vector>
#include "token.hpp"

class Lexer
{
public:
    Lexer(const std::string& source);
    std::vector<Token> tokenize();

private:
    std::string src;
    size_t pos = 0;
    int line = 1;

    char current();
    char peek(int offset = 1);
    char advance();
    bool isAtEnd();

    void skipWhitespace();

    Token readNumber();
    Token readIdentifier();
    Token readString();

    Token makeToken(TokenType type, std::string value = "");
};