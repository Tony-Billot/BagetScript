#include <fstream>
#include <string>

class Interpreter;

class REPL
{
    public:
        void run();
        void run(std::ifstream& file, bool debug = false);

    private:
        void execute(const std::string& source, Interpreter& interpreter);
};