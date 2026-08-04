#pragma once

#include <memory>
#include <string>
#include <vector>

#include "token.hpp"
#include "ast.hpp"

class Parser
{
public:
    Parser(const std::vector<Token>& tokens);

    std::unique_ptr<Program> parse();

private:
    std::vector<Token> tokens;
    size_t current = 0;
    Token& peek();
    Token& previous();
    Token advance();

    bool isAtEnd();
    bool check(TokenType type);
    bool match(TokenType type);
    bool checkIdentifierLike();

    Token consume(TokenType type, const std::string& message);
    Token consumeIdentifierLike(const std::string& message);

    std::unique_ptr<Stmt> declaration();
    std::unique_ptr<Stmt> statement();
    std::unique_ptr<Stmt> functionDeclaration();
    std::unique_ptr<Stmt> variableDeclaration();
    std::unique_ptr<Stmt> expressionStatement();
    std::unique_ptr<Stmt> printStatement();
    std::unique_ptr<Stmt> ifStatement();
    std::unique_ptr<Stmt> whileStatement();
    std::unique_ptr<Stmt> forStatement();
    std::unique_ptr<Stmt> blockStatement();
    std::unique_ptr<Expr> expression();
    std::unique_ptr<Expr> comparison();
    std::unique_ptr<Expr> term();
    std::unique_ptr<Expr> factor();
    std::vector<std::unique_ptr<Expr>> parseArguments();
    std::unique_ptr<Expr> primary();
};