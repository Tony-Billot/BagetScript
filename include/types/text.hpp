#include <string>

class Text
{
    
public:
    Text(const std::string& value);
    std::string get_value() const;

private:
    std::string value;

};