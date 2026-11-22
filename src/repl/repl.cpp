#include "repl/repl.hpp"
#include "parser/parser.hpp"
#include "interpreter/interpreter.hpp"
#include "common/exceptions.hpp"
#include "lexer/lexer.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <iterator>

void REPL::execute(const std::string& source, Interpreter& interpreter)
{
    std::vector<Token> tokens = to_tokens(source);
    Parser parser(tokens);

    for (auto& statement : parser.parse())
    {
        interpreter.execute(std::move(statement));
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
        print_tokens(to_tokens(source));
        return;
    }

    Interpreter interpreter;
    execute(source, interpreter);
}