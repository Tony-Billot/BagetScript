#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "lexer.hpp"
#include "token.hpp"


int main(int argc, char** argv) {

    if (argc < 2) {
        std::cout << "Commande à utiliser : bagetscript <ton_fichier>.bs [--dstokens]" << std::endl;
        return 1;
    }

    std::string path = argv[1];

    bool debugTokens = false;

    if (argc >= 3) {
        std::string option = argv[2];

        if (option == "--dstokens") {
            debugTokens = true;
        }
    }

    std::ifstream file(path);
    if (!file.is_open()) {
        std::cout << "Erreur: impossible d'ouvrir le fichier " << path << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    std::string code = buffer.str();

    std::cout << "===== CONTENU DU FICHIER =====" << std::endl;
    std::cout << code << std::endl;

    if (debugTokens) {

        std::cout << "\n===== TOKENS =====\n";

        try
            {
                Lexer lexer(code);
                std::vector<Token> tokens = lexer.tokenize();

                for (const auto& t : tokens)
                    {
                        std::cout << "L" << t.line << "\t"
                                << tokenTypeToString(t.type);

                        if (!t.value.empty())
                            std::cout << "\t" << t.value;

                        std::cout << "\n";
                    }

                std::cout << std::endl;
            }
            catch (const std::exception& e)
            {
                std::cerr << "\nErreur du lexer : " << e.what() << std::endl;
                return 1;
            }

        std::cout << std::endl;
    }

    return 0;
}