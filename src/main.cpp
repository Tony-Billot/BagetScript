#include "repl/repl.hpp"
#include "common/exceptions.hpp"
#include <iostream>
#include <fstream>

#ifdef _WIN32
#include <windows.h>
#endif

int main(int argc, char* argv[])
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    REPL repl;
    if (argc > 1)
    {
        bool debug = argc > 2 && std::string(argv[2]) == "--debug";
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
        try
        {
            repl.run(file, debug);
        }
        catch (const BagetScriptError& error)
        {
            std::cerr << argv[1] << ':' << error.line << ':' << error.column << ": erreur : " << error.what() << '\n';
            return 1;
        }
        catch (const std::exception& error)
        {
            std::cerr << "Erreur interne : " << error.what() << '\n';
            return 2;
        }
        return 0;
    }

    repl.run();

    return 0;
}