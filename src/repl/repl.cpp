#include "repl/repl.hpp"
#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>

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
    std::string line;

    while (std::getline(file, line))
    {
        std::vector<Token> tokens = lexer.to_tokens(line);
    }
}