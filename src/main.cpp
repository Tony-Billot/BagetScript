#include "repl/repl.hpp"
#include <iostream>
#include <fstream>

int main(int argc, char* argv[])
{
    REPL repl;
    if (argc > 1)
    {
        std::ifstream file(argv[1]);
        std::string extension = argv[1];
        if (extension.find_last_of('.') != std::string::npos)
        {
            extension = extension.substr(extension.find_last_of('.'));
        }
        else
        {
            extension = "";
        }   

        if (extension != ".bgt")
        {
            std::cerr << "Le fichier doit être un fichier bagetscript (.bgt).\n";
            return 1;
        }

        if (!file.is_open())
        {
            std::cerr << "Impossible d'ouvrir le fichier : " << argv[1] << '\n';
            return 1;
        }
        repl.run(file);
        return 0;
    }

    repl.run();

    return 0;
}