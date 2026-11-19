#pragma once

#include "ast_node.hpp"

struct Expression : ASTNode
{
    int line = 0;
    int column = 0;
};