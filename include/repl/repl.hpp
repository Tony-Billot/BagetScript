#include <fstream>

class REPL
{
public:
    void run();
    void run(std::ifstream& file);
};