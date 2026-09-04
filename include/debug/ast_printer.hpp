#pragma once

#include "ast/statements/declaration.hpp"

class ASTPrinter
{
public:
    static void print(const Declaration& declaration);
};