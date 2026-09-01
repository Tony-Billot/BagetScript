#include "common/token.hpp"
#include "parser/ast.hpp"

#include <vector>

class Parser
{
private:
    std::vector<Token> tokens;
    int position = 0;

public:
    Parser(std::vector<Token> tokens);

    Declaration parse_declaration();
};