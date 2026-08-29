#include <iostream>
#include <string>
#include <windows.h>

int main()
{
    // Activer accents et caractères UTF8
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

        std::cout << "Vous avez rentré : " << input << '\n';
    }

    return 0;
}