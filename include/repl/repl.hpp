#include <fstream>
#include <string>
#include "lexer/lexer.hpp"

class Interpreter;

class REPL
{
public:
    void run();
    void run(std::ifstream& file);

private:
    Lexer lexer;
    void execute(const std::string& source, Interpreter& interpreter);
};