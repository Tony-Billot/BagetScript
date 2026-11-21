#include "repl/repl.hpp"
#include "parser/parser.hpp"
#include "interpreter/interpreter.hpp"
#include "common/exceptions.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <iterator>
#include <utility>

void REPL::execute(const std::string& source, Interpreter& interpreter)
{
    std::vector<Token> tokens = lexer.to_tokens(source);
    Parser parser(tokens);

    while (parser.has_more_tokens())
    {
        interpreter.execute(parser.parse_statement());
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