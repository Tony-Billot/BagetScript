#include <fstream>
#include "lexer/lexer.hpp"

class REPL
{
public:
    void run();
    void run(std::ifstream& file);

private:
    Lexer lexer;
};