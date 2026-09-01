#include "repl/repl.hpp"
#include "parser/parser.hpp"
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

    if(tokens[0].type == TokenType::TYPE_NOMBRE || tokens[0].type == TokenType::TYPE_TEXTE)
    {
        try
        {
            Parser parser(tokens);
            Declaration declaration = parser.parse_declaration();

            std::cout << "Nom: " << declaration.name << ", Valeur: " << declaration.value << '\n';
        }
        catch (const std::runtime_error& error)
        {
            std::cout << error.what() << '\n';
        }
    }
}