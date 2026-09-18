#include "repl/repl.hpp"
#include "parser/parser.hpp"
#include "debug/ast_printer.hpp"

#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <iterator>


void REPL::run()
{
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    std::cout << "BagetScript v0.1\n";
    std::cout << "Tapez 'quitter' pour fermer cette fenêtre.\n\n";

    std::string input;

    while (true)
    {
        std::cout << ">>> ";

        if (!std::getline(std::cin, input))
            break;

        if (input == "quitter")
            break;

        std::vector<Token> tokens = lexer.to_tokens(input);
    }
}


void REPL::run(std::ifstream& file)
{
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    std::string source((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    std::vector<Token> tokens = lexer.to_tokens(source);

    if (tokens.empty()) {
        return;
    }

    try
    {
        Parser parser(tokens);

        while (parser.has_more_tokens())
        {
            if (parser.current_token().type == TokenType::AFFICHER)
            {
                PrintStatement statement = parser.parse_print_statement();
                ASTPrinter::print(statement);
            }
            else if (parser.current_token().type == TokenType::TYPE_NOMBRE || parser.current_token().type == TokenType::TYPE_TEXTE)
            {
                Declaration declaration = parser.parse_declaration();
                ASTPrinter::print(declaration);
            }
            else
            {
                throw std::runtime_error("Erreur : déclaration ou instruction afficher attendue ligne " + std::to_string(parser.current_token().line) + ", colonne " + std::to_string(parser.current_token().column) + ".");
            }
        }
    }
    catch (const std::runtime_error& error)
    {
        std::cout << error.what() << '\n';
    }
}