#include "types/boolean.hpp"

Boolean::Boolean(bool value) : value(value) {}

bool Boolean::get_value() const
{
    return value;
}
