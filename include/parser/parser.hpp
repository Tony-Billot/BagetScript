#include "common/token.hpp"
#include "ast/statements/declaration.hpp"

#include <vector>

class Parser
{
private:
    std::vector<Token> tokens;
    int position = 0;

    Token current_token();
    Token consume(TokenType expected);
    std::string token_type_to_string(TokenType type);

public:
    Parser(std::vector<Token> tokens);
    Declaration parse_declaration();
};