#include "common/exceptions.hpp"
#include "common/token.hpp"
#include "interpreter/interpreter.hpp"
#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include <cstdlib>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

namespace
{
int failures = 0;

void require(bool condition, const std::string& message)
{
    if (!condition)
    {
        throw std::runtime_error(message);
    }
}

enum class ExpectedError
{
    syntax,
    runtime
};

void require_error(const std::string& source, ExpectedError expected)
{
    try
    {
        Lexer lexer;
        std::vector<Token> tokens = lexer.to_tokens(source);
        Parser parser(tokens);
        Interpreter interpreter;

        while (parser.has_more_tokens())
        {
            if (parser.current_token().type == TokenType::AFFICHER){
                interpreter.execute(parser.parse_print_statement());
            }

            else if (parser.current_token().type == TokenType::SI) {
                interpreter.execute(parser.parse_if_statement());
            }
            else if (parser.current_token().type == TokenType::TYPE_NOMBRE || parser.current_token().type == TokenType::TYPE_TEXTE || parser.current_token().type == TokenType::TYPE_BOOLEEN) {
                interpreter.execute(parser.parse_declaration());
            }

            else if (parser.current_token().type == TokenType::IDENTIFIANT && parser.has_assignment_syntax()) {
                interpreter.execute(parser.parse_assignment());
            }

            else {
                throw SyntaxError("déclaration ou instruction attendue", parser.current_token().line, parser.current_token().column);
            }
        }
    }
    catch (const SyntaxError&)
    {
        if (expected != ExpectedError::syntax)
            throw std::runtime_error("type d'erreur inattendu");
        return;
    }
    catch (const RuntimeError&)
    {
        if (expected != ExpectedError::runtime)
            throw std::runtime_error("type d'erreur inattendu");
        return;
    }
    throw std::runtime_error("erreur attendue mais aucune erreur levée");
}

std::string execute_source(const std::string& source)
{
    Lexer lexer;
    std::vector<Token> tokens = lexer.to_tokens(source);
    Parser parser(tokens);
    Interpreter interpreter;
    std::ostringstream output;
    std::streambuf* previous = std::cout.rdbuf(output.rdbuf());

    try
    {
        while (parser.has_more_tokens())
        {
            if (parser.current_token().type == TokenType::AFFICHER){
                interpreter.execute(parser.parse_print_statement());
            }

            else if (parser.current_token().type == TokenType::SI) {
                interpreter.execute(parser.parse_if_statement());
            }
            else if (parser.current_token().type == TokenType::TYPE_NOMBRE || parser.current_token().type == TokenType::TYPE_TEXTE || parser.current_token().type == TokenType::TYPE_BOOLEEN) {
                interpreter.execute(parser.parse_declaration());
            }

            else if (parser.current_token().type == TokenType::IDENTIFIANT && parser.has_assignment_syntax()) {
                interpreter.execute(parser.parse_assignment());
            }

            else {
                throw SyntaxError("déclaration ou instruction attendue", parser.current_token().line, parser.current_token().column);
            }
        }
    }
    catch (const BagetScriptError&)
    {
        std::cout.rdbuf(previous);
        throw;
    }
    catch (const std::exception&)
    {
        std::cout.rdbuf(previous);
        throw;
    }

    std::cout.rdbuf(previous);
    return output.str();
}

struct OutputTest
{
    const char* name;
    const char* source;
    const char* expected;
};

const std::vector<OutputTest> output_tests = {
    {"declaration nombre", "nombre entier = 42; afficher(entier);", "42\n"},
    {"declaration texte", "texte message = \"Bonjour\"; afficher(message);", "Bonjour\n"},
    {"declaration booleen", "booléen actif = vrai; afficher(actif);", "vrai\n"},
    {"affectation", "nombre compteur = 1; compteur = 9; afficher(compteur);", "9\n"},
    {"priorite arithmetique", "afficher(1 + 2 * 3); afficher((1 + 2) * 3); afficher(-5);", "7\n9\n-5\n"},
    {"concatenation", "texte messageConcatene = \"Bon\" + \"jour\"; afficher(messageConcatene);", "Bonjour\n"},
    {"comparaisons", "afficher(3 < 5); afficher(5 > 3); afficher(4 == 4); afficher(4 == 5);", "vrai\nvrai\nvrai\nfaux\n"},
    {"operateurs booleens", "afficher(vrai et faux); afficher(vrai ou faux); afficher(non faux); afficher(vrai == vrai);", "faux\nvrai\nvrai\nvrai\n"},
    {"parentheses", "afficher((2 + 3) * (4 + 1));", "25\n"},
    {"si/sinon si/sinon", "nombre valeur = 2; si (valeur == 1) { afficher(\"un\"); } sinon si (valeur == 2) { afficher(\"deux\"); } sinon { afficher(\"autre\"); }", "deux\n"},
    {"conditions imbriquees", "booléen actif = vrai; si (actif) { si (non faux) { afficher(\"ok\"); } }", "ok\n"},
};

void run_output_tests()
{
    for (const OutputTest& test : output_tests)
    {
        std::cout << "[TEST] " << test.name << "... ";
        std::string actual = execute_source(test.source);
        require(actual == test.expected,
            "sortie attendue \"" + std::string(test.expected)
            + "\" mais obtenue \"" + actual + "\"");
        std::cout << "OK\n";
    }
}

void run_special_tests()
{
    struct NamedTest
    {
        const char* name;
        std::function<void()> function;
    };

    const std::vector<NamedTest> tests = {
        {"lexer : mots-clefs et fin de fichier", []()
        {
            Lexer lexer;
            std::vector<Token> tokens = lexer.to_tokens("nombre texte booléen vrai faux affichER + - * / ( ) { } ; si sinon identifiant");

            require(tokens.size() >= 19, "nombre de tokens incorrect");
            require(tokens[0].type == TokenType::TYPE_NOMBRE, "mot-clé nombre non reconnu");
            require(tokens[1].type == TokenType::TYPE_TEXTE, "mot-clé texte non reconnu");
            require(tokens[2].type == TokenType::TYPE_BOOLEEN, "mot-clé booléen non reconnu");
            require(tokens[3].type == TokenType::VRAI, "mot-clé vrai non reconnu");
            require(tokens[4].type == TokenType::FAUX, "mot-clé faux non reconnu");
            require(tokens[5].type == TokenType::IDENTIFIANT, "affichER doit rester un identifiant");
            require(tokens.back().type == TokenType::FIN_DE_FICHIER, "FIN_DE_FICHIER absent");
        }},
        {"erreur lexicale", []()
        {
            Lexer lexer;
            try
            {
                (void)lexer.to_tokens("texte message = \"non fermé;");
            }
            catch (const LexicalError&)
            {
                return;
            }
            throw std::runtime_error("LexicalError attendu");
        }},
        {"erreur syntaxique", []()
        {
            require_error("nombre x = 1", ExpectedError::syntax);
        }},
        {"variable inconnue", []()
        {
            require_error("afficher(inconnue);", ExpectedError::runtime);
        }},
        {"type incompatible", []()
        {
            require_error("nombre x = \"texte\";", ExpectedError::runtime);
        }},
    };

    for (const NamedTest& test : tests)
    {
        std::cout << "[TEST] " << test.name << "... ";
        test.function();
        std::cout << "OK\n";
    }
}
}

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    try
    {
        run_output_tests();
        run_special_tests();
        std::cout << "Tous les tests unitaires ont reussi.\n";
        return EXIT_SUCCESS;
    }
    catch (const std::exception& error)
    {
        ++failures;
        std::cout << "ECHEC\n";
        std::cerr << "  " << error.what() << '\n';
        std::cerr << failures << " test(s) en échec.\n";
        return EXIT_FAILURE;
    }
}
