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
    using List = std::vector<Value>;
    std::variant<double, std::string, bool, List> data;
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

class CallExpr : public Expr
{
public:
    CallExpr(std::unique_ptr<Expr> callee, std::vector<std::unique_ptr<Expr>> arguments)
        : callee(std::move(callee)), arguments(std::move(arguments))
    {
    }

    std::unique_ptr<Expr> callee;
    std::vector<std::unique_ptr<Expr>> arguments;
};

class ListExpr : public Expr
{
public:
    explicit ListExpr(std::vector<std::unique_ptr<Expr>> elements)
        : elements(std::move(elements))
    {
    }

    std::vector<std::unique_ptr<Expr>> elements;
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

class ForRangeStmt : public Stmt
{
public:
    ForRangeStmt(
        std::string variableName,
        std::unique_ptr<Expr> start,
        std::unique_ptr<Expr> end,
        std::unique_ptr<Stmt> body)
        : variableName(std::move(variableName)),
          start(std::move(start)),
          end(std::move(end)),
          body(std::move(body))
    {
    }

    std::string variableName;
    std::unique_ptr<Expr> start;
    std::unique_ptr<Expr> end;
    std::unique_ptr<Stmt> body;
};

class ForEachStmt : public Stmt
{
public:
    ForEachStmt(
        std::string variableName,
        std::unique_ptr<Expr> iterable,
        std::unique_ptr<Stmt> body)
        : variableName(std::move(variableName)),
          iterable(std::move(iterable)),
          body(std::move(body))
    {
    }

    std::string variableName;
    std::unique_ptr<Expr> iterable;
    std::unique_ptr<Stmt> body;
};

class FunctionStmt : public Stmt
{
public:
    FunctionStmt(
        std::string name,
        std::vector<std::string> parameters,
        std::vector<std::unique_ptr<Stmt>> body)
        : name(std::move(name)),
          parameters(std::move(parameters)),
          body(std::move(body))
    {
    }

    std::string name;
    std::vector<std::string> parameters;
    std::vector<std::unique_ptr<Stmt>> body;
};