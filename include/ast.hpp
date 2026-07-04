#pragma once

#include <memory>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include "token.hpp"

class Node
{
public:
    virtual ~Node() = default;
};

class Expr : public Node
{
public:
    virtual ~Expr() = default;
};

class Stmt : public Node
{
public:
    virtual ~Stmt() = default;
};

class Program : public Node
{
public:
    std::vector<std::unique_ptr<Stmt>> statements;
};

struct Value
{
    std::variant<double, std::string, bool> data;
};

class LiteralExpr : public Expr
{
public:
    explicit LiteralExpr(Value value)
        : value(std::move(value))
    {
    }

    Value value;
};

class VariableExpr : public Expr
{
public:
    explicit VariableExpr(std::string name)
        : name(std::move(name))
    {
    }

    std::string name;
};

class AssignExpr : public Expr
{
public:
    AssignExpr(std::string name, std::unique_ptr<Expr> value)
        : name(std::move(name)), value(std::move(value))
    {
    }

    std::string name;
    std::unique_ptr<Expr> value;
};

class UnaryExpr : public Expr
{
public:
    UnaryExpr(TokenType op, std::unique_ptr<Expr> right)
        : op(op), right(std::move(right))
    {
    }

    TokenType op;
    std::unique_ptr<Expr> right;
};

class BinaryExpr : public Expr
{
public:
    BinaryExpr(std::unique_ptr<Expr> left, TokenType op, std::unique_ptr<Expr> right)
        : left(std::move(left)), op(op), right(std::move(right))
    {
    }

    std::unique_ptr<Expr> left;
    TokenType op;
    std::unique_ptr<Expr> right;
};

class GroupingExpr : public Expr
{
public:
    explicit GroupingExpr(std::unique_ptr<Expr> expr)
        : expr(std::move(expr))
    {
    }

    std::unique_ptr<Expr> expr;
};

class ExpressionStmt : public Stmt
{
public:
    explicit ExpressionStmt(std::unique_ptr<Expr> expr)
        : expr(std::move(expr))
    {
    }

    std::unique_ptr<Expr> expr;
};

class PrintStmt : public Stmt
{
public:
    explicit PrintStmt(std::unique_ptr<Expr> expr)
        : expr(std::move(expr))
    {
    }

    std::unique_ptr<Expr> expr;
};

class VarDeclStmt : public Stmt
{
public:
    VarDeclStmt(TokenType typeToken, std::string name, std::unique_ptr<Expr> initializer)
        : typeToken(typeToken), name(std::move(name)), initializer(std::move(initializer))
    {
    }

    TokenType typeToken;
    std::string name;
    std::unique_ptr<Expr> initializer;
};

class BlockStmt : public Stmt
{
public:
    explicit BlockStmt(std::vector<std::unique_ptr<Stmt>> statements)
        : statements(std::move(statements))
    {
    }

    std::vector<std::unique_ptr<Stmt>> statements;
};

class IfStmt : public Stmt
{
public:
    IfStmt(
        std::unique_ptr<Expr> condition,
        std::unique_ptr<Stmt> thenBranch,
        std::unique_ptr<Stmt> elseBranch)
        : condition(std::move(condition)),
          thenBranch(std::move(thenBranch)),
          elseBranch(std::move(elseBranch))
    {
    }

    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> thenBranch;
    std::unique_ptr<Stmt> elseBranch;
};

class WhileStmt : public Stmt
{
public:
    WhileStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> body)
        : condition(std::move(condition)), body(std::move(body))
    {
    }

    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> body;
};