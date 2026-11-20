#include "repl/repl.hpp"
#include "parser/parser.hpp"
#include "interpreter/interpreter.hpp"
#include "common/exceptions.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <iterator>

void REPL::execute(const std::string& source, Interpreter& interpreter)
{
    std::vector<Token> tokens = lexer.to_tokens(source);
    Parser parser(tokens);

    while (parser.has_more_tokens())
    {
        if (parser.current_token().type == TokenType::AFFICHER)
        {
            PrintStatement statement = parser.parse_print_statement();
            interpreter.execute(statement);
        }
        else if (parser.current_token().type == TokenType::TYPE_NOMBRE || parser.current_token().type == TokenType::TYPE_TEXTE || parser.current_token().type == TokenType::TYPE_BOOLEEN)
        {
            Declaration declaration = parser.parse_declaration();
            interpreter.execute(declaration);
        }
        else if (parser.current_token().type == TokenType::IDENTIFIANT)
        {
            Assignment assignment = parser.parse_assignment();
            interpreter.execute(assignment);
        }
        else
        {
            throw SyntaxError("déclaration ou instruction afficher attendue", parser.current_token().line, parser.current_token().column);
        }
    }
}

void REPL::run()
{
    std::cout << "BagetScript v0.1\n";
    std::cout << "Tapez 'quitter' pour fermer cette fenêtre.\n\n";

    Interpreter interpreter;
    std::string input;

    while (true)
    {
        std::cout << ">>> ";

        if (!std::getline(std::cin, input))
            break;

        if (input == "quitter")
            break;

        try
        {
            execute(input, interpreter);
        }
        catch (const BagetScriptError& error)
        {
            std::cerr << error.line << ':' << error.column << ": erreur : " << error.what() << '\n';
        }
    }
}


void REPL::run(std::ifstream& file, bool debug)
{
    std::string source((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    if (debug)
    {
        std::vector<Token> tokens = lexer.to_tokens(source);
        for (const Token& token : tokens)
        {
            std::cout << token.line << ':' << token.column << " " << lexer.token_type_to_string(token.type);
            if (!token.valeur.empty()) {
                std::cout << " \"" << token.valeur << '"';
            }
            std::cout << '\n';

        }
        return;
    }

    Interpreter interpreter;
    execute(source, interpreter);
}