#pragma once

#include "ast_node.hpp"

struct Statement : ASTNode
{
    virtual ~Statement() = default;
};