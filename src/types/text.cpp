#include "types/text.hpp"

Text::Text(const std::string& value) : value(value) {}

std::string Text::get_value() const
{
    return value;
}