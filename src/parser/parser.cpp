#include "parser/parser.hpp"

Parser::Parser(std::vector<Token> tokens)
{
    this->tokens = tokens;
}

Declaration Parser::parse_declaration()
{
    Declaration declaration;

    position++;

    declaration.name = tokens[position].valeur;

    position += 2;

    declaration.value = tokens[position].valeur;

    return declaration;
}