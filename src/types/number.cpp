#include "types/number.hpp"

Number::Number(double value) : value(value) {}

double Number::get_value() const
{
    return value;
}