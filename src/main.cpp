#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <filesystem>

#include "interpreter.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "token.hpp"

namespace
{
std::string escapeForDisplay(const std::string& value)
{
    std::string out;

    for (char c : value)
    {
        if (c == '\n')
            out += "\\n";
        else if (c == '\t')
            out += "\\t";
        else if (c == '\r')
            out += "\\r";
        else
            out += c;
    }

    return out;
}

void printTokens(const std::vector<Token>& tokens)
{
    std::cout << "\n===== TOKENS =====\n";
    std::cout << std::left
              << std::setw(6) << "Ligne"
              << std::setw(18) << "Type"
              << "Lexeme"
              << '\n';
    std::cout << std::string(50, '-') << '\n';

    for (const auto& token : tokens)
    {
        std::string lexeme = escapeForDisplay(token.value);

        if (token.type == TokenType::STRING)
            lexeme = "\"" + lexeme + "\"";

        std::cout << std::left
                  << std::setw(6) << token.line
                  << std::setw(18) << tokenTypeToString(token.type)
                  << lexeme
                  << '\n';
    }

    std::cout << std::endl;
}
}

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        std::cout << "Commande à utiliser : bagetscript <ton_fichier>.bgt [--dstokens]" << std::endl;
        return 1;
    }

    std::filesystem::path path(argv[1]);

    // Vérifie que le fichier possède bien l'extension .bgt
    if (path.extension() != ".bgt")
    {
        std::cerr << "Erreur : le fichier " << path << " n'a pas l'extension .bgt" << std::endl;
        return 1;
    }

    bool debugTokens = false;

    if (argc >= 3)
    {
        std::string option = argv[2];

        if (option == "--dstokens")
        {
            debugTokens = true;
        }
    }

    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cerr << "Erreur : impossible d'ouvrir le fichier " << path << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    std::string code = buffer.str();

    try
    {
        Lexer lexer(code);
        std::vector<Token> tokens = lexer.tokenize();

        if (debugTokens)
            printTokens(tokens);

        Parser parser(tokens);
        auto program = parser.parse();

        Interpreter interpreter;
        interpreter.execute(*program);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Erreur : " << e.what() << std::endl;
        return 1;
    }

    return 0;
}