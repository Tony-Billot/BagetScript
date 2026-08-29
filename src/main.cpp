#include "repl/repl.hpp"
#include <iostream>
#include <fstream>

int main(int argc, char* argv[])
{
    REPL repl;
    if (argc > 1)
    {
        std::ifstream file(argv[1]);
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