#include "common/token.hpp"
#include "ast/statements/declaration.hpp"
#include "ast/statements/print_statement.hpp"

#include <vector>

class Parser
{
private:
    std::vector<Token> tokens;
    int position = 0;

    Token consume(TokenType expected);
    std::string token_type_to_string(TokenType type);

public:
    Parser(std::vector<Token> tokens);
    Token current_token();
    bool has_more_tokens() const;
    Declaration parse_declaration();
    PrintStatement parse_print_statement();
};